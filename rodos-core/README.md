Rodos
==========

##Tutorial: How to Begin


The directory structure of the current rodos ditribution ist divided in following dirs:
* **rodos-core**   api and implementations for many differnt hardware platforms
* **rodos-doc**   documentation
* **rodos-support**   programs and libs often used in space applications
* **rodos-tutorials**   Examples, for you

To begin you will need first the rodos-core and the rodos-tutorials


You can find general documentation, introduction and programming directives in the **rodos-
doc** directory.

In this distribution you will find following implementations: 
for Linux-x86, AMR V*, Cortext-*, LEON, PPC, Sparc, and on the top of other operating systems like Posix(Linux), Free-Rtos, Rtems, Windows. For ARM, PPC, LEON, etc you will need the corresponding cross compiler.

You may download from our home-page a Virtual-Box or VmWare boot image which include
already many of these compilers. As development computer we assume a linux installation.
All compile scripts are bash scripts.

In each directory you will find a README_* file. please read this first, then continue.

Please read first the documentation found in

1. rodos-doc/doc/wikipedia-rodos-operating-system.pdf
2. rodos-doc/doc/intro-details.pdf

then you may try some tutorials to get the feeling. 
Please begin with *rodos-tutorials/first_steps*

We recommend you to read:

* rodos-doc/doc/codingdirectives10b.pdf
* rodos-doc/doc/how_to_and_warnings

you will find doxigen code documentation, which you may use as developer reference. The
best is to find an example. We added many for almost every thing in tutorials/core and in all
others tutorials. (search with grep).

To compile the RODOS libraries and applications (test programs, tutorials, your applications)
we use shell (bash) scripts (no makefiles). All scripts are in the directory named "make".

1. Please start a bash shell.

 First we have to set some shell variables.

2. go to your rodos-core directory
3. type **source make/rodosenvs**

 Now you create the rodos libbraries

4. for linux type **linux-lib** (in which directory you are is not important)

 To remove older compilations you may call the **rodos-clear** script. 
 (do not do it now, else repeat step 4)

 By the way it would not be bad to take a look to all scripts in make/*

 Now go to the tutorials, read the README* files, compile and execute the delivered
 programs, then experiment with modifications and with your own programs.

5. go to the rodos-tutorials

   Please follow this sequence:

    **first-steps** how to begin, your first steps with rodos<br />
    **core** ref. basic functions: Threads, Time, Synchronisation<br />
    **udp** communication with the external world<br />
    **alice_bob_charly** simple intern communication using the middleware<br />
    **middleware** intern communication using the RODOS middleware<br />
    **middleware_distributed** networks, distributed computing and middleware<br />

    Other tutorials for special functions

    In each tutorail directory, please read the README_*.pdf file.
    for each file in the tutorial read the code, compile it using the command:

6. Compile:
  * for linux: **linux-executable *file-name* [*file-name* *file-name* *file-name*]**
   * (alternative) for arm: **arm-executable *file-name***

  **Please Note**: you may compile several independent programs, they will be executed
  together. You do not need to modify source code or implement a "main" which call other
  modules in order to integrate many applications. In our way you may develop applications
  totally independent from each other, and then just compile them together. Try this! is total
  different than what you have use until now!

7. To execute the linux version please type
   **tst**

  To load the program in an ARM board and to execute it, please connect the ARM board to
  your host computer (in an USB-UART Port) and type
  **arm-load**

Please be aware: All RODOS classes, functions, variables, etc are in the namespace RODOS;
For all RODOS sources, programs, documentation, algorithms, etc. apply:

Open-source and:
Copyright
2008, 2013 DLR
2010, 2013, University of Würzbrug
