const { classes: Cc, interfaces: Ci,	utils: Cu } = Components;

Cu.import("resource://gre/modules/Services.jsm");
Cu.import("resource://gre/modules/AddonManager.jsm");

var self = this;

var prefs = null;
var _appUrl = "http://localhost:8111";

var _settingsObserver = {
	observe: function(subject, topic, data)
	{
		if (topic != "nsPref:changed")
		{
			return;
		}

		switch(data)
		{
			case "agent.url":
				_appUrl = prefs.getCharPref("agent.url");
				break;
		}
	}
};

function include(path, context) {
    Services.scriptloader.loadSubScript(getResourceURI(path), context || self);
}

function getResourceURI(filePath) {
    return __SCRIPT_URI_SPEC__ + "/../" + filePath;
}

function startup(aData, aReason) {
    include("includes/utils.js");

    prefs = Components.classes["@mozilla.org/preferences-service;1"]
				.getService(Components.interfaces.nsIPrefService)
				.getBranch("sageaxcess.");

    prefs.QueryInterface(Components.interfaces.nsIPrefBranch2);
    prefs.addObserver("", _settingsObserver, false);

    _appUrl = prefs.getCharPref("agent.url");

    watchWindows(function(window) {

        if (!window)
            return;

        try {
            include("chrome/content/background.js", window);
        }
        catch (e) {
            console.log(e);
        }

        unload(function() {

        }, window);

    }, "navigator:browser");
}

function shutdown(aData, aReason) {
    prefs.removeObserver("", _settingsObserver);

    if (aReason == APP_SHUTDOWN) 
        return;

    if (aReason == ADDON_DISABLE || aReason == ADDON_UNINSTALL)
        unload();

}

function install() {}

function uninstall() {}