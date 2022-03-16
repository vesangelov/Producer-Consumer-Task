# Producer-Consumer-Task

implements a producer-consumer situation with POSIX threads;
    the producer will add a random or user input integer element in a shared resource, the integer value can be randomly generated;
    the consumer shall remove an element from the shared resource and will display its value on the stdout;
    the shared resource can be a simple variable (one element buffer/queue) or any queue/buffer implementation;
    both the producer and the consumer have to be implemented in separate threads;
    use mutexes and convars to protect the shared resources and synchronize the contexts to avoid any concurrency issues - starvation and blockage;
    bonus points for providing compilable solutions and error handling.
