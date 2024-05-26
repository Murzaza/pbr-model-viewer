#include <stdio.h>

#ifndef DEBUG_PRINT
	#define DEBUG_PRINT
	#ifdef _DEBUG
		#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
	#else
		#define DEBUG(...)
	#endif //_DEBUG
	#define MPR_PRINT_ERROR(...) fprintf(stderr, __VA_ARGS__)
#endif //DEBUG_PRINT