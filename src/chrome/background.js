var _port = '8111';
var _appUrl = 'http://localhost:' + _port;
var _fieldNamesUrl = 'http://private-ad516-browserlog.apiary-mock.com/f';
var _debug = false;
var _processingUrls = {};
var _requestTypes = ['main_frame', 'sub_frame', 'xmlhttprequest', 'other'];
var _requestFields = {};

/**
 * Get field names to check for
 */
function getFieldNames() {
    var dfd = $.Deferred();

    $.get(_fieldNamesUrl, function(fieldNames) {
        dfd.resolve(fieldNames);
    });

    return dfd.promise();
}

/**
 *  Get all user requests and send it to the provided url
 */
function interceptRequest(details) {
    var formData;
    var urlRegExp = new RegExp(_appUrl + '\/?');

    if (_requestTypes.indexOf(details.type) === -1 || details.url.search(urlRegExp) != -1) {
        return;
    }

    try {
        formData = details.requestBody.formData;
    } catch (e) {
        formData = {};
    }

    var data = {
        url: details.url,
        verb: details.method
    };

    _requestFields.forEach(function(field) {
        data[field] = formData[field] ? formData[field][0] : '';
    });

    $.ajax({
        url: _appUrl,
        method: 'POST',
        data: data
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



chrome.storage.sync.get({
  aegisAgentUrl: 'http://localhost:8111'
}, function(items) {
  _appUrl = items.aegisAgentUrl;
});

chrome.storage.onChanged.addListener(function(changes, areaName) {
   if(changes.aegisAgentUrl!=null && changes.aegisAgentUrl!=undefined)
     _appUrl = changes.aegisAgentUrl.newValue;
});

var _fields = [
    'username',
    'uname',
    'user',
    'login',
    'user_name'
];

function log() {
    if (!_debug)
        return;
    var str = [];
    for (var i = 0; i < arguments.length; i++) {
        str.push(typeof (arguments[i]) == "object" ? JSON.stringify(arguments[i]) : arguments[i]);
    }
    console.log(str.join(' '));
}

chrome.webRequest.onBeforeRequest.addListener(function (details) {
    var username = '';
    if (details.requestBody && details.requestBody.formData) {
        log(details.requestBody.formData);
        var formData = details.requestBody.formData;
        for (var field in formData) {
            if (_fields.indexOf(field.toLowerCase()) != -1) {
                if (Object.prototype.toString.call( formData[field] ) === '[object Array]') {
                   username = formData[field][0];
                } else {
                   username = formData[field];
                }
                break;
            }
        }
    }
    _processingUrls[details.requestId] = { verb: details.method, username: username };
}, { urls: ['<all_urls>'], types: ['main_frame', 'sub_frame'] }, ['requestBody']);

function processResponse(details) {
    if (_processingUrls[details.requestId]) {
        var completed = _processingUrls[details.requestId];
        delete _processingUrls[details.requestId];
        if (completed.username != '') {
            var url = details.url;
            var p = url.indexOf('?');
            if (p != -1) {
                url = url.substr(p + 1);
                var tmp = url.substring(0, url.indexOf('#'));
                if (tmp.length > 0)
                    url = tmp;
                var pairs = url.split('&');
                for (var i = 0; i < pairs.length; ++i) {
                    var v = pairs[i].split('=');
                    if (v && v.length == 2 && _fields.indexOf(v[0].toLowerCase()) != -1) {
                        completed.username = v[1];
                        completed.verb = 'GET';
                        break;
                    }
                }
            }
        }
        notifyApp(completed.verb, details.url, completed.username);
    }
}

chrome.webRequest.onCompleted.addListener(function (details) {
   processResponse(details);
}, { urls: ['<all_urls>'] }, ['responseHeaders']); //, types: ['main_frame', 'sub_frame']

chrome.webRequest.onHeadersReceived.addListener(function (details) {
   processResponse(details);
}, { urls: ['<all_urls>'] }, ['responseHeaders']); //, types: ['main_frame', 'sub_frame']

chrome.runtime.onMessage.addListener(function (message, sender, sendResponse) {
    if (message.action == 'hashchange') {
        notifyApp('GET', message.url, message.username);
    }
    else if (message.action == 'fieldsFound') {
        _fields = message.fields.concat(_fields);
    }
});

function notifyApp(verb, url, username) {
    var t = Date.now();
    var params = {
        verb: verb,
        url: url,
        username: username
    };
    try {
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.open("POST", _appUrl + '?_=' + t, true);
        xmlhttp.setRequestHeader("Content-Type", "application/json; charset=UTF-8");
        xmlhttp.onreadystatechange = function (e) {
            if (xmlhttp.readyState === 4) {
                if (xmlhttp.status === 200) {

                }
                else {

                }
            }
        };
        xmlhttp.send(JSON.stringify(params));
    }
    catch (e) {
        log(e);
    }
    log(params);
}


