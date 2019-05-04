#pragma once
/*
** Copyright (c) 2019 ZhangQuan
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public
** License as published by the Free Software Foundation; either
** version 3 of the License, or (at your option) any later version.
**
** Author contact information:
**   zhangqhn@foxmail.com
*/

#include <stdint.h>

enum VALUE_TYPE{
  VAL_NULL = 0,
  VAL_INT = 1,
  VAL_DOUBLE = 2,
  VAL_STRING = 3,
};

typedef struct _DBVal
{
  int valType_;          //value type , null, int, double, string
  int dataLen_;          //data length
  union {
    int64_t intVal_;
    double doubleVal_;
    const char* strVal_;
  } val_;
}DBVal;

#define DBVAL_SET_NULL(pVal) do { \
  (pVal)->valType_ = VALUE_TYPE::VAL_NULL; \
  (pVal)->dataLen_ = 0;  \
  (pVal)->val_.intVal_ = 0;  \
} while(false)

#define DBVAL_SET_INT(pVal, v) do { \
  (pVal)->valType_ = VALUE_TYPE::VAL_INT; \
  (pVal)->dataLen_ = 8;  \
  (pVal)->val_.intVal_ = v; \
} while(false)

#define DBVAL_SET_DOUBLE(pVal, v) do { \
  (pVal)->valType_ = VALUE_TYPE::VAL_DOUBLE; \
  (pVal)->dataLen_ = 8;  \
  (pVal)->val_.doubleVal_ = v; \
} while(false)

#define DBVAL_SET_STRING(pVal, pStr, len) do { \
  (pVal)->valType_ = VALUE_TYPE::VAL_STRING; \
  (pVal)->dataLen_ = len;  \
  (pVal)->val_.strVal_ = pStr; \
} while(false)

#define DBVAL_IS_NULL(pVal)   ((pVal)->valType_ == VALUE_TYPE::VAL_NULL)

