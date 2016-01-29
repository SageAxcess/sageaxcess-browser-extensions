window.addEventListener("hashchange", function (e) {
    var completed = processUrl(document.location.href);
    helper.onHashchange({ url: document.location.href });
}, false);

helper.onComplete({ url: document.location.href, host: document.location.host + document.location.pathname });


