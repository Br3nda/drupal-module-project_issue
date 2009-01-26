<?php
// $Id: project_issue_handler_field_issue_queue_link.inc,v 1.1 2009/01/26 22:50:35 dww Exp $


/**
 * Field handler to link to the issue queue of a given project.
 */
class project_issue_handler_field_issue_queue_link extends views_handler_field {
  /**
   * Constructor to provide additional field to add.
   */
  function construct() {
    parent::construct();
    $this->additional_fields['nid'] = 'nid';
    $this->additional_fields['uri'] = array('table' => 'project_projects', 'field' => 'uri');
  }

  function render($values) {
    return l($values->{$this->field_alias}, 'project/issues/'. $values->{$this->aliases['uri']});
  }
}

