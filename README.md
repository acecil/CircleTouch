CircleTouch
===========

Simple touch game using Cinder by Andrew Gascoyne-Cecil.

Playing
-------

1. Download the latest executable: [CircleTouch.exe](https://acecil.mooo.com:8181/job/CircleTouch/label=VS2013/lastSuccessfulBuild/artifact/Release/CircleTouch.exe)
2. Run the executable.
3. Press F11 to toggle full-screen if required.
4. Touch the circles.
5. Press the spacebar to pause.

Building
--------

CircleTouch is an application built with the Cinder framework. The Cinder framework can be downloaded from [libcinder.org](http://libcinder.org).

Once the Cinder framework is downloaded, set the $CINDER environment variable to point to the directory where Cinder resides.

CircleTouch is supplied with a Visual Studio 2013 project file. Open the solution of project file in Visual Studio and build the game. This relies on versions of Cinder (and the boost dependency) from the latest master branch.

There is also preliminary support for WinRT by compiling against the dev branch of Cinder using the DebugRT and ReleaseRT configurations.
