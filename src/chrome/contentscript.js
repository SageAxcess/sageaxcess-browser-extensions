var _fields = [
    'username',
    'uname',
    'user',
    'login',
    'user_name'
];

window.addEventListener('hashchange', function (e) {
    chrome.runtime.sendMessage({
        'url': document.location.href,
        'action': 'hashchange',
        'username': ''
    });
}, false);


