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

#include <Windows.h>
#include <vector>
#include <string>
#include <unordered_map>

#include "page.h"
#include "dbval.h"
#include "condition.h"
#include "sql_parser.h"
#include "query.h"
#include "arena.h"

class Table
{
public:
  Table();
  ~Table();

  int Create(const std::string& tabName, const CreateTableParam* pCreateParam, int* pErrPos);

  int Open(const char* pName);
  int Close();

  int Insert(const InsertParam* pInsertParam, int* pErrPos);
  int ExecQuery(const QueryParam* pQueryParam, Query** ppQuery, int* pErrPos);
  int ExecDelete(const DeleteParam* pDeleteParam, int* pErrPos);

  const std::string& GetTableName() const;
  void GetColumnsInfo(std::vector<ColInfo>& colVec) const;

private:
  int GetFieldPos(const char* pName);

  int BuildQuery(const QueryParam* pQueryParam, Query** ppQuery, int* pErrPos);
  int BuildCondition(const ExprItem* pCondiExpr, Condition* pCondition, int* pErrPos);

  ConditionItem* BuildIntCondition(int fieldPos, int op, int64_t val);
  ConditionItem* BuildDoubleCondition(int fieldPos, int op, double val);
  ConditionItem* BuildStringCondition(int fieldPos, int op, const std::string& val);

  int BuildTarget(Query* pQuery, bool isGroup, int groupFieldPos, const ExprItem* pTarget);

  void LoadRecord(const uint8_t* pRec, std::vector<DBVal>& recVal);
  void CleanPage(DataPage* pPage);

private:
  // No copying allowed
  Table(const Table&);
  void operator=(const Table&);

private:
  HANDLE fileHandle_;
  HANDLE mapHandle_;
  LPVOID pBase_;

  std::string tabName_;
  std::unordered_map<uint64_t, int> colPosMap_;

};

