General Description
===================
Each user task shows a different way to generate monitoring reports.
All of them are equivalent. You may use according to your convinience.

Printer shows different ways to extract (deserialize) the parameters
of the report. All them are equivalent. Use them according to your convinience.

Local Compilation and Usage
===========================
You may compile all sources:
                
        linux-executable *.cpp

or

        posix-executable *.cpp

and execute
                
        tst

afterwards.


Or you may select just one or more examples (first begin with only one example),
e.g.

        linux-executable monitoringMsg.cpp printer.cpp usertask_evencnt.cpp 


Distributed Compilation and Usage
=================================
To execute user_tasks and printer on different nodes, try the ready to use
shell script 

        execute-distributed

You may kill some user-windows to see how the rest works.
You may start on other terminals the user tasks again.

Note: 
     The user nodes have no prints, all prints will be in the monitor node.
     From the user node you may only see which applications are
     loaded in each node.
