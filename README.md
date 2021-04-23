**Inter Processor Communication via Tuples**

IPCT is a library to support IPC between multiple processor where the ABI
needs to remain stable but the message content and types are expected to
rapidly change.

IPCT messaging works at a high level on the basic principle of a standard
header followed by either a tuple array of data OR a private data structure.

Tuple Mode
----------
```
+------------------------+
| IPCT header            |
|   route (optional)     |
|   size (mandatory)     |
|   elems (mandatory)    |
+------------------------+
| Tuple ID | Data        |
+------------------------+
| Tuple ID | Data        |
+------------------------+
| Tuple ID | Data        |
+------------------------+
| etc ...................|
+------------------------+
```
(example 1 - IPCT header with tuples)

The tuple data is either fixed size or variable size and the tuples
are unordered in the IPC message.

Tuple mode is enabled by setting hdr.size = 1 and hdr.elems = 1

Tuple data can be represented by creating arrays using any combinations of
struct ipct_elem_std, ipct_elem_micro and ipct_elem_micro_array. This
provides flexibility over data expression and data density.


Private Data Mode
-----------------
```
+------------------------+
| IPCT header            |
|   route (optional)     |
|   size (optional)      |
|   elems (optional)     |
+------------------------+
| Private data block     |
+------------------------+
```
(example 2 - IPCT header with private data)

The private data can be anything - The primary use case is C data structures
from previous IPC ABIs. Private data only mode does not use the tuples below
but uses existing or legacy IPC ABI structures.

Private data block only mode is enabled by setting hdr.block = 1.


Tuples
======

Tuples come in two types where type is determined by MSB of tuple ID. This
is to provide flexibility for message density and data size. i.e. small
tuple dense messages are supported alongside large messages with variable
tuple density.

1) Standard tuple element - Minimum 2 words (1 data word) - max 256kB

2) Micro tuple element - Fixed size 1 word (1 data short).

Where the tuple IDs are in a continuous range then the protocol can compress
tuples and omit IDs for each tuple are ID[0] (the base tuple ID) meaning the
tuple array is data only with each subsequent word/short being the next
element in the array.

Tuple IDs
---------

The tuple ID is a 15bit number unique only to the class, subclass, AND action
meaning each action can have up to 2^14 standard tuple data elements and 2^14
micro tuple elements or 2^15 data element IDs in total.

This tuple ID range gives enough head room for ID deprication and new ID
additions without having to add and code new actions.


