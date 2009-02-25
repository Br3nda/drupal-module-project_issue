<?php
// $Id: project_issue_handler_filter_issue_status.inc,v 1.4 2009-02-25 10:31:14 dww Exp $


/**
 * Filter issues based on their status.
 *
 * Adds a special "- Open issues -" value that is used to select all of the
 * "open" issue status values as currently configured on the site.
 */
class project_issue_handler_filter_issue_status extends views_handler_filter_in_operator {
  var $open_states = array();

  /**
   * Return the values to use for this filter.
   *
   * Adds the special "- Open issues -" choice in addition to all of the
   * currently configured issue statue values.
   */
  function get_value_options() {
    module_load_include('inc', 'project_issue', 'issue');
    $this->value_options = array('Open' => t('- Open issues -')) + project_issue_state();
  }

  /**
   * Cache and return an array of status integers for "open" issue statuses.
   */
  function get_open_states() {
    if (empty($open_states)) {
      $open_states = array_keys(project_issue_state(0, FALSE, FALSE, 0, TRUE));
    }
    return $open_states;
  }

  /**
   * Accept the input to the exposed filter and map it to the right values.
   *
   * This is where the special logic for the "- Open issues -" choice is
   * converted back into the real status values for the underlying query.
   * First, we invoke the parent method to try to handle the input.  Then, we
   * see if the input includes a value for this filter.  If the value is an
   * array we know we're a multi-select filter, and we need to more carefully
   * handle the "open issues" choice (since someone might have selected that
   * and a few other individual issue statuses).
   */
  function accept_exposed_input($input) {
    $rc = parent::accept_exposed_input($input);
    if ($rc) {
      if (!empty($this->options['expose']['identifier'])) {
        $identifier = $this->options['expose']['identifier'];
        if (isset($input[$identifier])) {
          if (is_array($input[$identifier])) {
            $this->value = array();
            foreach ($input[$identifier] as $value) {
              if ($value == 'Open') {
                $this->value += $this->get_open_states();
              }
              else {
                $this->value[] = $value;
              }
            }
          }
          else {
            // Input is not an array (we're configured as "force single").
            switch ($input[$identifier]) {
              case 'All':
                // Work-around a bug in Views that doesn't handle 'All'
                // correctly if the view already has a default value.
                return FALSE;

              case 'Open':
                $this->value = $this->get_open_states();
                break;
            }
          }
        }
      }
    }
    return $rc;
  }

}

