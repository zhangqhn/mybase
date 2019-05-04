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

#include "dbval.h"
#include "parse.h"
#include <vector>
#include <string>

class ConditionItem
{
public:
  ConditionItem() {}
  virtual ~ConditionItem() {}

  virtual bool GetLogic(const std::vector<DBVal>& vals) = 0;
};

template<int action>
class IntCondition : public ConditionItem
{
public:
  IntCondition(int fieldPos, int64_t valParam)
  {
    this->fieldPos_ = fieldPos;
    this->valParam_ = valParam;
  }

  virtual ~IntCondition() {}

  virtual bool GetLogic(const std::vector<DBVal>& vals)
  {
    if (VALUE_TYPE::VAL_INT == vals[fieldPos_].valType_)
    {
      switch (action)
      {
      case TK_LT: //小于
        return vals[fieldPos_].val_.intVal_ < valParam_;
      case TK_LE: //小于等于
        return vals[fieldPos_].val_.intVal_ <= valParam_;
      case TK_GT: //大于
        return vals[fieldPos_].val_.intVal_ > valParam_;
      case TK_GE: //大于等于
        return vals[fieldPos_].val_.intVal_ >= valParam_;
      case TK_EQ: //等于
        return vals[fieldPos_].val_.intVal_ == valParam_;
      case TK_NE: //不等于
        return vals[fieldPos_].val_.intVal_ != valParam_;
      }
    }

    return false;
  }

private:
  int fieldPos_;
  int64_t valParam_;
};

template<int action>
class DoubleCondition : public ConditionItem
{
public:
  DoubleCondition(int fieldPos, double valParam)
  {
    this->fieldPos_ = fieldPos;
    this->valParam_ = valParam;
  }

  virtual ~DoubleCondition() {}

  virtual bool GetLogic(const std::vector<DBVal>& vals)
  {
    const double precision = 0.000000000001;
    if (VALUE_TYPE::VAL_DOUBLE == vals[fieldPos_].valType_)
    {
      switch (action)
      {
      case TK_LT: //小于
        return vals[fieldPos_].val_.doubleVal_ < valParam_;
      case TK_LE: //小于等于
        return vals[fieldPos_].val_.doubleVal_ < (valParam_ + precision);
      case TK_GT: //大于
        return vals[fieldPos_].val_.doubleVal_ > valParam_;
      case TK_GE: //大于等于
        return vals[fieldPos_].val_.doubleVal_ > (valParam_ - precision);
      case TK_EQ: //等于
        return vals[fieldPos_].val_.doubleVal_ > (valParam_ - precision)
          && vals[fieldPos_].val_.doubleVal_ < (valParam_ + precision);
      case TK_NE: //不等于
        return vals[fieldPos_].val_.doubleVal_ < (valParam_ - precision)
          || vals[fieldPos_].val_.doubleVal_ >(valParam_ + precision);
      }
    }

    return false;
  }

private:
  int fieldPos_;
  double valParam_;
};

template<int action>
class StringCondition : public ConditionItem
{
public:
  StringCondition(int fieldPos, const std::string& valParam)
  {
    this->fieldPos_ = fieldPos;
    this->valParam_ = valParam;
  }

  virtual ~StringCondition() {}

  virtual bool GetLogic(const std::vector<DBVal>& vals)
  {
    if (VALUE_TYPE::VAL_STRING == vals[fieldPos_].valType_)
    {
      switch (action)
      {
      case TK_EQ:
        if (vals[fieldPos_].dataLen_ != valParam_.size())
          return false;
        return strncmp(valParam_.c_str(), vals[fieldPos_].val_.strVal_, valParam_.size()) == 0;
      case TK_NE:
        if (vals[fieldPos_].dataLen_ != valParam_.size())
          return true;
        return strncmp(valParam_.c_str(), vals[fieldPos_].val_.strVal_, valParam_.size()) != 0;
      case TK_LIKE:
        return StringTool::Utf8LikeCompare(valParam_.c_str(), vals[fieldPos_].val_.strVal_, vals[fieldPos_].dataLen_);
      }
    }

    return false;
  }

private:
  int fieldPos_;
  std::string valParam_;
};

class IsNotNullCondition : public ConditionItem
{
public:
  IsNotNullCondition(int fieldPos)
  {
    this->fieldPos_ = fieldPos;
  }

  virtual ~IsNotNullCondition() {}

  virtual bool GetLogic(const std::vector<DBVal>& vals)
  {
    return VALUE_TYPE::VAL_NULL != vals[fieldPos_].valType_;
  }

private:
  int fieldPos_;
};

class IsNullCondition : public ConditionItem
{
public:
  IsNullCondition(int fieldPos)
  {
    this->fieldPos_ = fieldPos;
  }

  virtual ~IsNullCondition() {}

  virtual bool GetLogic(const std::vector<DBVal>& vals)
  {
    return VALUE_TYPE::VAL_NULL == vals[fieldPos_].valType_;
  }

private:
  int fieldPos_;
};

class Condition
{
public:
  Condition();
  ~Condition();

  void AddCondition(ConditionItem* pItem);

  bool RunCondition(const std::vector<DBVal>& rec);

private:
  std::vector<ConditionItem*> condiVec_;
};