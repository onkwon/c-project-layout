#include <pthread.h>
#include <errno.h>
#include <assert.h>

#include "nrf_mtx.h"

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
	assert(mutex != NULL);
	nrf_mtx_init((nrf_mtx_t *)mutex);

	return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	assert(mutex != NULL);
	nrf_mtx_destroy((nrf_mtx_t *)mutex);

	return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	assert(mutex != NULL);
	while (!nrf_mtx_trylock((nrf_mtx_t *)mutex))
		;

	return 0;
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *timeout)
{
	assert(mutex != NULL);
	assert(false);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
	assert(mutex != NULL);
	return !!nrf_mtx_trylock((nrf_mtx_t *)mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	assert(mutex != NULL);
	nrf_mtx_unlock((nrf_mtx_t *)mutex);

	return 0;
}
