<?php
// $Id: project_issue_handler_field_issue_category.inc,v 1.1 2009-01-16 01:41:15 dww Exp $


/**
 * Field handler to display the current category of an issue.
 */
class project_issue_handler_field_issue_category extends views_handler_field {
  function render($values) {
    require_once drupal_get_path('module', 'project_issue') .'/issue.inc';
    return check_plain(project_issue_category($values->{$this->field_alias}));
  }
}

