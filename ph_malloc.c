#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "ph_malloc.h"

Block   heap_start = NULL;

int     main(void)
{
    printf("PH_MALLOC\n\n\n");
    printf("Initial state:\n");
    print_heap();

    int *p_int = (int*)ph_malloc(sizeof(int) * 10);
    printf("After allocating 10 ints:\n");
    print_heap();

    for (int i = 0; i < 10; i++) 
        p_int[i] = i * 10;
    
    for (int i = 0; i < 10; i++) 
        printf("%d ", p_int[i]);
    printf("\n\n");

    p_int = (int*)ph_realloc(p_int, sizeof(int) * 20);
    for (int i = 0; i < 10; i++) 
        printf("%d ", p_int[i]);
    printf("\n");
    for (int i = 10; i < 20; i++) 
        p_int[i] = i * 10;

    for (int i = 0; i < 20; i++) 
        printf("%d ", p_int[i]);
    printf("\n\n");

    ph_free(p_int);
    // // printf("After freeing first int array:\n");
    // print_heap();

    // char *p_char = (char*)ph_malloc(sizeof(char));
    // // printf("After allocating 1 char:\n");
    // print_heap();
    // *p_char = 'A';
    // // printf("Char value: %c\n\n", *p_char);
    // ph_free(p_char);
    // // print_heap();

    int *p_int2 = (int*)ph_malloc(sizeof(int));
    // // printf("After allocating 1 int:\n");
    print_heap();
    *p_int2 = 42;
    printf("INT value: %d\n\n", *p_int2);
    ph_free(p_int2);
    print_heap();

    return (0);
}

void        *ph_malloc(size_t s)
{
    Block   block;
    Block   last;
    size_t  size;

    size = ALIGN4(s);
    if (heap_start) {
        last = heap_start;
        block = find_block(&last, size);
        if (block) {
            if ((block->size - size) >= (BLOCK_SIZE + 4))
                split_block(block, size);
            block->free = 0;
        } else {
            block = extend_heap(last, size);
            if (!block)
                return (NULL);
        }
    } else {
        block = extend_heap(NULL, size);
        if (!block)
            return (NULL);
        heap_start = block;
    }
    return (void*)((char*)(void*)block + BLOCK_SIZE);
}

void        ph_free(void *ptr)
{
    Block   block;

    if (ptr == NULL)
        return;
    
    block = get_block(ptr);
    block->free = 1;
    if (block->next && block->next->free) {
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
    }
}

void        *ph_realloc(void *ptr, size_t _size)
{
    size_t  size;
    Block   block;
    Block   new_block;
    void    *new_ptr;

    if (!ptr) 
        return (ph_malloc(_size));
    size = ALIGN4(_size);
    block = get_block(ptr);
    if (block->size >= size) {
        if (block->size - size >= (BLOCK_SIZE + 4))
            split_block(block, size);
    } else {
        new_ptr = ph_malloc(size);
        if (!new_ptr) 
            return (NULL);
        new_block = (Block)((void*)new_ptr - BLOCK_SIZE);
        copy_block(block, new_block);
        ph_free(block);
        return new_ptr;
    }
    return (ptr);
}


Block       find_block(Block *last, size_t size)
{
    Block   block;

    block = heap_start;
    while(block && (block->free && block->size >= size)) {
        *last = block;
        block = block->next;
    }
    return block;
}

Block       extend_heap(Block last, size_t size)
{
    Block   block;
    
    block =  sbrk(0);
    if (sbrk(BLOCK_SIZE + size) == (void*)-1)
        return (NULL);
    block->size = size;
    block->free = 0;
    block->next = NULL;
    if (last)
        last->next = block;
    return (block);
}

void        split_block(Block block, size_t size)
{
    Block   new_block;

    new_block = (Block)((char*)block + BLOCK_SIZE + size);
    new_block->size = block->size - size - BLOCK_SIZE;
    new_block->next = block->next;
    new_block->free = 1;
    block-> size = size;
    block->next = new_block;
}

Block       get_block(void *ptr)
{
    return (Block)((char*)ptr - BLOCK_SIZE);
}

void copy_block(Block src, Block dst) 
{
    char    *src_data;
    char    *dst_data;
    size_t  i;

    src_data = (char *)(src + 1);
    dst_data = (char *)(dst + 1);
    
    for (i = 0; i < src->size && i < dst->size; i++)
        dst_data[i] = src_data[i];
}


void    print_heap() {
    Block   current;
    int     i;
    
    current = heap_start;
    i = 0;
    while (current != NULL) {
        printf("Block %d: Address: %p, Size: %zu, Free: %ld\n", i++, (void*)current, current->size, current->free);
        current = current->next;
    }
    printf("Program break: %p\n\n", sbrk(0));
}
