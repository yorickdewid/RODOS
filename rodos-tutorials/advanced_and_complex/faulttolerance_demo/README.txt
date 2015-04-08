

Fault tolerance demonstrator

to compile see doit-linux
but first make in rodos source make/rodosenvs
then in supportprograms/doit-linux

-----
first compile
	doit-linux
then execute in several terminals:
	execute-it

Kill terminals and see how it continuew working

-----------------

content in progs:

topics.h topics.cpp :
common for all programs: topics for the middleware

steering.cpp: Very simple controller

steering_faulty.cpp: simple controller which makes it workt

checker.cpp: detects faulty nodes



--- select one:
io_simple.cpp: to test with random sensor, the simplest way

io_rodosgui: rodos gui now using QT

io_upd.cpp io_sensor.cpp: UDP connection to the GUI

java_userinterface: gui to test the controller

