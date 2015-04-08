This directory contains bash-sh scripts to compile and link applications on RODOS.
For small applications and examples you may call these scripts directly.
For bigger applications which usually consist of many software components
distributed in many files and directories and eventually written by different
programmers we recommend to write link and compile scripts which call
this basic scripts.

Some tutorials have an own compile script which is based on these basic scripts.

First you shall call make/rodosenv from the rodos root directory.
This will set path variable and other enviroments.
Then you may call any other script from anywhere.
All scripts will use path variables set by rodosenv.

There is a family of scripts one for each target hardware, for example:
generic-*
amr-*
cortex-*
leon2-*
linux-*
ppc-*
posix-*
freertos-*
xilinxos-*

The generic scripts may be called by other scripts when the have common
code and only differ on variables settigs.
For example
linux-lib, posix-lib and thers will call generlic-lib. 

For each family we have these scripts:
*-set-vars
	Called by other scripts, just to set shell variables
*-lib
	Generates all required libraries to be linked to RODOS applications

*-executable <list of source files>
	Compiles all files listed in the parameter list and links them to
	a single executable file called tst.

*-add-component <list of source files or directories>
	This is for bigger applications. It compiles o files in the
	directory “to-link”.
	From different calls, may o files may be accumulated in “to-link”
 
*-link-all-components
	Links all o files (grupped in directories) found in the directory “to-link”
	it generates an executalbe file tst

*-load
	To load the tst file in the corresponding target hardware, attached
	to the host computer (per USB, Parallelport or serial)


There are some other scripts:
logerrors		To write compile errors in a files
clean			To remove all generated files (*.o, libs, tst-*)
clean_components	To remeove all objects in the directory “to-link”
config-stty		To set tty parameters
config-stty-115200	To config tty to 115200 baud


-------------

For 64-bit posix linux (eg. for qt, openGL):

	rodos/api/string_pico.h
		< typedef unsigned int size_t;
		> typedef long unsigned int size_t;

	rodos/make/posix-set-vars
		< CFLAGS=" -g -m32 -Wall -pedantic -Wno-long-long -O2 -fno-exceptions -mfpmath=387 "
		> CFLAGS=" -g  -Wall -pedantic -Wno-long-long -O2 -fno-exceptions -mfpmath=387 "

------------------

