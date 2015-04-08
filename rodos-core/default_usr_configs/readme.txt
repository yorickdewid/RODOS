
This directory is just a place holder. 
It has no function for rodos.
This may be useful if:

  1. If you need a different configuration than the default rodos conf.
or
  2. you have diversity versions of your software system


in any case:
 DO NOT MODIFY this directory or its files!!!!!!!!!!!!!


If you have case 1 or 2 (mentioned above):

1. Create in you development tree a directory called for example "userconfs"
      for each of your diversity cases
2. Copy the file userconfig.h
3. If necessary, modify the content of YOUR userconfig.h
4. create inside of this directory other configuration files for YOUR
      application (They have no meaning for RODOS)
5. before compiling, set and export a global shell variable named exactly
      USERCONFIGDIR
      which contains "-I" and the absolute path of your userconf directory
      for example:
      %export USERCONFIGDIR="-I /home/galdalf/ring_support/plan_b/userconfs"
      for other configuration you could use "plan_c" instead of plan_b

Your application may include files from this userconf.
The rodos includes of userconfig.h will be redirected to your current version
and this directory will not be used any more until 
USERCONFIGDIR is unset, for example
       %unset USERCONFIGDIR




