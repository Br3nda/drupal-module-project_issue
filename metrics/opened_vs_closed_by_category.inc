<?php

// $Id: opened_vs_closed_by_category.inc,v 1.1 2010/09/17 17:47:46 thehunmonkgroup Exp $

/**
 * @file
 * Metric plugin for counting the number of total isses that are open versus
 * the total number of issues that are closed.  This provides both overall
 * totals, and totals per category.
 *
 * Note that this metric makes no use of the timestamps -- it simply provides
 * totals at the time the sample is being taken.
 *
 * This metric accepts the following options:
 * - object_ids:
 *     An array of nids. If passed it will restrict the results to nodes in
 *     this list.
 */

$plugin = array(
  'title' => t('Total open/closed issues'),
  'description' => t("Counts the total number of open and closed issues per category for all projects."),
  'data_type' => array(
    'bug_open' => 'int',
    'feature_open' => 'int',
    'task_open' => 'int',
    'support_open' => 'int',
    'bug_closed' => 'int',
    'feature_closed' => 'int',
    'task_closed' => 'int',
    'support_closed' => 'int',
    'total_open' => 'int',
    'total_closed' => 'int',
  ),
  'handler' => array(
    'class' => 'ProjectIssueOpenedVsClosedByCategory',
  ),
);
