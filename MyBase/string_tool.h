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
**   长沙巨松软件科技有限公司
**   http://www.mybase.vip
*/

#include <stdint.h>

class StringTool
{
public:

  static uint64_t Crc64(const char* pStr);
  static uint64_t Crc64(const char* pStr, size_t len);
  static uint64_t Crc64NoCase(const char* pStr);
  static uint64_t Crc64NoCase(const unsigned char* pStr, size_t len);
  static bool Utf8LikeCompare(const char* zPattern, const char* zString, size_t len);
  static bool StrToInt64(const char* pStr, size_t len, int64_t* pVal);
  static bool StrToDouble(const char* pStr, size_t len, double* pVal);
};

