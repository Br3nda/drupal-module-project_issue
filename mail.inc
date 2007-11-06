<?php
// $Id: mail.inc,v 1.93 2007/11/06 17:13:39 thehunmonkgroup Exp $
// $Name:  $

function project_issue_mailhandler($node, $result, $i, $header, $mailbox) {
  if ($node->type == 'project') {
    if (node_access('create', 'project_issue')) {
      $node->nid = preg_replace('/@.+/', '', $node->nid);

      if ($node->nid) {
        /*
        ** Base the new entry on the node it belongs to, this ensures all
        ** values are initially correct.
        */
        $entry = node_load(array('nid' => $node->nid, 'type' => 'project_issue'));
      }

      // Possible attributes
      $fields = array(
        'pid' => t('Project'),
        'category' => t('Category'),
        'component' => t('Component'),
        'priority' => t('Priority'),
        'rid' => t('Version'),
        'assigned' => t('Assigned to'),
        'sid' => t('Status')
      );

      /*
      ** Only change the title if it doesn't have the old title in it.
      ** This should prevent the title from changing due to added
      ** prefixes. It may on occasion make false positives, but if
      ** a title change is that minor who cares?
      */
      $entry->title = (strpos($node->title, $entry->title)) ? $entry->title : $node->title;

      $entry->teaser = $node->teaser;
      $entry->body = $node->body;
      $entry->uid = $node->uid;

      foreach ($fields as $var => $text) {
        $text = strtolower(str_replace(' ', '_', $text));
        if (isset($node->$text)) {
          $node->$text = trim($node->$text);
          switch ($var) {
            case 'pid':
              $project = node_load(array('title' => $node->$text, 'type' => 'project_project'));
              if ($project->nid) {
                $entry->pid = $project->nid;
              }
              break;
            case 'category':
              if (($category = array_search($node->$text, project_issue_category(0, 0)))) {
                $entry->category = $category;
              }
              break;
            case 'priority':
              if (($priority = array_search($node->$text, project_issue_priority()))) {
                $entry->priority = $priority;
              }
              break;
            case 'rid':
              if ($entry->pid && ($nid = db_result(db_query("SELECT nid FROM {project_release_nodes} WHERE pid = %d AND version = '%s'", $entry->pid, $node->$text), 0))) {
                $entry->rid = $nid;
              }
              break;
            case 'assigned':
              if ($user = user_load(array('name' => $node->$text))) {
                $entry->assigned = $user->uid;
              }
              break;
            case 'sid':
              if (($state = array_search($node->$text, project_issue_state()))) {
                $entry->sid = $state;
              }
              break;
            case 'component':
              if ($project->pid || ($entry->pid && ($project = node_load(array('nid' => $entry->pid, 'type' => 'project_project'))))) {
                if ($project && in_array($node->$text, $project->components)) {
                  $entry->component = $node->$text;
                }
              }
              break;
          }
        }
      }

      if (empty($entry->nid)) {
        $entry->sid = variable_get('project_issue_default_state', 1);
        $entry->type = 'project_issue';
        $entry = node_validate($entry, $error);
        $error or ($entry->nid = node_save($entry));
      }
      else {
        $error = project_comment_validate($entry);
        $error or project_comment_save($entry);
      }
    }
    else {
      $error['user'] = t('You are not authorized to access this page.');
    }

    if ($error && $mailbox['replies']) {
      // Send the user his errors
      $mailto = mailhandler_get_fromaddress($header, $mailbox);
      $mailfrom = variable_get('site_mail', ini_get('sendmail_from'));
      $headers = array(
        'X-Mailer' => 'Drupal Project module (http://drupal.org/project/project)',
      );

      $body = t('You had some errors in your submission:');
      foreach ($error as $field => $text) {
        $body .= "\n * $field: $text";
      }

      drupal_mail('project_issue_mailhandler_error', $mailto, t('Email submission to !sn failed - !subj', array('!sn' => variable_get('site_name', 'Drupal'), '!subj' => $header->subject)), $body, $mailfrom, $headers);
    }

    // Return a NULL result so mailhandler doesn't save the node using the default methods.
    return NULL;
  }
  else {
    return $node;
  }
}

function project_mail_urls($url = 0) {
  static $urls = array();
  if ($url) {
    $urls[] = strpos($url, '://') ? $url : url($url, NULL, NULL, 1);
    return count($urls);
  }
  return $urls;
}

function project_mail_output(&$body, $html = 1, $format = FILTER_FORMAT_DEFAULT) {
  static $i = 0;

  if ($html) {
    $pattern = '@<a +([^ >]+ )*?href *= *"([^>"]+?)"[^>]*>([^<]+?)</a>@ei';
    $body = preg_replace($pattern, "'\\3 ['. project_mail_urls('\\2') .']'", $body);
    $body = check_markup($body, $format, FALSE);
    $urls = project_mail_urls();
    if (count($urls)) {
      $body .= "\n";
      for ($max = count($urls); $i < $max; $i++) {
        $body .= '['. ($i + 1) .'] '. $urls[$i] ."\n";
      }
    }

    $body = preg_replace('!</?blockquote>!i', '"', $body);
    $body = preg_replace('!</?(em|i)>!i', '/', $body);
    $body = preg_replace('!</?(b|strong)>!i', '*', $body);
    $body = preg_replace("@<br />(?!\n)@i", "\n", $body);
    $body = preg_replace("@</p>(?!\n\n)@i", "\n\n", $body);
    $body = preg_replace("@<li>@i", "* ", $body);
    $body = preg_replace("@</li>\n?@i", "\n", $body);
    $body = strip_tags($body);
    $body = decode_entities($body);
    $body = wordwrap($body, 72);
  }
  else {
    $body = decode_entities($body);
  }
}

function project_mail_summary($field, $value) {
  switch ($field) {
    case 'pid':
      $project = node_load(array('nid' => $value, 'type' => 'project_project'));
      return $project->title;
    case 'category':
      return $value ? project_issue_category($value) : t('<none>');
    case 'priority':
      return $value ? project_issue_priority($value) : t('<none>');
    case 'rid':
      if ($value) {
        $release->nid = $value;
        if (module_exists('project_release')) {
          $release = project_release_load($release);
        }
        else {
          $release->version = t('Unknown');
        }
        return $release->version;
      }
      return t('<none>');
    case 'assigned':
      $user = user_load(array('uid' => $value));
      return $value === 0 ? variable_get('anonymous', t('Anonymous')) : $user->name;
    case 'sid':
      return $value ? project_issue_state($value) : t('<none>');
    default:
      return check_plain($value);
  }
}

function project_mail_notify($nid) {
  global $base_url, $user;

  if (defined('PROJECT_NOMAIL')) {
    return;
  }

  // There could be stale data in the cached node, so reset the cache.
  $node = node_load($nid, NULL, TRUE);
  $project = node_load(array('nid' => $node->pid, 'type' => 'project_project'));

  $fields = project_mail_summary_fields();

  // Store a copy of the issue, so we can load the original issue values
  // below.
  $issue = drupal_clone($node);

  // Load in the original issue data here, since we want a running
  // reverse history.
  $original_issue_data = unserialize($node->original_issue_data);
  foreach ($fields as $field => $label) {
    if ($field != 'name' && $field != 'updator') {
      $issue->$field = $original_issue_data->$field;
    }
  }

  // Record users that are connected to this issue.
  $uids = array();
  if ($node->assigned) {
    $uids[$node->assigned] = $node->assigned;
  }

  // Create complete history of the bug report.
  $history = array($issue);
  $result = db_query('SELECT u.name, c.cid, c.comment, c.uid, c.format, pic.* FROM {project_issue_comments} pic INNER JOIN {comments} c ON c.cid = pic.cid INNER JOIN {users} u ON u.uid = c.uid WHERE c.nid = %d AND c.status = %d ORDER BY pic.timestamp', $node->nid, COMMENT_PUBLISHED);

  while ($comment = db_fetch_object($result)) {
    $comment->comment = db_decode_blob($comment->comment);
    $comment->files = _comment_upload_load_files($comment->cid);
    $history[] = $comment;
    // Record users that are connected to this issue.
    if ($comment->uid) {
      $uids[$comment->uid] = $comment->uid;
    }
    // We need the most recent cid and the next most recent cid for the
    // message headers.  Instead of issuing another query, just keep track
    // of them here.
    $previous_cid = isset($cid) ? $cid : '';
    $cid = $comment->cid;
  }

  // Create mail header
  $sender->name = mime_header_encode(t('!name (!site)', array('!name' => $user->name, '!site' => variable_get('site_name', 'Drupal'))));
  $sender->mail = $project->mail;
  // The sender name is enclosed by double quotes below
  // to satisfy RFC2822 <http://www.faqs.org/rfcs/rfc2822.html>,
  // which requires double quotes when special characters (including
  // some punctuation) are used.  See example in Appendix A.1.2.
  $from = "\"$sender->name \"<$sender->mail>";
  $domain = preg_replace('|.+://([a-zA-Z0-9\._-]+).*|', '\1', $base_url);
  $header = array(
    'Return-Path' => "<$sender->mail>",
    'Date' => date('r'),
    'X-Mailer' => 'Drupal Project module (http://drupal.org/project/project)',
    'List-Id' => "$project->title <$project->uri-issues-$domain>",
    'List-Archive' => '<'. url("project/issues/$project->uri", NULL, NULL, 1) .'>',
    'List-Subscribe' => '<'. url("node/$project->uri", NULL, NULL, 1) .'>',
    'List-Unsubscribe' => '<'. url("node/$project->uri", NULL, NULL, 1) .'>'
  );

  // Comments exist, set headers accordingly.
  if (count($history) > 1) {
    $header['Message-Id'] = "<type=project&nid=$node->nid&cid=$cid&host=@$domain>";
    $header['In-Reply-To'] = "<type=project&nid=$node->nid&host=@$domain>";
    $header['References'] = "<type=project&nid=$node->nid&host=@$domain> <type=project&nid=$node->nid&cid=$previous_cid&host=@$domain> <type=project&nid=$node->nid&revcount=1&host=@$domain>";
  }
  // Only original issue in this email.
  else {
    $header['Message-Id'] = "<type=project&nid=$node->nid&host=@$domain>";
  }

  if (count($uids)) {
    $placeholders = implode(',', array_fill(0, count($uids), '%d'));
    array_unshift($uids, $node->pid);
    $result = db_query("SELECT p.*, u.uid, u.name, u.mail FROM {project_subscriptions} p INNER JOIN {users} u ON p.uid = u.uid WHERE u.status = 1 AND p.nid = %d AND (p.level = 2 OR (p.level = 1 AND u.uid IN ($placeholders)))", $uids);
  }
  else {
    $result = db_query('SELECT p.*, u.uid, u.name, u.mail FROM {project_subscriptions} p INNER JOIN {users} u ON p.uid = u.uid WHERE u.status = 1 AND p.nid = %d AND p.level = 2', $node->pid);
  }

  // Create link to related node
  $links = t('Issue status update for !link', array('!link' => "\n". url("node/$node->nid", NULL, NULL, 1))) ."\n";
  $links .= t('Post a follow up: !link', array('!link' => "\n". url("comment/reply/$node->nid", NULL, 'comment-form', 1))) ."\n";

  // To save workload, check here if either the anonymous role or the
  // authenticated role has the 'view uploaded files' permission, since
  // we only need to process each user's file access permission if this
  // is NOT the case.
  $check_file_perms = !db_num_rows(db_query("SELECT rid FROM {permission} WHERE perm LIKE '%view uploaded files%' AND rid IN (%d, %d)", DRUPAL_ANONYMOUS_RID, DRUPAL_AUTHENTICATED_RID));

  while ($recipient = db_fetch_object($result)) {
    if ($check_file_perms) {
      $account = user_load(array('uid' => $recipient->uid));
      $display_files = user_access('view uploaded files', $account);
    }
    else {
      $display_files = TRUE;
    }
    project_mail_output($node->title, 0);
    $subject = t('[!short_name] [!category] !title', array('!short_name' => $project->uri, '!category' => $node->category, '!title' => $node->title));
    $body = "$links\n". project_mail_generate_followup_mail_body($node, $history, $display_files);
    drupal_mail('project_issue_update', $recipient->mail, $subject, $body, $from, $header);
  }

  if (is_array($project->mail_copy_filter) && count(array_filter($project->mail_copy_filter)) && !$project->mail_copy_filter[$node->category]) {
    return;
  }

  if (is_array($project->mail_copy_filter_state) && count(array_filter($project->mail_copy_filter_state)) && !$project->mail_copy_filter_state[$node->sid]) {
    return;
  }

  if ($project->mail_copy) {
    drupal_mail('project_issue_update', $project->mail_copy, $subject, $body, $from, $header);
  }
}

/**
 * Provide an array of project issue summary fields.
 */
function project_mail_summary_fields() {
  return array(
    'pid' => t('Project'),
    'rid' => t('Version'),
    'component' => t('Component'),
    'category' => t('Category'),
    'priority' => t('Priority'),
    'assigned' => t('Assigned to'),
    'name' => t('Reported by'),
    'updator' => t('Updated by'),
    'sid' => t('Status')
  );
}

/**
 * Format the body of an issue followup email.
 *
 * @param $node
 *   The issue node.
 * @param $history
 *   An array containing the history of issue followups.
 * @param $display_files
 *   Boolean indicating if file attachments should be displayed.
 * @return
 *   A string of the email body.
 */
function project_mail_generate_followup_mail_body($node, $history, $display_files) {
  global $user;
  static $output_with_files =  NULL, $output_without_files = NULL;

  // Return cached output if available.
  if ($display_files) {
    if (isset($output_with_files)) {
      return $output_with_files;
    }
  }
  else {
    if (isset($output_without_files)) {
      return $output_without_files;
    }
  }

  $fields = project_mail_summary_fields();

  // Get most recent update.
  $entry = array_pop($history);
  $node->updator = $entry->name ? $entry->name : $user->name;

  // Check if the latest entry is actually the initial issue.
  if (empty($history)) {
    $content = $entry->body;
  }
  else {
    $previous = end($history);
    $content = $entry->comment;
  }

  // Mail summary (status values).
  $summary = '';
  foreach ($fields as $field => $text) {
    $text = str_pad("$text:", 14);
    if (isset($previous) && $previous->$field != $entry->$field && $field != 'updator' && $field != 'name') {
      $summary .= "-$text". project_mail_summary($field, $previous->$field) ."\n";
      $summary .= "+$text". project_mail_summary($field, $entry->$field) ."\n";
    }
    else {
      $summary .= " $text". project_mail_summary($field, $node->$field) ."\n";
    }
  }

  $summary .= project_mail_format_attachments($entry, $display_files);

  // Create main body content
  project_mail_output($content, 1, $entry->format);
  $body = "$content\n$entry->name\n";

  $hr = str_repeat('-', 72);

  if (count($history)) {

    $body .= "\n\n";
    $body .= t('Original issue:') ."\n";
    $body .= project_mail_format_entry(array_shift($history), $display_files, TRUE);
    if (count($history)) {
      $body .= "\n". t('Previous comments (!count):', array('!count' => count($history))) ."\n";
      foreach ($history as $entry) {
        $body .= project_mail_format_entry($entry, $display_files);
      }
    }
  }

  $output = "$summary\n$body";

  // Set cached output.
  if ($display_files) {
    $output_with_files = $output;
  }
  else {
    $output_without_files = $output;
  }

  return $output;
}

/**
 * Formats attachments for issue notification e-mails.
 *
 * @param $entry
 *   An issue or followup object containing the file data.
 * @param $display_files
 *   Boolean indicating if file attachments should be displayed.
 * @return
 *   A formatted string of file attachments.
 */
function project_mail_format_attachments($entry, $display_files) {
  $output = '';
  if ($display_files && is_array($entry->files)) {
    foreach ($entry->files as $file) {
      $output .= ' '. str_pad(t('Attachment') .':', 14) . file_create_url($file->filepath) .' ('. format_size($file->filesize) .")\n";
    }
  }
  return $output;
}

/**
 * Format an issue entry for display in an email.
 *
 * @param entry
 *   The entry to the formatted.
 * @param $display_files
 *   Boolean indicating if file attachments should be displayed.
 * @param is_original
 *   Whether this entry is the original issue or a followup. Followup issues
 *   will be automatically numbered.
 * @return
 *   Formatted text for the entry.
 */
function project_mail_format_entry($entry, $display_files, $is_original = FALSE) {
  static $history_count = 1;
  $hr = str_repeat('-', 72);
  $output = "$hr\n";

  // Nodes and comments have different stamp fields.
  $timestamp = isset($entry->created) ? $entry->created : $entry->timestamp;

  $output .= ($is_original ? '' : $history_count++ .'. ') . format_date($timestamp, 'large') ." : $entry->name\n";

  $output .= project_mail_format_attachments($entry, $display_files);

  // Must distinguish between nodes and comments.
  if (!$entry->vid) {
    $content = $entry->comment;
  }
  else {
    $content = $entry->body;
  }

  project_mail_output($content, 1, $entry->format);

  if ($content) {
    $output .= "\n$content";
  }
  return $output;
}

function project_mail_reminder() {
  global $base_url;

  $projects = array();
  $result = db_query(db_rewrite_sql('SELECT p.nid, n.title FROM {project_issue_projects} p INNER JOIN {node} n ON p.nid = n.nid WHERE p.mail_reminder = 1 AND n.status = 1', 'p'));
  while ($project = db_fetch_object($result)) {
    $projects[$project->nid] = $project->title;
    $pids[] = "p.pid = $project->nid";
  }
  if (count($projects)) {

    $sender->name = variable_get('site_name', '');
    $sender->mail = variable_get('site_mail', '');

    $result = db_query(db_rewrite_sql('SELECT p.nid, n.*, p.*, u.name, u.mail FROM {project_issues} p INNER JOIN {node} n ON p.nid = n.nid INNER JOIN {users} u ON n.uid = u.uid WHERE n.status = 1 AND u.status = 1 AND ('. implode(' OR ', $pids) .") AND u.mail <> '' AND (p.sid = 1 OR p.sid = 2) ORDER BY u.uid, p.pid, p.component, p.sid, n.changed DESC", 'p'));

    while (($node = db_fetch_object($result)) || !empty($body)) {
      if ($body && $mail && ((!$node) || ($mail != $node->mail))) {
        $header = array(
          'Return-Path' => "<$sender->mail;>",
          'Date' => date('r'),
          'From' => "$sender->name <$sender->mail>",
          'X-Mailer' => 'Drupal Project Issues module (http://drupal.org/project/project_issue)',
          'List-Id' => "$sender->name <project-reminder-". preg_replace('|.+://([a-zA-Z0-9\._-]+).*|', '\1', $base_url) .'>',
          'List-Archive' => '<'. url('project', NULL, NULL, 1) .'>',
        );

        drupal_mail('project_issue_reminder', $mail, t('Your submitted bugs for !date', array('!date' => date('F d, Y', time()))), $body, NULL, $header);

        $body = '';
      }
      $mail = $node->mail;

      if ($node->nid) {
        if ($pid != $node->pid || $component != $node->component) {
          $pid = $node->pid;
          $component = $node->component;
          $banner = "$projects[$pid] / $component";
          $body .= "[ $banner ]". str_repeat('=', 72 - 4 - strlen($banner)) ."\n";
        }
        $body .= "$node->title\n";
        if ($node->assigned && $assigned = user_load(array('uid' => $node->assigned))) {
          $body .= "  assigned: $assigned->name\n";
        }
        $body .= '  state: '. project_issue_state($node->sid) ."\n";
        $body .= '  age: '. format_interval(time() - $node->created) ."\n";
        $body .= '  url: '. url("node/$node->nid", NULL, NULL, 1) ."\n";
        $body .= "\n";
      }
    }
  }
}

function project_mail_digest() {
  global $base_url;

  $body = array();
  $header = array(
    'Date' => date('r'),
    'X-Mailer' => 'Drupal Project Issues module (http://drupal.org/project/project_issue)',
    'List-Id' => "$project->title <". preg_replace('|.+://([a-zA-Z0-9\._-]+).*|', '\1', $base_url) .'-project-issues-digest>',
    'List-Archive' => '<'. url('project/issues?priorities=1', NULL, NULL, 1) .'>',
  );

  $result = db_query(db_rewrite_sql("SELECT n.nid, n.title, p.* FROM {node} n INNER JOIN {project_issue_projects} p ON n.nid = p.nid WHERE n.status = 1 AND p.mail_digest <> '' ORDER BY n.title, p.mail_digest"));
  while ($project = db_fetch_object($result)) {
    $category = '';
    $result2 = db_query(db_rewrite_sql('SELECT p.nid, n.title, n.created, p.sid, p.category, p.component, p.priority, p.assigned FROM {project_issues} p INNER JOIN {node} n ON p.nid = n.nid WHERE n.status = 1 AND p.pid = %d AND p.sid = 1 AND p.priority = 1 ORDER BY p.category, n.created DESC', 'p'), $project->nid);
    while ($node = db_fetch_object($result2)) {
      if ($category != $node->category) {
        $category = $node->category;
        $banner = "$project->title / ". project_issue_category($node->category);
        $body[$project->mail_digest] .= "$banner\n". str_repeat('-', $banner) ."\n";
      }
      $body[$project->mail_digest] .= "$node->title\n";
      if ($node->assigned && $assigned = user_load(array('uid' => $node->assigned))) {
        $body[$project->mail_digest] .= '  assigned: '. $assigned->name ."\n";
      }
      $body[$project->mail_digest] .= '  age: '. format_interval(time() - $node->created) ."\n";
      $body[$project->mail_digest] .= '  url: '. url("node/$node->nid", NULL, NULL, 1) ."\n";
      $body[$project->mail_digest] .= "\n";
    }
  }

  foreach ($body as $mailto => $text) {
    $header['From'] = $mailto;
    $header['Return-Path'] = "<$mailto>";

    drupal_mail('project_issue_critical_summary', $mailto, t('Release critical bugs for !date', array('!date' => date('F d, Y', time()))), $text, NULL, $header);
  }
}
