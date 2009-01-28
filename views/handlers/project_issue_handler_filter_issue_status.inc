<?php
// $Id: project_issue_handler_filter_issue_status.inc,v 1.3 2009-01-28 21:59:12 dww Exp $


/**
 * Filter issues based on their status.
 */
class project_issue_handler_filter_issue_status extends views_handler_filter_in_operator {
  var $is_exposed = FALSE;
  var $default_states = array();

  function get_default_states() {
    if (empty($default_states)) {
      $default_states = project_issue_state(0, FALSE, FALSE, 0, TRUE);
    }
    return $default_states;
  }

  function get_value_options() {
    module_load_include('inc', 'project_issue', 'issue');
    $open_issues = array();
    if (empty($this->is_exposed) || empty($this->options['exposed']) || !empty($this->options['expose']['single'])) {
      $open_issues = array('open' => t('- Open issues -'));
    }
    $this->value_options = $open_issues + project_issue_state();
  }

  function value_form(&$form, &$form_state) {
    // Set a flag so that get_value_options() knows we're building a form for
    // the actual exposed filter for a view, not just configuring it.
    if (!empty($form_state['exposed'])) {
      $this->is_exposed = TRUE;
    }
    // Let our parent do most of its magic.
    parent::value_form($form, $form_state);

    // If this is the form for an exposed filter with multiple values, we need
    // to see if we're trying to use 'open' issues, and if so, translate into
    // the real default states to avoid FAPI validation errors.
    if ($this->is_exposed && empty($this->options['expose']['single'])) {
      $identifier = $this->options['expose']['identifier'];
      if (!empty($form_state['input'][$identifier]['open'])) {
        $default_value = array_keys($this->get_default_states());
        unset($form['value']['#default_value']);
        $form['value']['#default_value'] = $default_value;
        $form_state['input'][$identifier] = $default_value;
      }
    }
  }

  function accept_exposed_input($input) {
    $rc = parent::accept_exposed_input($input);
    if ($rc) {
      if (!empty($this->options['expose']['identifier'])) {
        $identifier = $this->options['expose']['identifier'];
        if (isset($input[$identifier])) {
          if ($input[$identifier] == 'open') {
            $this->value = array_keys($this->get_default_states());
          }
          // Work-around a bug in Views that doesn't handle 'All' correctly if
          // the view already has a default value.
          elseif ($input[$identifier] == 'All') {
            $rc = FALSE;
          }
        }
      }
    }
    return $rc;
  }

}

