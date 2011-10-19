package ee.smartlink.esteid;

import java.util.Map;
import com.vaadin.terminal.PaintException;
import com.vaadin.terminal.PaintTarget;
import com.vaadin.ui.AbstractComponent;
import com.vaadin.ui.Component;
import com.vaadin.ui.Window.Notification;

/**
 * Server side component for the VEstEidComponent widget.
 * @author Lauri Lüüs.
 */

@com.vaadin.ui.ClientWidget(ee.smartlink.esteid.widgetset.client.ui.VEstEidComponent.class)
public class EstEidComponent extends AbstractComponent {

    private static final long serialVersionUID = 6901433173868836522L;
    private String action = null;
    private String hash = null;

    public interface EstEidCardListener{
        public void onEstEidEvent(EstEidEvent event);
    }

    public void addListener(EstEidCardListener listener) {
        try {
            addListener(EstEidEvent.class, listener, EstEidCardListener.class.getDeclaredMethod("onEstEidEvent", new Class[] { EstEidEvent.class }));
        } catch (NoSuchMethodException e) {
            throw new java.lang.RuntimeException("Internal error finding methods in EstEidComponent");
        }
    }

    /**
     * EstEid event. This event is thrown, when the event is handled.
     * @author Lauri Lüüs.
     */
    public class EstEidEvent extends Component.Event {

        private static final long serialVersionUID = -567533739849771393L;
        private EstEidEventType eventType;
        private String message;

        /**
         * New instance of text change event.
         *
         * @param source
         *            the Source of the event.
         */
        public EstEidEvent(Component source) {
            super(source);
        }

        /**
         * Gets the EstEidComponent where the event occurred.
         *
         * @return the Source of the event.
         */
        public EstEidComponent getEstEidComponent() {
            return (EstEidComponent) getSource();
        }

        public EstEidEventType getEventType() {
            return eventType;
        }

        public String getMessage() {
            return message;
        }

        public void setEventType(EstEidEventType eventType) {
            this.eventType = eventType;
        }

        public void setMessage(String message) {
            this.message = message;
        }
    }

    public enum EstEidEventType {
        PLUGIN_READY, PLUGIN_FAILURE,
        CERT_LOADED, CERT_FAILURE,
        SIGN_SUCCESS, SIGN_FAILURE
    }

    /*
     * (non-Javadoc)
     * Deserialize changes received from client.
     * @see com.vaadin.ui.AbstractComponent#changeVariables(java.lang.Object,
     * java.util.Map)
     */
    @SuppressWarnings("unchecked")
    @Override
    public void changeVariables(Object source, Map variables) {
            EstEidEvent event = new EstEidEvent(this);
            if (variables.containsKey("onPluginReady")) {
                event.setMessage(variables.get("onPluginReady").toString());
                event.setEventType(EstEidEventType.PLUGIN_READY);
            }
            if (variables.containsKey("onPluginFailure")) {
                event.setEventType(EstEidEventType.PLUGIN_FAILURE);
                event.setMessage(variables.get("onPluginFailure").toString());
            }
            if (variables.containsKey("onCertificateLoaded")) {
                event.setEventType(EstEidEventType.CERT_LOADED);
                event.setMessage(variables.get("onCertificateLoaded").toString());
            }
            if (variables.containsKey("onCertificateFailure")) {
                event.setEventType(EstEidEventType.CERT_FAILURE);
                event.setMessage(variables.get("onCertificateFailure").toString());
            }
            if (variables.containsKey("onSignSuccess")) {
                event.setEventType(EstEidEventType.SIGN_SUCCESS);
                event.setMessage(variables.get("onSignSuccess").toString());
            }
            if (variables.containsKey("onSignFailure")) {
                event.setEventType(EstEidEventType.SIGN_FAILURE);
                event.setMessage(variables.get("onSignFailure").toString());
            }

            if(event.getEventType()!=null)
                fireEvent(event);
    }

    public void init() {
        action = "initialize";
    }

    @Override
    public void paintContent(PaintTarget target) throws PaintException {
        super.paintContent(target);

        // Paint any component specific content by setting attributes
        // These attributes can be read in updateFromUIDL in the widget.
        target.addAttribute(action, true);
        if(action == "sign" && this.hash != null)
            target.addAttribute("signHash", this.hash);
    }

    public void startSign(String hash)
    {
        action = "sign";
        this.hash = hash;
        requestRepaint();
    }

    public void pollCert()
    {
        action = "getCert";
        requestRepaint();
    }
}
