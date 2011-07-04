package ee.smartlink.esteid;

import java.util.Map;

import com.vaadin.Application;
import com.vaadin.ui.*;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Window.Notification;

import ee.smartlink.esteid.EstEidComponent.EstEidEvent;
import ee.smartlink.esteid.EstEidComponent.PersonInfoLoadedEvent;
import ee.smartlink.esteid.data.PersonInfo;


public class EsteidApplication extends Application {
	private Button b;
	private TextField tf;
	private Button sign;

	@Override
	public void init() {
		 final Window mainWindow = new Window("Ie6crash Application");
	        final EstEidComponent estEid = new EstEidComponent();
	        
	        estEid.addListener(new EstEidComponent.EstEidCardListener() {
				
				@Override
				public void onEstEidEvent(EstEidEvent event) {
					switch (event.getEventType()) {
					case CARD_INSERTED:	
						mainWindow.showNotification("Card inserted",event.getMessage(),Notification.TYPE_HUMANIZED_MESSAGE);
						b.setEnabled(true);
						tf.setEnabled(true);
						sign.setEnabled(true);
						break;
					case CARD_REMOVED:
						mainWindow.showNotification("Card removed","jup",Notification.TYPE_HUMANIZED_MESSAGE);
						b.setEnabled(false);
						tf.setEnabled(false);
						sign.setEnabled(false);
						break;
					case ON_ERROR:
						mainWindow.showNotification("Error",event.getMessage(),Notification.TYPE_ERROR_MESSAGE);
						break;
					case PLUGIN_READY:
						mainWindow.showNotification("Plugin ready",event.getMessage(),Notification.TYPE_HUMANIZED_MESSAGE);
						break;
					case SIGN_FAILURE:
						mainWindow.showNotification("Sign failure",event.getMessage(),Notification.TYPE_ERROR_MESSAGE);
						break;
					case SIGN_SUCCESS:
						mainWindow.showNotification("Sign success",event.getMessage(),Notification.TYPE_HUMANIZED_MESSAGE);
						break;
					default:
						break;
					}					
				}
			});
	        estEid.addListener(new EstEidComponent.PersonInfoLoadedListener() {
				@Override
				public void onLoaded(PersonInfoLoadedEvent event) {
					mainWindow.showNotification("Customer loaded","Welcome "+event.getPersonInfo().getFirstName(), Notification.TYPE_HUMANIZED_MESSAGE);
				}

				@Override
				public PersonInfo getPersonInfo() {
					// TODO Auto-generated method stub
					return null;
				}
			});
	        
	        estEid.init();
	        b = new Button("Load customer info");
	        b.addListener(new Button.ClickListener() {
	            @Override
	            public void buttonClick(ClickEvent event) {
	                estEid.LoadCustomerInfo();
	            	//PersonInfo pi=estEid.getPersonInfo();
	            	
	            }
	        });
	        
	        tf = new TextField("demoHash");
	        tf.setValue("26fd6dbee37c55c14af480f00daff8817b61967e");
	        sign = new Button("Sign the hash");
	        sign.addListener(new Button.ClickListener() {
	            @Override
	            public void buttonClick(ClickEvent event) {
	                estEid.startSign((String)tf.getValue());
	            }
	        });
	        
	        mainWindow.addComponent(b);
	        mainWindow.addComponent(estEid);
	        mainWindow.addComponent(tf);
	        mainWindow.addComponent(sign);
	        
	        setMainWindow(mainWindow);
	}

}
