<?php
// $Id: project-issue-issue-cockpit.tpl.php,v 1.4 2009/04/20 19:11:35 dww Exp $
?>

<?php if ($make_issues): ?>
  <?php print t('To avoid duplicates, please search before submitting a new issue.'); ?>
<?php endif; ?>

<?php if ($view_issues): ?>
  <?php print $form; ?>

  <div class="issue-cockpit-categories">
    <?php foreach($categories as $key => $category): ?>
      <div class="issue-cockpit-<?php print $key; ?>">
        <span class="category-header"><?php print ucfirst($categories[$key]['name']); ?></span>
        <div class="issue-cockpit-totals">
          <?php print l(t('!open open', array('!open' => $categories[$key]['open'])), $path, array('query' => 'categories='. $key)); ?>,
          <?php print l(t('!total total', array('!total' => $categories[$key]['total'])), $path, array('query' => 'status=All&categories='. $key)); ?>
        </div>
      </div>
    <?php endforeach; ?>
  </div>

  <div class="issue-cockpit-subscribe">
    <?php print $issue_subscribe; ?>
  </div>
  <div class="issue-cockpit-statistics">
    <?php print $issue_statistics; ?>
  </div>
  <div class="issue-cockpit-oldest">
    <?php print t('Oldest open issue: @date', array('@date' => format_date($oldest, 'custom', 'j M y'))); ?>
  </div>

<?php endif; ?>
