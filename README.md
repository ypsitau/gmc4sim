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
and and expand it in a directory you like.


Usage
-----
The file `gmc4sim.exe` is a GUI version of executable.

- メニューから [File] - [Open] で HEX ファイルを開くと、シミュレータの内部メモリにプログラムが読み込まれ、アセンブラエディタに逆アセンブル結果が出力されます。
Sample ディレクトリの下に、学研のホームページで公開されている「FX マイコン R-165 プログラム集」のプログラム、全 93 リストを HEX 形式にしたファイルを格納しています。

- [File] - [Save] でアセンブラコードを保存できます。

- [Simulator] - [Run Mode] を選択するとシミュレータが Run モードになりプログラムを実行します。[Simulator] - [Reset] でプログラムを中断して Program モードにします。Program モードの状態で右下のアセンブラエディタで編集をし、[Simulator] - [Build] を選択すると変更内容がオブジェクトコードに反映されます。

- 実機にプログラムを打ち込むときは、[Window] - [Show Dump] でダンプを表示すると便利です。
