Hashr C client
=====

CS 241 Honors Project. Given a hashed string, hashr will find the original string via brute force (hashing all possible strings and comparing the result with the hashed string until match is found).

This code is a C implementation for the hashr client.

Libraries
=====

Alexander Peslyak's C implementation of MD5

libcurl 3, as well as self-produced example code

json-c, as well as self-produced example code

Build
=====

First, ensure that you have met the following dependencies:

libcurl 3

json-c

Running make should be the only required step for builing.


Usage
=====
./md5 n

Where n is the number of threads to execute concurrently. Acceptable values for n are between 1 and 32.
