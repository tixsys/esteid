package ee.itp.dds.hibernate;

import java.util.HashSet;
import java.util.Set;

/**
 * Session entity.
 * @author Aleksandr Vassin
 * @created  11.04.2009
 */
public class Session implements java.io.Serializable {

  // Fields

  private Long id;
  private String code;
  private String host;
  private Long  createDate;
  private Long  accessDate;
  private Long  closeDate;
  private Set<Event> events = new HashSet<Event>(0);
  private Document document;

  // Constructors

  public Document getDocument() {
    return document;
  }

  public void setDocument(Document document) {
    this.document = document;
  }

  /** default constructor */
  public Session() {
  }

  /** minimal constructor */
  public Session(Long id, String code) {
    this.id = id;
    this.code = code;
  }

  /** full constructor */
  public Session(Long id, String code, String ip, Long  createDate, Long  accessDate, Long  closeDate, Set<Event> events) {
    this.id = id;
    this.code = code;
    this.host = ip;
    this.createDate = createDate;
    this.accessDate = accessDate;
    this.closeDate = closeDate;
    this.events = events;
  }

  // Property accessors

  public Long getId() {
    return this.id;
  }

  public void setId(Long id) {
    this.id = id;
  }

  public String getCode() {
    return this.code;
  }
  
  public int getCodeInt() {
     return Integer.valueOf(this.code).intValue();
  }

  public void setCode(String code) {
    this.code = code;
  }

  public String getHost() {
    return this.host;
  }

  public void setHost(String host) {
    this.host = host;
  }


  public Set<Event> getEvents() {
    return this.events;
  }

  public void setEvents(Set<Event> events) {
    this.events = events;
  }

  public Long  getCreateDate() {
    return createDate;
  }

  public void setCreateDate(Long  createDate) {
    this.createDate = createDate;
  }

  public Long  getAccessDate() {
    return accessDate;
  }

  public void setAccessDate(Long  accessDate) {
    this.accessDate = accessDate;
  }

  public Long  getCloseDate() {
    return closeDate;
  }

  public void setCloseDate(Long  closeDate) {
    this.closeDate = closeDate;
  }

}