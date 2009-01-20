<?php
// $Id: project_issue_handler_filter_issue_status.inc,v 1.2 2009-01-20 18:38:04 dww Exp $


/**
 * Filter issues based on their status.
 */
class project_issue_handler_filter_issue_status extends views_handler_filter_in_operator {
  function get_value_options() {
    module_load_include('inc', 'project_issue', 'issue');
    $this->value_options = project_issue_state();
  }
}

