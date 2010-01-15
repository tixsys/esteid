Running jDigiDoc example Web application
----------------------------------------

1. Get Maven from http://maven.apache.org
2. Build the example WAR
   mvn install
3. Run the example in Jetty
   mvn jetty:run
   Navigate to http://localhost:8080/jdigidoc-helloworld

If You want to test with a "real" appserver, pick up the WAR file
from target directory.
