<?php
// $Id: admin.batch_confirm.inc,v 1.1 2010-04-24 01:58:18 dww Exp $

/**
 * @file
 * Batch operations for updating denormalized issue priority weight and status.
 */

/**
 * Run a SQL query in batch for each row in the {project_issues} table.
 *
 * This function will find the range of all rows in the {project_issues} table
 * and run a query against that table incerementally, 100 rows at a time. This
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
 *     'min' The minimum value, or starting point.
 *     'max' The maximum value, or end point.
 *     'current' The nid of the last row that has been updated.
 *     'finished' Will contain a percentage of total rows processed, and will
 *        be set to 1 when all rows have been processed.
 */
function _project_issue_batch_update($sql, $base_arguments, &$context = array()) {
  $last_nid = 0;
  $first_nid = 0;
  if (!isset($context['sandbox']['min'])) {
    // Avoid COUNT(*) like hell.
    $context['sandbox']['max'] = db_result(db_query('SELECT MAX(nid) FROM {project_issues}'));
    // We will use > so use - 1 when choosing the smallest nid.
    $context['sandbox']['min'] = db_result(db_query('SELECT MIN(nid) - 1 FROM {project_issues}'));
    $context['sandbox']['current'] = $context['sandbox']['min'];
  }
  $arguments = $base_arguments;
  // MySQL does not support LIMIT & IN/ALL/ANY/SOME subquery so we do the hard
  // work ourselves: find 100 nids and record the first and the last.
  $results = db_query_range('SELECT nid FROM {project_issues} WHERE nid > %d ORDER BY nid ASC', $context['sandbox']['current'], 0, 100);
  while ($node = db_fetch_object($results)) {
    if (!isset($first_nid)) {
      $first_nid = $node->nid;
    }
    $last_nid = $node->nid;
  }
  $arguments[] = $first_nid;
  $arguments[] = $last_nid;
  db_query($sql, $arguments);
  // Note that we do not count exactly as there can be holes. That's still
  // better than running COUNT() on large datasets.
  if ($last_nid < $context['sandbox']['max']) {
    $context['finished'] = ($last_nid - $context['sandbox']['min']) / ($context['sandbox']['max'] - $context['sandbox']['min']);
    $context['sandbox']['current'] = $last_nid;
  }
  else {
    $context['finished'] = 1;
  }
}
