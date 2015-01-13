This is a Cmake build of the reference software of MPEG-4 Part 16 AFX FAMC 

Encoding/Decoding Syntax: 

Encoder:  //ctm-tools//Debug//ctm-tools.exe dog_mesh dog_mesh.mp4
Decoder:  //ctm-tools//Debug//ctm-tools.exe dog_mesh.mp4 dog_mesh

composed by Centrum Wiskunde Informatica Wiskunde Informatica.
contact: rufael mekuria rufael.mekuria@cwi.nl

All rights of the famc reference software belong to the respective contributors (see source code)

OpenCTM License and contributors are found in LICENSE.TXT

The cmake build of famc has several advantages:

1. separate the build and source and test_data files, keep the svn/source folder clean
2. easier integration with other tools that may need goemetry compression
3. full integration with openCTM allows folders of meshdata of many different file formats

To build, use cmake 2.8 or higher gui 

1. open the folder in cmake gui
2. select the build folder, should be respective to the source folder ( famc_dir/build)
3. configure and generate 
4. build the solution in visual studio, (linux support is not there yet)
5. the ctm-tools now supports famc and the testdata / configuration files have been automatically copied
6. run from ctm=tools/Relase or /debug as ctm-tools.exe dance_mesh dance_mesh.mp4 to encode an entire folder
7. the configuration parameters can be changed in famcParamFile.txt which are the regular 19 parameters needed by FAMC
8. To decode, run ctm-tools.exe dance_mesh.mp4 dance_mesh, 
the --Exp options allows to set output mesh format, default: --Exp ".off", other formats are .ply, .obj etc....

Warning: Only Tested on Windows 7 64-bit with Visual Studio 2010 !!!!!


TODO:

1. DEBUG FAMC (at the time of writing encoding in multiple segments does not work)
2. ADD LINUX SUPPORT (Both compilation (C++) and cmake need to be updated for this)

KNOWN ISSUES:

The decoder in release mode sometimes crashes, debug works fine

