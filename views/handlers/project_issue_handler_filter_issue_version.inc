<?php
// $Id: project_issue_handler_filter_issue_version.inc,v 1.2 2009-01-28 05:32:14 dww Exp $


/**
 * Filter issues based on their version.
 *
 * This filter only returns values to select if it's exposed and the view has
 * a project argument (either nid or uri), and that argument is first.
 *
 * This is almost a copy of project_issue_handler_filter_issue_component.
 */
class project_issue_handler_filter_issue_version extends views_handler_filter_in_operator {
  var $is_exposed = FALSE;

  function value_form(&$form, &$form_state) {
    if (!empty($form_state['exposed'])) {
      // Set a flag so get_value_options() knows to return real values.
      $this->is_exposed = TRUE;
      // Due to some unknown bug in views, it appears as though this filter
      // always acts as if it's a "force single" filter, even if it's not.
      // Therefore, it's (sometimes?) setting its own value to to 'All', when
      // GET is empty, which gives an invalid choice when you load the form.
      unset($this->value);
    }
    parent::value_form($form, $form_state);
    if (empty($form_state['exposed'])) {
      $form['warning'] = array(
        '#type' => 'markup',
        '#value' => t('You can only select options if you expose this filter and have a project argument'),
      );
    }
  }

  function get_value_options() {
    static $versions = array();
    // We only want to return real options if we're building an exposed filter
    // form and we have a project argument (either nid or uri).  So, we check
    // the flag set in value_form() to see if this is an exposed filter.
    if ($this->is_exposed) {
      if (empty($versions)) {
        $arg = reset($this->view->args);
        if (is_numeric($arg)) {
          $project_nid = $arg;
        }
        else {
          $project_nid = db_result(db_query("SELECT nid FROM {project_projects} WHERE uri = '%s'", $arg));
        }
        // This is needed by project_get_releases() and friends.
        $project = node_load($project_nid);
        $versions = project_release_get_releases($project, FALSE);
      }
      $this->value_options = $versions;
    }
    else {
      $this->value_options = array();
    }
  }

}

