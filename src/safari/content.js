$(function () {
    $(document).on('submit', function (event) {
        var $form = $(event.target);
        var url = event.target.action;
        var body = {};

        $form.find('input').each(function () {
            var $input = $(this);
            body[$input.attr('name')] = $input.val();
        });

        safari.self.tab.dispatchMessage('submit:sageaxcess', {
            url: url,
            body: body,
            method: 'POST'
        });
    });
});
