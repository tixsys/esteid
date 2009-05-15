package ee.itp.dds.hibernate;


/**
 * Action entity. 
 * @author AV
 */

public class Action implements java.io.Serializable {

  // Fields

  private Long id;
  private ActionEnum name;
  private Boolean valid;

  // Constructors

  /** default constructor */
  public Action() {
  }

  /** minimal constructor */
  public Action(Long id) {
    this.id = id;
  }

  /** full constructor */
  public Action(Long id, ActionEnum name, Boolean valid) {
    this.id = id;
    this.name = name;
    this.valid = valid;
  }

  // Property accessors

  public Long getId() {
    return this.id;
  }

  public void setId(Long id) {
    this.id = id;
  }

  public Boolean getValid() {
    return this.valid;
  }

  public void setValid(Boolean valid) {
    this.valid = valid;
  }

  public ActionEnum getName() {
    return name;
  }

  
  public void setName(ActionEnum name) {
    this.name = name;
  }
}