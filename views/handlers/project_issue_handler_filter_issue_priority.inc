<?php
// $Id: project_issue_handler_filter_issue_priority.inc,v 1.1 2009/01/16 01:41:15 dww Exp $


/**
 * Filter issues based on their priority.
 */
class project_issue_handler_filter_issue_priority extends views_handler_filter_in_operator {
  function get_value_options() {
    require_once drupal_get_path('module', 'project_issue') .'/issue.inc';
    $this->value_options = project_issue_priority();
  }
}

