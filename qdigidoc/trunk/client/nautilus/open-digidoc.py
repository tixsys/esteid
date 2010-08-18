#
# QDigiDoc Nautilus Extension
#
# Copyright (C) 2010  Erkko Kebbinau
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc, 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
import os
import urllib

import gtk
import gio
import nautilus
import gconf

class OpenDigidocExtension(nautilus.MenuProvider):
    def __init__(self):
        pass
        
    def _open_client(self, file):
        path = urllib.unquote(file.get_uri()[7:])
        cmd = ("qdigidocclient \"%s\" &" % path)
        os.system(cmd)
        
    def menu_activate_cb(self, menu, file):
        self._open_client(file)
       
    def debug_print(self, msg): 
        fo = open('/tmp/test.txt','a')
        fo.write(msg + "\n")
        fo.close()

    def get_file_items(self, window, files):
       
        if len(files) != 1:
            return
        file = files[0]
        self.debug_print(file.get_name())
        if (file.get_file_type() != gio.FILE_TYPE_REGULAR) or (file.get_uri_scheme() != 'file'):
            return
        item = nautilus.MenuItem('EsteidSigner',
                                 'Sign with ID card' ,
                                 'Sign the document %s' % file.get_name())
        item.connect('activate', self.menu_activate_cb, file)
        return item,

