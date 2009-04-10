<?php
// $Id: project-issue-issue-cockpit.tpl.php,v 1.1 2009/04/10 22:15:24 dww Exp $
?>

<?php if ($make_issues): ?>
  <p>
    <?php print t("To avoid duplicates, please check the queues for your issue before submitting a new one. As you search or browse you will find a '<strong>Create a new issue</strong>' link."); ?>
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
        <?php print l(t('!category: !open open', array('!open' => $category['open'], '!category' => ucfirst($category['name']))), $path, array('query' => 'categories='. $key)); ?>
        <?php print l(t('(!total total)', array('!total' => $category['total'])), $path, array('query' => 'status=All&categories='. $key)); ?>
      </div>
    <?php endforeach; ?>
  </div>

  <div class="issue-cockpit-oldest">
    <?php print t('Oldest open issue: @date', array('@date' => format_date($oldest, 'custom', 'j M y'))); ?>
  </div>
  <div class="issue-cockpit-subscribe">
    <?php print $issue_subscribe; ?>
  </div>

<?php endif; ?>
