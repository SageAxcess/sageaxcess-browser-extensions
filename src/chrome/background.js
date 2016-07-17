var _appUrl = 'https://local.sageaxcess.com/';
var _fieldNamesUrl = 'http://private-ad516-browserlog.apiary-mock.com/f';
var _debug = false;
var _requestTypes = ['main_frame', 'sub_frame', 'xmlhttprequest', 'other'];
var _requestFields = [];

/**
 * Get field names to check for
 * @returns {promise}
 */
function getFieldNames() {
    var dfd = $.Deferred();

    $.ajax({
        url: _fieldNamesUrl
    }).done(function (response) {
        dfd.resolve(response.fieldnames);
    }).fail(function () {
        dfd.resolve([]);
    });

    return dfd.promise();
}

/**
 * Get all user requests information
 * @param {Object} details
 */
function captureRequest(details) {
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

    submitData(data);
}

/**
 * Get all form submissions
 * @param {Object} data
 */
function captureFormSubmission(data) {
    if (data.username) {
        console.log('Got it from ' + data.url +  ' via form sending: ', data.username);
        submitData(data);
    }
}

/**
 * Submit the data from the captured request
 * @param {Object} data
 */
function submitData(data) {
    var dfd = $.Deferred();

    $.ajax({
        url: _appUrl,
        method: 'POST',
        data: JSON.stringify(data),
        dataType: 'json',
        contentType: 'application/json',
        processData: false
    }).always(function() {
        dfd.resolve();
    });

    return dfd.promise();
}

/**
 * Initiate Chrome background events
 */
function initiateEvents() {
    chrome.runtime.onMessage.addListener(function(data) {
        captureFormSubmission(data);
    });

    getFieldNames().then(function (fieldNames) {
        _requestFields = fieldNames;
        chrome.webRequest.onBeforeRequest.addListener(captureRequest, {
            urls: ['<all_urls>']
        }, ['blocking', 'requestBody']);
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
