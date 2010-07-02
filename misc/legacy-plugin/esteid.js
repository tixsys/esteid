function htmlLog(msg) {
    var e = document.getElementById("htmlLog");
    if(e) e.innerHTML = "[" + (new Date()) + "] " + 
                        msg + "<br />\n" + e.innerHTML;
}

function createEstEidObject(id) {
    htmlLog("Trying to load esteid browser plugin");

    var e = document.createElement('object');
    e.id           = id;
    e.type         = 'application/x-esteid';
    e.style.width  = '1px';
    e.style.height = '1px';
    var b = document.getElementsByTagName("body")[0];
    return b.appendChild(e);
}

function loadEstEidPlugin(id, pluginReady) {
    try {
        var e = null;
        if(navigator.mimeTypes) {
            if(navigator.mimeTypes['application/x-esteid'])
                e = createEstEidObject(id);
            else
                /* Do not attempt to load plugin if it's not there. Othervise
                   Firefox will show a yellow bar */
                htmlLog("Browser plugin (application/x-esteid) is not installed");
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

        if(load_legacy_esteid) {
            loadLegacySigner(id, pluginReady);
        } else {
            // Epic fail!
            htmlLog("Singing is impossible");
        }
    }
}
