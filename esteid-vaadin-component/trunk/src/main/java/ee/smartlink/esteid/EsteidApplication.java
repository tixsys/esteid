package ee.smartlink.esteid;

import java.util.Map;

import com.vaadin.Application;
import com.vaadin.ui.*;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Window.Notification;

import ee.smartlink.esteid.EstEidComponent.EstEidEvent;

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
					case PLUGIN_READY:
						mainWindow.showNotification("Plugin ready",event.getMessage(),Notification.TYPE_HUMANIZED_MESSAGE);
						estEid.pollCert();
						break;
					case PLUGIN_FAILURE:
						mainWindow.showNotification("Plugin error",event.getMessage(),Notification.TYPE_ERROR_MESSAGE);
						tf.setEnabled(false);
						sign.setEnabled(false);
						break;
					case CERT_LOADED:
						mainWindow.showNotification("Certificate loaded",event.getMessage(),Notification.TYPE_HUMANIZED_MESSAGE);
						tf.setEnabled(true);
						sign.setEnabled(true);
						break;
					case CERT_FAILURE:
						mainWindow.showNotification("Cert failure",event.getMessage(),Notification.TYPE_ERROR_MESSAGE);
						tf.setEnabled(false);
						sign.setEnabled(false);
						break;
					case SIGN_SUCCESS:
						mainWindow.showNotification("Sign success",event.getMessage(),Notification.TYPE_HUMANIZED_MESSAGE);
						break;
					case SIGN_FAILURE:
						mainWindow.showNotification("Sign failure",event.getMessage(),Notification.TYPE_ERROR_MESSAGE);
						break;
					default:
						break;
					}
				}
			});

		estEid.init();

		tf = new TextField("demoHash");
		tf.setValue("26fd6dbee37c55c14af480f00daff8817b61967e");
		sign = new Button("Sign the hash");
		sign.addListener(new Button.ClickListener() {
		    @Override
		    public void buttonClick(ClickEvent event) {
			estEid.startSign((String)tf.getValue());
		    }
		});

		mainWindow.addComponent(estEid);
		mainWindow.addComponent(tf);
		mainWindow.addComponent(sign);

		setMainWindow(mainWindow);
	}
}
