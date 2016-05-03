var _appUrl = safari.extension.settings.appUrl || 'http://localhost:8111/';
var _fieldNamesUrl = safari.extension.settings.fieldNamesUrl || 'http://private-ad516-browserlog.apiary-mock.com/f';
var _requestFields = [];

$(function () {
    safari.application.addEventListener('beforeNavigate', function (event) {
        interceptRequest(event.url, 'GET');
    }, true);

    getFieldNames().then(function (fieldNames) {
        _requestFields = fieldNames;

        safari.application.addEventListener('message', function (event) {
            if (event.name === 'submit:sageaxcess') {
                interceptRequest(event.message.url, event.message.method, event.message.body);
            }
        }, false);
    });

    safari.extension.settings.addEventListener('change', function (event) {
        if (event.key === 'appUrl') {
            _appUrl = event.newValue;
        } else if (event.key === 'fieldNamesUrl') {
            _fieldNamesUrl = event.newValue;
        }
    }, false);
});

/**
 * Get field names to check for
 * @returns {promise}
 */
function getFieldNames() {
    var dfd = $.Deferred();

    $.ajax({
        url: _fieldNamesUrl
    }).done(function (response) {
        dfd.resolve(response.fieldnames)
    }).fail(function () {
        dfd.resolve([]);
    });

    return dfd.promise();
}

/**
 * Get all user requests and send it to the provided url
 * @param {string} url
 * @param {string} method
 * @param {Object} [body]
 */
function interceptRequest(url, method, body) {
    var urlRegExp = new RegExp(_appUrl + '\/?');

    if (url.search(urlRegExp) != -1) {
        return;
    }

    var data = {
        url: url,
        verb: method,
        username: ''
    };

    if (body && _requestFields) {
        _(_requestFields).forEach(function (field) {
            if (body && body[field]) {
                data.username = body[field];
                return false;
            }
        });
    }

    $.ajax({
        url: _appUrl,
        method: 'POST',
        data: JSON.stringify(data),
        dataType: 'json',
        contentType: 'application/json',
        processData: false
    });
}
