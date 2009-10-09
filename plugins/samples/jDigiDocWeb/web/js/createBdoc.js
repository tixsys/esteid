function Info(message, precent)
{
    $("#info_text").html(message);
    $("#progressbar").progressbar( 'value' , precent);
}
 function Error(message)
 {
     $("#info").hide();
     $("#error_text").text(message);
     $("#error").fadeIn();
 }

 function addNewFile(obj) {
    var v = obj.value;
    var files=$('input.upload').size();
    if(files == 1) {
        $("#send").show();
        $("#ddocName").val(v);
    }
    if(files == 2) {
        $("#ddoc-container").show();
    }


    $(obj).hide();
    $(obj).parent().prepend('<input type="file" class="upload" name="fileX[]" />').find("input").change(function() {addNewFile(this)});

    if(v != '') {
        $("div#files_list").append('<div>'+v+'<input type="button" class="remove" value="Delete" /></div>')
        .find("input").click(function()
        {
            $(this).parent().remove();
            $(obj).remove();
            if($('input.upload').size()==1)
            {
                $("#send").hide();
            }
            return true;
        });
    }
}

$(document).ready(function()
{    
    $("input.upload").change(function(){
        addNewFile(this);
    });

    $("#send").click(function(){
        $("#id-upload-container").fadeOut("fast");
        $("#cert").remove();
        $("#info").fadeIn();
        $("#progressbar").progressbar({value: 0});
        Info("Loading certificate info from ID-Card", 0);
        $("#asdf").append("<input type='hidden' id='cert' name='cert' value='"+$.base64Encode($("#esteid").idCardData("signCert")["cert"])+"'/>");

        Info("Please wait, uploading files to server...", 20);
        $("#asdf").submit();
    });

    $("#asdf").ajaxForm({
        beforeSubmit: function(a,f,o) {
            o.dataType = "json";

        },
        success: function(obj) {
            //1. Files are loaded to server, BdocServlet servlet sends back signature hash and digidoc file name in json result
            Info("Files successfully uploaded to server, prepearing signing process", 50);
            if(obj.status=='ERROR')
            {
                Error(obj.message);
                return;
            }
            var sigHex = null;
            try {
                //2. IdCard will raise the PIN2 dialog and will compare the hash with BdocServlet.doGet digidoc stream
                var idcard=$("#esteid")[0];
                sigHex=idcard.sign(obj.hash, "BdocServlet?doc="+obj.doc);

                //3. IdCard validated the signature in browser
            }catch(e) {
                Error("ERROR in signing: " + e);
            }


            if(sigHex != null) {
                Info("Signing process successful, finishing document signing process", 80);

            //4. Sending signature back to server ...
                $.post(
                  "BdocServlet?mode=sign&sigHex=" + sigHex + "&docName="+obj.doc,
                  function(data, textStatus){
                      Info("Document signing process finished!<br><br/>You can <a href='docks/"+obj.doc+".bdoc'>download</a> signed document <br/> Or <a href='readBdoc.jsp?doc="+obj.doc+"'>inspect</a> uploaded document", 100);
                      //5. Server finished the document creation. Now digidoc is ready!


                  }
                  , "json"
                );
            }
        }
     });

      $("#esteid").idCard({
        cardInserted:function(){
            $("#id-upload-container").fadeIn();
            $("#userinfo").html(this.getPersonalData());
            //$("#certInfo").html(this.card.signCert.cert);
            $("#title").text("Create signed document");
        },
        cardRemoved:function(){
            $("#id-upload-container").fadeOut("fast", function(){
                $("#userinfo").html("");
            });
            $("#title").text("Please enter id card");
        }
    });
});



