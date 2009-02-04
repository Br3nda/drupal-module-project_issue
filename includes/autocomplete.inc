<?php
// $Id: autocomplete.inc,v 1.1 2009-02-04 01:46:23 dww Exp $


/**
 * @file
 * Autocomplete callback functions for the Project issue tracking module.
 *
 * Each function returns a JSON string for use with JS autocomplete fields.
 */

/**
 * Return valid issue-enabled project names for a single value.
 */
function project_issue_autocomplete_issue_project($string = '') {
  $matches = array();
  $result = db_query_range(db_rewrite_sql("SELECT n.title FROM {node} n INNER JOIN {project_issue_projects} p ON n.nid = p.nid WHERE n.status = %d AND LOWER(n.title) LIKE LOWER('%%%s%%') AND p.issues = %d"), 1, $string, 1, 0, 10);
  while ($project = db_fetch_object($result)) {
    $matches[$project->title] = check_plain($project->title);
  }
  drupal_json($matches);
}

/**
 * Return valid issue-enabled project names for comma-separated input.
 *
 * @TODO: deal with comma-separated input.
 */
function project_issue_autocomplete_issue_projects($string = '') {
  $matches = array();
  $result = db_query_range(db_rewrite_sql("SELECT n.title FROM {node} n INNER JOIN {project_issue_projects} p ON n.nid = p.nid WHERE n.status = %d AND LOWER(n.title) LIKE LOWER('%%%s%%') AND p.issues = %d"), 1, $string, 1, 0, 10);
  while ($project = db_fetch_object($result)) {
    $matches[$project->title] = check_plain($project->title);
  }
  drupal_json($matches);
}


/**
 * Return valid issue-enabled project names based on a user's own projects.
 *
 * Only returns matches for project titles from issues the user has either
 * submitted or commented on.
 *
 * @TODO: deal with comma-separated input.
 */
function project_issue_autocomplete_user_issue_project($uid, $string = '') {
  $matches = array();
  $result = db_query_range(db_rewrite_sql("SELECT n.title FROM {node} n INNER JOIN {project_issue_projects} pip ON n.nid = pip.nid INNER JOIN {project_issues} pi ON n.nid = pi.pid INNER JOIN {node} pin ON pi.nid = pin.nid LEFT JOIN {comments} c ON c.nid = pi.nid WHERE pip.issues = %d AND n.status = %d AND pin.status = %d AND (pin.uid = %d OR c.uid = %d) AND LOWER(n.title) LIKE LOWER('%%%s%%')"), 1, 1, 1, $uid, $uid, $string, 0, 10);
  while ($project = db_fetch_object($result)) {
    $matches[$project->title] = check_plain($project->title);
  }
  drupal_json($matches);
}

