Fusion Media Controller for OpenCPN
==============================

A simple dialog that controls a Fusion Media Player from OpenCPN.
Requires the TwoCan plugin to provide the connection from OpenCPN via the NMEA 2000 network to the Media Player.

Obtaining the source code
-------------------------

git clone https://github.com/twocanplugin/mediaplayer.git


Build Environment
-----------------

Windows, MacOS and Linux are currently supported.

This plugin builds outside of the OpenCPN source tree

Refer to the OpenCPN developer manual for details regarding other requirements such as git, cmake and wxWidgets.

For Windows you must place opencpn.lib into the twocan_pi/build directory to be able to link the plugin DLL. opencpn.lib can be obtained from your local OpenCPN build, or alternatively downloaded from http://sourceforge.net/projects/opencpnplugins/files/opencpn_lib/

Build Commands
--------------
 mkdir mediaplayer/build

 cd mediaplayer/build

 cmake ..

 cmake --build . --config debug

  or

 cmake --build . --config release

Creating an installation package
--------------------------------
 cmake --build . --config release --target package

  or

 cpack

Installation
------------
Run the resulting setup package created above for your platform.

Eg. For Windows run racing\_pi\_1.0.0-ov50.exe

For OpenCPN version 5.2 and later, this version may be installed directly by the OpenCPN Plugin Manager.

Problems
--------

Please send bug reports/questions/comments to the opencpn forum or via email to twocanplugin@hotmail.com

License
-------
The plugin code is licensed under the terms of the GPL v3 or, at your convenience, a later version.