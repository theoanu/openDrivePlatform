#ifndef __BUFFER_H
#define __BUFFER_H

#include "stdint.h"

#define EXTERN_BUFFER(name, size)										\
extern uint8_t name##_Buffer[size];										\
extern uint8_t *name##_Head, *name##_Tail;								\
extern const uint16_t name##_Size;										\
extern uint32_t name##_BytesRead, name##_BytesWritten;					\
uint16_t name##_Put(uint8_t* buffer, uint16_t length);					\
uint16_t name##_Get(uint8_t* buffer, uint16_t length);

#define BUFFER(name, size)												\
const uint16_t name##_Size = size;										\
uint8_t name##_Buffer[size];											\
uint8_t *name##_Head = name##_Buffer, *name##_Tail = name##_Buffer;		\
uint32_t name##_BytesRead, name##_BytesWritten;							\
																		\
uint16_t name##_Put(uint8_t* buffer, uint16_t length)					\
{																		\
	uint16_t count;														\
	uint8_t *next;														\
																		\
	count = 0;															\
	while(count < length)												\
	{																	\
		next = name##_Tail + 1;											\
		if(next - name##_Buffer >= name##_Size)							\
			next = name##_Buffer;										\
		if(next == name##_Head)											\
		{																\
			name##_BytesWritten += count;								\
			return count;												\
		}																\
		*name##_Tail = *(buffer + count);								\
		name##_Tail = next;												\
		count++;														\
	}																	\
	name##_BytesWritten += count;										\
	return count;														\
}																		\
																		\
uint16_t name##_Get(uint8_t* buffer, uint16_t length)					\
{																		\
	uint16_t count;														\
																		\
	count = 0;															\
	while(count < length)												\
	{																	\
		if(name##_Head == name##_Tail)									\
		{																\
			name##_BytesRead += count;									\
			return count;												\
		}																\
		*(buffer + count) = *name##_Head;								\
		name##_Head++;													\
		if(name##_Head - name##_Buffer >= name##_Size)					\
			name##_Head = name##_Buffer;								\
		count++;														\
	}																	\
	name##_BytesRead += count;											\
	return count;														\
}

#define EXTERN_BUFFER_COPY(source, dest)								\
uint16_t source##_##dest##_Copy(uint16_t length);

#define BUFFER_COPY(source, dest)										\
uint16_t source##_##dest##_Copy(uint16_t length)						\
{																		\
	uint16_t count;														\
	uint8_t *next;														\
																		\
	count = 0;															\
	while(count < length)												\
	{																	\
		if(source##_Head == source##_Tail)								\
			return count;												\
		next = dest##_Tail + 1;											\
		if(next - dest##_Buffer >= dest##_Size)							\
			next = dest##_Buffer;										\
		if(next == dest##_Head)											\
		{																\
			source##_BytesRead += count;								\
			dest##_BytesWritten += count;								\
			return count;												\
		}																\
		*dest##_Tail = *source##_Head;									\
		dest##_Tail = next;												\
		source##_Head++;												\
		if(source##_Head - source##_Buffer >= source##_Size)			\
			source##_Head = source##_Buffer;							\
		count++;														\
	}																	\
	source##_BytesRead += count;										\
	dest##_BytesWritten += count;										\
	return count;														\
}


#define BUFFER_IS_EMPTY(name)	(name##_BytesRead == name##_BytesWritten)
#define BUFFER_IS_FULL(name)	({uint8_t* next = name##_Tail + 1; ((next - name##_Buffer > name##_Size) ? next - name##_Size : next) == name##_Head; })
#define BUFFER_DATA_LENGTH(name)	({int16_t _length = name##_Tail - name##_Head; BUFFER_IS_EMPTY(name) ? 0 : (_length <= 0 ? _length + name##_Size : _length); })
#define BUFFER_CONTIGUOUS_DATA_LENGTH(name)	({int16_t _length = name##_Tail - name##_Head; BUFFER_IS_EMPTY(name) ? 0 : (_length <= 0 ? name##_Buffer + name##_Size - name##_Head : _length); })

#define BUFFER_SIZE(name)	(name##_Size)
#define BUFFER_FREE_SPACE(name)	({int16_t _length = name##_Tail - name##_Head; _length < 0 ? -_length : name##_Size - _length; })
#define BUFFER_CONTIGUOUS_FREE_SPACE(name)	({name##_Tail >= name##_Head ? name##_Buffer - name##_Tail + name##_Size : name##_Head - name##_Tail; })

#define BUFFER_MOVE_HEAD(name, length) ({ name##_Head += length; name##_BytesRead += length; if(name##_Head - name##_Buffer >= name##_Size) name##_Head -= name##_Size; })
#define BUFFER_MOVE_TAIL(name, length) ({ name##_Tail += length; name##_BytesWritten += length; if(name##_Tail - name##_Buffer >= name##_Size) name##_Tail -= name##_Size; })


#endif
