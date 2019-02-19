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

#include "condition.h"

Condition::Condition()
{

}

Condition::~Condition()
{
  for (auto condiIt = condiVec_.begin(); condiIt != condiVec_.end(); condiIt++)
  {
    delete *condiIt;
  }
}

void Condition::AddCondition(ConditionItem* pItem)
{
  if (pItem != nullptr)
  {
    condiVec_.push_back(pItem);
  }
}

bool Condition::RunCondition(const std::vector<DBVal>& rec)
{
  for (auto condiIt = condiVec_.begin(); condiIt != condiVec_.end(); condiIt++)
  {
    if (!((*condiIt)->GetLogic(rec)))
      return false;
  }

  return true;
}
