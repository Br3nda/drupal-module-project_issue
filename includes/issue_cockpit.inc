<?php
// $Id: issue_cockpit.inc,v 1.3 2009/04/13 09:20:49 dww Exp $

/**
 * Form builder for the search box in the issue cockpit block.
 */
function project_issue_issue_cockpit_searchbox(&$form_state, $node) {
  $search_path = 'project/issues/' . $node->project['uri'];
  $adv_search_path = 'project/issues/search/' . $node->project['uri'];
  return array(
    '#action' => url($search_path),
    '#method' => 'get',
    'text' => array(
      '#type' => 'textfield',
      '#size' => 20,
    ),
    'status' => array(
      '#type' => 'hidden',
      '#value' => 'All',
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('search'),
      '#name' => '',
    ),
    'advanced' => array(
      '#value' => l(t('advanced search'), $adv_search_path),
    ),
  );
}

/**
 * Theme template preprocess to declare variables for the issue cockpit.
 */
function template_preprocess_project_issue_issue_cockpit(&$vars) {
  $node = $vars['node'];

  // Explicitly set the template file since it's not in the same directory.
  $vars['template_files'][] = 'theme/project-issue-issue-cockpit';

  // Flags that indicate what kind of access to project issues to allow.
  $has_issues = !empty($node->project_issue['issues']);
  $vars['view_issues'] = $has_issues && (user_access('access project issues') || user_access('access own project issues') || user_access('administer projects'));
  $vars['make_issues'] = $has_issues && node_access('create', 'project_issue');

  if ($vars['view_issues']) {
    $open_states = '('. implode(',', array_keys(project_issue_state(0, FALSE, FALSE, 0, TRUE))) .')';

    $vars['open'] = db_result(db_query(db_rewrite_sql('SELECT count(*) FROM {node} n INNER JOIN {project_issues} i ON n.nid = i.nid WHERE n.status = 1 AND i.pid = %d AND i.sid IN '. $open_states), $node->nid));
    $vars['total'] = db_result(db_query(db_rewrite_sql('SELECT count(*) FROM {node} n INNER JOIN {project_issues} i ON n.nid = i.nid WHERE n.status = 1 AND i.pid = %d'), $node->nid));

    $vars['path'] = 'project/issues/'. $node->project['uri'];
    $vars['form'] = drupal_get_form('project_issue_issue_cockpit_searchbox', $node);

    $vars['categories'] = array();
    foreach (project_issue_category(0, 1) as $category => $name) {
      $vars['categories'][$category] = array('name' => $name, 'open' => 0, 'total' => 0);
    }

    $result = db_query(db_rewrite_sql('SELECT count(*) AS open, i.category FROM {node} n INNER JOIN {project_issues} i ON n.nid = i.nid WHERE n.status = 1 AND i.pid = %d AND i.sid IN '. $open_states .' GROUP BY i.category'), $node->nid);
    while ($category = db_fetch_object($result)) {
      if (isset($vars['categories'][$category->category])) {
        $vars['categories'][$category->category]['open'] = $category->open;
      }
    }

    $result = db_query(db_rewrite_sql('SELECT count(*) AS total, i.category FROM {node} n INNER JOIN {project_issues} i ON n.nid = i.nid WHERE n.status = 1 AND i.pid = %d GROUP BY i.category'), $node->nid);
    while ($category = db_fetch_object($result)) {
      if (isset($vars['categories'][$category->category])) {
        $vars['categories'][$category->category]['total'] = $category->total;
      }
    }

    $vars['issue_subscribe'] = l(t('Subcribe via e-mail'), 'project/issues/subscribe-mail/'. $node->project['uri'], array('query' => drupal_get_destination()));
    $vars['issue_statistics'] = l(t('Issue statistics'), 'project/issues/statistics/'. $node->project['uri']);
    $vars['oldest'] = db_result(db_query(db_rewrite_sql('SELECT min(n.created) FROM {node} n INNER JOIN {project_issues} i ON n.nid = i.nid WHERE n.status = 1 AND i.pid = %d AND i.sid IN '. $open_states), $node->nid));
  }
}

