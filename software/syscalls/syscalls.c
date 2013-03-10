/**************************************************************************//*****
 * @file     stdio.c
 * @brief    Implementation of newlib syscall
 ********************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "buffer.h"

EXTERN_BUFFER(USB_RX, 256);
EXTERN_BUFFER(USB_TX, 256);

#undef errno
extern int errno;
extern int  _end;

/* Register name faking - works in collusion with the linker.  */
register char * stack_ptr asm ("sp");

caddr_t _sbrk_r (struct _reent *r, int incr) {
    extern char   end asm ("_end"); /* Defined by the linker.  */
    static char * heap_end;
    char *        prev_heap_end;

    if (heap_end == NULL)
        heap_end = & end;

    prev_heap_end = heap_end;

    if (heap_end + incr > stack_ptr) {
        //errno = ENOMEM;
        return (caddr_t) -1;
    }

    heap_end += incr;

    return (caddr_t) prev_heap_end;
}

/*
caddr_t _sbrk ( int incr )
{
  static unsigned char *heap = NULL;
  unsigned char *prev_heap;

  if (heap == NULL) {
    heap = (unsigned char *)&_end;
  }
  prev_heap = heap;

  heap += incr;

  return (caddr_t) prev_heap;
}
*/

int link(char *old, char *new) {
return -1;
}

int _close(int file)
{
  return -1;
}

int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int file)
{
  return 1;
}

int _lseek(int file, int ptr, int dir)
{
  return 0;
}

EXTERN_BUFFER_COPY(USB_RX, USB_TX);

int _read(int file, char *ptr, int len)
{
    if (file != 0) {
        return 0;
    }

    // Use USB CDC Port for stdin
    //while(!VCP_get_char((uint8_t*)ptr)){};
    uint16_t RXLen = BUFFER_DATA_LENGTH(USB_RX);
    uint16_t TXLen = USB_RX_USB_TX_Copy(RXLen);

    // Echo typed characters
    //VCP_put_char((uint8_t)*ptr);

  return 1;
}

int _write(int file, char *ptr, int len)
{
    //VCP_send_buffer((uint8_t*)ptr, len);
	USB_TX_Put((uint8_t*)ptr, len);
	return len;
}

void abort(void)
{
  /* Abort called */
  while(1);
}
          
/* --------------------------------- End Of File ------------------------------ */
