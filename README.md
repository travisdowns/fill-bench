This is the supporting benchmark for [this post](https://travisdowns.github.io/blog/2020/01/20/zero.html) about the behavior of `std::fill` depending on the exactly type of the to-be-filled value.

I have tested this on Linux and on Windows using Windows Subsystem for Linux.

## Build

    make

## Running

    ./bench

The output on my Skylake i7-6700HQ host is:

~~~
Function   bytes/cycle     ns/byte cycles/byte

fill1              1.0        0.39        1.00
fill2             29.2        0.01        0.03
~~~
