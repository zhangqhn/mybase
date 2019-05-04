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

#include "sql_parser.h"
#include "arena.h"
#include <unordered_map>
#include <string>

class SQLTokenize
{
public:
  //解析SQL之前必须先调用此方法
  static void InitTokenize();

  //执行SQL解析
  static int RunParser(SQLParser* pParse, Arena* pArena, const char* pSql, size_t sqlLen);

private:
  static void GetStr(const char* pSourceStr, size_t sourceLen, char* pDest, size_t* pDestLen);
  static int GetToken(const unsigned char* z, size_t maxLen, int* tokenType);
  static int GetKeywordType(const char* z, int n);

  static std::unordered_map<std::string, int> keywordMap_;
};
