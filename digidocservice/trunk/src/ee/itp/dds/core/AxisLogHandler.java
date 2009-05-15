package ee.itp.dds.core;

import org.apache.axis.AxisFault;
import org.apache.axis.Message;
import org.apache.axis.MessageContext;
import org.apache.axis.handlers.BasicHandler;
import org.apache.axis.utils.Messages;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * Axis incoming/outgoing SOAP Messages logger.
 * 
 * @author Aleksandr Vassin
 * @created  11.04.2009
 */
public class AxisLogHandler extends BasicHandler {

  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = -212224240233682256L;
  
  /** The LOG. */
  protected static Log LOG = LogFactory.getLog(AxisLogHandler.class);

  /* (non-Javadoc)
   * @see org.apache.axis.handlers.BasicHandler#init()
   */
  public void init() {
    super.init();
  }

  /* (non-Javadoc)
   * @see org.apache.axis.Handler#invoke(org.apache.axis.MessageContext)
   */
  public void invoke(MessageContext messageContext) throws AxisFault {
    LOG.debug("Enter: AxisLogHandler::invoke");
    logMessages(messageContext);
    LOG.debug("Exit: AxisLogHandler::invoke");

  }

  /* (non-Javadoc)
   * @see org.apache.axis.handlers.BasicHandler#onFault(org.apache.axis.MessageContext)
   */
  public void onFault(MessageContext messageContext) {
    try {
      logMessages(messageContext);
    }
    catch (AxisFault axisFault) {
      LOG.error(Messages.getMessage("exception00"), axisFault);
    }
  }

  /**
	 * Log messages.
	 * 
	 * @param messageContext
	 *            the message context
	 * 
	 * @throws AxisFault
	 *             the axis fault
	 */
  private void logMessages(MessageContext messageContext) throws AxisFault {
    try {
      Message inMsg = messageContext.getRequestMessage();
      Message outMsg = messageContext.getResponseMessage();
      
      
      
      if (inMsg != null && outMsg != null) {
    	  LOG.info("IN MESSAGE:\r\n" + inMsg.getSOAPPartAsString());
    	  LOG.info("OUT MESSAGE:\r\n" + outMsg.getSOAPPartAsString());
      }

      
      ReceivedDataHolder.setHost(((String[])inMsg.getMimeHeaders().getHeader("host"))[0]); //.setMessage(inMsg.getSOAPPartAsBytes());

    } catch (Exception e) {
      LOG.error(Messages.getMessage("exception00"), e);
      throw AxisFault.makeFault(e);
    }
  }
}
