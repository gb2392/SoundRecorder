#ifndef INJECTOR_H
#define INJECTOR_H
#include "serial.h"

typedef uint8_t (*DATA_INJECTOR)(void);

extern DATA_INJECTOR injected_value;

#endif