
#include "Parallel.hpp"

#ifdef _OPENMP

#include <omp.h> // OpenMP

int ImekaCommon_EXPORT omp_max_threads()
{
    return omp_get_max_threads();
}
int ImekaCommon_EXPORT omp_thread_num()
{
    return omp_get_thread_num();
}

#else

int ImekaCommon_EXPORT omp_max_threads()
{
    return 1;
}
int ImekaCommon_EXPORT omp_thread_num()
{
    return 0;
}

#endif