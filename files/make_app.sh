#!/bin/bash
#
# This script creates an OS X application bundle from an IVT application using the Cocoa GUI toolkit
# This script takes two parameters
# 1st parameter: name of the application bundle without the '.app' at the end
# 2nd parameter: name of the compiled executable
#
# Please note that this script only works correctly if it is called in one of the directories of the examples folder
# or a similar location, so that the path to the icon file '../../files/IVT.icns' is correct.
#
# Author: Florian Hecht, 2008
#
rm -rf $1.app
mkdir $1.app
mkdir $1.app/Contents
mkdir $1.app/Contents/MacOS
mkdir $1.app/Contents/Resources
cp $2 $1.app/Contents/MacOS/$1
cp ../../files/IVT.icns $1.app/Contents/Resources/IVT.icns
echo '<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleExecutable</key>
	<string>DummyAppName</string>
	<key>CFBundleIdentifier</key>
	<string>com.IVT.DummyAppName</string>
	<key>CFBundleIconFile</key>
    <string>IVT</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundleName</key>
	<string>DummyAppName</string>
	<key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0.0</string>
</dict>
</plist>' | sed s/DummyAppName/$1/ > $1.app/Contents/Info.plist

