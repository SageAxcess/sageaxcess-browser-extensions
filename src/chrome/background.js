var _appUrl = 'http://localhost:8111';
var _fieldNamesUrl = 'http://private-ad516-browserlog.apiary-mock.com/f';
var _debug = false;
var _requestTypes = ['main_frame', 'sub_frame', 'xmlhttprequest', 'other'];
var _requestFields = [];

/**
 * Get field names to check for
 */
function getFieldNames() {
    var dfd = $.Deferred();

    $.get(_fieldNamesUrl, function (fieldNames) {
        dfd.resolve(fieldNames);
    });

    return dfd.promise();
}

/**
 *  Get all user requests and send it to the provided url
 */
function interceptRequest(details) {
    var formData = {};
    var urlRegExp = new RegExp(_appUrl + '\/?');

    if (_requestTypes.indexOf(details.type) === -1 || details.url.search(urlRegExp) != -1) {
        return;
    }

    formData = (details.requestBody && details.requestBody.formData) ? details.requestBody.formData : {};

    var data = {
        url: details.url,
        verb: details.method,
        username: ''
    };

    _(_requestFields).forEach(function (field) {
        if (formData && formData[field]) {
            data.username = formData[field][0];
            return false;
        }
    });

    $.ajax({
        url: _appUrl,
        method: 'POST',
        data: JSON.stringify(data),
        dataType: 'json',
        contentType: 'application/json',
        processData: false
    });
}

/**
 * Initiate Chrome background events
 */
function initiateEvents() {
    getFieldNames().then(function (fieldNames) {
        _requestFields = fieldNames.fieldnames;
        chrome.webRequest.onBeforeRequest.addListener(interceptRequest, {urls: ['<all_urls>']}, ['blocking', 'requestBody']);
    });
}

initiateEvents();

chrome.storage.sync.get(['aegisAgentUrl', 'aegisAgentFieldUrl'], function (items) {
    _appUrl = items.aegisAgentUrl || _appUrl;
    _fieldNamesUrl = items.aegisAgentFieldUrl || _fieldNamesUrl;
});

chrome.storage.onChanged.addListener(function (changes, areaName) {
    if (changes.aegisAgentUrl) {
        _appUrl = changes.aegisAgentUrl.newValue;
    }
    if (changes.aegisAgentFieldUrl) {
        _fieldNamesUrl = changes.aegisAgentFieldUrl.newValue;
    }
});

function log() {
    if (!_debug) {
        return;
    }

    var str = [];
    for (var i = 0; i < arguments.length; i++) {
        str.push(typeof (arguments[i]) == 'object' ? JSON.stringify(arguments[i]) : arguments[i]);
    }
    console.log(str.join(' '));
}
