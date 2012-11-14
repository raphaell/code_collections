// got it from http://bobah.net/d4d/source-code/misc/context-switch-benchmark-posix-cpp
#include <sched.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

#include <cstdint>
#include <cstdlib>

#include <iostream>


#define NUM_THREADS 20


namespace {

    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

    volatile long switches = 0L;
    long msec = 0L;

    /**
     * Current GMT time in ms since the Epoch
     */
    uint64_t epochmsec() {
	timeval tv = timeval();
	if (gettimeofday(&tv, 0) == 0) {
	    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}
	else {
	    return 0L;
	}
    }

    /**
     * ALRM signal handler, logs context switching statistics
     */
    void alarm_handler(int signum) {
	alarm(1);
	uint64_t tm = epochmsec();
	uint64_t delta = tm - msec;
	msec = tm;
	std::cout << (delta / (double) switches * 1000) << "us" << std::endl;
	switches = 0;
    }

    /**
     * Thread function
     */
    void* thread_func(void* arg) {
	for(;;) {
	    pthread_mutex_lock(&mtx);
	    ++switches;
	    pthread_mutex_unlock(&mtx);
	    sched_yield();
	}
    }

} // namespace

int main() {
    msec = epochmsec();

    pthread_t threads[NUM_THREADS];

    for (size_t i = 0; i < sizeof(threads) / sizeof(pthread_t); ++i) {
	pthread_create(&threads[i], 0, &thread_func, 0);
    }

    // setup an alarm to print log message every second
    signal(SIGALRM, &alarm_handler);
    alarm(1);

    for (;;) {
	sleep(1000);
    }

    return 0;
}
