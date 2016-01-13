if (window == window.top) {

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

    window.addEventListener('hashchange', function (e) {
        var completed = processUrl(document.location.href);
        safari.self.tab.dispatchMessage('sageaxcessMessage', JSON.stringify({
            'url': document.location.href,
            'action': 'hashchange',
            'username': completed ? completed.username : ''
        }));
    }, false);

    var completed = processUrl(document.location.href);
    safari.self.tab.dispatchMessage('sageaxcessMessage', JSON.stringify({
        'url': document.location.href,
        'action': 'complete',
        'username': completed ? completed.username : ''
    }));
}


