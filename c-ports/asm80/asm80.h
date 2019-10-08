#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>

#ifdef _MSC_VER
#define NORETURN(func)      __declspec(noreturn) void func
#elif __GNUC__
#define NORETURN(func)     __attribute__((noreturn)) void func 
#else
#define NORETURN(func)     void func
#endif

#include "plm80types.h"
#include "literals.h"
#include "data.h"
#include "procs.h"
#include "error.h"


