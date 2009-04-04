<?php
// $Id: project_edit_issues.inc,v 1.1 2009-04-04 06:09:28 dww Exp $

/**
 * @file
 * Code for the "issues" subtab to the edit tab on project nodes.
 */

/**
 * Page callback for the "issues" subtab to the edit tab on project nodes.
 */
function project_issue_project_edit_issues($node) {
  project_project_set_breadcrumb($node);
  drupal_set_title(check_plain($node->title));

  if (isset($node->project_issue['components']) && is_array($node->project_issue['components'])) {
    $node->project_issue['components'] = implode("\n", $node->project_issue['components']);
  }
  return drupal_get_form('project_issue_project_edit_form', $node);
}

/**
 * Form builder for the issues subtab on the edit tab for project nodes.
 */
function project_issue_project_edit_form(&$form_state, $node) {
  /* Issue properties */
  $form['issue'] = array(
    '#type' => 'fieldset',
    '#title' => t('Issue information'),
    '#collapsible' => TRUE,
  );
  $form['issue']['issue'] = array(
    '#type' => 'item',
    '#title' => t('Issue tracker'),
  );
  $form['issue']['issues'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable issue tracker'),
    '#return_value' => 1,
    '#default_value' => isset($node->project_issue['issues']) ? $node->project_issue['issues'] : 1,
    '#description' => t('Let users submit bug requests, patches, feature requests, support requests, etc.'),
  );
  $form['issue']['components'] = array(
    '#type' => 'textarea',
    '#title' => t('Components'),
    '#default_value' => isset($node->project_issue['components']) ? $node->project_issue['components'] : NULL,
    '#cols' => 20,
    '#rows' => 5,
  );
  $form['issue']['help'] = array(
    '#type' => 'textarea',
    '#title' => t('Submission guidelines'),
    '#default_value' => isset($node->project_issue['help']) ? $node->project_issue['help'] : NULL,
    '#cols' => 20,
    '#rows' => 5,
  );

  /* E-mail options */
  $form['email'] = array(
    '#type' => 'fieldset',
    '#title' => t('Issue e-mail options'),
    '#collapsible' => TRUE,
  );
  $form['email']['mail_digest'] = array(
    '#type' => 'textfield',
    '#title' => t('Weekly critical issues report'),
    '#default_value' => isset($node->project_issue['mail_digest']) ? $node->project_issue['mail_digest'] : NULL,
    '#size' => 20,
    '#maxlength' => 255,
    '#description' => t('To get a weekly digest of critical issues specify an e-mail address.'),
  );
  $form['email']['mail_copy'] = array(
    '#type' => 'textfield',
    '#title' => t('Issues e-mail address'),
    '#default_value' => isset($node->project_issue['mail_copy']) ? $node->project_issue['mail_copy'] : NULL,
    '#size' => 20,
    '#maxlength' => 255,
    '#description' => t('If you wish to receive a copy of all the issues to a central location specify an address here. <em>Note: the copy will contain links to file attachments.</em>'),
  );
  $options = project_issue_category();
  $form['email']['categories'] = array(
    '#type' => 'item',
    '#title' => t('Categories'),
    '#description' => t('Which issue categories to e-mail. If none is checked all categories will be posted.'),
  );
  $form['email']['mail_copy_filter']['#tree'] = TRUE;

  foreach ($options as $key => $choice) {
    $form['email']['mail_copy_filter'][$key] = array(
      '#type' => 'checkbox',
      '#title' => $choice,
      '#return_value' => $key,
      '#default_value' => isset($node->project_issue['mail_copy_filter'][$key]) ? $node->project_issue['mail_copy_filter'][$key] : NULL,
    );
  }
  $options = project_issue_state();
  $form['email']['states'] = array(
    '#type' => 'item',
    '#title' => t('States'),
    '#description' => t('Which issue states to e-mail. If none is checked all states will be posted.'),
  );
  $form['email']['mail_copy_filter_state']['#tree'] = TRUE;
  foreach ($options as $key => $choice) {
    $form['email']['mail_copy_filter_state'][$key] = array(
      '#type' => 'checkbox',
      '#title' => check_plain($choice),
      '#return_value' => $key,
      '#default_value' => isset($node->project_issue['mail_copy_filter_state'][$key]) ? $node->project_issue['mail_copy_filter_state'][$key] : NULL,
    );
  }

  if (user_access('administer projects')) {
    $form['email']['reminder'] = array(
      '#type' => 'item',
      '#title' => t('Monthly reminder'),
    );
    $form['email']['mail_reminder'] = array(
      '#type' => 'checkbox',
      '#title' => t('Send monthly reminders to users'),
      '#return_value' => 1,
      '#default_value' => isset($node->project_issue['mail_reminder']) ? $node->project_issue['mail_reminder'] : NULL,
      '#description' => t('Enabling this will send a monthly reminder to users that have open issues registered.'),
    );
  }

  $form['nid'] = array('#type' => 'value', '#value' => $node->nid);
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 45,
  );

  return $form;
}

/**
 * Validation handler when project admins use the issues subtab.
 * @see project_issue_project_edit_issues
 */
function project_issue_project_edit_form_validate($form, &$form_state) {
  if (!empty($form_state['values']['mail_digest']) && ($data = user_validate_mail($form_state['values']['mail_digest']))) {
    form_set_error('mail_digest', $data);
  }
  if (!empty($form_state['values']['mail_copy']) && ($data = user_validate_mail($form_state['values']['mail_copy']))) {
    form_set_error('mail_copy', $data);
  }
}

/**
 * Submit handler when project admins use the issues subtab.
 * @see project_issue_project_edit_issues
 */
function project_issue_project_edit_form_submit($form, &$form_state) {
  $components = explode("\n", str_replace("\r", '', $form_state['values']['components']));
  array_walk($components, 'project_issue_trim');
  $components = serialize(array_values(array_filter($components)));
  $mail_copy_filter = serialize($form_state['values']['mail_copy_filter']);
  $mail_copy_filter_state = serialize($form_state['values']['mail_copy_filter_state']);

  db_query("UPDATE {project_issue_projects} SET issues = %d, components = '%s', mail_digest = '%s', mail_reminder = %d, mail_copy = '%s', mail_copy_filter = '%s', mail_copy_filter_state = '%s', help = '%s' WHERE nid = %d", $form_state['values']['issues'], $components, $form_state['values']['mail_digest'], $form_state['values']['mail_reminder'], $form_state['values']['mail_copy'], $mail_copy_filter, $mail_copy_filter_state, $form_state['values']['help'], $form_state['values']['nid']);
  db_query("UPDATE {node} SET changed = %d WHERE nid = %d", time(), $form_state['values']['nid']);
  drupal_set_message(t('Issue settings have been saved.'));
}

