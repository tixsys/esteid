package ee.smartlink.esteid.gwt.client.ui;

import com.google.gwt.dom.client.Document;
import com.google.gwt.user.client.ui.Widget;
import com.google.gwt.user.client.Timer;
import com.vaadin.terminal.gwt.client.ApplicationConnection;
import com.vaadin.terminal.gwt.client.Paintable;
import com.vaadin.terminal.gwt.client.UIDL;

public class VEstEidComponent extends Widget implements Paintable{

    /** Set the tagn ame used to statically resolve widget from UIDL. */
    public static final String TAGNAME = "esteidcomponent";

    /** Set the CSS class name to allow styling. */
    public static final String CLASSNAME = "v-"+TAGNAME;

    /** The client side widget identifier */
    protected String uidlId;

    /** Reference to the server connection object. */
    ApplicationConnection client;

     /** current state **/
    private int state = 0;


    /** plugin tag ID in HTML */
    private String pluginId;

    /**
     * The constructor should first call super() to initialize the component and
     * then handle any initialization relevant to Vaadin.
     */
    public VEstEidComponent() {
        // TODO Example code is extending GWT Widget so it must set a root element.
        // Change to proper element or remove if extending another widget
        setElement(Document.get().createDivElement());

        // This method call of the Paintable interface sets the component
        // style name in DOM tree
        setStyleName(CLASSNAME);

        // set the plugin ID
        pluginId = "VEstEid" + (int)(Math.random() * 20 + 1);
    }


    public String getPluginId() {
        return pluginId;
    }

    /**
     * Called whenever an update is received from the server
     */
    public void updateFromUIDL(UIDL uidl, ApplicationConnection client) {
        if (client.updateComponent(this, uidl, true)) {
            // If client.updateComponent returns true there has been no changes and we
            // do not need to update anything.
            return;
        }

        this.client = client;

        // Save the client side identifier (paintable id) for the widget
        uidlId = uidl.getId();

        if(uidl.getBooleanAttribute("initialize"))
        {
            initialize();
        }

        if(uidl.hasAttribute("sign"))
        {
            String hash = uidl.getStringAttribute("signHash");
            sign(hash, "url");
        }
        if(uidl.hasAttribute("getCert"))
        {
            getCert();
        }
    }

    public void onPluginReady(String ver) {
        client.updateVariable(uidlId, "onPluginReady", ver, true);
    }

    public void onPluginFailure(String msg) {
        client.updateVariable(uidlId, "onPluginFailure", msg, true);
    }

    public void onSignSuccess(String hex) {
        client.updateVariable(uidlId, "onSignSuccess", hex, true);
    }

    public void onSignFailure(String msg) {
        client.updateVariable(uidlId, "onSignFailure", msg, true);
    }

    public void onCertificateLoaded(String cert) {
        client.updateVariable(uidlId, "onCertificateLoaded", cert, true);
    }

    public void onCertificateFailure(String msg) {
        client.updateVariable(uidlId, "onCertificateFailure", msg, true);
    }

    public void getCert() {
        final int timeout = 120;

        Timer t = new Timer() {
            int n = 0;
            public void run() {
                String cert = "";
                try {
                    cert = getSignCert();
                } catch(Exception e) { }

                if(cert != null && !cert.equals("")) {
                    onCertificateLoaded(cert);
                    cancel();
                }

                if(n > timeout) {
                    onCertificateFailure("Timed out waiting for certificate");
                    cancel();
                }
                n++;
            }
        };
        t.scheduleRepeating(1000);
    };

    // Use JSNI to grab the JSON object we care about
    // The JSON object gets its Java type implicitly
    // based on the method's return type

    native boolean isSupported() /*-{
      return typeof $wnd.estEidLoader != "undefined";
    }-*/;

    native void sign(String hash, String url) /*-{
        var that = this;
        var pluginId = this.@ee.smartlink.esteid.gwt.client.ui.VEstEidComponent::getPluginId() ();
        var plugin = $wnd.document.getElementById(pluginId);
        if(!plugin) {
            that.@ee.smartlink.esteid.gwt.client.ui.VEstEidComponent::onSignFailure(Ljava/lang/String;) (pluginId + ' not found in page');
            return;
        }
        try {
            plugin.signAsync(hash, url, {
                onSuccess: function(hex) {
                    that.@ee.smartlink.esteid.gwt.client.ui.VEstEidComponent::onSignSuccess(Ljava/lang/String;) (hex);
                },
                onError: function(msg) {
                    that.@ee.smartlink.esteid.gwt.client.ui.VEstEidComponent::onSignFailure(Ljava/lang/String;) (msg);
                }
            });
        } catch(err) {
            that.@ee.smartlink.esteid.gwt.client.ui.VEstEidComponent::onSignFailure(Ljava/lang/String;) (err.toString())
        }
    }-*/;

    private native String getSignCert() /*-{
        return $wnd.document.getElementById(this.@ee.smartlink.esteid.gwt.client.ui.VEstEidComponent::getPluginId() ()).signCert.cert;
    }-*/;

    private native void initialize() /*-{
        var that = this;
        var pluginId = this.@ee.smartlink.esteid.gwt.client.ui.VEstEidComponent::getPluginId() ();

        if (typeof($wnd.estEidLoader) != "undefined")
        {
            $wnd.estEidLoader.disable_java = true;
            try {
                $wnd.estEidLoader.loadPlugin(pluginId, {
                    pluginReady: function() {
                        var ver = $wnd.document.getElementById(pluginId).getVersion();
                        that.@ee.smartlink.esteid.gwt.client.ui.VEstEidComponent::onPluginReady(Ljava/lang/String;) (ver);
                    },
                    pluginFail: function(msg) {
                        that.@ee.smartlink.esteid.gwt.client.ui.VEstEidComponent::onPluginFailure(Ljava/lang/String;) (msg);
                    },
                });
            } catch(err) {
                this.@ee.smartlink.esteid.gwt.client.ui.VEstEidComponent::onPluginFailure(Ljava/lang/String;) (err.toString());
            }
        } else {
            this.@ee.smartlink.esteid.gwt.client.ui.VEstEidComponent::onPluginFailure(Ljava/lang/String;) ('esteid-plugin-loader js Library not available');
        }
    }-*/;

}
