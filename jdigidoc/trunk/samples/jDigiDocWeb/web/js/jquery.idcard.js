(function($){
    $.fn.idCard = function(options) {
        var names = [ "firstName", "lastName", "middleName", "sex",
              "citizenship", "birthDate", "personalID", "documentID",
              "expiryDate", "placeOfBirth", "issuedDate", "residencePermit",
              "comment1", "comment2", "comment3", "comment4"
         ];
        var defaults = {
                cardInserted: function() {},
		cardRemoved: function() {},
                card:'',
                
                getPersonalData: function() {
                  var result="<table>";
                    for(i = 0; i < names.length; i++){ 
                        result+="<tr><td>"+names[i]+"</td><td>";
                        result+=this.card[names[i]];
                        result+="</td></tr>";
                    }
                    result+="</table>";
                    return result;
              }
        };

       
        var opt = $.extend(defaults, options);

        return this.each(function() {
            obj = $(this);
            opt.card = obj[0];
            opt.card.addEventListener("OnCardInserted", function(){
                opt.cardInserted();
            });
            opt.card.addEventListener("OnCardRemoved", function(){
                opt.cardRemoved();
            });
            try{
                if(opt.card["firstName"])
                {
                    opt.cardInserted();
                }
            }catch(err){}//on loading when this method does not exist
        });

       
    };

    $.fn.idCardData = function(elementName)
        {
            var obj= $(this);
             try{
                 if(obj[0][elementName])
                    return obj[0][elementName];
             }
             catch(err){return "";}
        };

    
})(jQuery);