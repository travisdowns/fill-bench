
#include "env.hpp"
#include "hedley.h"
#include "opt-control.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cycle-timer.h"

// #include "dbg.h"

using namespace env;

// static bool verbose;
// static bool summary;
// static bool debug;
// static bool prefix_cols;
// static bool no_warm;  // true == skip the warmup stamp() each iteration

// static uint64_t tsc_freq;

void pinToCpu(int cpu) {
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    if (sched_setaffinity(0, sizeof(set), &set)) {
        assert("pinning failed" && false);
    }
}

using fill_fn = void(char *p, size_t n);

void fill1(char* p, size_t n) {
    std::fill(p, p + n, 0);
}

void fill2(char* p, size_t n) {
    std::fill(p, p + n, '\0');
}

void hot_wait(size_t cycles) {
    volatile int x = 0;
    (void)x;
    while (cycles--) {
        x = 1;
    }
}

HEDLEY_NEVER_INLINE
void do_bench(const char* name, fill_fn fn, size_t iters, size_t warms, char* buf, size_t size) {
    cl_timepoint start, stop;

    for (size_t outer = 0; outer <= warms; outer++) {
        start = cl_now();
        for (size_t i = 0; i < iters; i++) {
            fn(buf, size);
            sink_ptr(buf);
        }
        stop = cl_now();
    }
    auto delta = cl_delta(start, cl_now());

    auto total_size = iters * size;
    auto cycles = cl_to_cycles(delta);
    printf("%-10s %11.1f %11.2f %11.2f\n", name, total_size / cycles, cl_to_nanos(delta) / total_size, cycles / total_size);
}


int main(int argc, char** argv) {
    auto iters  = env::getenv_generic<size_t>("ITERS", 10000);
    auto buf_sz = env::getenv_generic<size_t>("BUFSZ", 10000);
    auto buf = new char[buf_sz]{};

    // best-effort attempt to get CPU in the highest perforamnce state
    hot_wait(1000 * 1000 * 1000);

    printf("%-10s %11s %11s %11s\n", "Function", "bytes/cycle", "ns/byte", "cycles/byte\n");
    do_bench("fill1", fill1, iters, 1, buf, buf_sz);
    do_bench("fill2", fill2, iters, 1, buf, buf_sz);

    delete [] buf;
}
