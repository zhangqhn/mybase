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

#include <string>
#include <vector>

typedef struct _Token
{
  const char* str_;
  int len_;
  int pos_;
}Token;

class ColumnItem
{
public:
  ColumnItem(Token* pToken, int32_t type);
  ~ColumnItem();

  const std::string& GetName() const { return name_; }
  int32_t GetType() const { return type_; }
  int32_t GetSqlPos() const { return sqlPos_; }

private:
  std::string name_;
  int32_t type_;
  int32_t sqlPos_;
};

////////////////////////////////////////////////////////////////////////

class ColumnList
{
public:
  ColumnList();
  ~ColumnList();

  ColumnList* AddColumnItem(ColumnItem* pColItem);
  const std::vector<ColumnItem*>& GetColumnList() const;

private:
  std::vector<ColumnItem*> colItemVec_;
};

/////////////////////////////////////////////////////////////////////////

class ExprList;

class ExprItem
{
public:
  // <expr> AND <expr>, <expr> GREAT <expr> ...
  ExprItem(int op, ExprItem* pLeft, ExprItem* pRight);

  //ID or INTEGER or DOUBLE  or STRING ...
  ExprItem(int op, Token* pTokenStr);

  //MAX(id) ...
  ExprItem(int op, Token* pTokenStr, ExprList* pArgList);

  ~ExprItem();

  const std::string& GetTokenStr() const { return this->tokenStr_; }

  int GetSqlPos() const { return sqlPos_; }
  int GetOp() const { return op_; }
  const ExprItem* GetLeftExpr() const { return pLeft_; }
  const ExprItem* GetRightExpr() const { return pRight_; }
  const ExprList* GetArgList() const { return pArgList_; }

private:
  int sqlPos_; 
  int op_;  //操作类型
  ExprItem* pLeft_;    //左节点, op_ 为 TK_AND 时
  ExprItem* pRight_;   //右节点, op_ 为 TK_AND 时

  //函数参数， 比如： MAX(<expr-list>) , AVG ...
  ExprList* pArgList_;

  std::string tokenStr_;
};

/////////////////////////////////////////////////////////////////////////////

class ExprList
{
public:
  ExprList();
  ~ExprList();

  ExprList* AddExprItem(ExprItem* pExprItem);
  const std::vector<ExprItem*>& GetExprList() const;

private:
  std::vector<ExprItem*> exprVec_;

};

//////////////////////////////////////////////////////////////////////////////

class GroupOpt
{
public:
  GroupOpt(Token* pName);
  ~GroupOpt();

  const std::string& GetGroupField() const;
  int32_t GetSqlPos() const { return sqlPos_; }

private:
  int32_t sqlPos_;
  std::string groupField_;
};

/////////////////////////////////////////////////////////////////////////////

class LimitOpt
{
public:
  LimitOpt(Token* pQueryCnt);
  LimitOpt(Token* pOffset, Token* pQueryCnt);
  ~LimitOpt();

  int32_t GetSqlPos() const { return sqlPos_; }
  int64_t GetOffset() const { return offset_; }
  int64_t GetQueryCnt() const { return queryCnt_; }

private:
  int32_t sqlPos_;
  int64_t offset_;
  int64_t queryCnt_;
};


ColumnList* sqlAppendColumn(ColumnList* pColList, ColumnItem* pColItem);
