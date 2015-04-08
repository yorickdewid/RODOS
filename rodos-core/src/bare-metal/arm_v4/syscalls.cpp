namespace RODOS {
extern "C"
{
	void* __dso_handle = (void*) &__dso_handle;
	
	int
	__cxa_atexit(void (*func) (void*), void* arg, void* dso_handle)
	{
		return 0;
	}
	
	void
	__cxa_pure_virtual(void)
	{
	}
	
	int
	__aeabi_unwind_cpp_pr0(int, void* , void*)
	{
		while(1) {}
		return 0;
	}
	
	int
	__aeabi_atexit(void (*func) (void*), void* arg, void* dso_handle)
	{
		return 0;
	}
	
	int __cxa_guard_acquire (void *g)
	{
		return !*(char *)(g);
	}
	
	void __cxa_guard_release (void *g)
	{
		*(char *) g = 1;
	}
	
	void __cxa_guard_abort (void *g) {
		*(char *) g = 1;
	}
}

void*
memcpy(void* dest, const void* src, int n)
{
	char* d = (char*) dest;
	char* s = (char*) src;
	for (int i = 0; i < n; ++i)
	{
		*d++ = *s++;
	}
	return dest;
}
}
