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

#include "string_tool.h"
#include <cctype>
#include <memory>


/*
** This table maps from the first byte of a UTF-8 character to the number
** of trailing bytes expected. A value '255' indicates that the table key
** is not a legal first byte for a UTF-8 character.
*/
static const uint8_t xtra_utf8_bytes[256] = {
  /* 0xxxxxxx */
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,

  /* 10wwwwww */
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

  /* 110yyyyy */
  1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,

  /* 1110zzzz */
  2, 2, 2, 2, 2, 2, 2, 2,     2, 2, 2, 2, 2, 2, 2, 2,

  /* 11110yyy */
  3, 3, 3, 3, 3, 3, 3, 3,     255, 255, 255, 255, 255, 255, 255, 255,
};

#define SKIP_UTF8(zIn) { zIn += (xtra_utf8_bytes[*(uint8_t*)zIn] + 1); }

const uint8_t upper_to_lower_char[256] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
  0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
  0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
  0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
  0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
  0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
  0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

bool StringTool::Utf8LikeCompare(const char* zPattern, const char* zString, size_t len)
{
  uint32_t c = 0;
  int32_t c2;

  const char* pEnd = zString + len;

  while ((zString < pEnd) && ((c = upper_to_lower_char[(uint8_t)*zPattern]) != 0))
  {
    switch (c)
    {
    case '%':
    {
      while ((c = zPattern[1]) == '%' || c == '_')
      {
        if (c == '_')
        {
          if (*zString == 0 || zString >= pEnd)
            return false;
          SKIP_UTF8(zString);
        }
        zPattern++;
      }

      if (c == 0)
        return true;

      c = upper_to_lower_char[(uint8_t)c];
      while (zString < pEnd && ((c2 = upper_to_lower_char[(uint8_t)*zString]) != 0))
      {
        while (c2 != 0 && c2 != c)
        {
          zString++;
          c2 = upper_to_lower_char[(uint8_t)*zString];
        }
        if (c2 == 0)
          return false;
        if (StringTool::Utf8LikeCompare(&zPattern[1], zString, (int32_t)(pEnd - zString)))
          return true;
        SKIP_UTF8(zString);
      }
      return false;
    }
    case '_':
    {
      if (zString >= pEnd || *zString == 0)
        return false;
      SKIP_UTF8(zString);
      zPattern++;
      break;
    }
    default:
    {
      if (c != upper_to_lower_char[(uint8_t)*zString])
        return false;
      zPattern++;
      zString++;
      break;
    }
    }
  }

  return (*zString == 0 || zString == pEnd) && (*zPattern == 0);
}

bool StringTool::StrToInt64(const char* pStr, size_t len, int64_t* pVal)
{
  if (pStr == nullptr || pVal == nullptr)
    return false;

  int64_t val = 0;

  bool symbolFlg = true;
  if (*pStr == '-')
  {
    symbolFlg = false;
    len--;
    pStr++;
  }
  else if (*pStr == '+')
  {
    len--;
    pStr++;
  }

  size_t i = 0;
  int c = pStr[0];
  for (i = 0; i < len && c >= '0' && c <= '9'; i++)
  {
    val = val * 10 + c - '0';
    c = pStr[i + 1];
  }
  if (i != len)
    return false;

  *pVal = symbolFlg ? val : -val;

  if (i > 0 &&
    (i < 19 || (i == 19 && memcmp(pStr, "9223372036854775807", 19) <= 0)))
  {
    return true;
  }

  return false;
}

bool StringTool::StrToDouble(const char* pStr, size_t len, double* pVal)
{
  bool symbolFlg = true;
  double v1 = 0.0;
  const char* pTmp = pStr;

  if (pVal == nullptr)
  {
    return false;
  }

  if (*pTmp == '-')
  {
    symbolFlg = false;
    pTmp++;
  }
  else if (*pTmp == '+')
  {
    symbolFlg = true;
    pTmp++;
  }

  while (isdigit(*pTmp)) {
    v1 = v1*10.0 + (*pTmp - '0');
    pTmp++;
  }
  if (*pTmp == '.')
  {
    double divisor = 1.0;
    pTmp++;
    while (isdigit(*pTmp))
    {
      v1 = v1 * 10.0 + (*pTmp - '0');
      divisor *= 10.0;
      pTmp++;
    }
    v1 /= divisor;
  }
  if (*pTmp == 'e' || *pTmp == 'E')
  {
    int esign = 1;
    int eval = 0;
    double scale = 1.0;
    pTmp++;
    if (*pTmp == '-')
    {
      esign = -1;
      pTmp++;
    }
    else if (*pTmp == '+')
    {
      pTmp++;
    }

    while (isdigit(*pTmp))
    {
      eval = eval * 10 + *pTmp - '0';
      pTmp++;
    }

    while (eval >= 64) { scale *= 1.0e+64; eval -= 64; }
    while (eval >= 16) { scale *= 1.0e+16; eval -= 16; }
    while (eval >= 4) { scale *= 1.0e+4; eval -= 16; }
    while (eval >= 1) { scale *= 1.0e+1; eval -= 1; }
    if (esign < 0)
    {
      v1 /= scale;
    }
    else
    {
      v1 *= scale;
    }
  }

  if (pTmp - pStr != len)
  {
    return false;
  }

  if (symbolFlg)
  {
    *pVal = v1;
  }
  else
  {
    *pVal = -v1;
  }

  return true;
}
