$(document).ready(function () {

    var fields = [
        'username',
        'uname',
        'user',
        'login',
        'user_name'
    ];

    function onSubmit(event) {
        for (var i = 0; i < fields.length; ++i) {
            $(event.target).find('input[name=' + fields[i] + ']').each(function (key, val) {
                safari.self.tab.dispatchMessage('sageaxcessMessage', JSON.stringify({
                    'action': 'usernameOnPostDetected',
                    'username': $(val).val()
                }));
            });
        }
    }

    $('input[type="password"]').each(function (key, val) {
        var inputs = $(val).closest('form').find('input').toArray();
        for (var i = inputs.indexOf(val) ; i >= 0; i--) {
            if ($(inputs[i]).attr('type') == 'text' || $(inputs[i]).attr('type') == 'email') {
                fields.push($(inputs[i]).attr('name'));
            }
        }
    });

    var forms = document.getElementsByTagName('form');
    for (var i = 0; i < forms.length; ++i) {
        forms[i].addEventListener('submit', onSubmit, false);
    }

});