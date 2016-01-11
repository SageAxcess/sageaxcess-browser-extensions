var _port = '8111';
var _appUrl = 'http://localhost:' + _port + '/';
var _debug = false;
var _processingUrls = {};

function log() {
    if (!_debug)
        return;
    var str = [];
    for (i = 0; i < arguments.length; i++) {
        str.push(typeof (arguments[i]) == 'object' ? JSON.stringify(arguments[i]) : arguments[i]);
    }
    console.log(str.join(' '));
}

if (!Array.prototype.indexOf) {
    Array.prototype.indexOf = function (obj, start) {
        for (var i = (start || 0), j = this.length; i < j; i++) {
            if (this[i] === obj) { return i; }
        }
        return -1;
    }
}

var _fields = [
    'username',
    'uname',
    'user',
    'login',
    'user_name'
];

helper.addUsernameMask(_fields);

function processUrl(url) {
    var completed = null;
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
                completed = {};
                completed.url = url;
                completed.username = v[1];
                completed.verb = 'GET';
                return completed;
            }
        }
    }
    return completed;
}

function onCompleted(ignorePost) {
    if (ignorePost === false) {
        var username = helper.getUsername();
        if (username !== '') {
            notifyApp('POST', document.location.href, username);
            return;
        }
    }
    var completed = processUrl(document.location.href);
    if (completed) {
        notifyApp(completed.verb, completed.url, completed.username);
        return;
    }
    notifyApp('URL', document.location.href, '');
}

onCompleted(false);

function notifyApp(verb, url, username) {
    var t = (new Date()).getTime();
    var params = {
        verb: verb,
        url: url,
        username: username
    };
    try {
        var xmlhttp = helper.getXMLHttpRequest();
        xmlhttp.open('POST', _appUrl + '?_=' + t, true);
        xmlhttp.setRequestHeader('Content-Type', 'application/json; charset=UTF-8');
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

if (typeof window.addEventListener != 'undefined') {
    window.addEventListener('hashchange', function (e) {
        onCompleted(true);
    }, false);
}
else {
    window.attachEvent('onhashchange', function (e) {
        onCompleted(true);
    });
}





