function htmlLog(msg) {
    var e = document.getElementById("htmlLog");
    if(e) e.innerHTML = "[" + (new Date()) + "] " + 
                        msg + "<br />\n" + e.innerHTML;
}

function appendToBody(e) {
    var b = document.getElementsByTagName("body")[0];
    return b.appendChild(e);
}

function removeFromBody(e) {
    var b = document.getElementsByTagName("body")[0];
    b.removeChild(e);
}

function createEstEidObject(id) {
    htmlLog("Trying to load esteid browser plugin");

    var e = document.createElement('span');
    e.innerHTML = '<object style="width: 1px; height: 1px;" id="' + id +
                        '" type="application/x-esteid" />';
    e = appendToBody(e);

    return document.getElementById(id);
}

function loadEstEidPlugin(id, pluginReady) {
    var e = null;
    try {
        if(esteid_config && esteid_config.disable_new)
            throw({message: "New plugin support disabled by config"});

        /* Many browsers implement it the FF way, but IE shows an empty list */
        if(navigator.mimeTypes && navigator.mimeTypes.length > 0) {
            if(navigator.mimeTypes['application/x-esteid']) {
                e = createEstEidObject(id);
            } else {
                /* Do not attempt to load plugin if it's not there. Othervise
                   Firefox will show a yellow bar */
                htmlLog("Browser plugin (application/x-esteid) is not installed");
		throw({message: ''});
            }
        } else {
                e = createEstEidObject(id);
        }

         htmlLog("Using browser plugin: " + e.getVersion()); // Loaded OK?
         if(typeof(e.signAsync) == "undefined")
             throw({message: "You are using an experimental software version. Please update!"});
         pluginReady();
    }
    catch(err) {
        htmlLog("Failed to load esteid browser plugin: " + err.message);
        if(e) removeFromBody(e);

        if(load_legacy_esteid) {
            loadLegacySigner(id, pluginReady);
        } else {
            // Epic fail!
            htmlLog("Singing is impossible");
        }
    }
}
