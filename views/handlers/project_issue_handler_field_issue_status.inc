<?php
// $Id: project_issue_handler_field_issue_status.inc,v 1.2 2009/01/20 18:38:04 dww Exp $


/**
 * Field handler to display the current status of an issue.
 */
class project_issue_handler_field_issue_status extends views_handler_field {
  function render($values) {
    module_load_include('inc', 'project_issue', 'issue');
    return check_plain(project_issue_state($values->{$this->field_alias}));
  }
}

