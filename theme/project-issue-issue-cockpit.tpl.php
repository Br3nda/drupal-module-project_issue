<?php
// $Id: project-issue-issue-cockpit.tpl.php,v 1.3 2009/04/13 09:20:49 dww Exp $
?>

<?php if ($make_issues): ?>
  <p>
    <?php print t("To avoid duplicates, please search before submitting a new issue."); ?>
  </p>
<?php endif; ?>

<?php if ($view_issues): ?>
  <div class="issue-cockpit-all">
    <?php print l(t('All Issues: !open open', array('!open' => $open)), $path); ?>
    <?php print l(t('(!total total)', array('!total' => $total)), $path, array('query' => 'status=All')); ?>
  </div>

  <?php print $form; ?>

  <div class="issue-cockpit-categories">
    <?php foreach($categories as $key => $category): ?>
      <div class="issue-cockpit-<?php print $key; ?>">
      <strong><?php print ucfirst($category['name']); ?></strong>
        <div class="issue-cockpit-totals">
        <?php print l(t('!open open', array('!open' => $category['open'])), $path, array('query' => 'categories='. $key)); ?>,
        <?php print l(t('!total total', array('!total' => $category['total'])), $path, array('query' => 'status=All&categories='. $key)); ?>
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
