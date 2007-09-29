<?php
// $Id: comment.inc,v 1.87 2007-09-29 22:07:26 thehunmonkgroup Exp $
// $Name:  $

function project_issue_comment(&$arg, $op) {
  static $edit;
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

  switch ($op) {
    case 'form':
      $project = node_load($node->pid);
      $extra = array();
      $extra[] = l($project->title, 'node/'. $project->nid);
      // Only add issue title to the breadcrumb if this comment is a reply
      // to another comment.
      if ($arg['pid']['#value']) {
        $extra[] = l($node->title, 'node/'. $node->nid);
      }
      project_project_set_breadcrumb($project, $extra);

      // $arg is a form
      if (!$node->comment_count) {
        foreach (array('nid', 'pid', 'rid', 'category', 'component', 'priority', 'assigned', 'sid', 'title') as $var) {
          $edit->$var = $node->$var;
        }
      }
      else {
        $edit = db_fetch_object(db_query_range('SELECT nid, title, pid, rid, component, category, priority, assigned, sid, timestamp FROM {project_issue_comments} WHERE nid = %d ORDER BY timestamp DESC ', $node->nid, 0, 1));
      }
      // We need to ask for almost the same metadata as project issue itself
      // so let's reuse the form.
      $form = drupal_retrieve_form('project_issue_form', $edit, NULL);
      // We need this otherwise pid collides with comment.
      $form['project_info']['#tree'] = TRUE;
      $form['project_info']['#weight'] = -2;
      $form['issue_info']['#weight'] = -1;
      $form['#prefix'] = '<div class="project-issue"><div class="node-form"><div class="standard">';
      $form['#suffix'] = '</div></div></div>';
      $form['title'] = array(
        '#type' => 'textfield',
        '#title' => t('Issue title'),
        '#maxlength' => 128,
        '#default_value' => $edit->title,
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
      db_query("INSERT INTO {project_issue_comments} (nid, cid, pid, rid, component, category, priority, assigned, sid, title, timestamp) VALUES (%d, %d, %d, %d, '%s', '%s', %d, %d, %d, '%s', %d)", $arg['nid'], $arg['cid'], $arg['project_info']['pid'], $arg['rid'], $arg['project_info']['component'], $arg['category'], $arg['priority'], $arg['assigned'], $arg['sid'], $arg['title'], $arg['timestamp']);
      project_issue_update_by_comment($arg, 'insert');
      break;
    case 'update':
      // Updated comments always get moved to the most recently posted comment -- otherwise
      // the state workflow tables can get screwy.
      $time = time();
      // Should we be using something like comment_save() here?
      db_query("UPDATE {comments} SET timestamp = %d WHERE cid = %d", $time, $arg['cid']);
      db_query("UPDATE {project_issue_comments} SET pid = %d, rid = %d, component = '%s', category = '%s', priority = %d, assigned = %d, sid = %d, title = '%s', timestamp = %d WHERE cid = %d", $arg['project_info']['pid'], $arg['rid'], $arg['project_info']['component'], $arg['category'], $arg['priority'], $arg['assigned'], $arg['sid'], $arg['title'], $time, $arg['cid']);
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
        // Add a dummy rid if necessary -- prevents incorrect change data
        if (!isset($arg->rid)) {
          $test->rid = 0;
        }
        $project_issue_table = _project_issue_comment_table(_project_issue_comment_labels(), $edit, $test);
      }
      if ($project_issue_table) {
        $arg->comment = '<div class="project-issue"><div class="summary">'. $project_issue_table .'</div></div>' . $arg->comment;
      }
      break;
    case 'validate':
      $project = node_load($node->pid);
      if (module_exists('project_release') && ($releases = project_release_get_releases($project, 0))) {
        empty($arg['project_info']['rid']) and form_set_error('project_info][rid', t('You have to specify a valid version.'));
      }
      $component = $arg['project_info']['component'];
      if ($component && !in_array($component, $project->components)) {
        $component = 0;
      }
      empty($component) && form_set_error('project_info][component', t('You have to specify a valid component.'));
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
  if (is_array($comment_data)) {
    // Massage the incoming data so the structure will work with the project_issue_update() function.
    $comment_data['component'] = $comment_data['project_info']['component'];
    $comment_data['pid'] = $comment_data['project_info']['pid'];
    unset ($comment_data['project_info']);
    $comment_data = (object) $comment_data;
  }

  switch ($op) {
    case 'insert':
    case 'update':
      project_issue_update($comment_data);
      $update_title = TRUE;
      break;
    case 'delete':
      // Get the cid of the most recent comment, so we know if that's the one we're deleting.
      $latest_cid = db_result(db_query_range('SELECT cid FROM {project_issue_comments} WHERE nid = %d ORDER BY timestamp DESC', $comment_data->nid, 0, 1));
      if ($comment_data->cid == $latest_cid) {
        // Get the cid of the next most recent comment, which will now be the last.
        $latest_cid = db_result(db_query_range('SELECT cid FROM {project_issue_comments} WHERE nid = %d AND cid <> %d ORDER BY timestamp DESC', $comment_data->nid, $comment_data->cid, 0, 1));
        $comment_data = db_fetch_object(db_query('SELECT * FROM {project_issue_comments} WHERE cid = %d', $latest_cid));
        // Update the issue data to reflect the new final states.
        project_issue_update($comment_data);
        $update_title = TRUE;
      }
      break;
  }

  // Issue title must be updated manually, when a comment is inserted, updated, or the last
  // comment is deleted.
  if (isset($update_title)) {
    $node = node_load($comment_data->nid);
    $node->title = $comment_data->title;
    node_save($node);
  }
}
