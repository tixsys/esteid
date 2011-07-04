package ee.smartlink.esteid;

import java.util.Map;
import com.google.gson.Gson;
import com.google.gson.JsonSyntaxException;
import com.vaadin.terminal.PaintException;
import com.vaadin.terminal.PaintTarget;
import com.vaadin.ui.AbstractComponent;
import com.vaadin.ui.Component;
import com.vaadin.ui.Window.Notification;

import ee.smartlink.esteid.data.PersonInfo;



/**
 * Server side component for the VEstEidComponent widget.
 * @author Lauri Lüüs.
 */

@com.vaadin.ui.ClientWidget(ee.smartlink.esteid.widgetset.client.ui.VEstEidComponent.class)
public class EstEidComponent extends AbstractComponent {

    private static final long serialVersionUID = 6901433173868836522L;
    private String action=null;
    private String cert64=null;
	private String hash=null;
	private String cert;
    private PersonInfoLoadedEvent personInfo;
	
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
	
	public void addListener(PersonInfoLoadedListener listener) {
    	try {
			addListener(PersonInfoLoadedEvent.class, listener, PersonInfoLoadedListener.class.getDeclaredMethod("onLoaded", new Class[] { PersonInfoLoadedEvent.class }));
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
    
    public enum EstEidEventType { CARD_INSERTED, CARD_REMOVED, ON_ERROR, PLUGIN_READY, SIGN_FAILURE, SIGN_SUCCESS}
    
    
    
    /**
     * This event will be thrown when user data has readed from the card 
     * @author Lauri Lüüs.
     */
    public class PersonInfoLoadedEvent extends Component.Event{
		private static final long serialVersionUID = 4737133317161132894L;
		
		private PersonInfo personInfo;
		public PersonInfoLoadedEvent(Component source) {
			super(source);			
		}
		public void setPersonInfo(PersonInfo personInfo) {
			this.personInfo = personInfo;
		}
		public PersonInfo getPersonInfo() {
			return personInfo;
		}
		
		
		
	}
    
    public interface PersonInfoLoadedListener{
        public ee.smartlink.esteid.data.PersonInfo getPersonInfo();
		public void onLoaded(PersonInfoLoadedEvent event);
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

    		if (variables.containsKey("onPersonInfoLoaded")) {        	
				PersonInfoLoadedEvent personLoadedEvent=new PersonInfoLoadedEvent(this);
				Object data=variables.get("onPersonInfoLoaded");
				
				PersonInfo pi=new Gson().fromJson(data.toString(), PersonInfo.class);
				personLoadedEvent.setPersonInfo(pi);
				synchronized (OBJ_LOCK) {
					fireEvent(personLoadedEvent);
				}
				return;
			}
			
			EstEidEvent event = new EstEidEvent(this);
			if (variables.containsKey("onPluginReady")) { 
				event.setMessage(variables.get("onPluginReady").toString());
				event.setEventType(EstEidEventType.PLUGIN_READY);
			}
			if (variables.containsKey("onSignSuccess")) {     
				event.setMessage(variables.get("onSignSuccess").toString());
				event.setEventType(EstEidEventType.SIGN_SUCCESS);
			}
			if (variables.containsKey("onSignFailure")) {        	
				event.setEventType(EstEidEventType.SIGN_FAILURE);
				event.setMessage(variables.get("onSignFailure").toString());
			}
			if (variables.containsKey("onCardInserted")) {   
				event.setEventType(EstEidEventType.CARD_INSERTED);
			}
			if (variables.containsKey("onCardRemoved")) {        	
			    event.setEventType(EstEidEventType.CARD_REMOVED);
			}
			if (variables.containsKey("onError")) {        	
				event.setEventType(EstEidEventType.ON_ERROR);
				event.setMessage(variables.get("onError").toString());
			}
			if(event.getEventType()!=null)
				fireEvent(event);
		
    }
	
	public String getCert64() {
		action = "getCertBase64";
		requestRepaint();
		return cert64;
	}
	
	public void init() {
		// TODO Auto-generated method stub
    	action="initialize";
	}  
	
	public void LoadCustomerInfo(){
    	action="loadPersonInfo";
    	requestRepaint();
    }
	private static final Object OBJ_LOCK = new Object(); //better
	/**
	 * Load person info from card, this call is blocking call, so yo should wait a bit for this method to execute
	 * @return
	 */
	/*public PersonInfo getPersonInfo(){
		
		EstEidComponent.PersonInfoLoadedListener listener=new EstEidComponent.PersonInfoLoadedListener() {
			private PersonInfo personInfo;
			public PersonInfo getPersonInfo(){
				return personInfo;
			}			
			@Override
			public void onLoaded(PersonInfoLoadedEvent event) {
				this.personInfo = event.getPersonInfo();
			}
		};
		try {
			addListener(listener);
			LoadCustomerInfo();
			Object sync=new Object();
			synchronized (OBJ_LOCK) {
				listener.wait(1000*30);//wait til 30sec
			}
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			//e.printStackTrace();
			return null;
		}
		  
		return listener.getPersonInfo();
		
	}*/
	
	@Override
    public void paintContent(PaintTarget target) throws PaintException {
        super.paintContent(target);

        // Paint any component specific content by setting attributes
        // These attributes can be read in updateFromUIDL in the widget.
        target.addAttribute(action, true);
        if(action=="sign" && this.hash!=null)
        	target.addAttribute("signHash", this.hash);       
    }
	
	public void startSign(String hash)
    {
    	action="sign";
    	this.hash=hash;
    	requestRepaint();
    }
}

