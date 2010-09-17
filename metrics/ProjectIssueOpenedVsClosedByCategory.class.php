<?php
// $Id: ProjectIssueOpenedVsClosedByCategory.class.php,v 1.1 2010/09/17 17:47:46 thehunmonkgroup Exp $

/**
 * @file
 * Class for opened_vs_closed_by_category metric.
 */

class ProjectIssueOpenedVsClosedByCategory extends SamplerMetric {

  public function computeSample() {
    // Load options.
    $options = $this->currentSample->options;

    $where_pieces = array();
    $args = array();

    // The initial arguments are the 'open' project issue status options.
    $open_states = project_issue_state(0, FALSE, FALSE, 0, TRUE);

    // TODO: Until http://drupal.org/node/214347 is resolved, we don't want
    // the 'fixed' status in the list of open statuses, so unset it here.
    unset($open_states[PROJECT_ISSUE_STATE_FIXED]);

    $open_states = array_keys($open_states);

    $args = array_merge($args, $open_states);

    // Don't compute if we're on the first sample in a sample set.
    if (!$this->getPreviousSample()) {
      return FALSE;
    }

    // Restrict to only the passed nids.
    if (!empty($options['object_ids'])) {
      $where_pieces[] = "pid IN (". db_placeholders($options['object_ids']) .")";
      $args = array_merge($args, $options['object_ids']);
    }

    if (empty($where_pieces)) {
      $where = '';
    }
    else {
      $where = ' AND ' . implode(' AND ', $where_pieces);
    }

    // Build total open issues counts.
    $this->buildSampleResults('open', $open_states, $where, $args);
    // Build total closed issues counts.
    $this->buildSampleResults('closed', $open_states, $where, $args);

    // Add in total counts across all categories.
    foreach ($this->currentSample->values as $project_id => $values_array) {
      $this->currentSample->values[$project_id]['total_open'] = $values_array['bug_open'] + $values_array['feature_open'] + $values_array['task_open'] + $values_array['support_open'];
      $this->currentSample->values[$project_id]['total_closed'] = $values_array['bug_closed'] + $values_array['feature_closed'] + $values_array['task_closed'] + $values_array['support_closed'];
    }
  }

  /**
   * Builds the final values for total open/closed issues per project
   *
   * @param $state
   *   Issue state, 'open' or 'closed'.
   * @param $open_states
   *   An array of state IDs that are considered 'open'.
   * @param $where
   *   Additional filters for the query.
   * @param $args
   *   Query arguments.
   */
  protected function buildSampleResults($state, $open_states, $where, $args) {

    // Determine IN or NOT IN based on the state we're querying for.
    $in = $state == 'open' ? 'IN' : 'NOT IN';

    // Pull all issues grouped by project/category -- this query will miss any
    // projects that don't have issues, but we wouldn't want to show any data
    // for them, anyways.
    $result = db_query("SELECT pid, category, COUNT(nid) AS count FROM {project_issues} WHERE sid $in (" . db_placeholders($open_states) . ")$where GROUP BY pid, category", $args);
    // TODO: When http://drupal.org/node/115553 lands, this hard-coded list of
    // categories will need to be handled differently.
    $categories = array('bug', 'feature', 'task', 'support');
    $new_row = array(
      'bug_open' => 0,
      'feature_open' => 0,
      'task_open' => 0,
      'support_open' => 0,
      'bug_closed' => 0,
      'feature_closed' => 0,
      'task_closed' => 0,
      'support_closed' => 0,
    );
    while ($row = db_fetch_object($result)) {
      // Initialize projects with zero values.
      if (!isset($this->currentSample->values[$row->pid])) {
        $this->currentSample->values[$row->pid] = $new_row;
      }
      // Add the total count for the category to the values array
      if (in_array($row->category, $categories)) {
        $this->currentSample->values[$row->pid]["{$row->category}_{$state}"] = $row->count;
      }
    }
  }
}
