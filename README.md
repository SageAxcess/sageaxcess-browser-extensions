# sageaxcess-browser-extensions
The browser extensions used for data collection in the Aegis platform

Requirements information can be found in /docs.

All code is under the /src directory and each browser has its own folder:

Chrome: /src/chrome   
Edge: /src/edge (when extensions become available)   
Firefox: /src/firefox   
Internet Explorer: /src/ie   
Safari: /src/safari  

## Compile

### Chrome

To publish in Chrome Web Store, create folder SageAxcess and copy everything from src/chrome.

Also, copy key file src/chrome-key.pem to SageAxcess/key.pem

Then, zip folder (folder itself, not it's contents) - and upload this archive to CWS.

It is possible to compile .crx file from command line, using command:

chrome --pack-extension=src\chrome --pack-extension-key=src\chrome-key.pem

It will make .crx file - which is now not supported in any of Chrome version.

### Firefox

Install node.js with npm

Install jpm tool with "npm install jpm" 

Then, go to plugin folder src\firefox and run

jpm xpi

jpm sign --xpi null.xpi --api-key your_api_key_here --api-secret your_api_secret

Unfortunately, it's possible to sign same version only once. It's impossible (or hard) to find/download signed plugin in case if you remove file.

Api keys can be retrieved at addons.mozilla.org developer portal

### IE

Install Visual Studio 2013, Windows SDK 7.1, NSIS

Compile src\ie\ie.sln for both architectures - x86 and x64. You may do this using "msbuild ie.sln /p:Platform=x86" and "msbuild ie.sln /p:Platform=x64".
In this case you don't have to open Visual Studio IDE.

Run src\ie\makeieinstaller.bat

### Safari


## Install

### Chrome

Windows: Create a new key equal to extension id under HKEY_LOCAL_MACHINE\Software\Wow6432Node\Google\Chrome\Extensions. Create property "update_url"=https://clients2.google.com/service/update2/crx

Linux: Copy extension_id.json to /opt/google/chrome/extensions or /usr/share/google-chrome/extensions

OSX: Copy extension_id.json to ~/Library/Application Support/Google/Chrome/External Extensions/

extension_id.json - extension_id must be replaced by real extension id.

File contents to load extension from Chrome Web Store:

{
    "external_update_url": "https://clients2.google.com/service/update2/crx"
}

https://developer.chrome.com/extensions/external_extensions

### Firefox

Windows: copy signed .xpi file to C:\Program Files (x86)\Mozilla Firefox\browser\extensions\ . It will be available when you restart browser. File must have this name: sageaxcess@sageaxcess.com.xpi

Linux: copy signed .xpi file to /usr/share/mozilla/extensions/\{ec8030f7-c20a-464f-9b0e-13a3a9e97384\}/ . It will be available when you restart browser. File must have this name: sageaxcess@sageaxcess.com.xpi

(not tested) OS X: copy signed .xpi file to /Library/Application Support/Mozilla/Extensions/

### IE

Just run .exe installer, it will do the rest. Call it with /S to do it silently.

### Safari


