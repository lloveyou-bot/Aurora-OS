// kernel/src/fifo.c
#include "fifo.h"

void fifo_init(struct FIFO* fifo, int size, unsigned char* buf) {
    fifo->size = size;
    fifo->buf = buf;
    fifo->head = 0;
    fifo->tail = 0;
    fifo->count = 0;
}

int fifo_put(struct FIFO* fifo, unsigned char data) {
    if (fifo->count >= fifo->size) return -1;  // 缓冲区满
    fifo->buf[fifo->tail] = data;
    fifo->tail = (fifo->tail + 1) % fifo->size;
    fifo->count++;
    return 0;
}

int fifo_get(struct FIFO* fifo, unsigned char* data) {
    if (fifo->count <= 0) return -1;  // 缓冲区空
    *data = fifo->buf[fifo->head];
    fifo->head = (fifo->head + 1) % fifo->size;
    fifo->count--;
    return 0;
}