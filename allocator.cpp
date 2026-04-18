#include <unistd.h> //Include unix standard 
#include <stddef.h> //for size_t
#include <stdio.h>

struct BlockHeader { 
    size_t size; 
    bool is_free; 
    BlockHeader* next; 
};
//Global pointer to start of the block list 
BlockHeader* free_list = nullptr; 

void* request_space(size_t size) {
    BlockHeader* block = (BlockHeader*)sbrk(0);
    void* result = sbrk(sizeof(BlockHeader) + size);
    if (result == (void*)-1) {
        return nullptr; 
    }
    return block; 
}

void* my_malloc(size_t size) {
    if (size == 0) {
        return nullptr; 
    }
    
    //If free list is empty go straight to OS
    if (free_list == nullptr) {
        BlockHeader* block = (BlockHeader*) request_space(size);
        if (block == nullptr) {
            return nullptr; 
        }
        block->size = size; 
        block->is_free = false; 
        block->next = nullptr; 
        free_list = block; 
        return (void*)(block + 1);
    }

    //Otherwise iterate through the free list to find empty block 
    BlockHeader* current = free_list; 
    while (current != nullptr) {
        if (current->is_free && current->size >= size) {
            //Split the block if possible and we have worthwhile space remaining 
            if (current->size >= size + sizeof(BlockHeader) + 16) {
                BlockHeader* new_block = (BlockHeader*) ((char*)(current + 1) + size); 
                new_block->size = current->size - sizeof(BlockHeader) - size; 
                new_block->is_free = true; 
                new_block->next = current->next; 
                current->next = new_block; 
                current->size = size; 
            }
            current->is_free = false; 
            return (void*)(current + 1);
        }
        current = current->next; 
    }

    //If no block is free then request from OS and add to free_list
    BlockHeader* block = (BlockHeader*) request_space(size);
    if (block == nullptr) {
        return nullptr; 
    }
    block->size = size; 
    block->is_free = false; 
    block->next = nullptr; 
    BlockHeader* tail = free_list; 
    while (tail->next != nullptr) {
        tail = tail->next; 
    }
    tail->next = block; 
    return (void*)(block + 1);
}

void my_free(void* ptr) {
    if (ptr == nullptr) {
        return; 
    }
    BlockHeader* block = (BlockHeader*)ptr - 1;
    block->is_free = true; 

    //Coalesce all free adjacent memory blocks 
    BlockHeader* current = free_list; 
    while (current != nullptr && current->next != nullptr) {
        if (current->is_free && current->next->is_free) {
            current->size += (sizeof(BlockHeader) + current->next->size);
            current->next = current->next->next;  
        } else {
            current = current->next; 
        }
    }
}

void print_heap() {
    BlockHeader* current = free_list; 
    int i = 0; 
    while (current != nullptr) {
        printf("Block %d: size=%zu, is_free=%d, address=%p\n",
            i,
            current->size,
            current->is_free,
            (void*)(current + 1));
        current = current->next;
        i++;
    }
    printf("---end of heap---\n");
}

int main() {
    void* ptr1 = my_malloc(64);
    void* ptr2 = my_malloc(64);
    void* ptr3 = my_malloc(128);

    printf("=== after 3 allocations ===\n");
    print_heap();

    my_free(ptr1);
    my_free(ptr2);

    printf("=== after freeing ptr1 and ptr2 (should coalesce) ===\n");
    print_heap();

    // this should reuse the coalesced block
    void* ptr4 = my_malloc(100);
    printf("=== after malloc(100) ===\n");
    print_heap();

    return 0;
}