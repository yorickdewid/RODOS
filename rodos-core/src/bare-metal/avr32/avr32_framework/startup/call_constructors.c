// Lizenz: public domain

typedef void (*constructor)();

// Im Linkerskript definiert
extern constructor __ctors_start;
extern constructor __ctors_end;

// Ruft die Konstruktoren für globale/statische Objekte auf
void call_constructors() {
	constructor* i;
	for (i = &__ctors_start; i != &__ctors_end; ++i) {
		(*i)();
	}
}

void *__dso_handle = 0;

// wird am Ende des Konstruktors globaler Objekte aufgerufen und speichert Destruktoradresse fuer spaeteren Aufruf
// -> Standardimplementierung fuerht zu Absturz, deswegen hier neu implementiert
// -> keine Funtionalität implementiert, weil Destruktoren globaler Objekte in
//    RODOS nie ausgefuehrt werden
// Anm: Diese Funktionalitaet kann beim Kompilieren des Compilers deaktiviert werden, indem
//      man --enable__cxa_atexit nicht angibt. Dann werden die Destruktoradressen beim Kompilieren
//      der embedded SW in eine eigene Section gepackt (.dtors) und koennen genauso wie die Konstruktoren
//      aufgerufen werden.
int __cxa_atexit(void(*f)(void *), void *p, void *d) {
	return 0;
}

// Errorhandler, der ausgefuehrt wird, wenn eine "pure virtual"
// Funktion aufgerufen wird
void __cxa_pure_virtual(void) {
	while(1){}
}
