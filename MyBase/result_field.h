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

#include <vector>
#include "arena.h"
#include "dbval.h"

class ResultField
{
public:
  ResultField() {}
  virtual ~ResultField() {}

  virtual void AppendData(const std::vector<DBVal>& recVals) = 0;
  virtual DBVal GetResult() = 0;
  virtual void SetArena(Arena* pArena) {  };
  virtual ResultField* NewField() = 0;

};

//查询原始数据
class RawResultField : public ResultField
{
public:
  RawResultField(int32_t fieldPos)
  {
    this->fieldPos_ = fieldPos;
    this->pArena_ = nullptr;
    DBVAL_SET_NULL(&val_);
  }

  virtual ~RawResultField() {}

  virtual void AppendData(const std::vector<DBVal>& recVals)
  {
    if (recVals[fieldPos_].valType_ == VALUE_TYPE::VAL_STRING)
    {
      if (recVals[fieldPos_].dataLen_ > 0)
      {
        char* pTmp = pArena_->Allocate(recVals[fieldPos_].dataLen_);
        memcpy(pTmp, recVals[fieldPos_].val_.strVal_, recVals[fieldPos_].dataLen_);
        DBVAL_SET_STRING(&val_, pTmp, recVals[fieldPos_].dataLen_);
      }
      else
      {
        DBVAL_SET_STRING(&val_, nullptr, 0);
      }
    }
    else
    {
      val_ = recVals[fieldPos_];
    }
  }

  virtual DBVal GetResult()
  {
    return val_;
  }

  virtual void SetArena(Arena* pArena)
  {
    pArena_ = pArena;
  }

  virtual ResultField* NewField()
  {
    ResultField* pTmp = new RawResultField(fieldPos_);
    pTmp->SetArena(pArena_);
    return pTmp;
  }

private:
  Arena* pArena_;
  int32_t fieldPos_;
  DBVal val_;
};

/////////////////////////////////////////////////////////

//聚合查询

class AvgIntField : public ResultField
{
public:
  AvgIntField(int32_t fieldPos)
  {
    this->fieldPos_ = fieldPos;
    this->totalVal_ = 0;
    this->recCnt_ = 0;
  }

  virtual ~AvgIntField() {}

  virtual void AppendData(const std::vector<DBVal>& recVals)
  {
    if (VALUE_TYPE::VAL_INT == recVals[fieldPos_].valType_)
    {
      totalVal_ += recVals[fieldPos_].val_.intVal_;
      recCnt_++;
    }
  }

  virtual DBVal GetResult()
  {
    DBVal retVal;
    DBVAL_SET_NULL(&retVal);

    if (recCnt_ > 0)
    {
      DBVAL_SET_INT(&retVal, (totalVal_ / recCnt_));
    }

    return retVal;
  }

  virtual ResultField* NewField()
  {
    return new AvgIntField(fieldPos_);
  }

private:
  int32_t fieldPos_;
  int64_t totalVal_;
  int64_t recCnt_;
};

class AvgDoubleField : public ResultField
{
public:
  AvgDoubleField(int32_t fieldPos)
  {
    this->fieldPos_ = fieldPos;
    this->totalVal_ = 0;
    this->recCnt_ = 0;
  }

  virtual ~AvgDoubleField() {}

  virtual void AppendData(const std::vector<DBVal>& recVals)
  {
    if (VALUE_TYPE::VAL_DOUBLE == recVals[fieldPos_].valType_)
    {
      totalVal_ += recVals[fieldPos_].val_.doubleVal_;
      recCnt_++;
    }
  }

  virtual DBVal GetResult()
  {
    DBVal retVal;
    DBVAL_SET_NULL(&retVal);

    if (recCnt_ > 0)
    {
      DBVAL_SET_DOUBLE(&retVal, (totalVal_ / recCnt_));
    }

    return retVal;
  }

  virtual ResultField* NewField()
  {
    return new AvgDoubleField(fieldPos_);
  }

private:
  int32_t fieldPos_;
  double totalVal_;
  int64_t recCnt_;
};

/////////////////////////////////////////////////////////

class CountField : public ResultField
{
public:
  CountField(int32_t fieldPos)
  {
    this->fieldPos_ = fieldPos;
    this->recCnt_ = 0;
  }

  virtual ~CountField() {}

  virtual void AppendData(const std::vector<DBVal>& recVals)
  {
    //fieldPos_ < 0 对应于 count(*) 的查询
    if (fieldPos_ < 0)
      recCnt_++;
    else if (VALUE_TYPE::VAL_NULL != recVals[fieldPos_].valType_)
    {
      recCnt_++;
    }
  }

  virtual DBVal GetResult()
  {
    DBVal retVal;
    DBVAL_SET_INT(&retVal, recCnt_);

    return retVal;
  }

  virtual ResultField* NewField()
  {
    return new CountField(fieldPos_);
  }

private:
  int32_t fieldPos_;
  int64_t recCnt_;
};

/////////////////////////////////////////////////////////

template<int fieldType>
class MaxField : public ResultField
{
public:
  MaxField(int32_t fieldPos)
  {
    this->fieldPos_ = fieldPos;
    DBVAL_SET_NULL(&val_);
  }

  virtual ~MaxField() {}

  virtual void AppendData(const std::vector<DBVal>& recVals)
  {
    if (fieldType == recVals[fieldPos_].valType_)
    {
      if (DBVAL_IS_NULL(&val_))
      {
        val_ = recVals[fieldPos_];
      }
      else if (fieldType == VALUE_TYPE::VAL_INT
        && val_.val_.intVal_ < recVals[fieldPos_].val_.intVal_)
      {
        val_ = recVals[fieldPos_];
      }
      else if (fieldType == VALUE_TYPE::VAL_DOUBLE
        && val_.val_.doubleVal_ < recVals[fieldPos_].val_.doubleVal_)
      {
        val_ = recVals[fieldPos_];
      }
    }
  }

  virtual DBVal GetResult()
  {
    return val_;
  }

  virtual ResultField* NewField()
  {
    return new MaxField<fieldType>(fieldPos_);
  }

private:
  int32_t fieldPos_;
  DBVal val_;
};

/////////////////////////////////////////////////////////

template<int fieldType>
class MinField : public ResultField
{
public:
  MinField(int32_t fieldPos)
  {
    this->fieldPos_ = fieldPos;
    DBVAL_SET_NULL(&val_);
  }

  virtual ~MinField() {}

  virtual void AppendData(const std::vector<DBVal>& recVals)
  {
    if (fieldType == recVals[fieldPos_].valType_)
    {
      if (DBVAL_IS_NULL(&val_))
      {
        val_ = recVals[fieldPos_];
      }
      else if (fieldType == VALUE_TYPE::VAL_INT
        && val_.val_.intVal_ > recVals[fieldPos_].val_.intVal_)
      {
        val_ = recVals[fieldPos_];
      }
      else if (fieldType == VALUE_TYPE::VAL_DOUBLE
        && val_.val_.doubleVal_ > recVals[fieldPos_].val_.doubleVal_)
      {
        val_ = recVals[fieldPos_];
      }

    }
  }

  virtual DBVal GetResult()
  {
    return val_;
  }

  virtual ResultField* NewField()
  {
    return new MinField<fieldType>(fieldPos_);
  }

private:
  int32_t fieldPos_;
  DBVal val_;
};

/////////////////////////////////////////////////////////

class SumIntField : public ResultField
{
public:
  SumIntField(int32_t fieldPos)
  {
    this->fieldPos_ = fieldPos;
    this->totalVal_ = 0;
    this->recCnt_ = 0;
  }

  virtual ~SumIntField() {}

  virtual void AppendData(const std::vector<DBVal>& recVals)
  {
    if (VALUE_TYPE::VAL_INT == recVals[fieldPos_].valType_)
    {
      totalVal_ += recVals[fieldPos_].val_.intVal_;
      recCnt_++;
    }
  }

  virtual DBVal GetResult()
  {
    DBVal ret;
    DBVAL_SET_NULL(&ret);

    if (recCnt_ > 0)
    {
      DBVAL_SET_INT(&ret, totalVal_);
    }

    return ret;
  }

  virtual ResultField* NewField()
  {
    return new SumIntField(fieldPos_);
  }

private:
  int32_t fieldPos_;
  int64_t totalVal_;
  int64_t recCnt_;
};

class SumDoubleField : public ResultField
{
public:
  SumDoubleField(int32_t fieldPos)
  {
    this->fieldPos_ = fieldPos;
    this->totalVal_ = 0;
    this->recCnt_ = 0;
  }

  virtual ~SumDoubleField() {}

  virtual void AppendData(const std::vector<DBVal>& recVals)
  {
    if (VALUE_TYPE::VAL_DOUBLE == recVals[fieldPos_].valType_)
    {
      totalVal_ += recVals[fieldPos_].val_.doubleVal_;
      recCnt_++;
    }
  }

  virtual DBVal GetResult()
  {
    DBVal ret;
    DBVAL_SET_NULL(&ret);

    if (recCnt_ > 0)
    {
      DBVAL_SET_DOUBLE(&ret, totalVal_);
    }

    return ret;
  }

  virtual ResultField* NewField()
  {
    return new SumDoubleField(fieldPos_);
  }

private:
  int32_t fieldPos_;
  double totalVal_;
  int64_t recCnt_;
};

////////////////////////////////////////////////////////
