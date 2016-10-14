# Shared Pages Communication Detection (SPCD)

SPCD is a Linux kernel module to detect communication of parallel applications in shared memory, and use that information to migrate threads between cores based on their affinity.

## Installation
Compile and insert SPCD kernel module:

     $ make
     $ make install
     
## Execution

Rename your binary that you want SPCD to handle so that it ends with ```.x```:

     $ ./app.x
     
Everything else is automatic.


## Publications

The main publications regarding SPCD are:

- Matthias Diener, Eduardo H. M. Cruz, Philippe O. A. Navaux, Anselm Busse, Hans-Ulrich Heiß. “Communication-Aware Process and Thread Mapping Using Online Communication Detection.”, Parallel Computing (PARCO), 2015.
- Matthias Diener, Eduardo H. M. Cruz, Philippe O. A. Navaux. “Communication-Based Mapping using Shared Pages.” International Parallel & Distributed Processing Symposium (IPDPS), 2013.
