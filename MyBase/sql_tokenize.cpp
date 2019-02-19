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

#include "sql_tokenize.h"
#include "string_tool.h"
#include "error_code.h"

void* mydbParseAlloc(void*(*)(size_t));
void mydbParseFree(void*p, void(*freeProc)(void*));
void mydbParse(void*, int, Token, SQLParser*);

typedef struct _Keyword
{
  const char* zName;
  int tokenType;
}Keyword;

/*
** If X is a character that can be used in an identifier and
** X&0x80==0 then isIdChar[X] will be 1.  If X&0x80==0x80 then
** X is always an identifier character.  (Hence all UTF-8
** characters can be part of an identifier).  isIdChar[X] will
** be 0 for every character in the lower 128 ASCII characters
** that cannot be used as part of an identifier.
**
** In this implementation, an identifier can be a string of
** alphabetic characters, digits, and "_" plus any character
** with the high-order bit set.  The latter rule means that
** any sequence of UTF-8 characters or characters taken from
** an extended ISO8859 character set can form an identifier.
*/
static const char isIdChar[] = {
  /* x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 0x */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 1x */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 2x */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  /* 3x */
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 4x */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,  /* 5x */
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 6x */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  /* 7x */
};

static Keyword keywordTable[] = {
  { "INSERT", TK_INSERT },
  { "INTO", TK_INTO },
  { "VALUES", TK_VALUES },
  { "IS", TK_IS },
  { "NOT", TK_NOT },
  { "NULL", TK_NULL },
  { "DELETE", TK_DELETE },
  { "SHOW", TK_SHOW },
  { "TABLES", TK_TABLES },
  { "COLUMNS", TK_COLUMNS },
  { "FROM", TK_FROM },
  { "DROP", TK_DROP },
  { "TABLE", TK_TABLE },
  { "CREATE", TK_CREATE },
  { "INT", TK_INT_TYPE },
  { "DOUBLE", TK_DOUBLE_TYPE },
  { "STRING", TK_STRING_TYPE },
  { "SELECT", TK_SELECT },
  { "WHERE", TK_WHERE },
  { "GROUP", TK_GROUP },
  { "BY", TK_BY },
  { "LIMIT", TK_LIMIT },
  { "AND", TK_AND },
  { "LIKE", TK_LIKE },
};

std::unordered_map<uint64_t, int> SQLTokenize::keywordMap_;

void SQLTokenize::InitTokenize()
{
  if (keywordMap_.empty())
  {
    int keyCnt = sizeof(keywordTable) / sizeof(keywordTable[0]);
    for (int i = 0; i < keyCnt; i++)
    {
      uint64_t keyCrc = StringTool::Crc64NoCase(keywordTable[i].zName);
      keywordMap_.insert(std::pair<uint64_t, int>(keyCrc, keywordTable[i].tokenType));
    }
  }
}

int SQLTokenize::RunParser(SQLParser* pParse, Arena* pArena, const char* pSql, size_t sqlLen)
{
  void* pEngine = mydbParseAlloc(malloc);
  if (pEngine == nullptr)
  {
    return ER_NOMEM;
  }

  char* pDestStr = nullptr;
  size_t destLen = 0;

  int tokenType = 0;
  size_t i = 0;
  Token lastToken;

  while (pSql[i] != '\0' && i < sqlLen && !pParse->GetError())
  {
    lastToken.str_ = &pSql[i];
    lastToken.pos_ = i;
    lastToken.len_ = GetToken((const unsigned char*)&pSql[i], (size_t)(sqlLen - i), &tokenType);
    i += lastToken.len_;

    //字符串可能会存在转义， "abc""def" ==> abc"def
    if (tokenType == TK_STRING)
    {
      pDestStr = (char*)pArena->Allocate(lastToken.len_);
      GetStr(lastToken.str_, lastToken.len_, pDestStr, &destLen);

      //字符串在使用的时候都是用长度，所以此处不用设置末尾为0
      lastToken.str_ = pDestStr;
      lastToken.len_ = static_cast<int>(destLen);
    }
    else if (tokenType == TK_ID && 
      (lastToken.str_[0] == '`' || lastToken.str_[0] == '['))
    {
      lastToken.str_++;
      lastToken.len_ -= 2;
    }

    switch (tokenType)
    {
    case TK_SPACE:
    case TK_COMMENT:
      break;
    case TK_ILLEGAL:
      pParse->SetError(lastToken.pos_);
      break;
    default:
      mydbParse(pEngine, tokenType, lastToken, pParse);
    }
  }

  if (!pParse->GetError())
  {
    if (tokenType != TK_SEMI)
    {
      mydbParse(pEngine, TK_SEMI, lastToken, pParse);
    }
    mydbParse(pEngine, 0, lastToken, pParse);
  }

  mydbParseFree(pEngine, free);
  return ER_OK;
}

void SQLTokenize::GetStr(const char* pSourceStr, size_t sourceLen, char* pDest, size_t* pDestLen)
{
  size_t idx = 0;

  char delim = *pSourceStr;

  for (size_t pos = 1; pos < sourceLen - 1; pos++)
  {
    if (pSourceStr[pos] == delim)
    {
      if (pSourceStr[pos + 1] == delim)
      {
        pDest[idx++] = pSourceStr[pos];
        pos++;
      }
      else
        break;
    }
    else
    {
      pDest[idx++] = pSourceStr[pos];
    }
  }

  if (pDestLen != nullptr)
    *pDestLen = idx;
}

int SQLTokenize::GetToken(const unsigned char* z, size_t maxLen, int* tokenType)
{
  size_t i;
  switch (*z) {
  case ' ': case '\t': case '\n': case '\f': case '\r': {
    for (i = 1; i < maxLen && isspace(z[i]); i++) {}
    *tokenType = TK_SPACE;
    return i;
  }
  case '-': {
    if (z[1] == '-') {
      for (i = 2; i < maxLen && z[i] && z[i] != '\n'; i++) {}
      *tokenType = TK_COMMENT;
      return i;
    }
    *tokenType = TK_MINUS;
    return 1;
  }
  case '(': {
    *tokenType = TK_LP;
    return 1;
  }
  case ')': {
    *tokenType = TK_RP;
    return 1;
  }
  case ';': {
    *tokenType = TK_SEMI;
    return 1;
  }
  case '+': {
    *tokenType = TK_PLUS;
    return 1;
  }
  case '*': {
    *tokenType = TK_STAR;
    return 1;
  }
  case '=': {
    *tokenType = TK_EQ;
    if (maxLen > 1)
    {
      return 1 + (z[1] == '=' ? 1 : 0);
    }

    return 1;
  }
  case '<': {
    if (maxLen > 1)
    {
      if (z[1] == '=') {
        *tokenType = TK_LE;
        return 2;
      }
      else if (z[1] == '>') {
        *tokenType = TK_NE;
        return 2;
      }
    }

    *tokenType = TK_LT;
    return 1;
  }
  case '>': {
    if (maxLen > 1 && z[1] == '=') {
      *tokenType = TK_GE;
      return 2;
    }
    else {
      *tokenType = TK_GT;
      return 1;
    }
  }
  case '!': {
    if (maxLen > 1 && z[1] == '=')
    {
      *tokenType = TK_NE;
      return 2;
    }
    else
    {
      *tokenType = TK_ILLEGAL;
      return 1;
    }
  }
  case ',': {
    *tokenType = TK_COMMA;
    return 1;
  }
  case '\'': case '"': {
    int delim = z[0];
    for (i = 1; z[i] && i < maxLen; i++) {
      if (z[i] == delim) {
        if ((i + 1) < maxLen && z[i + 1] == delim) {
          i++;
        }
        else {
          break;
        }
      }
    }
    if (i < maxLen && z[i] == delim)
    {
      i++;
      *tokenType = TK_STRING;
    }
    else
    {
      *tokenType = TK_ILLEGAL;
    }
    return i;
  }
  case '`': {
    for (i = 1; i < maxLen && z[i] != '`'; i++) { }
    if (i < maxLen && z[i] == '`')
    {
      i++;
      *tokenType = TK_ID;
    }
    else
    {
      *tokenType = TK_ILLEGAL;
    }
    return i;
  }
  case '[': {
    for (i = 1; i < maxLen && z[i] != ']'; i++) {}
    if (i < maxLen && z[i] == ']')
    {
      i++;
      *tokenType = TK_ID;
    }
    else
    {
      *tokenType = TK_ILLEGAL;
    }
    return i;
  }
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9': {
    *tokenType = TK_INTEGER;
    for (i = 1; i < maxLen && isdigit(z[i]); i++) {}
    if ((i + 1) < maxLen && z[i] == '.' && isdigit(z[i + 1])) {
      i += 2;
      while (i < maxLen && isdigit(z[i])) { i++; }
      *tokenType = TK_DOUBLE;
    }
    if ((i + 2) < maxLen && (z[i] == 'e' || z[i] == 'E') &&
      (isdigit(z[i + 1]) || ((z[i + 1] == '+' || z[i + 1] == '-') && isdigit(z[i + 2])))
      ) {
      i += 2;
      while (i < maxLen && isdigit(z[i])) { i++; }
      *tokenType = TK_DOUBLE;
    }
    return i;
  }
  default: {
    if ((*z & 0x80) == 0 && !isIdChar[*z]) {
      break;
    }
    for (i = 1; i < maxLen && ((z[i] & 0x80) != 0 || isIdChar[z[i]]); i++) {}
    *tokenType = GetKeywordType((char*)z, i);
    return i;
  }
  }
  *tokenType = TK_ILLEGAL;
  return 1;
}

int SQLTokenize::GetKeywordType(const char* z, int n)
{
  const int maxKeyLen = 32;

  if (n >= maxKeyLen)
    return TK_ID;

  uint64_t tmpKey = StringTool::Crc64NoCase((const unsigned char*)z, n);

  auto keyIter = keywordMap_.find(tmpKey);
  if (keyIter != keywordMap_.end())
  {
    return keyIter->second;
  }
  return TK_ID;
}

