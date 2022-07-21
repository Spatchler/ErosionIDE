# ErosionIDE
A little code editor and retained mode ui engine project i am working on.

It is designed to have an efficient workflow that uses only the keyboard whalst looking elgant, minimal and being highly customizable.

To build for linux or mac you need premake and then just run this command: premake5 gmake2 && make
To build for mac you will need command line tools and premake then run this command: premake5 gmake2 && make
To build for windows you need premake, a compiler and mingw then run this command: premake5 gmake2 && mingw32-make
Premake will default to debug build.

Im still working on the engine right now, so there is not really much to see.

Currently its main.cpp for config, it has a basic project template showing most of the engines functionality and how to use it,

when ever you change the config you will need to recompile just by running make, then to run it use this command, ./(project directory)/bin/debug/(your os, for example on linux it would just be linux)/erosion