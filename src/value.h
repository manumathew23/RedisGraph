/*
* Copyright 2018-2019 Redis Labs Ltd. and Contributors
*
* This file is available under the Apache License, Version 2.0,
* modified with the Commons Clause restriction.
*/

#ifndef __SECONDARY_VALUE_H__
#define __SECONDARY_VALUE_H__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "./util/vector.h"

typedef char *SIId;

/* Type defines the supported types by the indexing system. The types are powers
 * of 2 so they can be used in bitmasks of matching types.
 *
 * The order of these values is significant, as the delta between values of
 * differing types is used to maintain the Cypher-defined global sort order
 * in the SIValue_Compare routine. */
typedef enum {
  T_NULL = 0,
  T_STRING = 0x001,
  T_CONSTSTRING = 0x002,
  T_INT32 = 0x004,
  T_INT64 = 0x008,
  T_UINT = 0x010,
  T_BOOL = 0x020,
  T_FLOAT = 0x040,
  T_DOUBLE = 0x080,
  T_PTR = 0x100,

  // special types for +inf and -inf on all types:
  T_INF = 0x200,
  T_NEGINF = 0x400,

} SIType;

#define SI_STRING (T_STRING | T_CONSTSTRING)
#define SI_NUMERIC (T_INT32 | T_INT64 | T_UINT | T_FLOAT | T_DOUBLE)

/* Returns true if aVal and bVal are of the same type, are both string types, or are both numeric types. */
#define SI_COMPARABLE(aVal, bVal) ((aVal).type == (bVal).type || \
		(((aVal).type & SI_NUMERIC) && (bVal).type & SI_NUMERIC) || \
		(((aVal).type & SI_STRING) && (bVal).type & SI_STRING))

/* Returns 1 if argument is positive, -1 if argument is negative,
 * and 0 if argument is zero (matching the return style of the strcmp family).
 * This is necessary to construct safe integer returns when the delta between
 * two double values is < 1.0 (and would thus be rounded to 0). */
#define COMPARE_RETVAL(a) ((a) > 0) - ((a) < 0)

typedef struct {
  union {
    int32_t intval;
    int64_t longval;
    u_int64_t uintval;
    float floatval;
    double doubleval;
    int boolval;
    char *stringval;
    void* ptrval;
  };
  SIType type;
} SIValue;

typedef struct {
  SIValue *vals;
  size_t len;
  size_t cap;
} SIValueVector;

SIValueVector SI_NewValueVector(size_t cap);

/* Free an SIValue. Since we usually allocate values on the stack, this does not
 * free the actual value object, but the underlying value if needed - basically
 * when it's a string */
void SIValue_Free(SIValue *v);

void SIValueVector_Append(SIValueVector *v, SIValue val);
void SIValueVector_Free(SIValueVector *v);

SIValue SI_DuplicateStringVal(const char *s);
SIValue SI_ConstStringVal(const char *s);
SIValue SI_IntVal(int i);
SIValue SI_LongVal(int64_t i);
SIValue SI_UintVal(u_int64_t i);
SIValue SI_FloatVal(float f);
SIValue SI_DoubleVal(double d);
SIValue SI_NullVal();
SIValue SI_BoolVal(int b);
SIValue SI_PtrVal(void* v);
SIValue SI_Clone(SIValue v);

int SIValue_IsNull(SIValue v);
int SIValue_IsNullPtr(SIValue *v);

SIValue SI_InfVal();
SIValue SI_NegativeInfVal();
int SIValue_IsInf(SIValue *v);
int SIValue_IsNegativeInf(SIValue *v);

/*
 * Conversion functions used to make sure a comparison value in a query is of
 * the right type
 */
int SI_LongVal_Cast(SIValue *v, SIType type);
int SI_DoubleVal_Cast(SIValue *v, SIType type);
int SI_StringVal_Cast(SIValue *v, SIType type);

/* Try to parse a value by string. The value's type should be set to
* anything other than T_NULL, to force strict parsing. */
int SI_ParseValue(SIValue *v, char *str);

int SIValue_ToString(SIValue v, char *buf, size_t len);

int SIValue_ToDouble(SIValue *v, double *d);

/* Try to parse a value by string. */
SIValue SIValue_FromString(const char *s);

/* Determines number of bytes required to concat strings. */
size_t SIValue_StringConcatLen(SIValue* strings, unsigned int string_count);

/* Concats strings as a comma separated string. */
size_t SIValue_StringConcat(SIValue* strings, unsigned int string_count, char *buf, size_t buf_len);

/* Compares two SIValues and returns a value similar to strcmp.
 * If the values are not both strings or both numerics, the return value reflects
 * Cypher's orderability constraint, where string > number > NULL. */
int SIValue_Compare(SIValue a, SIValue b);

void SIValue_Print(FILE *outstream, SIValue *v);

#endif // __SECONDARY_VALUE_H__

