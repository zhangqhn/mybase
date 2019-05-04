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

bool StringTool::Utf8LikeCompare(const char* zPattern, const char* zString, size_t len)
{
  int32_t c = 0;
  int32_t c2;

  const char* pEnd = zString + len;

  while ((zString < pEnd) && ((c = tolower((uint8_t)*zPattern)) != 0))
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

      c = tolower(c);
      while (zString < pEnd && ((c2 = tolower((uint8_t)*zString)) != 0))
      {
        while (c2 != 0 && c2 != c)
        {
          zString++;
          c2 = tolower((uint8_t)*zString);
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
      if (c != tolower((uint8_t)*zString))
        return false;
      zPattern++;
      zString++;
      break;
    }
    }
  }

  return (*zString == 0 || zString == pEnd) && (*zPattern == 0);
}
