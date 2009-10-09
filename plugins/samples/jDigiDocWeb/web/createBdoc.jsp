<%-- 
    Document   : upload
    Created on : 6.10.2009, 14:52:45
    Author     : lauri
--%>

<%@page contentType="text/html" pageEncoding="UTF-8"%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">

<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title>Create bdoc</title>
        <script type="text/javascript" src="http://www.google.com/jsapi"></script>
        <script type="text/javascript">
          // Load jQuery from Google CDN
            google.load("jquery", "1.3.2");
            google.load("jqueryui", "1.7.2");
        </script>        
        <script src="js/jquery.base64.js" type="text/javascript"></script>
        <script src="js/jquery.form.js" type="text/javascript"></script>
        <script src="js/jquery.idcard.js" type="text/javascript"></script>
        <link type="text/css" href="ui/jquery-ui-1.7.2.custom.css" rel="stylesheet" />
<style type="text/css">
 body{ font: 70% "Trebuchet MS", sans-serif; margin: 50px;}
 .infoHeader { margin-top: 2em; }
</style>
    </head>
    <body>
        <h1 id="title" class="infoHeader">Please insert ID-card</h1>
        <div id="error" class="ui-widget" style="display:none">
            <div class="ui-state-error ui-corner-all" style="padding: 0pt 0.7em;">
            <p>
                <span class="ui-icon ui-icon-alert" style="float: left; margin-right: 0.3em;"></span>
                <strong>Error:</strong>
                <p id="error_text"></p>
            </p>
            </div>
        </div>
        <div id="info" class="ui-widget ui-corner-all" style="display:none; border: 4px solid rgb(204, 204, 204); padding: 10px; width: 300px; position: absolute; background-color: rgb(255, 255, 255); " >
            <p>
                <span class="ui-icon ui-icon-info" style="float: left; margin-right: 0.3em;"></span>
                <strong>Uploading document to server</strong>
                <p id="info_text"></p>
                <div id="progressbar"></div>
            </p>
        </div>
        <form action="BdocServlet" method="post" enctype="multipart/form-data" name="asdf" id="asdf">
            <object id="esteid" type="application/x-esteid" style="width: 20px; height: 20px; visibility: visible;">
            ID-Card not supported :(
            </object>
            <div id="id-upload-container" style="display:none">
                <div id="userinfo"></div>
                <div id="document" >
                    <div id="ddoc-container" style="display:none">
                    <input type="text" id="ddocName" name="ddocName">.ddoc
                    </div>
                    <div id="files_list" style="padding:5px; font-size:x-small;"><strong>Files:</strong></div>
                    <div id="mUpload">
                        <input type="file" id="element_input" class="upload" name="fileX[]" /><br />
                        <input type="button" name="Submit" value="Sign documents" id="send" style="display:none" />
                    </div>
                </div>
            </div>
        </form>
        <div id="certInfo"></div>
        <script src="js/createBdoc.js" type="text/javascript"></script>
    </body>
</html>
