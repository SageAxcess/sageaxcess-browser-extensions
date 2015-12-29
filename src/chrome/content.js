window.addEventListener('hashchange', function (e) {
    chrome.runtime.sendMessage({
        'url': document.location.href,
        'action': 'hashchange',
        'username': ''
    });
}, false);


