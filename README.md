# Memory Allocator

A custom memory allocator built from scratch in C++ that implements
malloc and free without using any standard library allocation functions.

## What I Built

- `request_space()` - grows the heap by requesting memory from the OS via sbrk()
- `my_malloc()` - finds or requests a block, splits large blocks, 8-byte alignment
- `my_free()` - marks blocks as free, coalesces adjacent free blocks
- `print_heap()` - visualizes the entire heap state for debugging

## Key Concepts

- **Block headers** - every allocation has a hidden metadata struct storing
  size, free status, and a pointer to the next block. This is how free()
  knows the size without being told.
- **Free list** - a linked list threaded through the heap itself. The allocator
  walks it on every malloc call looking for a suitable block.
- **Coalescing** - adjacent free blocks get merged into larger blocks to
  fight fragmentation.
- **Splitting** - large free blocks get carved up when a smaller allocation
  comes in so the remainder stays available.
- **8-byte alignment** - every returned pointer is rounded up to an 8-byte
  boundary so the CPU can read data efficiently.
- **sbrk()** - the syscall that shifts brk (the heap boundary pointer)
  upward into virtual address space to grow the heap.

## Build and Run

make
./allocator

## Stress Test

The allocator is verified with 1000 random allocations and frees with
no crashes or memory corruption.
