from trac.core import *
from trac.wiki.macros import WikiMacroBase
from trac.wiki.formatter import system_message
from trac.util.html import html as tag

class EstEIDModuleLoader(WikiMacroBase):
    """A macro for loading security modules into Mozilla."""

    SCRIPT_CODE = 'function loadEstEIDModule(tag) { \
                       path = tag.innerHTML;
                       pkcs11.addmodule("ID-Kaart", path, 0x1<<28, 0); \
                   }'
    
    def render_macro(self, req, name, content):
        content = content.strip()
        retval  = tag.script(SCRIPT_CODE, type='text/javascript')
        retval += tag.a(content, href='#', onclick='loadEstEIDModule(this);')
        return retval
