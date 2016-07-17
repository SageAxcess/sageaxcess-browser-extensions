/**
 * Get inputs with text and email types
 * @param {jQuery} $form
 * @returns {jQuery}
 */
function getPossibleInputs($form) {
    return $('input[type="text"], input[type="email"]', $form);
}

/**
 * Get password type input
 * @param {jQuery} $form
 * @returns {jQuery}
 */
function getPasswordInput($form) {
    return $('input[type="password"]:eq(0)', $form);
}

/**
 * Get email type input
 * @param {jQuery} $form
 * @returns {jQuery}
 */
function getEmailInput($form) {
    $('input[type="email"]:eq(0)', $form);
}

/**
 * Get a position number of the password input inside the form
 * @param {jQuery} $form
 * @returns {number}
 */
function getPasswordInputPositionNumber($form) {
    var currentNumber = 0;

    $('input[type="text"], input[type="password"], input[type="email"]', $form).each(function(i) {
        var $input = $(this);
        currentNumber = i + 1;

        if ($input.attr('type') === 'password') {
            return false;
        }
    });

    return currentNumber;
}

/**
 *
 * @param {jQuery} $possibleInputs
 * @returns {string|boolean}
 */
function getUsernameByInputName($possibleInputs) {
    var $input = null;
    var isFound = false;
    var nameRegExp = /(name|login|username)/ig;

    $possibleInputs.each(function () {
        $input = $(this);
        if ($input.attr('name').search(nameRegExp) !== -1) {
            isFound = true;
            return false;
        }
    });

    return isFound ? $input.val() : false;
}

function getUsernameByCheckingPossibleInputs($form) {
    var AMOUNT_OF_INPUTS_TO_CONSIDER_FORM_SMALL = 4;

    var $passwordInput = getPasswordInput($form);
    var $emailInput = getEmailInput($form);
    var $possibleInputs = getPossibleInputs($form);

    if (!$possibleInputs.length) {
        return false;
    }

    // Don't bother if form contains only 1 field
    if (($possibleInputs.length + $passwordInput.length) < 2) {
        return false;
    }

    // If form contains 2 fields and one of them is password, then another one is username
    if ($possibleInputs.length === 1 && $passwordInput.length) {
        return $possibleInputs.get(0).value;
    }

    // If form contains small amount of fields, an email field is most likely a username
    if ($possibleInputs.length < AMOUNT_OF_INPUTS_TO_CONSIDER_FORM_SMALL && $passwordInput.length && $emailInput) {
        return $emailInput.val();
    }

    // If form contains more than 2 fields and password is the second one, then the first input is a username
    if ($possibleInputs.length > 1 && getPasswordInputPositionNumber($form) === 2) {
        return $possibleInputs.eq(0).val();
    }

    return getUsernameByInputName($possibleInputs);
}

function sendFormSubmitMessage(data) {
    chrome.runtime.sendMessage(data, function(response) {
        console.log('Username: ', data.username);
    });
}

function initiateEvents() {
    $(document).on('submit', function (event) {
        var username = getUsernameByCheckingPossibleInputs($form);

        if (!username) {
            return;
        }

        var $form = $(event.target);
        var data = {
            url: location.href,//$form.attr('action') || location.href,
            verb: $form.attr('method') || 'POST',
            username: username
        };

        sendFormSubmitMessage(data);
    });
}

initiateEvents();
