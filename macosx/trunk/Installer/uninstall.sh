#!/usr/bin/env bash

echo 'Removing Mozilla extension'
rm -fR /Library/Application\ Support/Mozilla/Extensions/{3550f703-e582-4d05-9a08-453d09bdfdc6}/{aa84ce40-4253-11da-8cd6-0800200c9a66}
rm -fR /Library/Application\ Support/Mozilla/Extensions/{92650c4d-4b8e-4d2a-b7eb-24ecf4f6b63a}/{aa84ce40-4253-11da-8cd6-0800200c9a66}
rm -fR /Library/Application\ Support/Mozilla/Extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}/{aa84ce40-4253-11da-8cd6-0800200c9a66}

echo 'Removing Updater'
rm -fR /Library/Receipts/org.esteid.*
rm -f /Library/LaunchAgents/org.esteid.updater.plist
mv -f /Applications/Utilities/EstEIDSU.app ~/.Trash/ID-card\ Software

echo 'Removing Preferences'
mv -f /Library/PreferencePanes/EstEIDPP.prefPane ~/.Trash/ID-card\ Software

echo 'Removing Safari plug-in'
mv -f /Library/Internet\ Plug-Ins/esteid.plugin ~/.Trash/ID-card\ Software
mv -f /Library/Internet\ Plug-Ins/EstEIDWP.plugin ~/.Trash/ID-card\ Software

echo 'Removing Finder plug-in'
mv -f /Library/Contextual\ Menu\ Items/EstEIDCM.plugin ~/.Trash/ID-card\ Software

echo 'Removing OpenSC'
mv -f /Library/OpenSC ~/.Trash/ID-card\ Software
mv -f /usr/lib/opensc-pkcs11.so ~/.Trash/ID-card\ Software

echo 'Removing ID-card Utility'
mv -f /Applications/qesteidutil.app ~/.Trash/ID-card\ Software

echo 'Removing Digidoc Client'
mv -f /Applications/qdigidocclient.app ~/.Trash/ID-card\ Software

echo 'Removing Digidoc Crypto'
mv -f /Applications/Utilities/qdigidoccrypto.app ~/.Trash/ID-card\ Software

echo 'Removing Digidoc libraries'
mv -f /usr/local/etc/digidoc.conf ~/.Trash/ID-card\ Software
mv -f /usr/local/etc/digidocpp ~/.Trash/ID-card\ Software
mv -f /usr/local/share/libdigidoc ~/.Trash/ID-card\ Software
mv -f /usr/local/lib/libdigidoc.dylib ~/.Trash/ID-card\ Software
mv -f /usr/local/lib/libdigidoc.2.dylib ~/.Trash/ID-card\ Software
mv -f /usr/local/lib/libdigidocpp.dylib ~/.Trash/ID-card\ Software
mv -f /usr/local/lib/libdigidocpp.0.dylib ~/.Trash/ID-card\ Software
mv -f /usr/local/lib/libp11.dylib ~/.Trash/ID-card\ Software
mv -f /usr/local/lib/libp11.1.dylib ~/.Trash/ID-card\ Software


echo 'Removing .plist files'
mv -f ~/Library/Preferences/com.estonian-id-card.plist ~/.Trash/ID-card\ Software
mv -f ~/Library/Preferences/org.esteid.* ~/.Trash/ID-card\ Software

echo 'Emptying trash...'

sudo rm -rf ~/.Trash/ID-card\ Software

echo "Files removed."

echo 'Uninstall done!'