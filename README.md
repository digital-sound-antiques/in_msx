# in_msx.dll
MSX sound player plugin (a.k.a MSXplug) for Winamp 5.
It supports the following sound formats.

|EXT|Description|
|---|---|
|.kss|Sound data from varius systems|
|.mgs|MGSDRV data|
|.bgm|MuSICA or Kinrou5 data|
|.mbm|MoonBlaster data|
|.opx|OPLLDriver data|
|.mpk|MPK data|

Note: zip-compressed `.kss` files are not supported.

# How to install

1. Download in_msx.dll and in_msx.ini from our [repository](https://github.com/digital-sound-antiques/in_msx/tree/master/dist).
2. Copy the both in_msx.dll and in_msx.ini to Winamp 5's plugin directory.
3. Restart the Winamp.

# How to build

Install [cmake][] 3.4 or later, [git for windows][] and [Visual Studio] 2015 or later.

[cmake]: https://cmake.org/
[git for windows]: https://git-for-windows.github.io/
[Visual Studio]: https://www.visualstudio.com/

Then, run the following commands from git-bash shell to generate `in_msx.vcxproj`.

```
$ git clone --recursive https://github.com/digital-sound-antiques/in_msx.git
$ cd in_msx
$ mkdir build
$ cd build
$ cmake ..
```

You can open `in_msx.vcxproj` with Visual Studio. To obtain `in_msx.dll`, just build `in_msx` project.
If you have some error to find MSVC compiler from `cmake ..`, try CMake 3.6 or greater.
