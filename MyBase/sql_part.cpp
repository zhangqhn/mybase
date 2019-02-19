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

#include "sql_part.h"
#include "parse.h"
#include "string_tool.h"

ColumnItem::ColumnItem(Token* pToken, int32_t type)
{
  this->name_ = std::string(pToken->str_, pToken->len_);
  this->type_ = type;
  this->sqlPos_ = pToken->pos_;
}

ColumnItem::~ColumnItem()
{
}

/////////////////////////////////////////////////////////////////////////////

ColumnList::ColumnList()
{

}
ColumnList::~ColumnList()
{
  for (auto colIt = colItemVec_.begin(); colIt != colItemVec_.end(); colIt++)
  {
    delete *colIt;
  }
}

ColumnList* ColumnList::AddColumnItem(ColumnItem* pColItem)
{
  this->colItemVec_.push_back(pColItem);
  return this;
}
const std::vector<ColumnItem*>& ColumnList::GetColumnList() const
{
  return this->colItemVec_;
}

//////////////////////////////////////////////////////////////////////////////////

// 逻辑运算 <expr> AND <expr>, <expr> GREAT <expr> ...
ExprItem::ExprItem(int op, ExprItem* pLeft, ExprItem* pRight)
{
  this->sqlPos_ = -1;
  this->op_ = op;
  this->pLeft_ = pLeft;
  this->pRight_ = pRight;

  if (pLeft_ != nullptr)
    this->sqlPos_ = pLeft_->GetSqlPos();

  this->pArgList_ = nullptr;
}

//ID or INTEGER or DOUBLE  or STRING ...
ExprItem::ExprItem(int op, Token* pTokenStr)
{
  this->sqlPos_ = pTokenStr->pos_;
  this->op_ = op;
  this->pLeft_ = nullptr;
  this->pRight_ = nullptr;
  this->pArgList_ = nullptr;
  this->tokenStr_ = std::string(pTokenStr->str_, pTokenStr->len_);
}

//MAX(id) ...
ExprItem::ExprItem(int op, Token* pTokenStr, ExprList* pArgList)
{
  this->sqlPos_ = pTokenStr->pos_;
  this->op_ = op;
  this->pLeft_ = nullptr;
  this->pRight_ = nullptr;
  this->pArgList_ = pArgList;
  this->tokenStr_ = std::string(pTokenStr->str_, pTokenStr->len_);
}

ExprItem::~ExprItem()
{
  if (pLeft_ != nullptr)
    delete pLeft_;

  if (pRight_ != nullptr)
    delete pRight_;

  if (pArgList_ != nullptr)
    delete pArgList_;
}


////////////////////////////////////////////////////////////////////////////////////

ExprList::ExprList()
{

}

ExprList::~ExprList()
{
  for (auto iter = exprVec_.begin(); iter != exprVec_.end(); iter++)
  {
    delete *iter;
  }
}

ExprList* ExprList::AddExprItem(ExprItem* pExprItem)
{
  if (pExprItem != nullptr)
  {
    exprVec_.push_back(pExprItem);
  }

  return this;
}
const std::vector<ExprItem*>& ExprList::GetExprList() const
{
  return exprVec_;
}

/////////////////////////////////////////////////////////////////////

GroupOpt::GroupOpt(Token* pName)
{
  sqlPos_ = pName->pos_;
  groupField_ = std::string(pName->str_, pName->len_);
}
GroupOpt::~GroupOpt() { }

const std::string& GroupOpt::GetGroupField() const
{
  return groupField_;
}

/////////////////////////////////////////////////////////////////////

LimitOpt::LimitOpt(Token* pQueryCnt)
{
  this->sqlPos_ = pQueryCnt->pos_;
  this->offset_ = 0;
  this->queryCnt_ = -1;

  StringTool::StrToInt64(pQueryCnt->str_, pQueryCnt->len_, &queryCnt_);
}

LimitOpt::LimitOpt(Token* pOffset, Token* pQueryCnt)
{
  this->sqlPos_ = pOffset->pos_;
  this->offset_ = -1;
  this->queryCnt_ = -1;

  StringTool::StrToInt64(pOffset->str_, pOffset->len_, &offset_);
  StringTool::StrToInt64(pQueryCnt->str_, pQueryCnt->len_, &queryCnt_);
}

LimitOpt::~LimitOpt()
{
}

//////////////////////////////////////////////////////

ColumnList* sqlAppendColumn(ColumnList* pColList, ColumnItem* pColItem)
{
  if (pColList == nullptr)
    pColList = new ColumnList();

  pColList->AddColumnItem(pColItem);

  return pColList;
}