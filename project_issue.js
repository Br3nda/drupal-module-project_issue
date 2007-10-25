/* $Id: project_issue.js,v 1.4 2007/10/25 04:44:04 thehunmonkgroup Exp $ */

Drupal.projectSwitchAutoAttach = function () {
  $('#edit-project-info-pid').change(function () {
    var pid = this.value;

    // Temporarily disable the dynamic selects while we retrieve the new info.
    // The newly loaded selects will be enabled by default.
    $('#edit-project-info-rid, #edit-project-info-component').attr('disabled', 'disabled');

    // Get existing component setting.
    var cid = $('#edit-project-info-component').val();
    cid = Drupal.encodeURIComponent(cid);

    // Get existing version label.
    var nid = $('#edit-project-info-rid').val();
    var rid = $('#edit-project-info-rid option[@value=' + nid + ']').text();
    rid = Drupal.encodeURIComponent(rid);

    // Pass new project ID, existing version, existing component.
    var url = Drupal.settings.projectUrl + '/' + pid + '/' + cid + '/' + rid;

    // Ajax GET request.
    $.ajax({
      type: 'GET',
      url: url,
      success: function (data) {
        // Parse result and insert updated selects.
        var result = Drupal.parseJson(data);
        if (result.error) {
          alert(result.error);
        }
        else {
          $('#edit-project-info-rid, #edit-project-info-component').parent().remove();
          $('#edit-project-info-pid').parent().after(result.component).after(result.rid);
        }
      },
      error: function (xmlhttp) {
        alert('An HTTP error '+ xmlhttp.status +' occured.\n'+ url);
      }
    });
  });
}

// Global killswitch.
if (Drupal.jsEnabled) {
  $(document).ready(Drupal.projectSwitchAutoAttach);
}
