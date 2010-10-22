#!/usr/bin/env bash

if [ x"$(id -u)" != x"0" ] ; then
	echo "You do not have enough privileges to run this script" 2>&1
	echo "    run 'sudo $0'" 2>&1
	exit 1
fi

# esteid-contextmenu
echo 'Removing Finder plug-in'
rm -rf /Library/Contextual\ Menu\ Items/EstEIDCM.plugin

# esteid-digidoc
echo 'Removing Digidoc libraries'
rm -f /usr/local/etc/digidoc.conf
rm -rf /usr/local/etc/digidocpp
rm -rf /usr/local/share/libdigidoc
rm -f /usr/local/lib/libdigidoc.dylib
rm -f /usr/local/lib/libdigidoc.2.dylib
rm -f /usr/local/lib/libdigidocpp.dylib
rm -f /usr/local/lib/libdigidocpp.0.dylib
rm -f /usr/local/lib/libp11.dylib
rm -f /usr/local/lib/libp11.1.dylib
rm -rf /Library/EstonianIDCard

for f in /Users/*/.digidocpp/digidocpp.conf ; do
    grep -q EstonianIDCard "$f" 2> /dev/null && rm -f "$f"
done

# esteid-mozilla
echo 'Removing Mozilla extension'
rm -rf /Library/Application\ Support/Mozilla/Extensions/*/\{aa84ce40-4253-11da-8cd6-0800200c9a66\}
rm -rf /Library/Application\ Support/Mozilla/Extensions/*/\{aa84ce40-4253-a00a-8cd6-0800200f9a66\}

# esteid-opensc
echo 'Removing OpenSC'
rm -rf /Library/OpenSC
rm -f /usr/lib/opensc-pkcs11.so

# esteid-preferences
echo 'Removing Preferences'
rm -rf /Library/PreferencePanes/EstEIDPP.prefPane

# esteid-qdigidocclient
echo 'Removing Digidoc Client'
rm -rf /Applications/qdigidocclient.app

# esteid-qdigidoccrypto
echo 'Removing Digidoc Crypto'
rm -rf /Applications/Utilities/qdigidoccrypto.app
rm -rf /Applications/qdigidoccrypto.app

# esteid-qesteidutil
echo 'Removing ID-card Utility'
rm -rf /Applications/qesteidutil.app

# esteid-qt
echo 'Removing Qt'
rm -rf /Library/Frameworks/QtCore.framework
rm -rf /Library/Frameworks/QtDBus.framework
rm -rf /Library/Frameworks/QtGui.framework
rm -rf /Library/Frameworks/QtNetwork.framework
rm -rf /Library/Frameworks/QtWebKit.framework
rm -rf /Library/Frameworks/QtXml.framework
rm -rf /Library/Frameworks/QtXmlPatterns.framework 
rm -rf /Library/Frameworks/phonon.framework

# esteid-updater
echo 'Removing Updater'
rm -f /Library/LaunchAgents/org.esteid.updater.plist
rm -rf /Applications/Utilities/EstEIDSU.app

# esteid-webplugin
echo 'Removing Web Browser plug-ins'
rm -rf /Library/Internet\ Plug-Ins/esteid.plugin
rm -rf /Library/Internet\ Plug-Ins/EstEIDWP.plugin
rm -rf /Library/Internet\ Plug-Ins/esteidfirefoxplugin.bundle
rm -rf /Library/Internet\ Plug-Ins/esteidsafariplugin.webplugin

echo 'Removing receipts'
rm -rf /Library/Receipts/org.esteid.*

echo "Files removed."

echo 'Uninstall done!'
