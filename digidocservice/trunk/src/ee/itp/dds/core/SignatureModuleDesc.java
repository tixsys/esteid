package ee.itp.dds.core;

import java.util.ArrayList;
import java.util.List;

public class SignatureModuleDesc {

    public enum Platform { LINUX_MOZILLA,WIN32_MOZILLA,WIN32_IE }
    public enum Phase { PREPARE,FINALIZE }
    public enum Type { FILE,HTML,ALL }
    public enum ContentType { BASE64 }
    public enum Location { 
        HTML_HEAD,HTML_FORM_BEGIN,HTML_FORM_END,HTML_BODY,HTML_SCRIPT,LIBDIR
    }

    private List<Phase> phases;
    private List<Platform> platforms;
    private Type type;
    private Location location;
    private String name;
    private String file;
    private ContentType contentType;

    public SignatureModuleDesc() {
        phases = new ArrayList<Phase>();
        platforms = new ArrayList<Platform>();
        contentType = ContentType.BASE64; //default content type and only one witch supports 
    }


    public Type getType() {
        return type;
    }


    public void setType(Type type) {
        this.type = type;
    }


    public Location getLocation() {
        return location;
    }


    public void setLocation(Location location) {
        this.location = location;
    }


    public String getName() {
        return name;
    }


    public void setName(String name) {
        this.name = name;
    }


    public String getFile() {
        return file;
    }


    public void setFile(String file) {
        this.file = file;
    }


    public ContentType getContentType() {
        return contentType;
    }


    public void setContentType(ContentType contentType) {
        this.contentType = contentType;
    }


    public List<Phase> getPhases() {
        return phases;
    }


    public void setPhases(List<Phase> phases) {
        this.phases = phases;
    }

    public void setPhases(String[] phases) {
        for (String phase : phases)
            if (phase != null && phase.length() > 0)
                this.phases.add(Phase.valueOf(phase));
    }
    
    

    public List<Platform> getPlatforms() {
        return platforms;
    }

    public void setPlatforms(List<Platform> platforms) {
        this.platforms = platforms;
    }
    
    public void setPlatforms(String[] platforms) {
        for (String p : platforms)
            if (p != null && p.length() > 0)
                this.platforms.add(Platform.valueOf(p));
    }
    

}
