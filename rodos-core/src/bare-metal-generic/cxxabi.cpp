/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief GCC requirements according to the Itanium C++ ABI.
 *
 * For reference, see http://refspecs.linux-foundation.org/cxxabi-1.83.html
 *
 * According to the Itanium C++ ABI the C runtime library shall maintain a
 * list of termination function entries containing the following information:
 *    - A pointer to a termination function.
 *    - An operand to be passed to the function.
 *    - A handle identifying the home shared library of the entry.
 *
 * The list is populated by entries of two kinds:
 *    - Destructors of global (or local static) C++ objects that require
 *      destruction on exit.
 *    - Functions registered by the user with atexit().
 */

// To ready an architecture to use these globals
// - delete local __cxa* functions
// - provide an atomic_cmpxchg in the atomic.h header
// - add the compiler flag -D__USE_GLOBAL_CXXABI__
// Once all architectures are ported to use this interface,
// this #ifdef can be deleted.
#ifdef __USE_GLOBAL_CXXABI__

#include <atomic.h>
#include <params.h>
#include <hw_datatypes.h>

#include <stdint.h>

namespace __cxxabiv1 {

extern "C" {

/**
 * @brief Dynamic shared object handle.
 */
void *__dso_handle = &__dso_handle;

/**
 * @brief If a function is called with an unfilled virtual function pointer.
 *
 * This should never be called since it is not possible to instantiate a
 * class that does not define all virtual functions. If the vtable
 * contains a NULL pointer we call it a 'pure virtual function'.
 */
void __cxa_pure_virtual() {
  // kernel panic
  //panic("\n\nKernel panic: pure virtual called");
}

/**
 * @brief Register a destructor or cleanup function to be called on exit.
 *
 * @param func a pointer to a termination function.
 * @param arg an operand to be passed to the function.
 * @param dso_handle a handle identifying the library of the entry.
 *
 * @retval 0 on success
 * @retval -1 on failure
 */
//int __cxa_atexit(void (*func) (void *), void *arg, void *dso_handle) {
int __cxa_atexit(void (*) (void *), void *, void *) {
  // unnecessary
  return 0;
}

/**
 * @brief Call destructors and exit functions registered with atexit.
 * According to the Itanium C++ ABI the termination function list is to be
 * executed in reverse order.
 *
 * @param func The destructor or exit function to be called.
 */
//void __cxa_finalize(void *func) {
void __cxa_finalize(void *) {
  // unnecessary
}

/*
 * Functions for thread-safe static initialization (from the Itanium C++ ABI).
 * See also gcc/libstdc++-v3/libsupc++/cxxabi.h
 *
 * Associated with each function-scope static variable requiring runtime
 * construction is a guard variable which is used to guarantee that
 * construction occurs only once. Its name is mangled based on the mangling
 * of the guarded object name, to allow distinct instances of the function
 * (e.g. due to inlining) to use the same guard.
 *
 * The size of the guard variable is 64 bits. The first byte (i.e. the byte
 * at the address of the full variable) shall contain the value 0 prior to
 * initialization of the associated variable, and 1 after initialization is
 * complete. Usage of the other bytes of the guard variable is
 * implementation-defined.
 *
 * For static locals GCC generates code like the following:
 *
 *   if (obj_guard.first_byte == 0) {
 *     if ( __cxa_guard_acquire (&obj_guard) ) {
 *       try {
 *         ... initialize the object ...;
 *       } catch (...) {
 *         __cxa_guard_abort (&obj_guard);
 *         throw;
 *       }
 *       ... queue object destructor with __cxa_atexit() ...;
 *       __cxa_guard_release (&obj_guard);
 *     }
 *   }
 *
 * An implementation intending to support automatically thread-safe, one-time
 * initialization makes use of the following API functions:
 *
 *    extern "C" int __cxa_guard_acquire ( __int64_t *guard_object );
 *    extern "C" void __cxa_guard_release ( __int64_t *guard_object );
 *    extern "C" void __cxa_guard_abort ( __int64_t *guard_object );
 */

typedef enum {
  INIT_DONE,
  INIT_PENDING,
  INIT_NONE
} mstate_t;

/**
 * @brief Acquire guard to ensure thread safety for static local variables.
 * This function is called before initialization takes place. If this
 * function returns 1, either __cxa_guard_release or __cxa_guard_abort must
 * be called with the same argument. The first byte of the guard_object is
 * not modified by this function.
 *
 * @param g 64-bit __guard variable
 *
 * @retval 1 if the initialization is not yet completed
 * @retval 0 initialization is completed
 */
int __cxa_guard_acquire (__guard *g) {
  atomic_t *m = (atomic_t *) g;

  /* check that the initializer has not already been run */
  if(m->counter == INIT_DONE)
    return 0;

  /* acquire a look:
   * if possible, we are ready to initialize, set state to PENDING and
   * return 1
   */
  if(INIT_NONE == atomic_cmpxchg(m, INIT_NONE, INIT_PENDING))
    return 1;
  else {
    /* if not possible, wait for completion of the other thread */
    while(m->counter == INIT_PENDING);;
    return 0;
  }
}

/**
 * @brief Mark __guard as initialized.
 *
 * @param g 64-bit __guard variable
 */
void __cxa_guard_release (__guard *g) {
  atomic_t *m = (atomic_t *) g;
  m->counter = INIT_DONE;
}

/**
 * @brief Mark __guard as uninitialized.
 *
 * @param g 64-bit __guard variable
 */
void __cxa_guard_abort (__guard *g) {
  atomic_t *m = (atomic_t *) g;
  m->counter = INIT_NONE;
}

} // extern "C"
} // namespace __cxxabiv1

#endif // __GLOBAL_CXXABI__

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
