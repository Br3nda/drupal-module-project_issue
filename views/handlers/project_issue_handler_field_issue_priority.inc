<?php
// $Id: project_issue_handler_field_issue_priority.inc,v 1.3 2010-04-24 01:58:18 dww Exp $


/**
 * Field handler to display the current priority of an issue.
 */
class project_issue_handler_field_issue_priority extends views_handler_field {
  function render($values) {
    module_load_include('inc', 'project_issue', 'issue');
    return check_plain(project_issue_priority($values->{$this->field_alias}));
  }

  function click_sort($order) {
    // Reverse the sort order since more important priorities are weighted
    // lighter.
    $order = ($order == 'asc') ? 'desc' : 'asc';
    // Override the sort field, since we are storing denormalized weight.
    $this->query->add_orderby($this->table, 'priority_weight', $order);
  }
}
