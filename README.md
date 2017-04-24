Gakken GMC-4 Simulator
======================

What's This?
------------
This is a program that simulates a board of 4-bit microprocessor named GMC-4
that has been provided by Gakken (http://otonanokagaku.net/magazine/vol24/index.html).
This also includes an assembler, disassembler and a simple
assembler editor so that you can develop a program for that processor.

![main](image/main-shrink.png)

Source Build
------------
Make sure that Visual Studio 2015 has been installed and follow the steps below:

1. Run `setup.bat` in `guest` directory, which creates wxWidgets library files.

2. Open `src/gmc4sim.sln` with Visual Studio 2015 and buid it in `Release` configuration.
   This will create `gmc4sim.exe` and `gmc4tool.exe` in `bin-x86`.


Binary Install
--------------
Download a package file
[GMC4Sim-1.39.zip](https://github.com/ypsitau/gmc4sim/releases/download/v1.39/gmc4sim-1.39.zip)
and expand it in a directory you like.


Operation
---------
The file `gmc4sim.exe` is the executable.

- `[File]` - `[Open]` will open a HEX or an assember file.
  Opening a HEX file will load the binary data into an internal memory of the simulator
  and outputs disassembled result in the assembler editor.
  There are a lot of sample HEX files under `Sample` directory, so you can easily try some examples.

- `[File]` - `[Save]` will save the source code that is edited in the assembler editor.

- `[Simulator]` - `[Run Mode]` will run the program after turning the simulator into "Run" mode.

- `[Simulator]` - `[Reset]` will pause the program and turn the simulator into "Program" mode.

- `[Simulator]` - `[Build]` will reflect the modification in the assembler editor.

- `[Window]` - `[Show Dump]` will print the object in a dump format.

See [User Manual](https://github.com/ypsitau/gmc4sim/blob/master/doc/GMC4Sim.pdf)
for more detail on operation, though it's written in Japanese.
