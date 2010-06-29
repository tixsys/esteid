<%-- 
    Document   : uploadBdoc
    Created on : 8.10.2009, 16:11:01
    Author     : lauri
--%>

<%@page contentType="text/html" pageEncoding="UTF-8"%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">

<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title>Upload bdoc</title>
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
        <h1>Uploade BDoc file for document data valuation</h1>
        <div id="info" class="ui-widget ui-state-info ui-corner-all" style="padding: 10px; width: 300px;" >
            <p>
                <span class="ui-icon ui-icon-info" style="float: left; margin-right: 0.3em;"></span>
                <strong>Upload bdoc document to server</strong>
                <p id="info_text"></p>
                <div id="progressbar"></div>
            </p>
        </div>
        <div id="docUploader">
            <form action="UploadBdocController" method="post" enctype="multipart/form-data" name="fileUploadForm" id="fileUploadForm">
                <input type="file" id="element_input" class="upload" name="fileToUpload" />
                <input type="submit" name="Submit" value="Verify on server" id="send"  />
            </form>
        </div>
        <div id="docResult" style="display:none"></div>
        <script type="text/javascript">
            
$(document).ready(function(){
     $("#fileUploadForm").ajaxForm({
        beforeSubmit: function(a,f,o) {
            o.dataType = "html";
        },
        success: function(obj) {
            $("#docResult").html(obj).fadeIn("slow");
        }
     });
});
        </script>
    </body>
</html>
