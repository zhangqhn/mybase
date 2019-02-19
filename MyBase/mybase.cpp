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

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "page.h"
#include "table.h"
#include "sql_tokenize.h"
#include "sql_parser.h"
#include "table_set.h"
#include "error_code.h"

std::string glbDataPath;
const char * pVersion = "1.0";

std::string GbkToUtf8(const std::string& gbkStr);
std::string Utf8ToGbk(const std::string& utf8Str);
int ExecuteSql(TableSet* pTabSet, const char* pSql, int* pErrPos);
void ShowHelp();

void ShowTables(TableSet* pTabSet);
int ShowTableColumns(TableSet* pTabSet, const std::string& tabName);
int ExecQuery(TableSet* pTabSet, const std::string& tabName, const QueryParam* pQueryParam, int* pErrPos);
void SourceFile(TableSet* pTabSet, const char* pFile);

int main(int argc, char* argv[])
{
  //将数据目录设置为应用程序所在目录
  TCHAR szFilePath[MAX_PATH];
  ::GetModuleFileName(NULL, szFilePath, MAX_PATH);
  glbDataPath = szFilePath;
  glbDataPath = glbDataPath.substr(0, glbDataPath.find_last_of('\\'));

  char readBuf[2048];

  //初始化关键字
  SQLTokenize::InitTokenize();

  std::cout << "MyBase 版本: " << pVersion << std::endl;
  std::cout << "长沙巨松软件科技有限公司 (C) 2019 http://www.mybase.vip" << std::endl;

  TableSet tabSet;
  if (tabSet.LoadTables() != 0)
  {
    std::cout << "加载表失败" << std::endl;
    system("pause");
    return -1;
  }

  while (true)
  {
    std::cout << "MyBase > " << std::flush;

    std::cin.getline(readBuf, sizeof(readBuf));
    std::string cmdStr = readBuf;
    size_t bgpos = cmdStr.find_first_not_of(' ');
    if (bgpos != std::string::npos)
    {
      size_t edpos = cmdStr.find_first_of(' ', bgpos);
      std::string startStr;

      if (edpos != std::string::npos)
        startStr = cmdStr.substr(bgpos, (edpos - bgpos));
      else
        startStr = cmdStr.substr(bgpos);

      std::transform(startStr.begin(), startStr.end(), startStr.begin(), ::tolower);
      if (startStr.compare("exit") == 0 || startStr.compare("quit") == 0)
        break;
      else if (startStr.compare("clear") == 0 || startStr.compare("cls") == 0)
        system("cls");
      else if (startStr.compare("help") == 0 || startStr.compare("?") == 0)
        ShowHelp();
      else if (startStr.compare("source") == 0)
      {
        if (edpos == std::string::npos)
        {
          std::cout << "未指定输入文件" << std::endl;
          continue;
        }

        size_t fileBg = cmdStr.find_first_not_of(' ', edpos);
        if (fileBg == std::string::npos)
        {
          std::cout << "未指定输入文件" << std::endl;
          continue;
        }

        std::string filePath = cmdStr.substr(fileBg);
        SourceFile(&tabSet, filePath.c_str());
      }
      else
      {
        //SQL 只有 sql 才需要转utf8编码
        std::string sqlStr = GbkToUtf8(cmdStr);
        int errPos = -1;
        int retVal = ExecuteSql(&tabSet, sqlStr.c_str(), &errPos);
        if (retVal != ER_OK)
        {
          if (errPos >= 0)
          {
            //errPos是UTF8编码， 输出时是GBK编码
            size_t spacePos = sqlStr.find_first_of(' ', errPos);
            std::string errPart = sqlStr.substr(errPos, (spacePos - errPos));
            std::cout <<"SQL: " << Utf8ToGbk(errPart).c_str() <<" ,附近有错误" << std::endl;
          }
          std::cout << GetErrMsg(retVal) << std::endl;
        }
      }
    }
  }

  return 0;
}

int ExecuteSql(TableSet* pTabSet, const char* pSql, int* pErrPos)
{
  SQLParser parser;
  Arena arena;
  int retVal = ER_OK;

  if (pErrPos != nullptr)
    *pErrPos = -1;

  retVal = SQLTokenize::RunParser(&parser, &arena, pSql, strlen(pSql));
  if (retVal != ER_OK)
    return retVal;

  if (parser.GetError())
  {
    if (pErrPos != nullptr)
      *pErrPos = parser.GetErrPos();

    return ER_SQL_ERR;
  }
  else
  {
    switch (parser.GetCmdType())
    {
    case SQLParser::SQLType::ShowTables:
      ShowTables(pTabSet);
      break;
    case SQLParser::SQLType::ShowTableColumns:
      return ShowTableColumns(pTabSet, parser.GetTableName());
    case SQLParser::SQLType::CreateTable:
      retVal = pTabSet->CreateTable(parser.GetTableName(), parser.GetCreateTableParam(), pErrPos);
      break;
    case SQLParser::SQLType::DropTable:
      retVal = pTabSet->DropTable(parser.GetTableName());
      break;
    case SQLParser::SQLType::Delete:
      retVal = pTabSet->Delete(parser.GetTableName(), parser.GetDeleteParam(), pErrPos);
      break;
    case SQLParser::SQLType::Query:
      retVal = ExecQuery(pTabSet, parser.GetTableName(), parser.GetQueryParam(), pErrPos);
      break;
    case SQLParser::SQLType::Insert:
      retVal = pTabSet->Insert(parser.GetTableName(), parser.GetInsertParam(), pErrPos);
      break;
    default:
      retVal = ER_SQL_ERR;
      break;
    }
  }

  return retVal;
}

void ShowHelp()
{
  std::cout << "MyBase 版本: " << pVersion << std::endl;
  std::cout << "长沙巨松软件科技有限公司 (C) 2019 http://www.mybase.vip" << std::endl;
  std::cout << "CMD:" << std::endl;
  std::cout << "  ? | HELP       - 打印帮助信息" << std::endl;
  std::cout << "  CLS | CLEAR    - 清屏" << std::endl;
  std::cout << "  EXIT | QUIT    - 退出" << std::endl;
  std::cout << "  SOURCE <file>  - 执行SQL文件" << std::endl;
  std::cout << "SQL:" << std::endl;
  std::cout << "  SHOW TABLES" << std::endl;
  std::cout << "  SHOW COLUMNS FROM <tab_name>" << std::endl;
  std::cout << "  CREATE TABLE <tab_name>(<col_name> <INT|DOUBLE|STRING> [, ...])" << std::endl;
  std::cout << "  DROP TABLE <tab_name>" << std::endl;
  std::cout << "  INSERT INTO <tab_name>( <col_list> ) VALUES( <val_list> )" << std::endl;
  std::cout << "  SELECT <col_list> FROM <tab_name> [WHERE <condi_list>] [GROUP BY <col_name>] [LIMIT offset,cnt]" << std::endl;
  std::cout << "  DELETE FROM <tab_name> [WHERE <condi_list>]" << std::endl;
}

std::string GbkToUtf8(const std::string& gbkStr)
{
  int len = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, NULL, 0);
  wchar_t* wstr = new wchar_t[len + 1];
  memset(wstr, 0, len + 1);
  MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, wstr, len);
  len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
  char* pStr = new char[len + 1];
  memset(pStr, 0, len + 1);
  WideCharToMultiByte(CP_UTF8, 0, wstr, -1, pStr, len, NULL, NULL);
  std::string result(pStr);
  delete pStr;
  delete wstr;
  return result;
}

std::string Utf8ToGbk(const std::string& utf8Str)
{
  int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
  wchar_t* wstr = new wchar_t[len + 1];
  memset(wstr, 0, len + 1);
  MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, wstr, len);
  len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
  char* pStr = new char[len + 1];
  memset(pStr, 0, len + 1);
  WideCharToMultiByte(CP_ACP, 0, wstr, -1, pStr, len, NULL, NULL);
  std::string result(pStr);
  delete pStr;
  delete wstr;
  return result;
}

#define PRINT_LINE(lineLen) do { std::cout.fill('-'); std::cout.width(lineLen); std::cout<< "-" << std::endl; std::cout.fill(' '); } while(false)
#define PRINT_VAL_LEFT(val) do { std::cout.width(9); std::cout.setf(std::ios::left); std::cout << (val) << "|"; std::cout.unsetf(std::ios::left); } while(false)
#define PRINT_VAL_RIGHT(val) do { std::cout.width(9); std::cout.setf(std::ios::right); std::cout << (val) << "|"; std::cout.unsetf(std::ios::right); } while(false)

int ExecQuery(TableSet* pTabSet, const std::string& tabName, const QueryParam* pQueryParam, int* pErrPos)
{
  int retVal = ER_OK;
  Query* pQuery = nullptr;

  retVal = pTabSet->ExecQuery(tabName, pQueryParam, &pQuery, pErrPos);
  if (retVal != ER_OK)
    return retVal;

  //输出查询结果
  const std::vector<std::string>& nameVec = pQuery->getNameVec();
  size_t lineSize = nameVec.size() * 10;
  PRINT_LINE(lineSize);
  for (auto nameIt = nameVec.begin(); nameIt != nameVec.end(); nameIt++)
  {
    PRINT_VAL_LEFT(nameIt->c_str());
  }
  std::cout << std::endl;
  PRINT_LINE(lineSize);

  std::string strVal;
  std::vector<DBVal> valVec;
  const std::vector<DataRec*>& dataVec = pQuery->getResultData();
  for (auto dataIt = dataVec.begin(); dataIt != dataVec.end(); dataIt++)
  {
    (*dataIt)->GetDataRec(valVec);

    for (auto valIt = valVec.begin(); valIt != valVec.end(); valIt++)
    {
      switch (valIt->valType_)
      {
      case VALUE_TYPE::VAL_NULL:
        PRINT_VAL_RIGHT("null");
        break;
      case VALUE_TYPE::VAL_INT:
        PRINT_VAL_RIGHT(valIt->val_.intVal_);
        break;
      case VALUE_TYPE::VAL_DOUBLE:
        PRINT_VAL_RIGHT(valIt->val_.doubleVal_);
        break;
      case VALUE_TYPE::VAL_STRING:
        strVal = std::string(valIt->val_.strVal_, valIt->dataLen_);
        PRINT_VAL_LEFT(Utf8ToGbk(strVal).c_str());
        break;
      }
    }

    std::cout << std::endl;
  }
  PRINT_LINE(lineSize);

  if (pQuery != nullptr)
  {
    delete pQuery;
  }

  return ER_OK;
}

void ShowTables(TableSet* pTabSet)
{
  std::vector<std::string> nameVec;
  pTabSet->ShowTables(nameVec);
  std::cout << "TABLES:" << std::endl;
  for (auto nameIt = nameVec.begin(); nameIt != nameVec.end(); nameIt++)
  {
    std::cout << "  " << nameIt->c_str() << std::endl;
  }
}

int ShowTableColumns(TableSet* pTabSet, const std::string& tabName)
{
  int retVal = ER_OK;
  std::vector<ColInfo> colVec;
  retVal = pTabSet->ShowTableColumns(tabName, colVec);
  if (retVal != ER_OK)
    return retVal;

  std::cout << tabName.c_str() << std::endl;
  PRINT_LINE(20);
  PRINT_VAL_LEFT("COL NAME");
  PRINT_VAL_LEFT("TYPE");
  std::cout << std::endl;
  PRINT_LINE(20);
  for (auto colIt = colVec.begin(); colIt != colVec.end(); colIt++)
  {
    PRINT_VAL_LEFT(colIt->colName_);
    switch (colIt->colType_)
    {
    case VALUE_TYPE::VAL_INT:
      PRINT_VAL_LEFT("INT");
      break;
    case VALUE_TYPE::VAL_DOUBLE:
      PRINT_VAL_LEFT("DOUBLE");
      break;
    case VALUE_TYPE::VAL_STRING:
      PRINT_VAL_LEFT("STRING");
      break;
    }
    std::cout << std::endl;
  }
  PRINT_LINE(20);

  return ER_OK;
}

void SourceFile(TableSet* pTabSet, const char* pFile)
{
  std::ifstream sqlFile;
  sqlFile.open(pFile);
  if (!sqlFile.is_open())
  {
    std::cout << "打开文件:" << pFile << ", 失败" << std::endl;
    return;
  }
  
  int retVal = ER_OK;
  int errPos = -1;
  int idx = 0;

  std::string line;
  while (std::getline(sqlFile, line))
  {
    idx++;

    if (line.find_first_not_of(' ') == std::string::npos)
      continue;

    retVal = ExecuteSql(pTabSet, line.c_str(), &errPos);
    if (retVal != ER_OK)
    {
      std::cout << "第 " << idx << " 条SQL错误" << std::endl;
      if (errPos >= 0)
      {
        //errPos是UTF8编码， 输出时是GBK编码
        size_t spacePos = line.find_first_of(' ', errPos);
        std::string errPart = line.substr(errPos, (spacePos - errPos));
        std::cout << "SQL: " << Utf8ToGbk(errPart).c_str() << " ,附近有错误" << std::endl;
      }
      std::cout << GetErrMsg(retVal) << std::endl;
      break;
    }
  }
  sqlFile.close();
}
