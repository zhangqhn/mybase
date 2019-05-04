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

#include "sql_part.h"
#include "parse.h"

class CreateTableParam
{
public:
  ColumnList* pColList_;

  CreateTableParam()
  {
    this->pColList_ = nullptr;
  }

  ~CreateTableParam()
  {
    if (this->pColList_ != nullptr)
      delete this->pColList_;
  }
};

class DeleteParam
{
public:
  ExprItem* pCondition_;

  DeleteParam()
  {
    this->pCondition_ = nullptr;
  }

  ~DeleteParam()
  {
    if (this->pCondition_ != nullptr)
      delete this->pCondition_;
  }
};

class QueryParam
{
public:
  ExprList* pTargetList_;
  ExprItem* pCondition_;
  GroupOpt* pGroup_;
  LimitOpt* pLimit_;

  QueryParam()
  {
    this->pTargetList_ = nullptr;
    this->pCondition_ = nullptr;
    this->pGroup_ = nullptr;
    this->pLimit_ = nullptr;
  }

  ~QueryParam()
  {
    if (this->pTargetList_ != nullptr)
      delete this->pTargetList_;

    if (this->pCondition_ != nullptr)
      delete this->pCondition_;

    if (this->pGroup_ != nullptr)
      delete this->pGroup_;

    if (this->pLimit_ != nullptr)
      delete this->pLimit_;
  }
};

class InsertParam
{
public:
  ExprList* pNameList_;
  ExprList* pValList_;

  InsertParam()
  {
    this->pNameList_ = nullptr;
    this->pValList_ = nullptr;
  }

  ~InsertParam()
  {
    if (this->pNameList_ != nullptr)
      delete this->pNameList_;

    if (this->pValList_ != nullptr)
      delete this->pValList_;
  }
};

class SQLParser
{
public:
  SQLParser();
  ~SQLParser();

  int GetErrPos() const;
  bool GetError() const;
  void SetError(int errPos);

  void SetShowTables();
  void SetShowTableColumns(Token* pTab);
  void SetCreateTable(Token* pTab, ColumnList* pColList);
  void SetDropTable(Token* pTab);
  void SetDelete(Token* pTab, ExprItem* pCondition);
  void SetQuery(ExprList* pTargetList, Token* pTab, ExprItem* pCondition, GroupOpt* pGroup, LimitOpt* pLimit);
  void SetInsert(Token* pTab, ExprList* pNameList, ExprList* pValList);

  enum SQLType
  {
    None = 0,
    ShowTables = 1,
    ShowTableColumns = 2,
    CreateTable = 3,
    DropTable = 4,
    Delete = 5,
    Query = 6,
    Insert = 7,
  };

  const std::string& GetTableName() const;

  const CreateTableParam* GetCreateTableParam() const;
  const DeleteParam* GetDeleteParam() const;
  const QueryParam* GetQueryParam() const;
  const InsertParam* GetInsertParam() const;
  int32_t GetCmdType() const;

private:
  int32_t cmdType_;
  bool isError_;
  int errPos_; 
  
  std::string tabName_;

  CreateTableParam createTableParam_;
  DeleteParam deleteParam_;
  QueryParam queryParam_;
  InsertParam insertParam_;
};
