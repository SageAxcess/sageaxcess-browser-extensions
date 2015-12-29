var _port = '8111';
var _appUrl = 'http://localhost:' + _port + '/';
var _debug = true;
var _processingUrls = {};

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
    for (i = 0; i < arguments.length; i++) {
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
                username = formData[field];
                break;
            }
        }        
    }
    _processingUrls[details.requestId] = { verb: details.method, username: username };
}, { urls: ['<all_urls>'], types: ['main_frame', 'sub_frame'] }, ['requestBody']);

chrome.webRequest.onCompleted.addListener(function (details) {
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
}, { urls: ['<all_urls>'], types: ['main_frame', 'sub_frame'] }, ['responseHeaders']);

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


