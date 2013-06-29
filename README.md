os-scheduler
============
Author: Yuriy Skobov
Date: 06/29/2013
============

Scheduler lab for Franke's OS course.

=====================
====== COMPILE ======
=====================

$ make

The above command will create an executable named "sched".

=====================
======== RUN ========
=====================

$ ./sched -sP p_arrival_file random_nums_file

The scheduler simulation takes three required arguments shown above,
where "P" in "-sP" is the scheduling policy to be used and can be one of the following:
  -sF    : First come, first serve
  -sL    : Last come, first serve
  -sS    : Shortest job first
  -sRxx  : Round-Robin where "xx" is an integer quantum

"p_arrival_file" is the file descibing the processes with their arrival times, lengths of execution, etc
"random_nums_file" is the file containing the numbers used for random number generation.

NOTE: random_nums_file and p_arrival_file must be the last and second to last arguments respectively.

====================
====== OUTPUT ======
====================

The results and statistics are printed to standard out.
Execution details are printed to a file named "trace.log".
