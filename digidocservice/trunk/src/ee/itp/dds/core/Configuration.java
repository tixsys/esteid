package ee.itp.dds.core;

import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.jdom.Document;
import org.jdom.Element;
import org.springframework.beans.factory.InitializingBean;
import org.springframework.core.io.Resource;
import org.springframework.util.Assert;

import ee.itp.dds.core.SignatureModuleDesc.ContentType;
import ee.itp.dds.core.SignatureModuleDesc.Location;
import ee.itp.dds.core.SignatureModuleDesc.Phase;
import ee.itp.dds.core.SignatureModuleDesc.Platform;
import ee.itp.dds.core.SignatureModuleDesc.Type;
import ee.itp.dds.util.XmlUtil;

/**
 * DigiDocServices configuration holder
 * 
 * @author Aleksandr Vassin
 * @created 26.04.2009
 */
public class Configuration implements InitializingBean {

    private final String MODULES_PATH = "modules.path";
    private final String JDIGIDOC_CONFIG = "jdigidoc.config";
    private final String DEBUG_MODE = "debugMode";
    
    private static final Log log = LogFactory.getLog (Configuration.class);    
    
    /** The location. */
    private Resource location;

    /** The properties. */
    private Properties properties;

    private List<SignatureModuleDesc> signatureModuleDescs;

    /**
     * Gets the parameter.
     * 
     * @param key
     *            the key
     * 
     * @return the parameter
     */
    private String getParameter(final String key) {
        return properties.getProperty(key);
    }
    
    public boolean isDebugMode() {
       String dm = properties.getProperty(DEBUG_MODE);
       return (dm == null || dm.equals("false"))?false:true;
    }

    /**
     * Sets the location.
     * 
     * @param location
     *            the new location
     */
    public void setLocation(Resource location) {
        this.location = location;
    }

    public List<SignatureModuleDesc> getSignatureModules() {
        return signatureModuleDescs;
    }

    public String getModulesPath() {
        return getParameter(MODULES_PATH);
    }
    
    public String getJDigiDocConfig() {
        return getParameter(JDIGIDOC_CONFIG);
    }
    
    public List<SignatureModuleDesc> findSignatureModules(Platform platform, Phase phase , Type type) {
        List<SignatureModuleDesc> ret = new ArrayList<SignatureModuleDesc>();
        for (SignatureModuleDesc sm : this.signatureModuleDescs) 
            if (sm.getPlatforms().contains(platform) 
                    && sm.getPhases().contains(phase) 
                    && ( type.equals(Type.ALL) || (sm.getType().equals(type)))) 
                        ret.add(sm);
        return ret;
    }
    
    
    
    /**
     * After properties set.
     * 
     * @throws Exception
     *             the exception
     * 
     * @see org.springframework.beans.factory.InitializingBean#afterPropertiesSet()
     */
    public void afterPropertiesSet() throws Exception {
        Assert.notNull(location, "Location for configuration properties missing");
        properties = new Properties();
        signatureModuleDescs = new ArrayList<SignatureModuleDesc>();
        if (!location.exists()) {
            log.error("File [" + location.getFilename() + "] not found" );
            return;
        }
        Document doc = XmlUtil.parseXML(location.getFile());
        log.info("Loading DigiDoc services configuration ... ");
        Element root = doc.getRootElement();
        List<Element> p =  root.getChildren("property");
        for (Element e : p) {
            String name = e.getAttributeValue("name");
            String value = e.getAttributeValue("value");
            properties.put(name, value);
        }
        List<Element> mList =  root.getChildren("signature-module");
        for (Element module : mList) {
            SignatureModuleDesc sm = new SignatureModuleDesc();
            String[] phases = module.getAttributeValue("phase").split(",");
            sm.setPhases(phases);
            sm.setFile(module.getAttributeValue("file"));
            sm.setName(module.getAttributeValue("name"));
            sm.setLocation(Location.valueOf(module.getAttributeValue("location")));
            sm.setType(Type.valueOf(module.getAttributeValue("type")));
            String[] platforms = module.getAttributeValue("platform").split(",");
            sm.setPlatforms(platforms);
            this.signatureModuleDescs.add(sm);
        }
    }

}
