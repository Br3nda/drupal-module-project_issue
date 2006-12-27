/* $Id: project_issue.js,v 1.1 2006/12/27 19:44:40 dww Exp $ */

function mod_project(base, pid) {
  if (Drupal.jsEnabled) {
    // Temporarily disable the dynamic selects while we retrieve the new info.
    // The newly loaded selects will be enabled by default.
    $('#edit-rid, #edit-component').attr('disabled', 'disabled');

    // Get existing component setting.
    var cid = $('#edit-component').val();
    cid = Drupal.encodeURIComponent(cid);

    // Get existing version label.
    var nid = $('#edit-rid').val();
    var rid = $('#edit-rid option[@value=' + nid + ']').text();
    rid = Drupal.encodeURIComponent(rid);

    // Ajax GET request.
    $.ajax({
      type: 'GET',
      url: base + pid + '/' + cid + '/' + rid,
      success: function (data) {
        // Parse back result
        var result = Drupal.parseJson(data);
  	    $('#edit-rid, #edit-component').parent().remove();
  	    $('#edit-pid').parent().after(result['component']).after(result['rid']);
      },
      error: function (xmlhttp) {
        alert('An HTTP error '+ xmlhttp.status +' occured.\n'+ url);
      }
    });
  }
}
