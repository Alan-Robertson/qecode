#ifndef HEAPSORT
#define HEAPSORT
#include <stdio.h>
#include <stdint.h>
 
#ifndef BYTE
#define BYTE unsigned char
#endif


// Sample compare function
/*
 *   int cmpf(void* v_a, void* v_b)
 *   {
 *       return *((int*)v_a)  - *((int*)v_b);
 *   }
 */


typedef int32_t (*comparison_f)(const void* v_a, const void* v_b);
void heapsort(void* buf, const size_t n_elements, const size_t element_size, comparison_f cmp);

void heapify(BYTE* buf, const size_t n_elements, const size_t element_size, comparison_f cmp);

void siftdown(BYTE* buf, const size_t n_elements, const size_t element_size, comparison_f cmp, size_t start);

void byteswap(BYTE* v_a, BYTE* v_b, size_t n_bytes);

size_t tri_max(
    BYTE* buf, 
    const size_t n_elements,
    const size_t element_size,
    comparison_f cmp,
    const size_t parent, 
    const size_t l_child, 
    const size_t r_child);

void heapsort(void* v_buf, const size_t n_elements, const size_t element_size, comparison_f cmp)
{
    BYTE* buf = (BYTE*)v_buf;
    // Turn the array into a heap
    //heapify(buf, n_elements, element_size, cmp);

    for (int i = 0; i < n_elements; i++)
    {
        sym_print(((sym**)buf)[i]);
    }

    
    // Find the largest element of the heap and swap it with the n-1 th element
    size_t end = n_elements - 1;
    while(end > 0)
    {
        byteswap(buf, buf + (end * element_size), element_size);
        end--;

        // Shift the heap to put the next largest element at the start of the heap
        siftdown(buf, end, element_size, cmp, 0);
    }
    return;
}

void heapify(BYTE* buf, const size_t n_elements, const size_t element_size, comparison_f cmp)
{
    // Convert the list to a heap
    size_t start = (n_elements - 2) / 2;

    for (int32_t i = start; i >= 0; i--)
    {
        siftdown(buf, n_elements, element_size, cmp, i);
    }

    return;
}

// Siftdown the heap
void siftdown(BYTE* buf, const size_t n_elements, const size_t element_size, comparison_f cmp, size_t start)
{
    size_t root = start;
    
    size_t l_child = 2 * root + 1;
    size_t r_child = 2 * root + 2;
    size_t swap = tri_max(buf, n_elements, element_size, cmp, root, l_child, r_child);
    
    while (swap != root)
    {
        byteswap(buf + (swap * element_size), buf + (root * element_size), element_size);
        root = swap;

        l_child = 2 * root + 1;
        r_child = 2 * root + 2;
        swap = tri_max(buf, n_elements, element_size, cmp, root, l_child, r_child);
    }
    return;
}

void byteswap(BYTE* a, BYTE* b, size_t n_bytes)
{
    for (size_t i = 0; i < n_bytes; i++)
    {
        a[i] ^= b[i];
        b[i] ^= a[i];
        a[i] ^= b[i];
    }
    return;
}

// Find the largest node between a parent and its children
size_t tri_max(
    BYTE* buf, 
    const size_t n_elements,
    const size_t element_size,
    comparison_f cmp,
    const size_t parent, 
    const size_t l_child, 
    const size_t r_child)
{
    // Assume that the parent is the largest
    size_t max_node = parent;

    // Check the parent vs the left child
    if (l_child < n_elements && 0 < cmp(buf + (l_child * element_size), buf + (max_node * element_size)))
    {
        max_node = l_child;
    }

    // And check the parent vs the right child
    if (r_child < n_elements && 0 < cmp(buf + (r_child * element_size), buf + (max_node * element_size)))
    {
        max_node = r_child;
    }
    return max_node;
}


#endif