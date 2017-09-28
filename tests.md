# TESTS
Tests for simple Web server.

## Http request
Example htttp request as seen by a server running on port 8080. Run dumper.py and type in the url 0.0.0.0:8080 in your browser to see incoming requests.


## Memory Leak Check

We used the memcheck tool from Valgrind to detect if there's any memory leaks in our program.
>From Valgrind.org :

>Valgrind is an instrumentation framework for building dynamic analysis tools. It comes with a set of tools each of which performs some kind of debugging, profiling, or similar task that helps you improve your programs. Valgrind’s architecture is modular, so new tools can be created easily and without disturbing the existing structure.

One of the useful tools provided by Valgring is Memcheck, which is a memory error detector. It helps you make your programs, particularly those written in C and C++, more correct.

To test our server code, run:
>valgrind --leak-check=full -v ./lisod <portnumber>

Here's the result we got after running the memory check:
![memCheck](https://github.com/cornell-cs5450/project-1/blob/master/memLeak.png)
We can see that there appears to be no leaks in our server code. Hooray!
