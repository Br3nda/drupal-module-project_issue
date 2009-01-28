<?php
// $Id: project_issue_table_plugin_style.inc,v 1.1 2009-01-28 06:01:07 dww Exp $

/**
 * @file
 * Empty class for the project_issue_table Views style plugin.
 *
 * We just exactly re-use all of the code of the core Views table style, but
 * by having our own style plugin, we can inject our own custom classes into
 * the table HTML to style the rows by issue status and priority.
 */

class project_issue_table_plugin_style extends views_plugin_style_table { 
  
}

