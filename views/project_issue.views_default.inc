<?php
// $Id: project_issue.views_default.inc,v 1.13 2009-01-28 15:14:24 dww Exp $


/**
 * @file
 * Provides default views on behalf of the project_issue module.
 */

/**
 * Implementation of hook_views_default_views().
 */
function project_issue_views_default_views() {
  // ----------------------------------------
  // Issue queue across all projects
  // ----------------------------------------
  $view = new view;
  $view->name = 'project_issue_all_projects';
  $view->description = 'Project issues for all projects';
  $view->tag = 'Project issue';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'assigned' => array(
      'label' => 'Assigned user',
      'required' => 0,
      'id' => 'assigned',
      'table' => 'project_issues',
      'field' => 'assigned',
      'relationship' => 'none',
    ),
    'rid' => array(
      'label' => 'Version',
      'required' => 0,
      'id' => 'rid',
      'table' => 'project_issues',
      'field' => 'rid',
      'relationship' => 'none',
    ),
    'pid' => array(
      'id' => 'pid',
      'table' => 'project_issues',
      'field' => 'pid',
    ),
  ));
  $handler->override_option('fields', array(
    'project_issue_queue' => array(
      'label' => 'Project',
      'exclude' => 0,
      'id' => 'project_issue_queue',
      'table' => 'node',
      'field' => 'project_issue_queue',
      'relationship' => 'pid',
    ),
    'title' => array(
      'label' => 'Summary',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'sid' => array(
      'id' => 'sid',
      'table' => 'project_issues',
      'field' => 'sid',
    ),
    'priority' => array(
      'id' => 'priority',
      'table' => 'project_issues',
      'field' => 'priority',
    ),
    'category' => array(
      'label' => 'Category',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'category',
      'table' => 'project_issues',
      'field' => 'category',
      'relationship' => 'none',
    ),
    'comment_count' => array(
      'label' => 'Replies',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'comment_count',
      'table' => 'node_comment_statistics',
      'field' => 'comment_count',
      'relationship' => 'none',
    ),
    'new_comments' => array(
      'label' => 'New replies',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => ' new',
      'link_to_comment' => 1,
      'no_empty' => 1,
      'exclude' => 0,
      'id' => 'new_comments',
      'table' => 'node',
      'field' => 'new_comments',
      'relationship' => 'none',
    ),
    'last_comment_timestamp' => array(
      'label' => 'Last updated',
      'date_format' => 'time ago',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'last_comment_timestamp',
      'table' => 'node_comment_statistics',
      'field' => 'last_comment_timestamp',
      'relationship' => 'none',
    ),
    'name' => array(
      'label' => 'Assigned to',
      'link_to_user' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'assigned',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'project_issue' => 'project_issue',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'status_extra' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status_extra',
      'table' => 'node',
      'field' => 'status_extra',
      'relationship' => 'none',
    ),
    'title' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'title_op',
        'identifier' => 'title',
        'label' => 'Project',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'pid',
    ),
    'sid' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'sid_op',
        'identifier' => 'status',
        'label' => 'Status',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'sid',
      'table' => 'project_issues',
      'field' => 'sid',
      'relationship' => 'none',
    ),
    'priority' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'priority_op',
        'identifier' => 'priorities',
        'label' => 'Priority',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'priority',
      'table' => 'project_issues',
      'field' => 'priority',
      'relationship' => 'none',
    ),
    'category' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'category_op',
        'identifier' => 'categories',
        'label' => 'Category',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'category',
      'table' => 'project_issues',
      'field' => 'category',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Issues for all projects');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'project_issue_table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'desc',
    'columns' => array(
      'project_issue_queue' => 'project_issue_queue',
      'title' => 'title',
      'sid' => 'sid',
      'priority' => 'priority',
      'category' => 'category',
      'comment_count' => 'comment_count',
      'new_comments' => 'comment_count',
      'last_comment_timestamp' => 'last_comment_timestamp',
      'name' => 'name',
    ),
    'info' => array(
      'project_issue_queue' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'title' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'sid' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'priority' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'category' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'comment_count' => array(
        'sortable' => 1,
        'separator' => '<br />',
      ),
      'new_comments' => array(
        'separator' => '',
      ),
      'last_comment_timestamp' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'last_comment_timestamp',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'project/issues');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Issues',
    'description' => '',
    'weight' => '0',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
  ));
  $handler = $view->new_display('feed', 'Feed', 'feed_1');
  $handler->override_option('sorts', array(
    'last_comment_timestamp' => array(
      'order' => 'DESC',
      'granularity' => 'second',
      'id' => 'last_comment_timestamp',
      'table' => 'node_comment_statistics',
      'field' => 'last_comment_timestamp',
      'override' => array(
        'button' => 'Use default',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('style_plugin', 'rss');
  $handler->override_option('style_options', array(
    'mission_description' => FALSE,
    'description' => '',
  ));
  $handler->override_option('row_plugin', 'node_rss');
  $handler->override_option('path', 'project/issues/rss');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => '',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
  ));
  $handler->override_option('displays', array(
    'page_1' => 'page_1',
    'default' => 0,
  ));
  $handler->override_option('sitename_title', FALSE);

  $views[$view->name] = $view;


  // ----------------------------------------
  // Issue queue for a specific project
  // ----------------------------------------
  $view = new view;
  $view->name = 'project_issue_project';
  $view->description = 'Project issues by project';
  $view->tag = 'Project issue';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'assigned' => array(
      'label' => 'Assigned user',
      'required' => 0,
      'id' => 'assigned',
      'table' => 'project_issues',
      'field' => 'assigned',
      'relationship' => 'none',
    ),
    'rid' => array(
      'label' => 'Version',
      'required' => 0,
      'id' => 'rid',
      'table' => 'project_issues',
      'field' => 'rid',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('fields', array(
    'title' => array(
      'label' => 'Summary',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'sid' => array(
      'id' => 'sid',
      'table' => 'project_issues',
      'field' => 'sid',
    ),
    'priority' => array(
      'id' => 'priority',
      'table' => 'project_issues',
      'field' => 'priority',
    ),
    'category' => array(
      'label' => 'Category',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'category',
      'table' => 'project_issues',
      'field' => 'category',
      'relationship' => 'none',
    ),
    'version' => array(
      'label' => 'Version',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'version',
      'table' => 'project_release_nodes',
      'field' => 'version',
      'relationship' => 'rid',
    ),
    'component' => array(
      'label' => 'Component',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'component',
      'table' => 'project_issues',
      'field' => 'component',
      'relationship' => 'none',
    ),
    'comment_count' => array(
      'label' => 'Replies',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'comment_count',
      'table' => 'node_comment_statistics',
      'field' => 'comment_count',
      'relationship' => 'none',
    ),
    'new_comments' => array(
      'label' => 'New replies',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => ' new',
      'link_to_comment' => 1,
      'no_empty' => 1,
      'exclude' => 0,
      'id' => 'new_comments',
      'table' => 'node',
      'field' => 'new_comments',
      'relationship' => 'none',
    ),
    'last_comment_timestamp' => array(
      'label' => 'Last updated',
      'date_format' => 'time ago',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'last_comment_timestamp',
      'table' => 'node_comment_statistics',
      'field' => 'last_comment_timestamp',
      'relationship' => 'none',
    ),
    'name' => array(
      'label' => 'Assigned to',
      'link_to_user' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'assigned',
    ),
  ));
  $handler->override_option('arguments', array(
    'pid' => array(
      'default_action' => 'not found',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => '',
      'wildcard_substitution' => '',
      'title' => 'Issues for %1',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'project_nid',
      'validate_fail' => 'not found',
      'break_phrase' => 0,
      'not' => 0,
      'id' => 'pid',
      'table' => 'project_issues',
      'field' => 'pid',
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_php' => '',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'project_issue' => 'project_issue',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'status_extra' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status_extra',
      'table' => 'node',
      'field' => 'status_extra',
      'relationship' => 'none',
    ),
    'sid' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'sid_op',
        'identifier' => 'status',
        'label' => 'Status',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'sid',
      'table' => 'project_issues',
      'field' => 'sid',
      'relationship' => 'none',
    ),
    'priority' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'priority_op',
        'identifier' => 'priorities',
        'label' => 'Priority',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'priority',
      'table' => 'project_issues',
      'field' => 'priority',
      'relationship' => 'none',
    ),
    'category' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'category_op',
        'identifier' => 'categories',
        'label' => 'Category',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'category',
      'table' => 'project_issues',
      'field' => 'category',
      'relationship' => 'none',
    ),
    'rid' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'rid_op',
        'identifier' => 'version',
        'label' => 'Version',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'rid',
      'table' => 'project_issues',
      'field' => 'rid',
      'relationship' => 'none',
    ),
    'component' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'component_op',
        'identifier' => 'component',
        'label' => 'Component',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'component',
      'table' => 'project_issues',
      'field' => 'component',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Issues by project');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'project_issue_table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'desc',
    'columns' => array(
      'title' => 'title',
      'sid' => 'sid',
      'priority' => 'priority',
      'category' => 'category',
      'version' => 'version',
      'component' => 'component',
      'comment_count' => 'comment_count',
      'new_comments' => 'comment_count',
      'last_comment_timestamp' => 'last_comment_timestamp',
      'name' => 'name',
    ),
    'info' => array(
      'title' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'sid' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'priority' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'category' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'version' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'component' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'comment_count' => array(
        'sortable' => 1,
        'separator' => '<br />',
      ),
      'new_comments' => array(
        'separator' => '',
      ),
      'last_comment_timestamp' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'last_comment_timestamp',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'project/issues/%');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => '0',
    'name' => '',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
  ));
  $handler = $view->new_display('feed', 'Feed', 'feed_1');
  $handler->override_option('sorts', array(
    'last_comment_timestamp' => array(
      'order' => 'DESC',
      'granularity' => 'second',
      'id' => 'last_comment_timestamp',
      'table' => 'node_comment_statistics',
      'field' => 'last_comment_timestamp',
      'override' => array(
        'button' => 'Use default',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('style_plugin', 'rss');
  $handler->override_option('style_options', array(
    'mission_description' => FALSE,
    'description' => '',
  ));
  $handler->override_option('row_plugin', 'node_rss');
  $handler->override_option('path', 'project/issues/rss/%');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => '',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
  ));
  $handler->override_option('displays', array(
    'page_1' => 'page_1',
    'default' => 0,
  ));
  $handler->override_option('sitename_title', FALSE);

  $views[$view->name] = $view;


  // ---------------------------------------------
  // All issues a user has created or commented on
  // ---------------------------------------------
  $view = new view;
  $view->name = 'project_issue_user_issues';
  $view->description = 'Project issues for a specific user';
  $view->tag = 'Project issue';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'assigned' => array(
      'label' => 'Assigned user',
      'required' => 0,
      'id' => 'assigned',
      'table' => 'project_issues',
      'field' => 'assigned',
      'relationship' => 'none',
    ),
    'rid' => array(
      'label' => 'Version',
      'required' => 0,
      'id' => 'rid',
      'table' => 'project_issues',
      'field' => 'rid',
      'relationship' => 'none',
    ),
    'pid' => array(
      'id' => 'pid',
      'table' => 'project_issues',
      'field' => 'pid',
    ),
  ));
  $handler->override_option('fields', array(
    'project_issue_queue' => array(
      'label' => 'Project',
      'exclude' => 0,
      'id' => 'project_issue_queue',
      'table' => 'node',
      'field' => 'project_issue_queue',
      'relationship' => 'pid',
    ),
    'title' => array(
      'label' => 'Summary',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'sid' => array(
      'id' => 'sid',
      'table' => 'project_issues',
      'field' => 'sid',
    ),
    'priority' => array(
      'id' => 'priority',
      'table' => 'project_issues',
      'field' => 'priority',
    ),
    'category' => array(
      'label' => 'Category',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'category',
      'table' => 'project_issues',
      'field' => 'category',
      'relationship' => 'none',
    ),
    'version' => array(
      'label' => 'Version',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'version',
      'table' => 'project_release_nodes',
      'field' => 'version',
      'relationship' => 'rid',
    ),
    'comment_count' => array(
      'label' => 'Replies',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'comment_count',
      'table' => 'node_comment_statistics',
      'field' => 'comment_count',
      'relationship' => 'none',
    ),
    'new_comments' => array(
      'label' => '',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => ' new',
      'link_to_comment' => 1,
      'no_empty' => 1,
      'exclude' => 0,
      'id' => 'new_comments',
      'table' => 'node',
      'field' => 'new_comments',
      'relationship' => 'none',
    ),
    'last_comment_timestamp' => array(
      'label' => 'Last updated',
      'date_format' => 'time ago',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'last_comment_timestamp',
      'table' => 'node_comment_statistics',
      'field' => 'last_comment_timestamp',
      'relationship' => 'none',
    ),
    'name' => array(
      'label' => 'Assigned to',
      'link_to_user' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'assigned',
    ),
  ));
  $handler->override_option('arguments', array(
    'uid_touch' => array(
      'default_action' => 'default',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => 'Issues for %1',
      'default_argument_type' => 'current_user',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'id' => 'uid_touch',
      'table' => 'node',
      'field' => 'uid_touch',
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_project_term_vocabulary' => array(),
      'validate_argument_project_term_argument_type' => 'tid',
      'validate_argument_project_term_argument_action_top_without' => 'pass',
      'validate_argument_project_term_argument_action_top_with' => 'pass',
      'validate_argument_project_term_argument_action_child' => 'pass',
      'validate_argument_php' => '',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'project_issue' => 'project_issue',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'status_extra' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status_extra',
      'table' => 'node',
      'field' => 'status_extra',
      'relationship' => 'none',
    ),
    'title' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'title_op',
        'identifier' => 'title',
        'label' => 'Project',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'pid',
    ),
    'sid' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'sid_op',
        'identifier' => 'status',
        'label' => 'Status',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'sid',
      'table' => 'project_issues',
      'field' => 'sid',
      'relationship' => 'none',
    ),
    'priority' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'priority_op',
        'identifier' => 'priorities',
        'label' => 'Priority',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'priority',
      'table' => 'project_issues',
      'field' => 'priority',
      'relationship' => 'none',
    ),
    'category' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'category_op',
        'identifier' => 'categories',
        'label' => 'Category',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'category',
      'table' => 'project_issues',
      'field' => 'category',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'My issues');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'project_issue_table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'desc',
    'columns' => array(
      'project_issue_queue' => 'project_issue_queue',
      'title' => 'title',
      'sid' => 'sid',
      'priority' => 'priority',
      'category' => 'category',
      'version' => 'version',
      'comment_count' => 'comment_count',
      'new_comments' => 'comment_count',
      'last_comment_timestamp' => 'last_comment_timestamp',
      'name' => 'name',
    ),
    'info' => array(
      'project_issue_queue' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'title' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'sid' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'priority' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'category' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'version' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'comment_count' => array(
        'sortable' => 1,
        'separator' => '<br />',
      ),
      'new_comments' => array(
        'separator' => '',
      ),
      'last_comment_timestamp' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'project/issues/user');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'My issues',
    'description' => '',
    'weight' => '0',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
  ));

  $views[$view->name] = $view;


  // ------------------------------------------
  // Issues across all projects owned by a user
  // ------------------------------------------
  $view = new view;
  $view->name = 'project_issue_user_projects';
  $view->description = "Project issues for a specific user's projects";
  $view->tag = 'Project issue';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'assigned' => array(
      'label' => 'Assigned user',
      'required' => 0,
      'id' => 'assigned',
      'table' => 'project_issues',
      'field' => 'assigned',
      'relationship' => 'none',
    ),
    'rid' => array(
      'label' => 'Version',
      'required' => 0,
      'id' => 'rid',
      'table' => 'project_issues',
      'field' => 'rid',
      'relationship' => 'none',
    ),
    'pid' => array(
      'id' => 'pid',
      'table' => 'project_issues',
      'field' => 'pid',
    ),
  ));
  $handler->override_option('fields', array(
    'project_issue_queue' => array(
      'label' => 'Project',
      'exclude' => 0,
      'id' => 'project_issue_queue',
      'table' => 'node',
      'field' => 'project_issue_queue',
      'relationship' => 'pid',
    ),
    'title' => array(
      'label' => 'Summary',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'sid' => array(
      'id' => 'sid',
      'table' => 'project_issues',
      'field' => 'sid',
    ),
    'priority' => array(
      'id' => 'priority',
      'table' => 'project_issues',
      'field' => 'priority',
    ),
    'category' => array(
      'label' => 'Category',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'category',
      'table' => 'project_issues',
      'field' => 'category',
      'relationship' => 'none',
    ),
    'version' => array(
      'label' => 'Version',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'version',
      'table' => 'project_release_nodes',
      'field' => 'version',
      'relationship' => 'rid',
    ),
    'comment_count' => array(
      'label' => 'Replies',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'comment_count',
      'table' => 'node_comment_statistics',
      'field' => 'comment_count',
      'relationship' => 'none',
    ),
    'new_comments' => array(
      'label' => '',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => ' new',
      'link_to_comment' => 1,
      'no_empty' => 1,
      'exclude' => 0,
      'id' => 'new_comments',
      'table' => 'node',
      'field' => 'new_comments',
      'relationship' => 'none',
    ),
    'last_comment_timestamp' => array(
      'label' => 'Last updated',
      'date_format' => 'time ago',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'last_comment_timestamp',
      'table' => 'node_comment_statistics',
      'field' => 'last_comment_timestamp',
      'relationship' => 'none',
    ),
    'name' => array(
      'label' => 'Assigned to',
      'link_to_user' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'assigned',
    ),
  ));
  $handler->override_option('arguments', array(
    'name' => array(
      'default_action' => 'not found',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => 'Projects by %1',
      'default_argument_type' => 'current_user',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'glossary' => 0,
      'limit' => '0',
      'case' => 'none',
      'path_case' => 'none',
      'transform_dash' => 0,
      'id' => 'name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'pid',
      'default_options_div_prefix' => '',
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_project_term_vocabulary' => array(),
      'validate_argument_project_term_argument_type' => 'tid',
      'validate_argument_project_term_argument_action_top_without' => 'pass',
      'validate_argument_project_term_argument_action_top_with' => 'pass',
      'validate_argument_project_term_argument_action_child' => 'pass',
      'validate_argument_php' => '',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'project_issue' => 'project_issue',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'status_extra' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status_extra',
      'table' => 'node',
      'field' => 'status_extra',
      'relationship' => 'none',
    ),
    'title' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'title_op',
        'identifier' => 'title',
        'label' => 'Project',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'pid',
    ),
    'sid' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'sid_op',
        'identifier' => 'status',
        'label' => 'Status',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'sid',
      'table' => 'project_issues',
      'field' => 'sid',
      'relationship' => 'none',
    ),
    'priority' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'priority_op',
        'identifier' => 'priorities',
        'label' => 'Priority',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'priority',
      'table' => 'project_issues',
      'field' => 'priority',
      'relationship' => 'none',
    ),
    'category' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'category_op',
        'identifier' => 'categories',
        'label' => 'Category',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'category',
      'table' => 'project_issues',
      'field' => 'category',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'project_issue_table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'desc',
    'columns' => array(
      'project_issue_queue' => 'project_issue_queue',
      'title' => 'title',
      'sid' => 'sid',
      'priority' => 'priority',
      'category' => 'category',
      'version' => 'version',
      'comment_count' => 'comment_count',
      'new_comments' => 'comment_count',
      'last_comment_timestamp' => 'last_comment_timestamp',
      'name' => 'name',
    ),
    'info' => array(
      'project_issue_queue' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'title' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'sid' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'priority' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'category' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'version' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'comment_count' => array(
        'sortable' => 1,
        'separator' => '<br />',
      ),
      'new_comments' => array(
        'separator' => '',
      ),
      'last_comment_timestamp' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'last_comment_timestamp',
  ));

  $views[$view->name] = $view;


  // ------------------------------------------
  // Advanced issue search for all projects
  // ------------------------------------------
  $view = new view;
  $view->name = 'project_issue_search_all';
  $view->description = 'Project issue advanced search for all projects';
  $view->tag = 'Project issue';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'assigned' => array(
      'label' => 'Assigned user',
      'required' => 0,
      'id' => 'assigned',
      'table' => 'project_issues',
      'field' => 'assigned',
      'relationship' => 'none',
    ),
    'rid' => array(
      'label' => 'Version',
      'required' => 0,
      'id' => 'rid',
      'table' => 'project_issues',
      'field' => 'rid',
      'relationship' => 'none',
    ),
    'pid' => array(
      'id' => 'pid',
      'table' => 'project_issues',
      'field' => 'pid',
    ),
  ));
  $handler->override_option('fields', array(
    'project_issue_queue' => array(
      'label' => 'Project',
      'exclude' => 0,
      'id' => 'project_issue_queue',
      'table' => 'node',
      'field' => 'project_issue_queue',
      'relationship' => 'pid',
    ),
    'title' => array(
      'label' => 'Summary',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'sid' => array(
      'id' => 'sid',
      'table' => 'project_issues',
      'field' => 'sid',
    ),
    'priority' => array(
      'id' => 'priority',
      'table' => 'project_issues',
      'field' => 'priority',
    ),
    'category' => array(
      'label' => 'Category',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'category',
      'table' => 'project_issues',
      'field' => 'category',
      'relationship' => 'none',
    ),
    'version' => array(
      'label' => 'Version',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'version',
      'table' => 'project_release_nodes',
      'field' => 'version',
      'relationship' => 'rid',
    ),
    'last_comment_timestamp' => array(
      'label' => 'Last updated',
      'date_format' => 'time ago',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'last_comment_timestamp',
      'table' => 'node_comment_statistics',
      'field' => 'last_comment_timestamp',
      'relationship' => 'none',
    ),
    'name' => array(
      'label' => 'Assigned to',
      'link_to_user' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'assigned',
    ),
  ));
  $custom_filters = array();
  if (module_exists('search')) {
    $custom_filters['keys'] = array(
      'operator' => 'required',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'keys_op',
        'identifier' => 'text',
        'label' => 'Search for',
        'optional' => 1,
        'remember' => 0,
      ),
      'id' => 'keys',
      'table' => 'search_index',
      'field' => 'keys',
      'relationship' => 'none',
    );
  }
  $shared_filters = array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'project_issue' => 'project_issue',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'status_extra' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status_extra',
      'table' => 'node',
      'field' => 'status_extra',
      'relationship' => 'none',
    ),
    'title' => array(
      'operator' => 'word',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'title_op',
        'identifier' => 'projects',
        'label' => 'Project',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'pid',
    ),
    'category' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'category_op',
        'identifier' => 'categories',
        'label' => 'Category',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'category',
      'table' => 'project_issues',
      'field' => 'category',
      'relationship' => 'none',
    ),
    'priority' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'priority_op',
        'identifier' => 'priorities',
        'label' => 'Priority',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'priority',
      'table' => 'project_issues',
      'field' => 'priority',
      'relationship' => 'none',
    ),
    'sid' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'sid_op',
        'identifier' => 'status',
        'label' => 'Status',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'sid',
      'table' => 'project_issues',
      'field' => 'sid',
      'relationship' => 'none',
    ),
    'uid' => array(
      'operator' => 'in',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'uid_op',
        'identifier' => 'assigned',
        'label' => 'Assigned',
        'optional' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'uid',
      'table' => 'users',
      'field' => 'uid',
      'relationship' => 'assigned',
    ),
    'uid_1' => array(
      'operator' => 'in',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'uid_1_op',
        'identifier' => 'submitted',
        'label' => 'Submitted by',
        'optional' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'uid_1',
      'table' => 'users',
      'field' => 'uid',
      'relationship' => 'none',
    ),
  );
  $handler->override_option('filters', $custom_filters + $shared_filters);
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Search issues for all projects');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'desc',
    'columns' => array(
      'project_issue_queue' => 'project_issue_queue',
      'title' => 'title',
      'sid' => 'sid',
      'priority' => 'priority',
      'category' => 'category',
      'version' => 'version',
      'last_comment_timestamp' => 'last_comment_timestamp',
      'name' => 'name',
    ),
    'info' => array(
      'project_issue_queue' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'title' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'sid' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'priority' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'category' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'version' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'last_comment_timestamp' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'last_comment_timestamp',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'project/issues/search');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Advanced search',
    'weight' => '0',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $handler = $view->new_display('feed', 'Feed', 'feed_1');
  $handler->override_option('sorts', array(
    'last_comment_timestamp' => array(
      'order' => 'DESC',
      'granularity' => 'second',
      'id' => 'last_comment_timestamp',
      'table' => 'node_comment_statistics',
      'field' => 'last_comment_timestamp',
      'override' => array(
        'button' => 'Use default',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('style_plugin', 'rss');
  $handler->override_option('style_options', array(
    'mission_description' => FALSE,
    'description' => '',
  ));
  $handler->override_option('row_plugin', 'node_rss');
  $handler->override_option('path', 'project/issues/search/rss');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
    'name' => '',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $handler->override_option('displays', array(
    'page_1' => 'page_1',
    'default' => 0,
  ));
  $handler->override_option('sitename_title', FALSE);
  
  $views[$view->name] = $view;

  // --------------------------------------------
  // Advanced issue search for a specific project
  // --------------------------------------------
  $view = new view;
  $view->name = 'project_issue_search_project';
  $view->description = 'Project issue advanced search for specific projects';
  $view->tag = 'Project issue';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'assigned' => array(
      'label' => 'Assigned user',
      'required' => 0,
      'id' => 'assigned',
      'table' => 'project_issues',
      'field' => 'assigned',
      'relationship' => 'none',
    ),
    'rid' => array(
      'label' => 'Version',
      'required' => 0,
      'id' => 'rid',
      'table' => 'project_issues',
      'field' => 'rid',
      'relationship' => 'none',
    ),
    'pid' => array(
      'id' => 'pid',
      'table' => 'project_issues',
      'field' => 'pid',
    ),
  ));
  $handler->override_option('fields', array(
    'title' => array(
      'label' => 'Summary',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'sid' => array(
      'id' => 'sid',
      'table' => 'project_issues',
      'field' => 'sid',
    ),
    'priority' => array(
      'id' => 'priority',
      'table' => 'project_issues',
      'field' => 'priority',
    ),
    'category' => array(
      'label' => 'Category',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'category',
      'table' => 'project_issues',
      'field' => 'category',
      'relationship' => 'none',
    ),
    'version' => array(
      'label' => 'Version',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'version',
      'table' => 'project_release_nodes',
      'field' => 'version',
      'relationship' => 'rid',
    ),
    'component' => array(
      'label' => 'Component',
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'component',
      'table' => 'project_issues',
      'field' => 'component',
      'relationship' => 'none',
    ),
    'last_comment_timestamp' => array(
      'label' => 'Last updated',
      'date_format' => 'time ago',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'last_comment_timestamp',
      'table' => 'node_comment_statistics',
      'field' => 'last_comment_timestamp',
      'relationship' => 'none',
    ),
    'name' => array(
      'label' => 'Assigned to',
      'link_to_user' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'assigned',
    ),
  ));
  $handler->override_option('arguments', array(
    'uri' => array(
      'default_action' => 'not found',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => '',
      'wildcard_substitution' => '',
      'title' => 'Search issues for %1',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'glossary' => 0,
      'limit' => '0',
      'case' => 'none',
      'path_case' => 'none',
      'transform_dash' => 0,
      'id' => 'uri',
      'table' => 'project_projects',
      'field' => 'uri',
      'relationship' => 'pid',
      'default_options_div_prefix' => '',
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_project_term_vocabulary' => array(),
      'validate_argument_project_term_argument_type' => 'tid',
      'validate_argument_project_term_argument_action_top_without' => 'pass',
      'validate_argument_project_term_argument_action_top_with' => 'pass',
      'validate_argument_project_term_argument_action_child' => 'pass',
      'validate_argument_php' => '',
    ),
  ));
  $custom_filters = array();
  if (module_exists('search')) {
    $custom_filters['keys'] = array(
      'operator' => 'required',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'keys_op',
        'identifier' => 'text',
        'label' => 'Search for',
        'optional' => 1,
        'remember' => 0,
      ),
      'id' => 'keys',
      'table' => 'search_index',
      'field' => 'keys',
      'relationship' => 'none',
    );
  }
  $shared_filters = array(
    'keys' => array(
      'operator' => 'required',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'keys_op',
        'identifier' => 'text',
        'label' => 'Search for',
        'optional' => 1,
        'remember' => 0,
      ),
      'id' => 'keys',
      'table' => 'search_index',
      'field' => 'keys',
      'relationship' => 'none',
    ),
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'project_issue' => 'project_issue',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'status_extra' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status_extra',
      'table' => 'node',
      'field' => 'status_extra',
      'relationship' => 'none',
    ),
    'category' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'category_op',
        'identifier' => 'categories',
        'label' => 'Category',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'category',
      'table' => 'project_issues',
      'field' => 'category',
      'relationship' => 'none',
    ),
    'priority' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'priority_op',
        'identifier' => 'priorities',
        'label' => 'Priority',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'priority',
      'table' => 'project_issues',
      'field' => 'priority',
      'relationship' => 'none',
    ),
    'sid' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'sid_op',
        'identifier' => 'status',
        'label' => 'Status',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'sid',
      'table' => 'project_issues',
      'field' => 'sid',
      'relationship' => 'none',
    ),
    'component' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'component_op',
        'identifier' => 'component',
        'label' => 'Component',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'component',
      'table' => 'project_issues',
      'field' => 'component',
      'relationship' => 'none',
    ),
    'rid' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'rid_op',
        'identifier' => 'version',
        'label' => 'Version',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'rid',
      'table' => 'project_issues',
      'field' => 'rid',
      'relationship' => 'none',
    ),
    'uid' => array(
      'operator' => 'in',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'uid_op',
        'identifier' => 'assigned',
        'label' => 'Assigned',
        'optional' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'uid',
      'table' => 'users',
      'field' => 'uid',
      'relationship' => 'assigned',
    ),
    'uid_1' => array(
      'operator' => 'in',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'uid_1_op',
        'identifier' => 'submitted',
        'label' => 'Submitted by',
        'optional' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'uid_1',
      'table' => 'users',
      'field' => 'uid',
      'relationship' => 'none',
    ),
  );
  $handler->override_option('filters', $custom_filters + $shared_filters);
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Search issues for a project');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'desc',
    'columns' => array(
      'project_issue_queue' => 'project_issue_queue',
      'title' => 'title',
      'sid' => 'sid',
      'priority' => 'priority',
      'category' => 'category',
      'version' => 'version',
      'last_comment_timestamp' => 'last_comment_timestamp',
      'name' => 'name',
    ),
    'info' => array(
      'project_issue_queue' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'title' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'sid' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'priority' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'category' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'version' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'last_comment_timestamp' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'last_comment_timestamp',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'project/issues/search/%');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => 'Advanced search',
    'weight' => '0',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $handler = $view->new_display('feed', 'Feed', 'feed_1');
  $handler->override_option('sorts', array(
    'last_comment_timestamp' => array(
      'order' => 'DESC',
      'granularity' => 'second',
      'id' => 'last_comment_timestamp',
      'table' => 'node_comment_statistics',
      'field' => 'last_comment_timestamp',
      'override' => array(
        'button' => 'Use default',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('style_plugin', 'rss');
  $handler->override_option('style_options', array(
    'mission_description' => FALSE,
    'description' => '',
  ));
  $handler->override_option('row_plugin', 'node_rss');
  $handler->override_option('path', 'project/issues/search/%/rss');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
    'name' => '',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $handler->override_option('displays', array(
    'page_1' => 'page_1',
    'default' => 0,
  ));
  $handler->override_option('sitename_title', FALSE);
  
  $views[$view->name] = $view;

  return $views;
}

