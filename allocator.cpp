#include <unistd.h> //Include unix standard 
#include <stddef.h> //for size_t

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
}

int main() {
    // test our allocator
    void* ptr1 = my_malloc(64);
    void* ptr2 = my_malloc(32);
    void* ptr3 = my_malloc(128);

    my_free(ptr1);
    my_free(ptr2);
    my_free(ptr3);

    return 0;
}