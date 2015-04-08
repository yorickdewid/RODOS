

---------------------------- files and (%linux-programs) ----------------------------


GUI Independent
--------------
(%cp) --> main.cpp  // GUI Independent


GUI Specific
-------------

   (%designer)   
       ^
       |
       |
       V
myGui-designer.ui -->(%uic-qt4) --->  ui_myGui-designer.h
                                            |
                                            V include
                       (%vi) <------> interfaceToMyGui.h -->(%moc-qt4) --> moc_interfaceToMyGui.cpp
                                            |
                                            V include
                       (%vi) <------>interfaceToMyGui.cpp



all together
-------------

(%g++) *.cpp --> tst


------------------------------------------------------------------------------------
---------------------------------- funcion hyrarchie -------------------------------



    client      |                                     server         
<-------------->|<-------------------------------------------------------------------->|
                            main()
               UDP            |
           Port 6021          V                                   generated-GUI-drawer.cpp
RODOS-app ----------->  controller() -----> InterfaceToMyGUI -->  (Ui_MainWindow) <--> QT
                                                |
          <----------- -------------------------+
           Port 6022


          |<----------Independent------->|<--------- GUI specific -------------------->|


