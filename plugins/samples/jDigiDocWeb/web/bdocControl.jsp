<%-- 
    Document   : docInfo
    Created on : 7.10.2009, 15:33:46
    Author     : lauri
--%>

<%@page contentType="text/html" pageEncoding="UTF-8"%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
<%@page import="ee.sk.digidoc.*, ee.sk.utils.ConfigManager, ee.sk.digidoc.factory.DigiDocFactory;"%>

        <%
        String doc=request.getParameter("doc");
        SignedDoc sdoc=null;
        if(doc!=null)
        {
            String rootPath=getServletContext().getRealPath("/").toString();
            ConfigManager.init(rootPath+ "WEB-INF/jdigidoc.cfg");

            try{
            DigiDocFactory digFac = ConfigManager.instance().getDigiDocFactory();
            String filename=rootPath+"docks\\"+doc+".bdoc";
            sdoc = digFac.readSignedDoc(filename);
            }
            catch(Exception ex)
            {
                out.println("<h1 class='error'>"+ex.getMessage()+"</h1></body></html>");
                return;
            }
        }
        else{
            sdoc = (SignedDoc)request.getAttribute("bdoc");
        }         
        %>
        <div class="dDoc">

        <div class="documentFiles">
            <p>Document files:</p>
            <%
            if(sdoc.countDataFiles()>0)
            {
            %>
            <table border="1">
                <tr>
                    <td>File name</td>
                    <td>File type</td>
                    <td>File size</td>
                </tr>
                <%for(int i=0; i<sdoc.countDataFiles(); i++){
                DataFile df= sdoc.getDataFile(i);

                %>
                <tr>
                    <td><%=df.getFileName()%></td>
                    <td><%=df.getContentType()%></td>
                    <td><%=df.getSize()%></td>
                </tr>
                <%}%>
            </table>
            <%
            }
            else{
            %>
            <p>There are no documents in this container!</p>
            <%}%>
        </div>
        <div id="documentSignatures">
            <p>Document signatures:</p>
            <%
            if(sdoc.countSignatures()>0)
            {
            %>
            <table border="1">
                <tr>
                    <td>User code</td>
                    <td>First name</td>
                    <td>Last name</td>
                    <td>Signed date</td>
                    <td>Additional info</td>
                </tr>
                <%for(int i=0; i<sdoc.countSignatures(); i++){
                Signature sign= sdoc.getSignature(i);
                KeyInfo keyInfo = sign.getKeyInfo();
     		SignedProperties prop=sign.getSignedProperties();
                %>
                <tr>
                    <td><%=keyInfo.getSubjectPersonalCode()%></td>
                    <td><%=keyInfo.getSubjectFirstName()%></td>
                    <td><%=keyInfo.getSubjectLastName()%></td>
                    <td><%=prop.getSigningTime().toString()%></td>
                    <td><%=prop.toString()%></td>
                </tr>
                <%}%>
            </table>
            <%
            }
            else{
            %>
            <p>There are no signatures in this container!</p>
            <%}%>
        </div>
</div>
