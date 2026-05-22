#include "ringbuf.h"
#include <stdlib.h>

ringbuf_t *rb_create(size_t capacity)
{
    if (capacity < 1) {
        return NULL;
    } 

    ringbuf_t *rb = malloc(sizeof(ringbuf_t));
    if (!rb){
        return NULL;
    }

    rb->buffer = malloc(sizeof(struct accel_sample) * capacity);
    if(!rb->buffer){
        free(rb);
        return NULL;
    }

    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;

    return rb;
}

void rb_destroy(ringbuf_t *rb)
{
    if (!rb) return;

    free(rb->buffer);
    free(rb);
}

static size_t next_index(size_t current, size_t capacity)
{
    return (current + 1) % capacity;
}

bool rb_push(ringbuf_t *rb, const struct accel_sample *s)
{
    size_t next_head = next_index(rb->head, rb->capacity);

    // Buffer full condition
    if (next_head == rb->tail){
        return false;
    }

    rb->buffer[rb->head] = *s;
    rb->head = next_head;

    return true;
}

bool rb_pop(ringbuf_t *rb, struct accel_sample *out)
{
    // Buffer empty condition
    if (rb->head == rb->tail){
        return false;
    }

    *out = rb->buffer[rb->tail];
    rb->tail = next_index(rb->tail, rb->capacity);

    return true;
} 

size_t rb_size(const ringbuf_t *rb)
{
    if (rb->head >= rb->tail){
        return (rb->head - rb->tail);
    }
    else {
        return (rb->capacity - rb->tail + rb->head);
    }
}

size_t rb_capacity(const ringbuf_t *rb)
{
    return rb->capacity;
} 

size_t rb_drain(ringbuf_t *rb, struct accel_sample *dst, size_t max)
{
    size_t count = 0;

    while (count < max && rb_pop(rb, &dst[count])) {
        count++;
    }

    return count;
}