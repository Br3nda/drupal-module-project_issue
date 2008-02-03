<?php
// $Id: comment.inc,v 1.120 2008-02-03 04:47:43 thehunmonkgroup Exp $
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
  // Make a copy here so we have all the original metadata, since some
  // of it can change below.
  $original_node = drupal_clone($node);

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

      project_issue_set_breadcrumb($node, $project);

      // We need to ask for almost the same metadata as project issue itself
      // so let's reuse the form.
      $form = drupal_retrieve_form('project_issue_form', $node, NULL);
      // We need this otherwise pid collides with comment.
      $form['project_info']['#tree'] = TRUE;
      $form['project_info']['#weight'] = -2;

      $form['issue_info']['#weight'] = -1;
      $form['#prefix'] = '<div class="project-issue"><div class="node-form"><div class="standard">';
      $form['#suffix'] = '</div></div></div>';

      $form['original_issue'] = array(
        '#type' => 'fieldset',
        '#title' => t('Edit issue settings'),
        '#description' => t('Note: changing any of these items will update the issue\'s overall values.'),
        '#collapsible' => TRUE,
        '#weight' => -10,
      );

      $form['original_issue']['title'] = array(
        '#type' => 'textfield',
        '#title' => t('Issue title'),
        '#maxlength' => 128,
        '#default_value' => $node->title,
        '#weight' => -30,
        '#required' => TRUE,
      );

      // Remove the 'Project information' and 'Issue information' fieldsets,
      // they're ugly after we move things inside the 'Edit issue settings' fieldset.
      unset($form['project_info']['#type'], $form['project_info']['#title'], $form['issue_info']['#type'], $form['issue_info']['#title']);

      // Restructure the UI to de-emphasize the original project form inputs.
      $form['original_issue']['project_info'] = $form['project_info'];
      $form['original_issue']['issue_info'] = $form['issue_info'];
      unset($form['project_info'], $form['issue_info']);
      unset($form['issue_details'], $form['project_help']);
      return $form;
    case 'insert':
      // Get a lock on the issue in order to generate the next comment ID.
      $tries = 20;
      $sleep_increment = 0;
      while ($tries) {
        $lock = db_query("UPDATE {project_issues} SET db_lock = 1 WHERE nid = %d AND db_lock = 0", $arg['nid']);
        if (db_affected_rows()) {
          $id = db_result(db_query("SELECT last_comment_id FROM {project_issues} WHERE nid = %d", $arg['nid'])) + 1;
          db_query("UPDATE {project_issues} SET last_comment_id = %d, db_lock = 0 WHERE nid = %d", $id, $arg['nid']);
          break;
        }

        // Wait a random and increasing amount of time before the next attempt.
        $sleep = rand(10000, 1000000) + $sleep_increment;
        usleep($sleep);
        $sleep_increment += 50000;
        $tries--;
      }

      if (isset($id)) {
        db_query("INSERT INTO {project_issue_comments} (nid, cid, pid, rid, component, category, priority, assigned, sid, title, timestamp) VALUES (%d, %d, %d, %d, '%s', '%s', %d, %d, %d, '%s', %d)", $arg['nid'], $arg['cid'], $arg['project_info']['pid'], $arg['project_info']['rid'], $arg['project_info']['component'], $arg['category'], $arg['priority'], $arg['assigned'], $arg['sid'], $arg['title'], $arg['timestamp']);
        db_query("UPDATE {comments} SET subject = '%s' WHERE cid = %d", "#$id", $arg['cid']);
        project_issue_update_by_comment($arg, 'insert');
      }
      else {
        drupal_set_message(t('There was an error submitting your comment -- please try again. If the problem persists, contact the system administrator.'), 'error');
        watchdog('project_issue', t('Error obtaining lock for project issue %nid', array('%nid' => $arg['nid'])), WATCHDOG_ERROR, 'node/'. $arg['nid']);
        // This is a bit extreme, but we have to clean up the failed comment,
        // or it will appear on the issue.
        _comment_delete_thread((object) $arg);
        _comment_update_node_statistics($arg['nid']);
        cache_clear_all();
        // The hard redirect prevents any bogus data from being inserted for the failed comment.
        drupal_goto('node/'. $arg['nid']);
      }

      break;
    case 'update':
      project_issue_update_by_comment($arg, 'update');
      break;
    case 'delete':
      db_query("DELETE FROM {project_issue_comments} WHERE cid = %d", $arg->cid);
      project_issue_update_by_comment($arg, 'delete');
      break;
    case 'view':
      if (isset($arg->cid)) {
        $project_issue_table = project_issue_comment_view($original_node, $arg);
      }
      else {
        $test = drupal_clone($arg);
        $test->pid = $arg->project_info['pid'];
        $test->component = $arg->project_info['component'];
        // Add a dummy rid if necessary -- prevents incorrect change data.
        $test->rid = $arg->project_info['rid'] ? $arg->project_info['rid'] : 0;
        $comment_changes = project_issue_comment_changes($node, $original_node, $test, project_issue_field_labels('web'));
        $project_issue_table = _project_issue_comment_table($comment_changes);
      }
      if ($project_issue_table) {
        $arg->comment = '<div class="project-issue"><div class="summary">'. $project_issue_table .'</div></div>' . $arg->comment;
      }
      break;
    case 'validate':
      // Adjust new file attachments to go to the issues directory.
      // We have to do this during validate, otherwise we might miss
      // adjusting the filename before comment upload saves it (module weighting)
      project_issue_change_comment_upload_path($arg);

      // Only validate metadata changes on new followups.
      if (isset($arg['cid'])) {
        return;
      }
      if ($project) {
        // Force all comments to be a child of the main issue, to match the
        // flat display, and also to prevent accidentally deleting a thread.
        form_set_value(array('#parents' => array('pid')), 0);

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

      // Now, make sure the comment changes *something* about the issue.
      // Unfortunately, hook_comment() is evil, and in the 'validate' case, we
      // get a form_values array in $arg, whereas in 'view', we get an
      // object. So, we can't really share the code to initialize this array
      // of changes, and have to do it again here using the array.
      if (empty($arg['comment'])) {
        $comment = drupal_clone((object)$arg);
        $comment->pid = $arg['project_info']['pid'];
        $comment->component = $arg['project_info']['component'];
        // Add a dummy rid if necessary -- prevents incorrect change data.
        $comment->rid = $arg['project_info']['rid'] ? $arg['project_info']['rid'] : 0;
        $comment_changes = project_issue_comment_changes($node, $original_node, $comment, project_issue_field_labels('web'));
        $has_change = FALSE;
        $files = isset($arg['files']) ? $arg['files'] : array();
        // For testing purposes, get rid of any files that were loaded
        // then marked for removal.
        foreach ($files as $number => $data) {
          if ($data['remove']) {
            unset($files[$number]);
          }
        }
        foreach ($comment_changes as $field => $changes) {
          // Comment changes something if any metadata has been changed,
          // or if files have been added.
          if (isset($changes['new']) || !empty($files)) {
            $has_change = TRUE;
            break;
          }
        }
        if (!$has_change) {
          form_set_error('comment', t('You must either add a comment or change something about this issue.'));
        }
      }
      break;
  }
}

/**
 * Create a project issue metadata table.
 *
 * @param $comment_changes
 *  Array containing metadata differences between comments
 *  as returned by project_issue_comment_changes().
 */
function _project_issue_comment_table($comment_changes) {
  $rows = array();
  foreach ($comment_changes as $field => $change) {
    if (!empty($change['label']) && isset($change['old']) && isset($change['new'])) {
      $rows[] = array(
        check_plain($change['label']) .':',
        check_plain(project_issue_change_summary($field, $change['old'])),
        '&raquo; '. check_plain(project_issue_change_summary($field, $change['new'])),
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
    $labels = project_issue_field_labels('web');
    $result = db_query('SELECT cid, title, pid, rid, component, category, priority, assigned, sid FROM {project_issue_comments} WHERE nid = %d ORDER BY timestamp ASC', $node->nid);
    while ($followup = db_fetch_object($result)) {
      $followup_changes = project_issue_comment_changes($node, $old, $followup, project_issue_field_labels('web'));
      $project_issue_tables[$followup->cid] = _project_issue_comment_table($followup_changes);
      $old = $followup;
    }
  }
}

/**
 * Calculate the differences in project_issue comment metadata
 * between the original issue and a comment or between two
 * comments.
 *
 * @param $node
 *  The issue node.
 * @param $old_data
 *  Object containing old metadata.
 * @param $new_data
 *  Object containing new metadata.
 * @param $field_labels
 *  An associative array of field_name=>display_name pairs.
 *  In most cases, this will be the array returned by project_issue_change_summary().
 *
 * @return
 *  An associative array containing information about changes between
 *  the two objects.
 *  For example:
 *  array(
 *    'component' => array(
 *      'label' => t('Component'),
 *      'old' => 'Code',
 *      'new' => 'User interface',
 *    ),
 *    'sid' => array(
 *      'label' => t('Status'),
 *      'old' => 8,
 *      'new' => 13,
 *    ),
 *  )
 */
function project_issue_comment_changes($node, $old_data, $new_data, $field_labels = array()) {
  $changes = array();
  foreach ($field_labels as $property => $name) {
    if ($property == 'rid' && empty($old_data->rid) && empty($new_data->rid)) {
      // Special case for version -- if both are empty, leave it out entirely,
      // since maybe this project doesn't have (and/or disabled) releases.
      continue;
    }
    if (isset($old_data->$property) || isset($new_data->$property)) {
      $changes[$property] = array('label' => $name);
    }
    if (isset($old_data->$property) && isset($new_data->$property)) {
      if ($old_data->$property != $new_data->$property) {
        $changes[$property]['old'] = $old_data->$property;
        $changes[$property]['new'] = $new_data->$property;
      }
    }
    elseif (isset($old_data->$property)) {
      $changes[$property]['old'] = $old_data->$property;
    }
    else {
      $changes[$property]['new'] = $new_data->$property;
    }
  }

  // Allow other modules to implement hook_followup_metadata_changes() so that they
  // can find changes in additional metadata.  In most cases other modules will
  // be responsible for storing this metadata in their own tables.  Developers
  // of modules that implement this hook should keep in mind the following:
  // 1.  Implementations of hook_followup_metadata_changes() must take the
  //     $project_issue_comment_changes array by reference.
  // 2.  Differences in properties will only be processed later on for
  //     elements of the array which have the 'label', 'old', and 'new' properties
  //     defined.
  // In other words, for each line in the differences table (or field in the email)
  // that is displayed, your hook should add something like the following as a
  // new element of the $changes:
  //    'taxonomy_vid_10' => array(
  //       'label' => 'Vocabulary 10',
  //       'old' => 'MySQL,PGSQL',
  //       'new' => 'PGSQL',
  //     ),
  foreach (module_implements('followup_metadata_changes') as $module) {
    $function = $module .'_followup_metadata_changes';
    $function($node, $changes, $old_data, $new_data);
  }
  return $changes;
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
      // Mark the node for email notification during hook_exit(), so all issue
      // and file data is in a consistent state before we generate the email.
      project_issue_set_mail_notify($comment_data->nid);
      break;
    case 'update':
      $comment_data = (object) $comment_data;
      break;
  }

  // In order to deal with deleted/unpublished comments, make sure that we're performing
  // the updates to the issue with the latest available published comment.
  $comment_data = project_issue_get_newest_comment($comment_data);

  // Update the issue data to reflect the new final states.
  db_query("UPDATE {project_issues} SET pid = %d, category = '%s', component = '%s', priority = %d, rid = %d, assigned = %d, sid = %d WHERE nid = %d", $comment_data->pid, $comment_data->category, $comment_data->component, $comment_data->priority, $comment_data->rid, $comment_data->assigned, $comment_data->sid, $comment_data->nid);

  // Update the issue title.
  $node = node_load($comment_data->nid, NULL, TRUE);  // Don't use cached since we changed data above.
  $node->title = $comment_data->title;

  // This also updates the changed date of the issue.
  node_save($node);
}

/**
 * Adjusts the filepath of issue followups so files are saved to
 * the correct issues directory.
 *
 * @param $comment
 *   An array of the submitted comment values.
 */
function project_issue_change_comment_upload_path(&$comment) {
  static $run = NULL;

  $node = node_load($comment['nid']);
  if (isset($comment['files']) && !isset($run)) {
    $run = TRUE;  // Make sure this only gets run once.
    $issue_dir = variable_get('project_directory_issues', 'issues');
    foreach ($comment['files'] as $key => $file) {
      if ($issue_dir && 0 === strpos($key, 'upload_')) {  // Only rewrite the name when adding the file, not when updating it
        // Change where the file will be saved to the specified directory.
        // Since changes to the comment aren't carried to submit, we have
        // to adjust $form_values here.
        $filename = $issue_dir .'/'. $comment['files'][$key]['filename'];
        form_set_value(array('#parents' => array('files', $key, 'filename')), $filename);
      }
    }
  }
}

/**
 * Retrieves the newest published comment for an issue.
 *
 * @param $comment_data
 *   An object representing the current comment being edited
 * @return
 *   An object representing the most recent published comment for the issue.
 */
function project_issue_get_newest_comment($comment_data) {
  // Get the cid of the most recent comment.
  $latest_cid = db_result(db_query_range('SELECT pic.cid FROM {project_issue_comments} pic INNER JOIN {comments} c ON c.cid = pic.cid WHERE c.nid = %d AND c.status = %d ORDER BY pic.timestamp DESC', $comment_data->nid, COMMENT_PUBLISHED, 0, 1));
  if ($latest_cid) {
    $comment_data = db_fetch_object(db_query('SELECT * FROM {project_issue_comments} WHERE cid = %d', $latest_cid));
  }
  // No more comments on the issue -- use the original issue metadata.
  else {
    // nid isn't stored in the original issue data, so capture it here and pass back
    // into the object.
    $nid = $comment_data->nid;
    $comment_data = unserialize(db_result(db_query("SELECT original_issue_data FROM {project_issues} WHERE nid = %d", $comment_data->nid)));
    $comment_data->nid = $nid;
  }
  return $comment_data;
}

/**
 * Test to determine if the active page is the comment reply form.
 *
 * @return
 *   TRUE if the active page is the comment reply form, FALSE otherwise.
 */
function project_issue_is_comment_reply() {
  return arg(0) == 'comment' && arg(1) == 'reply';
}

/**
 * Appends the comment thread to the comment reply form.
 */
function project_issue_comment_pre_render($form_id, &$form) {

  // Force the correct formatting.
  $_GET['mode'] = COMMENT_MODE_FLAT_EXPANDED;
  $_GET['sort'] = COMMENT_ORDER_OLDEST_FIRST;

  $suffix = empty($form['#suffix']) ? '' : $form['#suffix'];
  $node = node_load($form['nid']['#value']);

  // Unfortunately, the comment module blindly puts the node view
  // after the comment form on preview, in the case where the comment
  // parent is 0.  If we want our issue previews to be consistent, this
  // ugly hack is necessary.
  if ($_POST['op'] == t('Preview comment')) {
    $preview = comment_render($node, 0);
  }
  else {
    $preview = node_show($node, 0);
  }

  $form['#suffix'] = $suffix . $preview;
}
