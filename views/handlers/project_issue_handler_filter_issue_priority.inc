<?php
// $Id: project_issue_handler_filter_issue_priority.inc,v 1.3 2010-04-24 01:58:18 dww Exp $


/**
 * Filter issues based on their priority.
 */
class project_issue_handler_filter_issue_priority extends views_handler_filter_in_operator {
  function get_value_options() {
    module_load_include('inc', 'project_issue', 'issue');
    $this->value_options = project_issue_priorities();
  }
}

