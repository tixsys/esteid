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
        
    def _open_client(self, paths):
        args = ""
        for path in paths:
            args = args + "\"%s\" " % path
        cmd = ("qdigidocclient " + args + "&")
        os.system(cmd)
        
    def menu_activate_cb(self, menu, paths):
        self._open_client(paths)
       
    def debug_print(self, msg): 
        fo = open('/tmp/test.txt','a')
        fo.write(msg + "\n")
        fo.close()

    def valid_file(self, file):
        return file.get_file_type() == gio.FILE_TYPE_REGULAR and file.get_uri_scheme() == 'file'

    def get_file_items(self, window, files):
        paths = []
        for file in files:
            if self.valid_file(file):
                path = urllib.unquote(file.get_uri()[7:])
                paths.append(path)

        if len(paths) < 1:
            return
        
        item = nautilus.MenuItem('DigidocSigner',
                                 'Sign with ID card',
                                 'Sign selected file(s) with Digidoc3 Client')

        item.connect('activate', self.menu_activate_cb, paths)
        return item,
