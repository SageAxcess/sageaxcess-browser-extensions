var _appUrl = 'https://local.sageaxcess.com:8112/';
var _fieldNamesUrl = 'http://private-ad516-browserlog.apiary-mock.com/f';
var _fieldNames = [];

/**
 * Get field names to check for
 * @returns {promise}
 */
function getFieldNames() {
    var dfd = $.Deferred();

    $.ajax({
        url: _fieldNamesUrl
    }).done(function (response) {
        dfd.resolve(response.fieldnames);
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

    if (body && _fieldNames.length) {
        _(_fieldNames).forEach(function (field) {
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

/**
 * Get form data on submit and make a request
 * @param {event} event
 */
function handleSubmitEvent(event) {
    var $form = $(event.target);
    var url = event.target.action || location.href;
    var method = $form.attr('method');
    var body = {};
    alert(url);
    $form.find('input').each(function () {
        var $input = $(this);
        body[$input.attr('name')] = $input.val();
    });

    interceptRequest(url, 'POST', body);
}

$(function () {
    getFieldNames().then(function (fieldNames) {
        _fieldNames = fieldNames;
        $(document).on('submit', function (event) {
            handleSubmitEvent(event)
        });
    });

    interceptRequest(location.href, 'GET');
});
