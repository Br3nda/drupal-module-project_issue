<?php
// $Id: admin.issue_priority.inc,v 1.1 2010/04/24 01:58:18 dww Exp $

/**
 * @file
 * Code for the issue priorities admin configuration form.
 */

/**
 * Build the form for project_issue_admin_priority_form.
 */
function project_issue_admin_priority_form(&$form_state) {
  $result = db_query('SELECT priority, name, weight FROM {project_issue_priority} ORDER BY weight');
  $max = 15;
  $delta = 0;
  while ($priority = db_fetch_object($result)) {
    // We need to ensure the weight options include the maximum existing
    // weight.
    $max = max($delta, $priority->weight);
    $form['priority'][$priority->priority]['name'] = array(
      '#type' => 'textfield',
      '#default_value' => $priority->name,
      '#size' => 20,
      '#maxlength' => 255,
    );
    $form['priority'][$priority->priority]['weight'] = array(
      '#type' => 'weight',
      '#default_value' => $priority->weight,
      '#delta' => &$delta,
      '#attributes' => array('class' => 'project-issue-priority-weight'),
    );
    $form['delete'][$priority->priority]['#value'] = l(t('Delete'), 'admin/project/project-issue-priority/delete/'. $priority->priority);
  }
  $delta = $max + 1;
  $form['priority'][0]['name'] = array(
    '#type' => 'textfield',
    '#default_value' => '',
    '#size' => 20,
    '#maxlength' => 255,
  );
  $form['priority'][0]['weight'] = array(
    '#type' => 'weight',
    // Make sure that the new item has a weight higher than highest priority
    // since new item appears at bottom of the form by default.
    '#default_value' => $max + 1, 
    '#delta' => $max + 1,
    '#attributes' => array('class' => 'project-issue-priority-weight'),
  );
  $form['delete'][0]['#value'] = '';

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#tree'] = TRUE;
  return $form;
}

/**
 * Render the HTML for the admin issue priority form.
 *
 * @see project_issue_admin_priority_form()
 * @see drupal_add_tabledrag()
 */
function theme_project_issue_admin_priority_form($form) {
  drupal_add_tabledrag('project-issue-admin-priority-table', 'order', 'self', 'project-issue-priority-weight');
  $header = array(
    t('Priority'),
    t('Weight'),
    t('Operations'),
  );
  foreach (element_children($form['priority']) as $key) {
    $rows[] = array(
      'class' => 'draggable',
      'data' => array(
        drupal_render($form['priority'][$key]['name']),
        drupal_render($form['priority'][$key]['weight']),
        drupal_render($form['delete'][$key]),
      ),
    );
  }
  $output = '<div>' . theme('table', $header, $rows, array('id' => 'project-issue-admin-priority-table')) . '</div>';
  $output .= drupal_render($form);
  return $output;
}

/**
 * Submit handler for project_issue_admin_states_form.
 */
function project_issue_admin_priority_form_submit($form, &$form_state) {
  $offset = 0;
  foreach ($form_state['values']['priority'] as $priority_id => $value) {
    $offset = min($offset, $value['weight']);
  }
  // $offset at this point contains the smallest weight. If that's below zero,
  // then adding it to every weight will result in unsigned weights. Otherwise
  // such addition is not necessary.
  $offset = $offset < 0 ? -$offset : 0;
  // Update existing priorities.
  $update_expression = '';
  $weights = array();
  foreach ($form_state['values']['priority'] as $priority_id => $value) {
    $weight = $value['weight'] + $offset;
    $form_state['values']['priority'][$priority_id]['weight'] = $weight;
    // Update existing.
    if ($priority_id) {
      $priority = db_fetch_object(db_query('SELECT priority, name, weight FROM {project_issue_priority} WHERE priority = %d', $priority_id));
      $update = $priority->name !== $value['name'];
      // Check to see whether the record needs updating. We love PHP and '0'.
      if ((string)$priority->weight !== (string)$weight) {
        $update_expression .= " WHEN %d THEN %d";
        $weights[] = $priority->priority;
        $weights[] = $weight;
        $update = TRUE;
      }
      if ($update) {
        db_query("UPDATE {project_issue_priority} SET name = '%s', weight = %d WHERE priority = %d", $value['name'], $weight, $priority_id);
      }
    }
    // Add new priority.
    elseif ($value['name']) {
      // Check to see whether the state already exists:
      $issue_state = db_result(db_query("SELECT COUNT(*) FROM {project_issue_priority} WHERE name = '%s'", $value['name']));
      if (empty($issue_state)) {
        db_query("INSERT INTO {project_issue_priority} (name, weight) VALUES ('%s', %d)", $value['name'], $weight);
      }
      else {
        drupal_set_message(t('Priority %priority already exists.', array ('%priority' => $value['name'])), 'error');
      }
    }
  }
  if ($weights) {
    $weight_sql = "UPDATE {project_issues} SET priority_weight = CASE priority $update_expression ELSE priority_weight END WHERE nid BETWEEN %d AND %d";
    $batch = array(
      'operations' => array(
        array('_project_issue_batch_update', array($weight_sql, $weights)),
      ),
      'title' => t('Processing'),
      // We use a single multi-pass operation, so the default
      // 'Remaining x of y operations' message will be confusing here.
      'progress_message' => '',
      'error_message' => t('The update has encountered an error.'),
      // The operations do not live in the .module file, so we need to
      // tell the batch engine which file to load before calling them.
      'file' => drupal_get_path('module', 'project_issue') .'/includes/admin.batch_confirm.inc',
    );
    batch_set($batch);
  }
}

/**
 * Build a confirmation form when deleting an issue priority.
 *
 * This allows the admin to re-prioritize any issues with the priority being
 * deleted using admin.batch_confirm.inc.
 *
 * @param $form_state
 *   The state of the form we're trying to build.
 * @param $priority_id
 *   The {project_issue_priority}.priority ID being deleted.
 */
function project_issue_delete_priority_confirm(&$form_state, $priority_id) {
  // Helper functions are in issue.inc.
  require_once drupal_get_path('module', 'project_issue') .'/issue.inc';
  $schema = drupal_get_schema('project_issues');
  if (!function_exists('project_issue_priority') || !isset($schema['fields']['priority']) || !db_table_exists('project_issue_priority')) {
    return drupal_access_denied();
  }
  $priorities = project_issue_priorities();
  $name = $priorities[$priority_id];
  unset($priorities[$priority_id]);

  // $column is verified to exist.
  $total = db_result(db_query("SELECT COUNT(nid) AS total FROM {project_issues} WHERE priority = %d", $priority_id));
  if ($total > 0) {
    $form['new_pid'] = array(
      '#type' => 'select',
      '#title' => t('Reassign priority'),
      '#default_value' => $priority_id,
      '#options' => $priorities,
      '#description' => format_plural($total, 'There is 1 existing issue assigned @name priority. Please select a new priority for this issue.', 'There are @count existing issues assigned @name priority. Please select a new priority for these issues.', array('@name' => $name)),
    );
  }
  $form['pid'] = array(
    '#type' => 'value',
    '#value' => $priority_id,
  );
  $form['name'] = array(
    '#type' => 'hidden',
    '#value' => $name,
  );
  return confirm_form(
    $form,
    t('Are you sure you want to delete the priority %name?', array('%name' => $name)),
    'admin/project/project-issue-priority',
    t('This action cannot be undone.'),
    t('Delete'), t('Cancel')
  );
}

/**
 * Submit handler for confirm form when deleting an issue priority.
 */
function project_issue_delete_priority_confirm_submit($form, &$form_state) {
  db_query('DELETE FROM {project_issue_priority} WHERE priority = %d', $form_state['values']['pid']);
  $form_state['redirect'] = 'admin/project/project-issue-priority';
  drupal_set_message(t('Project issue priority %name deleted.', array('%name' => $form_state['values']['name'])));
  if (isset($form_state['values']['new_pid'])) {
    $priority_weight = db_result(db_query('SELECT weight FROM {project_issue_priority} WHERE priority = %d', $form_state['values']['new_pid']));
    $arguments = array($form_state['values']['new_pid'], $priority_weight, $form_state['values']['pid']);
    $update_sql = "UPDATE {project_issues} SET priority = %d, priority_weight = %d WHERE priority = %d AND nid BETWEEN %d AND %d";
    $batch = array(
      'operations' => array(
        array('_project_issue_batch_update', array($update_sql, $arguments)),
      ),
      'title' => t('Processing'),
      // We use a single multi-pass operation, so the default
      // 'Remaining x of y operations' message will be confusing here.
      'progress_message' => '',
      'error_message' => t('The update has encountered an error.'),
      // The operations do not live in the .module file, so we need to
      // tell the batch engine which file to load before calling them.
      'file' => drupal_get_path('module', 'project_issue') .'/includes/admin.batch_confirm.inc',
    );
    batch_set($batch);
  }
}
