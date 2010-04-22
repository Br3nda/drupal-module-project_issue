<?php
// $Id: project_issue_handler_filter_issue_status.inc,v 1.5 2009-04-09 18:32:58 dww Exp $


/**
 * Filter issues based on their status.
 *
 * Adds a special "- Open issues -" value that is used to select all of the
 * "open" issue status values as currently configured on the site.
 */
class project_issue_handler_filter_issue_status extends views_handler_filter_in_operator {
  var $open_statuses = array();

  /**
   * Return the values to use for this filter.
   *
   * Adds the special "- Open issues -" choice in addition to all of the
   * currently configured issue statue values.
   */
  function get_value_options() {
    module_load_include('inc', 'project_issue', 'issue');
    $this->value_options = array('Open' => t('- Open issues -')) + project_issue_status();
  }

  /**
   * Cache and return an array of status integers for "open" issue statuses.
   */
  function get_open_statuses() {
    if (empty($open_statuses)) {
      $open_statuses = array_keys(project_issue_status(0, FALSE, FALSE, 0, TRUE));
    }
    return $open_statuses;
  }

  /**
   * Generate the query based on this filter.
   *
   * This is where the special logic for the "- Open issues -" choice is
   * converted back into the real status values for the underlying query.
   */
  function query() {
    $values = array();
    foreach ($this->value as $key => $value) {
      if ($value == 'Open') {
        $values += $this->get_open_statuses();
      }
      else {
        $values[$key] = $value;
      }
    }
    $this->value = $values;
    parent::query();
  }
}

