#pragma GCC diagnostic push

// Some warnings from minimp4.h
#pragma GCC diagnostic ignored "-Wshift-count-overflow"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

#define MINIMP4_IMPLEMENTATION
#include "minimp4.h"

#pragma GCC diagnostic pop
