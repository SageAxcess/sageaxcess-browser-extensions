const { classes: Cc, interfaces: Ci,	utils: Cu } = Components;

Cu.import("resource://gre/modules/Services.jsm");
Cu.import("resource://gre/modules/AddonManager.jsm");

var self = this;

function include(path, context) {
    Services.scriptloader.loadSubScript(getResourceURI(path), context || self);
}

function getResourceURI(filePath) {
    return __SCRIPT_URI_SPEC__ + "/../" + filePath;
}

function startup(aData, aReason) {

    include("includes/utils.js");

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

    if (aReason == APP_SHUTDOWN) 
        return;

    if (aReason == ADDON_DISABLE || aReason == ADDON_UNINSTALL)
        unload();

}

function install() {}

function uninstall() {}