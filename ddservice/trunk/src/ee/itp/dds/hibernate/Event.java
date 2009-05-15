package ee.itp.dds.hibernate;

import java.util.HashSet;
import java.util.Set;

/**
 * Event entity.
 * @author Aleksandr Vassin
 * @created  11.04.2009
 */
public class Event implements java.io.Serializable {

  // Fields

  private Long id;
  private Event event;
  private Action action;
  private Session session;
  private Long eventTime;
  private String comment;
  private Set events = new HashSet(0);

  // Constructors

  /** default constructor */
  public Event() {
  }

  
  /** minimal constructor */
  public Event(Long id) {
    this.id = id;
  }

  /** full constructor */
  public Event(Long id, Event event, Action action, Session session, Long eventTime, String comment, Set events) {
    this.id = id;
    this.event = event;
    this.action = action;
    this.session = session;
    this.eventTime = eventTime;
    this.comment = comment;
    this.events = events;
  }

  // Property accessors

  public Long getId() {
    return this.id;
  }

  public void setId(Long id) {
    this.id = id;
  }

  public Event getEvent() {
    return this.event;
  }

  public void setEvent(Event event) {
    this.event = event;
  }

  public Session getSession() {
    return this.session;
  }

  public void setSession(Session session) {
    this.session = session;
  }

  public Long  getEventTime() {
    return this.eventTime;
  }

  public void setEventTime(Long  eventTime) {
    this.eventTime = eventTime;
  }

  public String getComment() {
    return this.comment;
  }

  public void setComment(String comment) {
    this.comment = comment;
  }

  public Set getEvents() {
    return this.events;
  }

  public void setEvents(Set events) {
    this.events = events;
  }


  public Action getAction() {
    return action;
  }


  public void setAction(Action action) {
    this.action = action;
  }

}