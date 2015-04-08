
Dieses Demonstrator besteht aus

Vier Computer, jeder wird mittels eines Linux Prozess
auf einen eigenen xterm simuliert

Drei Aufgaben (X, Y, Z), die auf den Vier Computer verteilt sind.
Jede Aufgabe soll genau von einem Computer ausgeführt werden,
alle anderen stehen bereit um diese Aufgabe in Notfall zu übernehmen.
Diese Aufgaben werden durch die Tasks controllerX... ControllerZ
ausgeführt.

Ein Actuator Simulator:. Er berechnet nichts, er
bekommt nur die Ergebnisse aus den Berechnungen
für X, Y, Z und gibt sie aus.

Jeder Computer hat ein NodeManager, der die Belastung
des eigenen Computers berechnet und verteilt an allen anderen Computern.
Er sammelt Belastungen von anderen Computern und
berechnet welche ist die Minimale Belastung im Netz.


Ein Netzwerk:. Der Netzwerk wird mittels UDP Nachrichten
Simuliert. Er verbindet die 4 Computer und die Actuatoren
miteinander.


------------------------


Globale Variablen:
	myLoadGlobal  // von eigenen Computer
	minLoadGlobal // con allen Computern


NodeManager {
    TIME_LOOP_EVERY(500*MILLISECONDS) {
        myLoadGlobal = <compute my current cpu load>
        distribute(myLoadGlobal)
        <get load messages from all computern>
        minLoadGlobal = <compute the minimal load from all reports>
    }

}

RecoveryAble_Task {
    Context myContext; // context for task X / Y / Z
    bool activated;    // Shall I control : am I Worker or monitor?
    TIME_LOOP_EVERY(500*MILLISECONDS) {
        <get context from any working controller of the same type>
        if(<other controller is working>) {
            <update my context>
            if(<other controler is running in a node with lower ID>) { // yield
                activated = false;
            }
        } else {     // no one else working, then I do!
            activated = true;
        }

        if(activated && <some node has a load much less than mine>) // yield
            activated = false;
        }

        if(activated) {
        	performTask();            // the task which is different for X, Y, Z
 		<distribute context>
	}
    } // time loop
}


