<?php
// $Id: admin.issue_status.inc,v 1.1 2009-04-04 07:03:28 dww Exp $

/**
 * @file
 * Code for the issue status admin configuration form.
 */

function theme_project_issue_admin_statuses_form($form) {
  drupal_add_tabledrag('project-issue-status-admin-table', 'order', 'self', 'project-issue-status-weight');
  $header = array(
    array('data' => t('ID')),
    array('data' => t('Name')),
    array('data' => t('Weight')),
    array('data' => t('Author may set')),
    array('data' => t('In default queries')),
    array('data' => t('Default status')),
    array('data' => t('Operations'))
  );
  foreach (element_children($form['status']) as $key) {
    $rows[] = array(
      'class' => 'draggable',
      'data' => array(
        drupal_render($form['status'][$key]['sid']),
        drupal_render($form['status'][$key]['name']),
        drupal_render($form['status'][$key]['weight']),
        drupal_render($form['status'][$key]['author_has']),
        drupal_render($form['status'][$key]['default_query']),
        drupal_render($form['default_status'][$key]),
        drupal_render($form['delete'][$key]),
      ),
    );
  }
  $rows[] = array(
    'class' => 'draggable',
    'data' => array(
      NULL,
      drupal_render($form['status_add']['name']),
      drupal_render($form['status_add']['weight']),
      drupal_render($form['status_add']['author_has']),
      drupal_render($form['status_add']['default_query']),
      NULL, NULL,
    ),
  );
  $output = '<div>' . theme('table', $header, $rows, array('id' => 'project-issue-status-admin-table')) . '</div>';
  $output .= drupal_render($form);
  return $output;
}

function project_issue_admin_statuses_form(&$form_state) {
  $result = db_query('SELECT * FROM {project_issue_status} ORDER BY weight');
  $default_status = variable_get('project_issue_default_status', 1);
  $default_statuses = project_issue_default_statuses();
  $form['status']['#tree'] = TRUE;
  while ($status = db_fetch_object($result)) {
    $options[$status->sid] = '';
    $form['status'][$status->sid]['sid'] = array(
      '#type' => 'markup',
      '#value' => $status->sid,
    );
    $form['status'][$status->sid]['name'] = array(
      '#type' => 'textfield',
      '#default_value' => $status->name,
      '#size' => 20,
      '#maxlength' => 255,
    );
    $form['status'][$status->sid]['weight'] = array(
      '#type' => 'weight',
      '#default_value' => $status->weight,
      '#delta' => 15,
      '#attributes' => array('class' => 'project-issue-status-weight'),
    );
    $form['status'][$status->sid]['author_has'] = array(
      '#type' => 'checkbox',
      '#default_value' => $status->author_has,
    );
    $form['status'][$status->sid]['default_query'] = array(
      '#type' => 'checkbox',
      '#default_value' => in_array($status->sid, $default_statuses),
    );
    $del_link = ($status->sid != $default_status) ? l(t('Delete'), 'admin/project/project-issue-status/delete/'. $status->sid) : '';
    $form['delete'][$status->sid] = array(
      '#type' => 'markup',
      '#value' => $del_link,
    );
  }
  $form['default_status'] = array(
    '#type' => 'radios',
    '#options' => $options,
    '#default_value' => $default_status,
  );
  $form['status_add']['name'] = array(
    '#type' => 'textfield',
    '#size' => 20,
    '#maxlength' => 255,
  );
  $form['status_add']['weight'] = array(
    '#type' => 'weight',
    '#default_value' => 0,
    '#delta' => 15,
    '#attributes' => array('class' => 'project-issue-status-weight'),
  );
  $form['status_add']['author_has'] = array(
    '#type' => 'checkbox',
  );
  $form['status_add']['default_query'] = array(
    '#type' => 'checkbox',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#tree'] = TRUE;
  return $form;
}

/**
 * Submit handler for project_issue_admin_statuses_form.
 */
function project_issue_admin_statuses_form_submit($form, &$form_state) {
  // Check for and apply changes or additions to project issue status options.
  if (isset($form_state['values']['default_status'])) {
    variable_set('project_issue_default_status', $form_state['values']['default_status']);
  }
  // Update existing status options.
  if($form_state['values']['status']) {
    foreach ($form_state['values']['status'] as $sid => $value) {
      $status = db_fetch_object(db_query('SELECT name, weight, author_has, default_query FROM {project_issue_status} WHERE sid = %d', $sid));
      // Check to see whether the record needs updating.
      if (($status->name != $value['name']) || ($status->weight != $value['weight']) || ($status->author_has != $value['author_has']) || ($status->default_query != $value['default_query'])) {
        db_query("UPDATE {project_issue_status} SET name = '%s', weight = %d, author_has = %d, default_query = %d WHERE sid = %d", $value['name'], $value['weight'], $value['author_has'], $value['default_query'], $sid);
      }
    }
  }
  // Add any new status options.
  if (isset($form_state['values']['status_add']) && !empty($form_state['values']['status_add']['name'])) {
    // Check to see whether the status already exists:
    $issue_status = db_result(db_query("SELECT COUNT(*) FROM {project_issue_status} WHERE name = '%s'", $form_state['values']['status_add']['name']));
    if (empty($issue_status)) {
      db_query("INSERT INTO {project_issue_status} (name, weight, author_has, default_query) VALUES ('%s', %d, %d, %d)", $form_state['values']['status_add']['name'], $form_state['values']['status_add']['weight'], $form_state['values']['status_add']['author_has'], $form_state['values']['status_add']['default_query']);
    }
    else {
      drupal_set_message(t('Status %status already exists.', array ('%status' => $form_state['values']['status_add']['name'])), 'error');
    }
  }
}

function project_issue_delete_status_confirm(&$form_state, $sid) {
  $statuses = project_issue_status();
  $name = $statuses[$sid];

  $total = db_result(db_query('SELECT COUNT(nid) AS total FROM {project_issues} WHERE sid = %d', $sid));
  if ($total > 0) {
    $form['new_sid'] = array(
      '#type' => 'select',
      '#title' => t('Reassign status'),
      '#default_value' => $sid,
      '#options' => $statuses,
      '#description' => t('There are !total existing issues with the status of @name. Please select a new status for these issues.', array('!total' => $total, '@name' => $name)),
    );
  }
  $form['sid'] = array(
    '#type' => 'value',
    '#value' => $sid,
  );
  $form['name'] = array(
    '#type' => 'hidden',
    '#value' => $name,
  );
  return confirm_form(
    $form,
    t('Are you sure you want to delete the status option %name?', array('%name' => $name)),
    'admin/project/project-issue-status',
    t('This action cannot be undone.'),
    t('Delete'), t('Cancel')
  );
}

function project_issue_delete_status_confirm_validate($form, &$form_state) {
  if ($form_state['values']['new_sid'] == $form_state['values']['sid']) {
    form_set_error('new_sid', t('Choose a new issue status for existing issues of status %name.', array('%name' => $form_state['values']['name'])));
  }
}

function project_issue_delete_status_confirm_submit($form, &$form_state) {
  if ($form_state['values']['new_sid']) {
    db_query('UPDATE {project_issues} SET sid = %d WHERE sid = %d', $form_state['values']['new_sid'], $form_state['values']['sid']);
  }
  db_query('DELETE FROM {project_issue_status} WHERE sid = %d', $form_state['values']['sid']);
  drupal_set_message(t('Project issue status %issue deleted.', array('%issue' => $form_state['values']['name'])));
  $form_state['redirect'] ='admin/project/project-issue-status';
}

