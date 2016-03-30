// Saves options to chrome.storage.sync.
function save_options() {
  var agentUrl = document.getElementById('agentUrl').value;
  chrome.storage.sync.set({
    aegisAgentUrl: agentUrl
  }, function() {
    // Update status to let user know options were saved.
    var status = document.getElementById('status');
    status.textContent = 'Options saved.';
    setTimeout(function() {
      status.textContent = '';
    }, 750);
  });
}

function restore_options() {
  chrome.storage.sync.get({
    aegisAgentUrl: 'http://localhost:8111'
  }, function(items) {
    document.getElementById('agentUrl').value = items.aegisAgentUrl;
  });
}

document.addEventListener('DOMContentLoaded', restore_options);
document.getElementById('save').addEventListener('click',
    save_options);