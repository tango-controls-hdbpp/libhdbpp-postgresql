/*
Source code file src/pgtypesuint.c
Part of PostgreSQL backend for HDB++ 
Description: Definitions of put and get functions for libpqtypes for pguint-extension types

Authors: Vladimir Sitnov, Alexander Senchenko, George Fatkin
Licence: see LICENCE file
*/

#include "pgtypesuint.h"
#include <libpqtypes.h>
#include <stdint.h>
#include <endian.h>
#include <stdlib.h>
#include <libpq-fe.h>

#include <iostream>

// int uint1_put(PGtypeArgs *args) {
// 	std::cout << "store U1" << std::endl;
// 	int d = va_arg(args->ap, int);
// 	args->format = 0;
// 	args->put.expandBuffer(args, 4);
// 	return snprintf(args->put.out, 4, "%u", d)+1;
// }

// int uint2_put(PGtypeArgs *args) {
// 	std::cout << "store U2" << std::endl;
// 	int d = va_arg(args->ap, int);
// 	args->format = 0;
// 	args->put.expandBuffer(args, 6);
// 	return snprintf(args->put.out, 6, "%u", d)+1;
// }

// int uint4_put(PGtypeArgs *args) {
// 	std::cout << "store U4" << std::endl;
// 	uint32_t d = va_arg(args->ap, uint32_t);
// 	args->format = 0;
// 	args->put.expandBuffer(args, 11);
// 	return snprintf(args->put.out, 11, "%u", d)+1;
// }

// int uint8_put(PGtypeArgs *args) {
// 	std::cout << "store U8" << std::endl;
// 	uint64_t d = va_arg(args->ap, uint64_t);
// 	args->format = 0;
// 	args->put.expandBuffer(args, 21);
// 	return snprintf(args->put.out, 21, "%lu", d)+1;
// }

int int1_put(PGtypeArgs *args) {
	int8_t d = va_arg(args->ap, int);
	*((int8_t *)args->put.out) = d;
	return 1;
}

int uint1_put(PGtypeArgs *args) {
	uint8_t d = va_arg(args->ap, int);
	*((uint8_t *)args->put.out) = d;
	return 1;
}

int uint2_put(PGtypeArgs *args) {
	uint16_t d = va_arg(args->ap, int);
	*((uint16_t *)args->put.out) = htobe16(d);
	return 2;
}

int uint4_put(PGtypeArgs *args) {
	uint32_t d = va_arg(args->ap, uint32_t);
	*((uint32_t *)args->put.out) = htobe32(d);
	return 4;
}

int uint8_put(PGtypeArgs *args) {
	uint64_t d = va_arg(args->ap, uint64_t);
	*((uint64_t *)args->put.out) = htobe64(d);
	return 8;
}

int int1_get(PGtypeArgs *args) {
	int8_t *dp = va_arg(args->ap, int8_t *);
	char *value = PQgetvalue(args->get.result, args->get.tup_num, args->get.field_num);
	if (!dp) return args->errorf(args, "arg cannot be NULL");

	if (PQgetisnull(args->get.result, args->get.tup_num, args->get.field_num) == 1) return 0;
	if (PQfformat(args->get.result, args->get.field_num) == 0) {
		*dp = (int8_t)atoi(value);
		return 0;
	}
	*dp = (int8_t)value[0];
	return 0;
}

int uint1_get(PGtypeArgs *args) {
	uint8_t *dp = va_arg(args->ap, uint8_t *);
	char *value = PQgetvalue(args->get.result, args->get.tup_num, args->get.field_num);
	if (!dp) return args->errorf(args, "arg cannot be NULL");

	if (PQgetisnull(args->get.result, args->get.tup_num, args->get.field_num) == 1) return 0;
	if (PQfformat(args->get.result, args->get.field_num) == 0) {
		*dp = (uint8_t)strtoul(value, 0, 10);
		return 0;
	}
	*dp = (uint8_t)value[0];
	return 0;
}

int uint2_get(PGtypeArgs *args) {
	uint16_t *dp = va_arg(args->ap, uint16_t *);
	char *value = PQgetvalue(args->get.result, args->get.tup_num, args->get.field_num);
	if (!dp) return args->errorf(args, "arg cannot be NULL");

	if (PQgetisnull(args->get.result, args->get.tup_num, args->get.field_num) == 1) return 0;
	if (PQfformat(args->get.result, args->get.field_num) == 0) {
		*dp = (uint16_t)strtoul(value, 0, 10);
		return 0;
	}
	*dp = be16toh(((uint16_t *)value)[0]);
	return 0;
}

int uint4_get(PGtypeArgs *args) {
	uint32_t *dp = va_arg(args->ap, uint32_t *);
	char *value = PQgetvalue(args->get.result, args->get.tup_num, args->get.field_num);
	if (!dp) return args->errorf(args, "arg cannot be NULL");

	if (PQgetisnull(args->get.result, args->get.tup_num, args->get.field_num) == 1) return 0;
	if (PQfformat(args->get.result, args->get.field_num) == 0) {
		*dp = (uint32_t)strtoul(value, 0, 10);
		return 0;
	}
	*dp = be32toh(((uint32_t *)value)[0]);
	return 0;
}

int uint8_get(PGtypeArgs *args) {
	uint64_t *dp = va_arg(args->ap, uint64_t *);
	char *value = PQgetvalue(args->get.result, args->get.tup_num, args->get.field_num);
	if (!dp) return args->errorf(args, "arg cannot be NULL");

	if (PQgetisnull(args->get.result, args->get.tup_num, args->get.field_num) == 1) return 0;
	if (PQfformat(args->get.result, args->get.field_num) == 0) {
		*dp = (uint64_t)strtoull(value, 0, 10);
		return 0;
	}
	*dp = be64toh(((uint64_t *)value)[0]);
	return 0;
}