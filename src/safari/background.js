var _port = '8111';
var _appUrl = 'http://localhost:' + _port + '/';
var _debug = false;

// array of { tab: SafariBrowserTab, username: string }
var _tabs = [];

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

safari.application.addEventListener('activate', function (event) {
    var tab = event.target;
    if (tab instanceof SafariBrowserWindow)
        tab = tab.activeTab;
    if (tab instanceof SafariBrowserTab) {
        for (var i = 0; i < _tabs.length; i++) {
            if (!_tabs[i].tab.browserWindow) {
                _tabs.splice(i, 1);
                i--;
            }
        }
        _tabs.push({ tab: tab, username: '' });
    }
}, true);

safari.application.addEventListener('close', function (event) {
    var tab = event.target;
    for (var i = 0; i < _tabs.length; i++) {
        if (_tabs[i].tab === tab || !_tabs[i].tab.browserWindow) {
            _tabs.splice(i, 1);
            i--;
        }
    }
}, true);

safari.application.addEventListener('message', function (event) {
    var message = JSON.parse(event.message);
    var tab = event.target;
    if (message.action == 'hashchange') {
        notifyApp('GET', message.url, message.username);
    }
    else if (message.action == 'complete') {
        var username = '';
        for (var i = 0; i < _tabs.length; i++) {
            if (_tabs[i].tab === tab) {
                username = _tabs[i].username;
                _tabs[i].username = '';
                break;
            }
        }
        if (username !== '')
            notifyApp('POST', message.url, username);
        else
            notifyApp('GET', message.url, message.username);
    }
    else if (message.action == 'usernameOnPostDetected') {
        for (var i = 0; i < _tabs.length; i++) {
            if (_tabs[i].tab === tab) {
                _tabs[i].username = message.username;
                break;
            }
        }
    }
}, false);

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


