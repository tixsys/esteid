ESTEID PLUGIN LOADER
====================

Esteid plugin loader is a JavaScript library for loading the 
esteid-browser-plugin into a web page. The library has fallback
capabilities which can utilize all known estonian ID-card
signer methods. A transparent API converter will make those old
technologies behave the same as the new plugin. All known signers
will be automatically probed for and the best one will be enabled.
No user input or "signing preference" is required. Legacy support is
encapsulated into a separate JS file and designed in a way that it's
easy to remove when old plugins finally fade out of existence.


BUILD INSTRUCTIONS
==================
* Uncompress
* Point Your web browser to demo.html


TESTED BROWSERS AND OPERATING SYSTEMS
=====================================

Windows XP
 * Firefox 3.6
 * Internet Explorer 6
 * Internet Explorer 8
 * Google Chrome 6.0

Windows 7 32bit and 64bit
 * Firefox 3.6, 4.0b6, 7.0
 * Internet Explorer 8 (see note)
 * Internet Explorer 9
 * Google Chrome 6, 15

MacOSX 10.6.4
 * Safari 5.0
 * Firefox 3.6
 * Google Chrome 6.0

Linux (Fedora 13 64bit):
 * Chromium 7.0.542.0
 * Firefox 3.6 and 4.0b6
 * Arora 0.10.2

Note: There is/was a known bug in esteid-browser-plugin that manifests itself
      when IE is running in "Protected Mode": The PIN2 prompt is not displayed
      and no callback is executed either. This is a plugin problem and
      does not affect this library in any way. Please add Your site
      to "Trusted Sites" as a temporary fix if You seem to be affected.
      See http://code.google.com/p/esteid/issues/detail?id=116 for details.


DEVELOPER DOCUMENTATION
=======================

The usage of this library is detailed on Open EstEID web developer pages.
Please navigate to http://code.google.com/p/esteid/wiki/EstEIDPluginLoader
