package ee.smartlink.esteid.widgetset.client.ui;

import com.google.gwt.dom.client.Document;
import com.google.gwt.user.client.ui.Widget;
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
            initialize(this);
        }

        if(uidl.getBooleanAttribute("loadPersonInfo"))
        {
            String person=getPersonInfo();
            client.updateVariable(uidlId, "onPersonInfoLoaded", person, true);
        }

        if(uidl.getBooleanAttribute("getCertBase64"))
        {
            String cert=getCertBase64();
            client.updateVariable(uidlId, "getCertBase64", cert, true);
        }

        if(uidl.hasAttribute("sign"))
        {
            String hash=uidl.getStringAttribute("signHash");
            sign(this, hash);
        }
    }

    public void onPluginReady(String cert) {
        client.updateVariable(uidlId, "onPluginReady", cert, true);
    }

    public void onSignSuccess(String hex) {
        client.updateVariable(uidlId, "onSignSuccess", hex, true);
    }

    public void onSignFailure(String msg) {
        client.updateVariable(uidlId, "onSignFailure", msg, true);
    }

    public void onCardInserted(String cert) {
        client.updateVariable(uidlId, "onCardInserted", cert, true);
    }

    public void onCardRemoved() {
        client.updateVariable(uidlId, "onCardRemoved", true, true);
    }

    public void onError(String msg) {
        client.updateVariable(uidlId, "onError", msg, true);
    }

     // Use JSNI to grab the JSON object we care about
     // The JSON object gets its Java type implicitly
     // based on the method's return type
     public native String getPersonInfo() /*-{
        // Get a reference to the first customer in the JSON array from earlier
        return $wnd.JSON.stringify($wnd.esteid_component.readPersonInfo());
     }-*/;

    native boolean isSupported() /*-{
    return typeof $wnd.esteid_component != "undefined";
    }-*/;

    native void sign(VEstEidComponent component, String hash) /*-{
        $wnd.esteid_component.doSign(hash, "url", {
            onSuccess:function(hex) {
                component.@ee.smartlink.esteid.widgetset.client.ui.VEstEidComponent::onSignSuccess(Ljava/lang/String;) (hex);
            },
            onError:function(msg) {
                component.@ee.smartlink.esteid.widgetset.client.ui.VEstEidComponent::onSignFailure(Ljava/lang/String;) (msg);
            }
        });
    }-*/;

    native String getCertBase64() /*-{
        return $wnd.esteid_component.getCertBase64();
    }-*/;

    public native void initialize(VEstEidComponent component) /*-{
        var self=this;
        if (typeof($wnd.esteid_component)!="undefined")
        {
            $wnd.esteid_component.loadPlugin('__esteid', {
                pluginReady: function(cert) {
                    component.@ee.smartlink.esteid.widgetset.client.ui.VEstEidComponent::onPluginReady(Ljava/lang/String;) (cert);
                },
                pluginFail: function(msg) {
                    component.@ee.smartlink.esteid.widgetset.client.ui.VEstEidComponent::onError(Ljava/lang/String;) (msg);
                },
                onCardInserted: function(cert) {
                    component.@ee.smartlink.esteid.widgetset.client.ui.VEstEidComponent::onCardInserted(Ljava/lang/String;) (cert);
                },
                onCardRemoved: function() {
                    component.@ee.smartlink.esteid.widgetset.client.ui.VEstEidComponent::onCardRemoved() ();
                },
                onSignSuccess: function(msg) {
                    component.@ee.smartlink.esteid.widgetset.client.ui.VEstEidComponent::onSignSuccess(Ljava/lang/String;) (msg);
                },
                onSignFailure: function(msg) {
                    component.@ee.smartlink.esteid.widgetset.client.ui.VEstEidComponent::onSignFailure(Ljava/lang/String;) (msg);
                }
            });
        }else{
            component.@ee.smartlink.esteid.widgetset.client.ui.VEstEidComponent::onError(Ljava/lang/String;) ('esteid js Library not available');
        }
    }-*/;

}
