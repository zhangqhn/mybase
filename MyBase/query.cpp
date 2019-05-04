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
#include "query.h"
#include "string_tool.h"

////////////////////////////////////////////////
DataRec::DataRec(const std::vector<ResultField*>& fieldVec)
{
  for (auto fieldIt = fieldVec.begin(); fieldIt != fieldVec.end(); fieldIt++)
  {
    fieldVec_.push_back((*fieldIt)->NewField());
  }
}

DataRec::~DataRec()
{
  for (auto fieldIt = fieldVec_.begin(); fieldIt != fieldVec_.end(); fieldIt++)
  {
    delete (*fieldIt);
  }
}

void DataRec::AppendData(const std::vector<DBVal>& recVals)
{
  for (auto fieldIt = fieldVec_.begin(); fieldIt != fieldVec_.end(); fieldIt++)
  {
    (*fieldIt)->AppendData(recVals);
  }
}

void DataRec::GetDataRec(std::vector<DBVal>& resultRec)
{
  resultRec.clear();

  for (auto fieldIt = fieldVec_.begin(); fieldIt != fieldVec_.end(); fieldIt++)
  {
    resultRec.push_back((*fieldIt)->GetResult());
  }
}

///////////////////////////////////////////////////////////////////////////


Query::Query()
{
  this->queryOffset_ = 0;
  this->queryCnt_ = 1000;
  this->pCondition_ = nullptr;
}

Query::~Query()
{
  if (pCondition_ != nullptr)
    delete pCondition_;

  for (auto targetIt = targetVec_.begin(); targetIt != targetVec_.end(); targetIt++)
  {
    delete (*targetIt);
  }

  for (auto recIt = recVec_.begin(); recIt != recVec_.end(); recIt++)
  {
    delete (*recIt);
  }
}


void Query::AddTarget(const char* pName, ResultField* pField)
{
  pField->SetArena(&arena_);
  nameVec_.push_back(pName);
  targetVec_.push_back(pField);
}

void Query::SetCondition(Condition* pCondition)
{
  this->pCondition_ = pCondition;
}

void Query::SetQueryOffset(int64_t queryOffset)
{
  this->queryOffset_ = queryOffset;
}

void Query::SetQueryCnt(int64_t queryCnt)
{
  this->queryCnt_ = queryCnt;
}

const std::vector<std::string>& Query::getNameVec() const
{
  return nameVec_;
}

const std::vector<DataRec*>& Query::getResultData() const
{
  return recVec_;
}

//////////////////////////////////////////////////////////////////////////////////


RawQuery::RawQuery()
{
}

RawQuery::~RawQuery()
{

}

void RawQuery::AppendData(const std::vector<DBVal>& recVals)
{
  if (pCondition_ != nullptr)
  {
    if (!pCondition_->RunCondition(recVals))
      return;
  }

  if (queryOffset_ > 0)
  {
    queryOffset_--;
    return;
  }

  if (recVec_.size() < queryCnt_)
  {
    DataRec* pRec = new DataRec(targetVec_);
    pRec->AppendData(recVals);
    recVec_.push_back(pRec);
  }
}

////////////////////////////////////////////////////////
//
GroupQuery::GroupQuery(int groupFieldPos)
{
  this->groupFieldPos_ = groupFieldPos;
  this->pNullGroup_ = nullptr;
  this->skipNull_ = false;
}

GroupQuery::~GroupQuery()
{

}

void GroupQuery::AppendData(const std::vector<DBVal>& recVals)
{
  if (pCondition_ != nullptr)
  {
    if (!pCondition_->RunCondition(recVals))
      return;
  }

  if (groupFieldPos_ < 0)
  {
    //将所有数据分为一组
    recVec_[0]->AppendData(recVals);
    return;
  }

  if (VALUE_TYPE::VAL_NULL == recVals[groupFieldPos_].valType_)
  {
    if (skipNull_)
      return;

    if (queryOffset_ > 0)
    {
      skipNull_ = true;
      queryOffset_--;
      return;
    }

    if (pNullGroup_ != nullptr)
    {
      pNullGroup_->AppendData(recVals);
    }
    else if (recVec_.size() < queryCnt_)
    {
      pNullGroup_ = new DataRec(targetVec_);
      pNullGroup_->AppendData(recVals);
      recVec_.push_back(pNullGroup_);
    }
  }
  else
  {
    std::string groupKey;
    if (VALUE_TYPE::VAL_INT == recVals[groupFieldPos_].valType_)
      groupKey = std::to_string(recVals[groupFieldPos_].val_.intVal_);
    else if (VALUE_TYPE::VAL_STRING == recVals[groupFieldPos_].valType_)
      groupKey = std::string(recVals[groupFieldPos_].val_.strVal_, recVals[groupFieldPos_].dataLen_);
    else
      return;

    if (skipGroup_.find(groupKey) != skipGroup_.end())
      return;

    if (queryOffset_ > 0)
    {
      skipGroup_.insert(groupKey);
      queryOffset_--;
      return;
    }

    auto groupIt = groupMap_.find(groupKey);
    if (groupIt != groupMap_.end())
    {
      groupIt->second->AppendData(recVals);
    }
    else if (recVec_.size() < queryCnt_)
    {
      DataRec* pRec = new DataRec(targetVec_);
      pRec->AppendData(recVals);
      recVec_.push_back(pRec);
      groupMap_.insert(std::pair<std::string, DataRec*>(groupKey, pRec));
    }
  }

}

void GroupQuery::InitResult()
{
  if (groupFieldPos_ < 0)
  {
    DataRec* pRec = new DataRec(targetVec_);
    recVec_.push_back(pRec);
  }
}
