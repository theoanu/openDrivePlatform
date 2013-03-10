#include "buffer.h"

template<uint16_t size>
uint16_t Buffer<size>::Put(uint8_t* buffer, uint16_t length)
{
	uint16_t count;
	uint8_t *next;

	count = 0;
	while(count < length)
	{
		next = _tail + 1;
		if(next - _buffer > size)
			next = _buffer;
		if(next == _head)
			return count;
		*_tail = *(buffer + count);
		_tail = next;
		count++;
	}
	return count;
}

template<uint16_t size>
uint16_t Buffer<size>::Get(uint8_t* buffer, uint16_t length)
{
	uint16_t count;

	count = 0;
	while(count < length)
	{
		if(_head == _tail)
			return count;
		*(buffer + count) = *_head;
		_head++;
		if(_head - _buffer > size)
			_head = _buffer;
		count++;
	}
	return count;
}
