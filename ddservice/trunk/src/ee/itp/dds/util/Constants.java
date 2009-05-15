package ee.itp.dds.util;

public interface Constants {
  
  public static final String DataFile = "DataFile";
  public static final String ContentType = "ContentType";
  public static final String Filename = "Filename";
  public static final String Id = "Id";
  public static final String MimeType = "MimeType";
  public static final String Size = "Size";
  public static final String DigestType = "DigestType"; 
  public static final String DigestValue = "DigestValue";
  
  public static enum SignatureInfoStatus{OK,ERROR}
  public static enum SignatureInfoErrorCategory{TEHNICAL,USER,LIBRARY}
  
}
