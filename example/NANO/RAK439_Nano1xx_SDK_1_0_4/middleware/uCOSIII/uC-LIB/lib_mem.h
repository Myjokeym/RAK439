#ifndef _LIB_MEM_H
#define _LIB_MEM_H

#include <stdlib.h>
#include "cpu.h"
#include "os_type.h"

void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );
void pvPortMemDeinit(void);


#endif