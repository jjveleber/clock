/*
 * RingBuffer.h
 *
 * Created: 8/21/2016 10:04:29 AM
 *  Author: jjveleber
 */ 

#include <avr/io.h>

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#define RING_BUFFER_SIZE    256

typedef struct  RingBuffer
{
	uint8_t buf[RING_BUFFER_SIZE];
	uint8_t head;
	uint8_t tail;
} RingBuffer;

uint8_t ringBufferGet(RingBuffer *_this);
int8_t ringBufferPut(RingBuffer *_this, const uint8_t value);


#endif /* RINGBUFFER_H_ */