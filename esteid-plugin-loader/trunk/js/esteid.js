var estEidLoader = {
  pluginTimeout: 10,
  waitForObjectStart: null,

  /**
   * Log a message to DOM element with htmlLog
   *
   * @param {String} msg a message to log
   */
  htmlLog: function(msg) {
    if(window.console && console.log) {
      console.log(msg);
    }

    var e = document.getElementById("htmlLog");
    if(e) e.innerHTML = "[" + (new Date()) + "] " + 
                        msg + "<br />\n" + e.innerHTML;
  },

  /**
   * Append element to documents "body" tag
   *
   * @param {DOMObject} e an element to add
   * @return a reference to the added object
   */
  appendToBody: function(e) {
    var b = document.getElementsByTagName("body")[0];
    return b.appendChild(e);
  },

  /**
   * Remove element from documents "body" tag
   * @param {DOMObject} e an element to remove
   */
  removeFromBody: function(e) {
    try {
        var b = document.getElementsByTagName("body")[0];
        b.removeChild(e);
    } catch(err) {
        estEidLoader.htmlLog("Failed to remove object from body: " + err.message);
    }
  },

  /**
   * Wait for an object to start responding.
   *
   * @param {String} id id of the object to poll
   * @param {int} timeout how many seconds to wait
   * @param {Object} callbacks an object that has following properties:
   *   isWorking a function to validate the loaded object
   *   onSuccess a function to be called on success
   *   onFail a function to be called on failure
   */
  waitForObject: function(id, timeout, callbacks) {
    var e = document.getElementById(id);
    var ok = false;

    /* Return immediately if everything is OK */
    try { ok = callbacks.isWorking(e); } catch(err) { }
    if(ok) {
      estEidLoader.waitForObjectStart = null;
      callbacks.onSuccess(e);
      return;
    }

    /* Check if browser has completed loading of the plugin.
     * If plugin is loaded but does not respond it must of failed. */
    if(typeof e.readyState !== "undefined" &&
        (e.readyState == 4 || e.readyState == "complete")) {
      estEidLoader.htmlLog("Plugin ready, but does not respond. Stop waiting.");
      callbacks.onFail(e);
      return;
    }

    /* Set starting timestamp */
    if(!estEidLoader.waitForObjectStart) {
      estEidLoader.waitForObjectStart = (new Date()).getTime();
    }

    /* Check time */
    var now = (new Date()).getTime();
    var millis = now - estEidLoader.waitForObjectStart
    if(millis > timeout * 1000) {
      estEidLoader.waitForObjectStart = null;
      callbacks.onFail(e);
      return;
    }

    /* Schedule another check */
    estEidLoader.htmlLog("Waiting for plugin to load ...");
    window.setTimeout(function() {
      estEidLoader.waitForObject(id, timeout, callbacks);
    }, 1000);
  },

  /**
   * Inject an object tag into DOM with specified id and mimeType.
   * The object will not be created if the specified MIME type is not
   * listed in plugin registry othervise Firefox will show a
   * "missing plugin" notification bar.
   *
   * @parem {String} id id for the new object
   * @param {String} mimeType MIME type of the new object
   * @param {Object} callbacks an object that has following properties:
   *   isWorking a function to validate the loaded object
   *   onSuccess a function to be called on success
   *   onFail a function to be called on failure
   */
  createMimeObject: function(id, mimeType, callbacks) {
    var e = null;
    var errpfx = "Failed to load plugin for " + mimeType + ": ";

    /* Check if the requested MIME type is available in the plugin registry.
     * If we try to create an object for MIME type that is not available,
     * Firefox will show a notification bar asking the user to install it.
     *
     * For browsers that do not expose plugin registry, we try to load
     * the object anyway. There is another quirk that must be taken into
     * account: many browsers implement the registry in Firefox way,
     * but IE exposes an empty list. That's why we must check
     * if the length of the list is greater than zero below.
     */
    if(navigator.mimeTypes && navigator.mimeTypes.length > 0 &&
       !(navigator.mimeTypes[mimeType])) {
      estEidLoader.htmlLog("Plugin for " + mimeType + " is not installed");
      callbacks.onFail(e);
      return;
    }

    try {
      e = document.createElement('span');
      e.innerHTML = '<object style="width: 1px; height: 1px;" id="' + id +
                          '" type="' + mimeType + '" />';
      e = estEidLoader.appendToBody(e);
    } catch(err) {
      estEidLoader.htmlLog(errpfx + err.message);
      callbacks.onFail(e);
      return;
    }

    /* Wait for object to become active */
    estEidLoader.waitForObject(id, estEidLoader.pluginTimeout, {
      isWorking: callbacks.isWorking,
      onSuccess: callbacks.onSuccess,
      onFail: function(e) {
        estEidLoader.htmlLog(errpfx + "timed out while waiting");
        callbacks.onFail(e);
      }
    });
  },

  /**
   * Inject an esteid plugin object into DOM using a specified id
   *
   * @param {String} id id of the new object
   * @param {Object} callbacks an object wirh following properties
   *   onSuccess called on successful load
   *   onFail called on failure
   */
  createEstEidObject: function(id, callbacks) {
    if(estEidLoader.disable_new) {
      estEidLoader.htmlLog("New plugin support disabled by config");
      callbacks.onFail(null);
      return;
    }

    estEidLoader.htmlLog("Trying to load esteid browser plugin");

    estEidLoader.createMimeObject(id, "application/x-esteid", {
      isWorking: function(e) { return (e.getVersion()) ? true : false; },
      onSuccess: function(e) {
        if(typeof(e.signAsync) == "undefined") {
          estEidLoader.htmlLog("Failed to utilize plugin, You are using an experimental software version, please update!");
          callbacks.onFail(e);
        } else {
          estEidLoader.htmlLog("Using browser plugin: " + e.getVersion());
          callbacks.onSuccess(e);
        }
      },
      onFail: callbacks.onFail
    });
  },

  /**
   * Load esteid signing plugin into document.
   * The function will attempt to find the esteid browser plugin
   * or load some legacy signing plugin if it's not available.
   * Will call pluginReady when plugin is loaded or pluginFail
   * when no plugin could be activated.
   */
  loadPlugin: function(id, callbacks) {
    estEidLoader.createEstEidObject(id, {
      onSuccess: function(e) { callbacks.pluginReady(); },
      onFail: function(e) {
        if(e) estEidLoader.removeFromBody(e);

        if(estEidLoader.loadLegacySigner) {
          estEidLoader.loadLegacySigner(id, callbacks);
        } else {
          callbacks.pluginFail();
        }
      }
    });

    return;
  }
}

/* Compatibility function. This shall be removed soon */
function loadEstEidPlugin(id, pluginReady) {
  estEidLoader.htmlLog("WARNING You are using loadEstEidPlugin compatibility function that will be removed soon");

  if(!load_legacy_esteid)
    estEidLoader.loadLegacySigner = null;

  if(esteid_config)
    for(i in esteid_config)
      estEidLoader[i] = esteid_config[i];

  estEidLoader.loadPlugin(id, pluginReady, {
      pluginReady: pluginReady,
      pluginFail: function() {
        estEidLoader.htmlLog("Signing is impossible");
      }
  });
}
