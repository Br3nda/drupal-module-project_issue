<?php
// $Id: admin.batch_confirm.inc,v 1.2 2010/07/19 19:00:34 thehunmonkgroup Exp $

/**
 * @file
 * Batch operations for updating denormalized issue priority weight and status.
 */

/**
 * Run a SQL query in batch for each row in the {project_issues} table.
 *
 * This function will find the range of all rows in the {project_issues} table
 * and run a query against that table incerementally, 500 rows at a time. This
 * function is intended to be used with batch_set().
 *
 * @param $sql
 *   A SQL query intended to be run for each row in the {project_issues} table.
 *   Usually an update query, it is recommended to include 'BETWEEN %d AND %d'
 *   to prevent timeouts.
 * @param $base_arguments
 *   An array of base arguments to pass to the query. The first and last nid of
 *   the current range are appended to the end of this query, for specifying a
 *   range with BETWEEN.
 * @param $context
 *   An array containing information about batch progress. $context['sandbox']
 *   contains the following keys which will be populated after this function
 *   has been called:
 *     'max' The maximum value, or end point.
 *     'current' The nid of the last row that has been updated.
 */
function _project_issue_batch_update($sql, $base_arguments, &$context = array()) {
  if (!isset($context['sandbox']['current'])) {
    $context['sandbox']['max'] = db_result(db_query('SELECT MAX(nid) FROM {project_issues}'));
    $context['sandbox']['current'] = 0;
  }

  $limit = 500;

  // Add range of rows to process to query arguments.
  $arguments = $base_arguments;
  $last_nid = $context['sandbox']['current'] + $limit;
  $arguments[] = $context['sandbox']['current'];
  $arguments[] = $last_nid;

  db_query($sql, $arguments);

  // Check to see if finished, report progress and update $context['sandbox']['current'].
  if ($last_nid < $context['sandbox']['max']) {
    $context['finished'] = $last_nid / $context['sandbox']['max'];
    $context['sandbox']['current'] = $last_nid;
  }
  else {
    $context['finished'] = 1;
  }
}
