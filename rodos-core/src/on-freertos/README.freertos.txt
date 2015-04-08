General remarks
---------------

Purpose of using the FreeRTOS library is to uses its support to different
hardware platforms, but keeping the Rodos API. Like RODOS, FreeRTOS owns also
source files and macros to run on Windows using MinGW and Posix systems used
for development.

Free RTOS comes for a large number of platforms. The implementations provided
are often created with a specific functionality in mind. Many functions might
be used directly, but to a large extend these functions are seen as examples
on how to utilize the platform. There is also caution needed, e. g. in terms
of memory allocation where malloc is used, what can not be applied in RODOS,
or signal handler might interfer with the timer. Basically it is very hard to
make a generic library, and the structure provided here has to be seen as an
example. Some boards and processors come with there own IDE and settings. It
might be necessary to retsructure the folder than.


Posix library
-------------

In this version the POSIX interface has been used. It has been ported from
FreeRTOS version 5.1 to match FreeRTOS version 7.2.
Here not the full functionality of FreeRTOS has been used, instead only the
task interface has been ported, which in turn wraps Posix threads.
The event handler remains as it is. Also Queues from FreeRTOS not used, since
RODOS buffers are slightly different and designed to work with topics.


Adding user specific platforms
-------------------------------

The current folder structure has been selected as shown below.  All FreeRTOS
specific files are located under the on-freertos folder, which in turn hosts
under the folder portable paltform specific settings.  For compilation this
means, that also variables have to be set specific to the hardware.

# rodos
#  |- api
#  |- src
#     |- on-freertos       - Wrapper for FreeRTOS kernel: ${ARCH}
#        |- FreeRTOS       - FreeRTOS kernel 
#           |- include     - FreeRTOS kernel includes
#           |- portable
#              |- MemMang  - varies possibly with the architecture
#              |- Posix    - Posix dependend ports: ${SUB_ARCH}
#              |- PC       - PC architecture dependend ports: ${SUB_ARCH}
#              |- ARM_XXX  - library for processor ARM xxx: ${SUB_ARCH}
#              |- avr32_uc3 - library for a specific processor : ${SUB_ARCH}
#              |- : 


When including a header file from FreeRTOS into a source file, the generic
header needs to be included first to provide the definitions. E. g.
#include "FreeRTOS.h"  // Contains type specifications of FreeRTOS
#include "serial.h"    // FreeRTOS definitions for serial port


FreeRTOSConfig.h
----------------

This include file contains specific settings to the application but
independend from the platform.


FreeRTOSConfig.h
----------------

Each application needs a FreeRTOSConfig.h It is difficult to decide where is
the correct place for it, originally it was located in the FreeRTOS root
directory. It is in the users hands to configure FreeRTOS for his own
needs. Since FreeRTOSConfig contains also platform specific settings, that
will vary also if the project remains the same, e. g. using Posix under Linux
and switch to the destination platform more than one FreeRTOSConfig might be
used located under the "portable/platform" folder.


Timer
------

The timer functionality is optional in FreeRTOS. It might be used instead of
the RODOS timer. In order to de-/activate it the flag configUSE_TIMERS must be
un-/set in FreeRTOS.h

It requires  FreeRTOSConfig.h the following additional settings:
#define configTIMER_TASK_PRIORITY  	4
#define configTIMER_QUEUE_LENGTH	10
#define configTIMER_TASK_STACK_DEPTH	1000
in FreeRTOSConfig.h


Customization for specific hardware
===================================

FreeRTOS/portable/Posix
-----------------------
Version that uses POSIX pthread library. Works on POSIX like systems.


FreeRTOS/portable/MemMang
-------------------------
Contains examples for a heap memory manager. Due to fact that RODOS does not
free any threads or memory heap_1.c is used.
heap_1.c - Using a static memory block, without freeing any memory (deletion
of tasks or threads or deletion of buffer). Simplest version.
heap_2.c - Using a static block, but manage the memory allocation dynamically
heap_3.c - Dynamic memory allocation, works on Posix or MinGW using malloc and free
If heap_1.c and heap_2.c are used, the variable configTOTAL_HEAP_SIZE in
FreeRTOSConfig.h must be set to the correct size.

Also the variable configMINIMAL_STACK_SIZE needs to be adjusted to the actual
needs.


FreeRTOS/portable/Common
------------------------

This folder contains examples and source code files wrapping port specific
functions and which are more or less generic for different platforms. Some
functions are quite useful and might be incorporated in a user application.
Depending on the complexity needed either functions of subfolder Full/ or
Minimal/ might be used.  The subfolder include/ contains the declarations for
these functions. If the provided functions are not suitable one can also write
his own functions. By using the declarations provided the user functions
adheres to the overal design.


FreeRTOS/portable/${PLATFORM}
-----------------------------

Under the folder portable the different implementations for platform specific
functionalities are provided. Each folder contains the two files port.c and
portmacro.h which provide definitions common to FreeRTOS with definitions and
macros for a specific platform. Some of these implementations are supported by
the manufacturer of the processor, others are provided by the
community. Therefore it is likely that not the whole range of functionalities
is provided and that is the reason because these files differ in the number of
definitions significantly. Variation is also caused by the fact that different
platforms provide different functions. For development platforms like Posix
and MinGW the own simulations or stubs are needed. 


TODOS
-----

In order to include harware specific libraries the demo examples might be
insufficient. 
For compiling it might be better to divert from the shell scripts in order to
handle recursive compiling or use makefiles instead.
