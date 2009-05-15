package ee.itp.dds.hibernate;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

import ee.sk.digidoc.DigiDocException;
import ee.sk.digidoc.SignedDoc;

/**
 * 
 * 
 * @author Konstantin Tarletsky
 * @created  11.04.2009
 */
public class Document {
  
  private Long id;
  private byte[] signedDocByteArray;
  private String comment;
  
  
  public Long getId() {
    return id;
  }
  public void setId(Long id) {
    this.id = id;
  }
  
  
  public SignedDoc getSignedDoc() throws IOException, ClassNotFoundException {  
    SignedDoc ret = null; 
    if(signedDocByteArray != null){
      
      ByteArrayInputStream bais = new  ByteArrayInputStream(signedDocByteArray);
      ret = (SignedDoc)new ObjectInputStream(bais).readObject();
      
    }//if
    return ret;
  }
  
  
  public void setSignedDoc(SignedDoc signedDoc)throws DigiDocException, IOException  {
    
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    ObjectOutputStream oos = new ObjectOutputStream(baos);
    oos.writeObject(signedDoc);
    oos.flush();
    signedDocByteArray = baos.toByteArray();

  }
  
  
  public String getComment() {
    return comment;
  }
  
  public void setComment(String comment) {
    this.comment = comment;
  }
  
  public byte[] getSignedDocByteArray() throws DigiDocException {
    return signedDocByteArray;
  }
  
  public void setSignedDocByteArray(byte[] signedDocByteArray) {
    this.signedDocByteArray = signedDocByteArray;
  }
  
  @Override
  public String toString(){
   
    return "ID = " + id ;
    
  }
  
}
