#ifndef _LIB_MEM_H
#define _LIB_MEM_H

#include <stdlib.h>

void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );
void pvPortMemDeinit(void);


#endif
