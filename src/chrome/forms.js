$(document).ready(function () {
    var _fields = [];

    $("input[type='password']").each(function (key, val) {
        var inputs = $(val).closest("form").find("input").toArray();
        for (var i = inputs.indexOf(val) ; i >= 0; i--) {
            if ($(inputs[i]).attr("type") == "text" || $(inputs[i]).attr("type") == "email")
                _fields.push($(inputs[i]).attr("name"));
        }
    });

    if (_fields.length > 0) {
        chrome.runtime.sendMessage({ action: "fieldsFound", fields: _fields });
    }
});
