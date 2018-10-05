/*
Source code file src/pgtypesuint.h
Part of PostgreSQL backend for HDB++ 
Description: Declarations of put and get functions for libpqtypes for pguint-extension types

Authors: Vladimir Sitnov, Alexander Senchenko, George Fatkin
Licence: see LICENCE file
*/

#ifndef _PG_TYPES_UINT_H
#define _PG_TYPES_UINT_H

#include <libpqtypes.h>

int int1_put(PGtypeArgs *args);
int uint1_put(PGtypeArgs *args);
int uint2_put(PGtypeArgs *args);
int uint4_put(PGtypeArgs *args);
int uint8_put(PGtypeArgs *args);
int int1_get(PGtypeArgs *args);
int uint1_get(PGtypeArgs *args);
int uint2_get(PGtypeArgs *args);
int uint4_get(PGtypeArgs *args);
int uint8_get(PGtypeArgs *args);

#endif