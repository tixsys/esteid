from trac.core import *
from trac.wiki.macros import WikiMacroBase
from trac.wiki.formatter import system_message
from trac.util.html import html as tag

revision = "$Rev$"
url = "$URL$"

class EstEIDModuleLoaderHeaderMacro(WikiMacroBase):
    """This macro will set up javascript needes to load EstEID modules."""

    def render_macro(self, req, name, content):
        return '<script type="text/javascript">\n\
                <!--\n\
                function loadEstEIDModule(tag) {\n\
                    var res = 0;\n\
                    path = tag.innerHTML;\n\
                    res = pkcs11.addmodule("ID-Kaart", path, 0x1<<28, 0);\n\
                    if ( res >= 0) { /* Success */\n\
                    } else if ( res  == -2 ) { /* Cancelled by user */\n\
                    } else if ( res == -5 ) { /* Module not found */\n\
                        window.alert("Moodulit ei leitud");\n\
                    } else if ( res == -10 ) { /* Module already exists */\n\
                        window.alert("Moodul juba eksisteerib");\n\
                    } else {\n\
                        window.alert("Tundmatu viga " + res);\n\
                    }\n\
                    return false;\n\
               } //--> \n\
               </script>'



class EstEIDLoadModuleMacro(WikiMacroBase):
    """A macro for loading security modules into Mozilla."""

    def render_macro(self, req, name, content):
        content = content.strip()
        return tag.a(content, href='#', onclick='loadEstEIDModule(this);')
