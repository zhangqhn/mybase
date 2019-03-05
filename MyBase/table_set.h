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

#include <unordered_map>
#include "table.h"
#include "sql_parser.h"
#include "query.h"

class TableSet
{
public:
  TableSet();
  ~TableSet();

  int LoadTables();

  int ShowTables(std::vector<std::string>& tabVec);
  int ShowTableColumns(const std::string& tabName, std::vector<ColInfo>& colVec);
  int CreateTable(const std::string& tabName, const CreateTableParam* pCreateParam, int* pErrPos);
  int DropTable(const std::string& tabName);
  int Insert(const std::string& tabName, const InsertParam* pInsertParam, int* pErrPos);
  int ExecQuery(const std::string& tabName, const QueryParam* pQueryParam, Query** ppQuery, int* pErrPos);
  int Delete(const std::string& tabName, const DeleteParam* pDeleteParam, int* pErrPos);

private:
  Table* GetTable(const char* pName);

private:
  // No copying allowed
  TableSet(const TableSet&);
  void operator=(const TableSet&);

private:
  std::unordered_map<std::string, Table*> tabSet_;
};