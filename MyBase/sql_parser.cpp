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

SQLParser::SQLParser()
{
  cmdType_ = SQLType::None;
  isError_ = false;
  errPos_ = -1;
}
SQLParser::~SQLParser()
{

}

int SQLParser::GetErrPos() const
{
  return errPos_;
}

bool SQLParser::GetError() const
{
  return isError_;
}

void SQLParser::SetError(int errPos)
{
  this->isError_ = true;
  this->errPos_ = errPos;
}

void SQLParser::SetShowTables()
{
  this->cmdType_ = SQLType::ShowTables;
}
void SQLParser::SetShowTableColumns(Token* pTab)
{
  this->cmdType_ = SQLType::ShowTableColumns;
  this->tabName_ = std::string(pTab->str_, pTab->len_);
}
void SQLParser::SetCreateTable(Token* pTab, ColumnList* pColList)
{
  this->cmdType_ = SQLType::CreateTable;
  this->tabName_ = std::string(pTab->str_, pTab->len_);
  this->createTableParam_.pColList_ = pColList;
}
void SQLParser::SetDropTable(Token* pTab)
{
  this->cmdType_ = SQLType::DropTable;
  this->tabName_ = std::string(pTab->str_, pTab->len_);
}
void SQLParser::SetDelete(Token* pTab, ExprItem* pCondition)
{
  this->cmdType_ = SQLType::Delete;
  this->tabName_ = std::string(pTab->str_, pTab->len_);
  this->deleteParam_.pCondition_ = pCondition;
}
void SQLParser::SetQuery(ExprList* pTargetList, Token* pTab, ExprItem* pCondition, GroupOpt* pGroup, LimitOpt* pLimit)
{
  this->cmdType_ = SQLType::Query;
  this->tabName_ = std::string(pTab->str_, pTab->len_);
  this->queryParam_.pTargetList_ = pTargetList;
  this->queryParam_.pCondition_ = pCondition;
  this->queryParam_.pGroup_ = pGroup;
  this->queryParam_.pLimit_ = pLimit;
}
void SQLParser::SetInsert(Token* pTab, ExprList* pNameList, ExprList* pValList)
{
  this->cmdType_ = SQLType::Insert;
  this->tabName_ = std::string(pTab->str_, pTab->len_);
  this->insertParam_.pNameList_ = pNameList;
  this->insertParam_.pValList_ = pValList;
}

const std::string& SQLParser::GetTableName() const
{
  return tabName_;
}

const CreateTableParam* SQLParser::GetCreateTableParam() const
{
  return &createTableParam_;
}
const DeleteParam* SQLParser::GetDeleteParam() const
{
  return &deleteParam_;
}
const QueryParam* SQLParser::GetQueryParam() const
{
  return &queryParam_;
}
const InsertParam* SQLParser::GetInsertParam() const
{
  return &insertParam_;
}

int32_t SQLParser::GetCmdType() const
{
  return cmdType_;
}
