# Define "data" symbol to mark beginning of data segment.
# Must be linked before any other data on ld command line.
.data
.globl data
data:
  .word 1

