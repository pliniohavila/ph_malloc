#ifndef PH_MALLOC_H
#define PH_MALLOC_H

#define BLOCK_SIZE (sizeof(struct s_block))
#define ALIGN4(x) ((((((x)-1)>>2)<<2)+4))

struct s_block {
    size_t          size;
    size_t          free;
    struct s_block  *next;
};

typedef struct s_block *Block;

Block       find_block(Block *last, size_t size);
Block       extend_heap(Block last, size_t size);
void        split_block(Block block, size_t size); 
Block       get_block(void *ptr);
void        print_heap();
void        copy_block(Block src, Block dst);

void        *ph_malloc(size_t s);
void        *ph_realloc(void *ptr, size_t _size);
void        ph_free(void *ptr);

#endif
