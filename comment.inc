<?php
// $Id: comment.inc,v 1.96 2007/10/02 07:02:32 thehunmonkgroup Exp $
// $Name:  $

function project_issue_comment(&$arg, $op) {
  // $arg can be a comment object, or a form or form_values.
  if (is_object($arg)) {
    $nid = $arg->nid;
  }
  elseif (is_array($arg)) {
    $nid = is_array($arg['nid']) ? $arg['nid']['#value'] : $arg['nid'];
  }
  $node = node_load($nid);
  if ($node->type != 'project_issue') {
    return;
  }

  // Make sure project is current here -- it may have changed when posted.
  // This is ugly, but the form workflow doesn't really offer a better
  // choice for this scenario.
  if (isset($_POST['project_info']['pid'])) {
    $node->pid = $_POST['project_info']['pid'];
  }
  $project = node_load(array('nid' => $node->pid, 'type' => 'project_project'));

  switch ($op) {
    case 'form':
      // Only allow metadata changes on new followups.
      if (isset($arg['cid']['#value'])) {
        return array();
      }
      // Set breadcrumb.
      $extra = array();
      $extra[] = l($project->title, 'node/'. $project->nid);
      // Only add issue title to the breadcrumb if this comment is a reply
      // to another comment.
      if ($arg['pid']['#value']) {
        $extra[] = l($node->title, 'node/'. $node->nid);
      }
      project_project_set_breadcrumb($project, $extra);

      // We need to ask for almost the same metadata as project issue itself
      // so let's reuse the form.
      $form = drupal_retrieve_form('project_issue_form', $node, NULL);
      // We need this otherwise pid collides with comment.
      $form['project_info']['#tree'] = TRUE;
      $form['project_info']['#weight'] = -2;

      // Because we've tree'd project_info, the id's for these elements must
      // be set by hand, or the js project reloads will break.
      $form['project_info']['pid']['#id'] = 'edit-pid';
      $form['project_info']['rid']['#id'] = 'edit-rid';
      $form['project_info']['component']['#id'] = 'edit-component';

      $form['issue_info']['#weight'] = -1;
      $form['#prefix'] = '<div class="project-issue"><div class="node-form"><div class="standard">';
      $form['#suffix'] = '</div></div></div>';
      $form['title'] = array(
        '#type' => 'textfield',
        '#title' => t('Issue title'),
        '#maxlength' => 128,
        '#default_value' => $node->title,
        '#weight' => -30,
        '#required' => TRUE,
        '#description' => '<em>'. t('Note: modifying this value will change the title of the entire issue, not your follow-up comment.') .'</em>',
      );
      // Mark necessary required fields now, as these aren't added in the initial
      // building of the project issue form.
      _project_issue_form_add_required_fields($form, FALSE);
      unset($form['page'], $form['issue_details']);
      return $form;
    case 'insert':
      db_query("INSERT INTO {project_issue_comments} (nid, cid, pid, rid, component, category, priority, assigned, sid, title, timestamp) VALUES (%d, %d, %d, %d, '%s', '%s', %d, %d, %d, '%s', %d)", $arg['nid'], $arg['cid'], $arg['project_info']['pid'], $arg['project_info']['rid'], $arg['project_info']['component'], $arg['category'], $arg['priority'], $arg['assigned'], $arg['sid'], $arg['title'], $arg['timestamp']);
      project_issue_update_by_comment($arg, 'insert');
      break;
    case 'update':
      project_issue_update_by_comment($arg, 'update');
      break;
    case 'delete':
      // Call the update by comment function first, so it can correctly determine if this
      // is the most recent comment.
      project_issue_update_by_comment($arg, 'delete');
      db_query("DELETE FROM {project_issue_comments} WHERE cid = %d", $arg->cid);

      break;
    case 'view':
      if (isset($arg->cid)) {
        $project_issue_table = project_issue_comment_view($node, $arg);
      }
      else {
        $test = drupal_clone($arg);
        $test->pid = $arg->project_info['pid'];
        $test->component = $arg->project_info['component'];
        // Add a dummy rid if necessary -- prevents incorrect change data.
        $test->rid = $arg->project_info['rid'] ? $arg->project_info['rid'] : 0;
        $project_issue_table = _project_issue_comment_table(_project_issue_comment_labels(), $node, $test);
      }
      if ($project_issue_table) {
        $arg->comment = '<div class="project-issue"><div class="summary">'. $project_issue_table .'</div></div>' . $arg->comment;
      }
      break;
    case 'validate':
      // Only validate metadata changes on new followups.
      if (isset($arg['cid'])) {
        return;
      }
      if ($project) {
        // Force all comments to be a child of the main issue, to match the
        // flat display, and also to prevent accidentally deleting a thread.
        form_set_value(array('#parents' => array('pid')), 0);

        // Auto numbering for comment subjects.
        $count = db_result(db_query('SELECT COUNT(cid) FROM {project_issue_comments} WHERE nid = %d', $arg['nid'])) + 1;
        form_set_value(array('#parents' => array('subject')), "#$count");

        // Validate version.
        if (module_exists('project_release') && ($releases = project_release_get_releases($project, 0))) {
          $rid = $arg['project_info']['rid'];
          if ($rid && !in_array($rid, array_keys($releases))) {
            $rid = 0;
          }
          empty($rid) and form_set_error('project_info][rid', t('You have to specify a valid version.'));
        }
        // Validate component.
        $component = $arg['project_info']['component'];
        if ($component && !in_array($component, $project->components)) {
          $component = 0;
        }
        empty($component) && form_set_error('project_info][component', t('You have to specify a valid component.'));
      }
      else {
        form_set_error('project_info][pid', t('You have to specify a valid project.'));
      }
      empty($arg['category']) && form_set_error('category', t('You have to specify a valid category.'));
      break;
  }
}

function _project_issue_comment_labels() {
  return array(
    'title' => t('Title'),
    'pid' => t('Project'),
    'rid' => t('Version'),
    'component' => t('Component'),
    'category' => t('Category'),
    'priority' => t('Priority'),
    'assigned' => t('Assigned to'),
    'sid' => t('Status'),
  );
}

/**
 * Create a project issue metadata table.
 *
 * @param $labels
 *  An array, keys are field names, values are the displayed labels.
 * @param $old
 *  The previous comment (or the node).
 * @param $followup
 *  The current comment.
 */
function _project_issue_comment_table($labels, $old, $followup) {
  $rows = array();
  foreach ($labels as $field => $text) {
    if ($old->$field != $followup->$field) {
      $rows[] = array(
        $labels[$field] .':',
        project_mail_summary($field, $old->$field),
        '&raquo; '. project_mail_summary($field, $followup->$field),
      );
    }
  }
  return theme('table', array(), $rows);
}

/**
 * Returns the issue metadata table for a comment.
 *
 * @param $node
 *  The corresponding node.
 * @param $comment
 *  The comment, if it's set then metadata will be returned. If it's not
 *  set then metadata will be precalculated.
 * @return
 *  A themed table of issue metadata.
 */
function project_issue_comment_view(&$node, $comment = NULL) {
  static $project_issue_tables;

  if (isset($comment)) {
    return $project_issue_tables[$comment->cid];
  }
  if ($node->comment_count) {
    $old = unserialize(db_result(db_query('SELECT original_issue_data FROM {project_issues} WHERE nid = %d', $node->nid)));
    $labels = _project_issue_comment_labels();
    $result = db_query('SELECT cid, title, pid, rid, component, category, priority, assigned, sid FROM {project_issue_comments} WHERE nid = %d ORDER BY timestamp ASC', $node->nid);
    while ($followup = db_fetch_object($result)) {
      $project_issue_tables[$followup->cid] = _project_issue_comment_table($labels, $old, $followup);
      $old = $followup;
    }
  }
}

/**
 * Updates the project issue based on the comment inserted/updated/deleted.
 *
 * @param $comment_data
 *  The comment data that's been submitted.
 * @param $op
 *  The comment operation performed, 'insert', 'update', 'delete'.
 */
function project_issue_update_by_comment($comment_data, $op) {
  switch ($op) {
    case 'insert':
      // Massage the incoming data so the structure is consistent throughout the function.
      $comment_data['component'] = $comment_data['project_info']['component'];
      $comment_data['pid'] = $comment_data['project_info']['pid'];
      $comment_data['rid'] = $comment_data['project_info']['rid'];
      unset ($comment_data['project_info']);
      $comment_data = (object) $comment_data;
      $update_issue = TRUE;
      break;
    case 'update':
      $comment_data = (object) $comment_data;
      break;
    case 'delete':
      // Get the cid of the most recent comment, so we know if that's the one we're deleting.
      $latest_cid = db_result(db_query_range('SELECT cid FROM {project_issue_comments} WHERE nid = %d ORDER BY timestamp DESC', $comment_data->nid, 0, 1));
      if ($comment_data->cid == $latest_cid) {
        // Get the cid of the next most recent comment, which will now be the last.
        $latest_cid = db_result(db_query_range('SELECT cid FROM {project_issue_comments} WHERE nid = %d AND cid <> %d ORDER BY timestamp DESC', $comment_data->nid, $comment_data->cid, 0, 1));
        if ($latest_cid) {
          $comment_data = db_fetch_object(db_query('SELECT * FROM {project_issue_comments} WHERE cid = %d', $latest_cid));
        }
        // If this was the last comment on the issue, then treat the original issue as the last comment.
        else {
          $comment_data = node_load($comment_data->nid);
        }

        $update_issue = TRUE;
      }
      break;
  }

  // Update the issue data to reflect the new final states.
  if (isset($update_issue)) {
    db_query("UPDATE {project_issues} SET pid = %d, category = '%s', component = '%s', priority = %d, rid = %d, assigned = %d, sid = %d WHERE nid = %d", $comment_data->pid, $comment_data->category, $comment_data->component, $comment_data->priority, $comment_data->rid, $comment_data->assigned, $comment_data->sid, $comment_data->nid);
  }

  // Update the issue title if necessary.
  $node = node_load($comment_data->nid, NULL, TRUE);  // Don't use cached since we changed data above.
  if (isset($update_issue)) {
    $node->title = $comment_data->title;
  }
  // This also triggers the mail notification, and updates the changed date of the issue.
  node_save($node);
}
