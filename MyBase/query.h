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

#include <vector>
#include <stdint.h>
#include "dbval.h"
#include "result_field.h"
#include "condition.h"
#include "arena.h"
#include <unordered_set>
#include <unordered_map>

class DataRec
{
public:
  DataRec(const std::vector<ResultField*>& fieldVec);
  ~DataRec();

  void AppendData(const std::vector<DBVal>& recVals);
  void GetDataRec(std::vector<DBVal>& resultRec);

private:
  // No copying allowed
  DataRec(const DataRec&);
  void operator=(const DataRec&);

private:
  std::vector<ResultField*> fieldVec_;
};

class Query
{
public:
  Query();
  virtual ~Query();

  virtual void AppendData(const std::vector<DBVal>& recVals) = 0;
  virtual void InitResult() { }

  void AddTarget(const char* pName, ResultField* pField);

  //为了解决 select count(*) from tab 必须有一条数据的问题
  void SetCondition(Condition* pCondition); 
  void SetQueryOffset(int64_t queryOffset);
  void SetQueryCnt(int64_t queryCnt);
  
  const std::vector<std::string>& getNameVec() const;
  const std::vector<DataRec*>& getResultData() const;

protected:
  int64_t queryOffset_;
  int64_t queryCnt_;
  Arena arena_;

  Condition* pCondition_;

  std::vector<std::string> nameVec_;
  std::vector<ResultField*> targetVec_;

  std::vector<DataRec*> recVec_;
};

//原始数据查询
class RawQuery : public Query
{
public:
  RawQuery();
  virtual ~RawQuery();
  virtual void AppendData(const std::vector<DBVal>& recVals);
};

//分组查询
class GroupQuery : public Query
{
public:
  GroupQuery(int groupFieldPos);
  virtual ~GroupQuery();
  virtual void AppendData(const std::vector<DBVal>& recVals);
  virtual void InitResult();

private:
  int32_t groupFieldPos_;
  DataRec* pNullGroup_;    //null 组的数据
  std::unordered_map<std::string, DataRec*> groupMap_;

  //要跳过的分组，目前group by 后只能跟一个int或string字段
  bool skipNull_; // 是否跳过NULL值
  std::unordered_set<std::string> skipGroup_;
};
