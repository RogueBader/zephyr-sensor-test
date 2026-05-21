#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define RB_CAPACITY 256

struct accel_sample
{
    int64_t t_ms;
    float ax_ms2;
    float ay_ms2;
    float az_ms2;
};

typedef struct ringbuf
{
    struct accel_sample *buffer;
    size_t capacity;
    size_t head;
    size_t tail;
} ringbuf_t;

ringbuf_t *rb_create(size_t capacity);
void rb_destroy(ringbuf_t *rb);

bool rb_push(ringbuf_t *rb, const struct accel_sample *s);
bool rb_pop(ringbuf_t *rb, struct accel_sample *out);

size_t rb_size(const ringbuf_t *rb);
size_t rb_capacity(const ringbuf_t *rb);

size_t rb_drain(ringbuf_t *rb, struct accel_sample *dst, size_t max);

#endif