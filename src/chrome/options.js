/**
 * Save options
 */
function saveOptions() {
    var agentUrl = document.getElementById('agentUrl').value;
    var fieldsUrl = document.getElementById('fieldsUrl').value;

    chrome.storage.sync.set({
        aegisAgentUrl: agentUrl,
        aegisAgentFieldUrl: fieldsUrl
    }, function () {
        var status = document.getElementById('status');
        status.textContent = 'Options saved.';
        setTimeout(function () {
            status.textContent = '';
        }, 750);
    });
}

/**
 * Set the default values of options
 */
function restoreOptions() {
    chrome.storage.sync.set({
        aegisAgentUrl: 'https://local.sageaxcess.com:8112/',
        aegisAgentFieldUrl: 'http://private-ad516-browserlog.apiary-mock.com/f'
    }, function () {
        chrome.storage.sync.get(['aegisAgentUrl', 'aegisAgentFieldUrl'], function (items) {
            document.getElementById('agentUrl').value = items.aegisAgentUrl;
            document.getElementById('fieldsUrl').value = items.aegisAgentFieldUrl;
        });
    });
}

document.addEventListener('DOMContentLoaded', function () {
    chrome.storage.sync.get(['aegisAgentUrl', 'aegisAgentFieldUrl'], function (items) {
        if (!items.aegisAgentUrl || !items.aegisAgentFieldUrl) {
            restoreOptions();
        } else {
            document.getElementById('agentUrl').value = items.aegisAgentUrl;
            document.getElementById('fieldsUrl').value = items.aegisAgentFieldUrl;
        }
    });
});

document.getElementById('save').addEventListener('click', saveOptions);
document.getElementById('restore').addEventListener('click', restoreOptions);


