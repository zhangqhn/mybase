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
#include "table.h"
#include "string_tool.h"
#include "Log.h"
#include "condition.h"
#include "error_code.h"
#include <algorithm>

#define TABLE_FILE_SIZE     (32 * 1024 * 1024)

extern std::string glbDataPath;

Table::Table()
{
  this->fileHandle_ = INVALID_HANDLE_VALUE;
  this->mapHandle_ = INVALID_HANDLE_VALUE;
  this->pBase_ = nullptr;
}

Table::~Table()
{
  Close();
}

int Table::Create(const std::string& tabName, const CreateTableParam* pCreateParam, int* pErrPos)
{
  std::string filePath = glbDataPath + "\\" + tabName + ".dat";

  if (pErrPos != nullptr)
    *pErrPos = -1;

  if (INVALID_HANDLE_VALUE != fileHandle_)
    return ER_HANDLE_OPEND;

  if (pCreateParam->pColList_ == nullptr)
    return ER_INVALID_PARAM;

  const std::vector<ColumnItem*>& colVec = pCreateParam->pColList_->GetColumnList();
  if (colVec.size() == 0)
    return ER_TOO_LESS_COLUMN;

  if (colVec.size() > TABLE_MAX_COL)
    return ER_TOO_MANY_COLUMN;

  MetaPage mpage;
  ZeroMemory(&mpage, sizeof(MetaPage));
  strncpy_s(mpage.headStr_, TABLE_HEAD_STR, _TRUNCATE);
  mpage.colCnt_ = static_cast<int>(colVec.size());

  for (size_t idx = 0; idx < colVec.size(); idx++)
  {
    const std::string& colName = colVec[idx]->GetName();
    if (colName.size() == 0 || colName.size() >= COL_NAME_LEN)
    {
      if (pErrPos != nullptr)
        *pErrPos = colVec[idx]->GetSqlPos();

      return ER_INVALID_FIELD_NAME;
    }
    
    strncpy_s(mpage.cols_[idx].colName_, colName.c_str(), _TRUNCATE);
    mpage.cols_[idx].colType_ = colVec[idx]->GetType();
  }

  //判断文件是否存在
  DWORD dwAttribute = GetFileAttributes(filePath.c_str());
  if (INVALID_FILE_ATTRIBUTES != dwAttribute
    && ((FILE_ATTRIBUTE_DIRECTORY & dwAttribute) == 0))
  {
    return ER_FILE_EXISTS;
  }

  //此处可以创建一个临时文件，如: tab.dat.tmp 待所有操作完成后修改为 tab.dat
  HANDLE tmpHandle = CreateFile(filePath.c_str(),
    GENERIC_READ | GENERIC_WRITE,
    0,
    NULL,
    CREATE_NEW,
    FILE_ATTRIBUTE_NORMAL,
    NULL);

  if (INVALID_HANDLE_VALUE == tmpHandle)
  {
    LOG_OUT("CreateFile 失败, %d, %s", GetLastError(), filePath.c_str());
    return ER_IO_ERR;
  }

  SetFilePointer(tmpHandle, TABLE_FILE_SIZE, 0, FILE_BEGIN);
  SetEndOfFile(tmpHandle);

  DWORD bytesWritten = 0;
  SetFilePointer(tmpHandle, 0, 0, FILE_BEGIN);
  BOOL writeRet = WriteFile(tmpHandle, (LPCVOID)&mpage, sizeof(mpage), &bytesWritten, NULL);
  DWORD lastErr = GetLastError();
  CloseHandle(tmpHandle);
  if (!writeRet || sizeof(mpage) != bytesWritten)
  {
    LOG_OUT("WriteFile 失败, %d, %s", lastErr, filePath.c_str());
    DeleteFile(filePath.c_str());
    return ER_IO_ERR;
  }

  return Open(tabName.c_str());
}

int Table::Open(const char* pName)
{
  std::string filePath = glbDataPath + "\\" + pName + ".dat";
  if (INVALID_HANDLE_VALUE != fileHandle_)
  {
    return ER_HANDLE_OPEND;
  }

  fileHandle_ = CreateFile(filePath.c_str(),
    GENERIC_READ | GENERIC_WRITE,
    0,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL);

  if (INVALID_HANDLE_VALUE == fileHandle_)
  {
    return ER_IO_ERR;
  }

  mapHandle_ = CreateFileMapping(fileHandle_, NULL, PAGE_READWRITE, 0, TABLE_FILE_SIZE, NULL);
  if (NULL == mapHandle_)
  {
    LOG_OUT("CreateFileMapping 失败, %d, %s", GetLastError(), filePath.c_str());
    Close();
    return ER_IO_ERR;
  }

  pBase_ = MapViewOfFile(mapHandle_, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, TABLE_FILE_SIZE);
  if (NULL == pBase_)
  {
    LOG_OUT("MapViewOfFile 失败, %d, %s", GetLastError(), filePath.c_str());
    Close();
    return ER_IO_ERR;
  }

  //加载元数据
  MetaPage* pMeta = (MetaPage*)pBase_;
  for (int i = 0; i < pMeta->colCnt_; i++)
  {
    uint64_t nameCrc = StringTool::Crc64(pMeta->cols_[i].colName_);
    colPosMap_.insert(std::pair<uint64_t, int>(nameCrc, i));
  }

  this->tabName_ = pName;
  return ER_OK;
}

int Table::Close()
{
  if (this->pBase_ != nullptr)
  {
    FlushViewOfFile(this->pBase_, TABLE_FILE_SIZE);
    UnmapViewOfFile(this->pBase_);
    this->pBase_ = nullptr;
  }

  if (this->mapHandle_ != INVALID_HANDLE_VALUE)
  {
    CloseHandle(this->mapHandle_);
    this->mapHandle_ = INVALID_HANDLE_VALUE;
  }
  
  if (this->fileHandle_ != INVALID_HANDLE_VALUE)
  {
    CloseHandle(this->fileHandle_);
    this->fileHandle_ = INVALID_HANDLE_VALUE;
  }

  tabName_ = "";
  colPosMap_.clear();

  return ER_OK;
}

int Table::Insert(const InsertParam* pInsertParam, int* pErrPos)
{
  if (pInsertParam == nullptr)
    return ER_INVALID_PARAM;

  const MetaPage* pMeta = (const MetaPage*)pBase_;
  std::vector<DBVal> rec(pMeta->colCnt_);
  for (size_t idx = 0; idx < rec.size(); idx++)
  {
    DBVAL_SET_NULL(&(rec[idx]));
  }

  const std::vector<ExprItem*>& nameVec = pInsertParam->pNameList_->GetExprList();
  const std::vector<ExprItem*>& valVec = pInsertParam->pValList_->GetExprList();
  
  if (nameVec.empty())
  {
    return ER_SQL_ERR;
  }

  if (nameVec.size() != valVec.size())
  {
    return ER_SQL_ERR;
  }

  int64_t intVal = 0;
  double doubleVal = 0;

  for (size_t idx = 0; idx < nameVec.size(); idx++)
  {
    const std::string& nameStr = nameVec[idx]->GetTokenStr();
    int fieldPos = GetFieldPos(nameStr.c_str());
    if (fieldPos < 0)
    {
      if (pErrPos != nullptr)
        *pErrPos = nameVec[idx]->GetSqlPos();

      return ER_FIELD_NOT_FOUND;
    }
    int fieldType = pMeta->cols_[fieldPos].colType_;

    const std::string& valStr = valVec[idx]->GetTokenStr();
    int valOp = valVec[idx]->GetOp();
    switch (valOp)
    {
    case TK_UINTEGER:
    case TK_INTEGER:
    {
      if (!StringTool::StrToInt64(valStr.c_str(), valStr.size(), &intVal))
      {
        if (pErrPos != nullptr)
          *pErrPos = valVec[idx]->GetSqlPos();

        return ER_SQL_ERR;
      }

      if (valOp == TK_UINTEGER)
        intVal *= -1;

      if (fieldType == VALUE_TYPE::VAL_INT)
      {
        DBVAL_SET_INT(&(rec[fieldPos]), intVal);
      }
      else if (fieldType == VALUE_TYPE::VAL_DOUBLE)
      {
        DBVAL_SET_DOUBLE(&(rec[fieldPos]), static_cast<double>(intVal));
      }
      else
      {
        if (pErrPos != nullptr)
          *pErrPos = valVec[idx]->GetSqlPos();

        return ER_TYPE_MISTAKE;
      }

      break;
    }
    case TK_DOUBLE:
    case TK_UDOUBLE:
    {
      if (!StringTool::StrToDouble(valStr.c_str(), valStr.size(), &doubleVal))
      {
        if (pErrPos != nullptr)
          *pErrPos = valVec[idx]->GetSqlPos();

        return ER_SQL_ERR;
      }

      if (valOp == TK_UDOUBLE)
        doubleVal *= -1;

      if (fieldType == VALUE_TYPE::VAL_DOUBLE)
      {
        DBVAL_SET_DOUBLE(&(rec[fieldPos]), doubleVal);
      }
      else
      {
        if (pErrPos != nullptr)
          *pErrPos = valVec[idx]->GetSqlPos();

        return ER_TYPE_MISTAKE;
      }

      break;
    }
    case TK_STRING:
    {
      if (fieldType == VALUE_TYPE::VAL_STRING)
      {
        DBVAL_SET_STRING(&(rec[fieldPos]), valStr.c_str(), valStr.size());
      }
      else
      {
        if (pErrPos != nullptr)
          *pErrPos = valVec[idx]->GetSqlPos();

        return ER_TYPE_MISTAKE;
      }
      break;
    }
    default:
      if (pErrPos != nullptr)
        *pErrPos = valVec[idx]->GetSqlPos();

      return ER_SQL_ERR;
    }

  }

  //将rec序列化并存储
  //| recLen |  type  |  data         | type | strLen | strData   |
  uint8_t recBuf[MAX_REC_LEN];
  ZeroMemory(recBuf, sizeof(recBuf));
  uint8_t* pTmp = recBuf + 1;
  for (size_t idx = 0; idx < rec.size(); idx++)
  {
    if ((pTmp + 1 + rec[idx].dataLen_) >(recBuf + MAX_REC_LEN))
    {
      return ER_DATA_TOO_LONG;
    }

    switch (rec[idx].valType_)
    {
    case VALUE_TYPE::VAL_NULL:
      *pTmp++ = VALUE_TYPE::VAL_NULL;
      break;
    case VALUE_TYPE::VAL_INT:
      *pTmp++ = VALUE_TYPE::VAL_INT;
      *((int64_t*)pTmp) = rec[idx].val_.intVal_;
      pTmp += sizeof(int64_t);
      break;
    case VALUE_TYPE::VAL_DOUBLE:
      *pTmp++ = VALUE_TYPE::VAL_DOUBLE;
      *((double*)pTmp) = rec[idx].val_.doubleVal_;
      pTmp += sizeof(double);
      break;
    case VALUE_TYPE::VAL_STRING:
      *pTmp++ = VALUE_TYPE::VAL_STRING;
      *pTmp++ = static_cast<uint8_t>(rec[idx].dataLen_);
      strncpy_s((char*)pTmp, (MAX_REC_LEN - (pTmp - recBuf)), rec[idx].val_.strVal_, rec[idx].dataLen_);
      pTmp += rec[idx].dataLen_;
      break;
    }
  }

  uint8_t recLen = static_cast<uint8_t>(pTmp - recBuf);
  recBuf[0] = recLen;

  //找一个页将数据存进去
  for (int idx = 1; idx < (TABLE_FILE_SIZE / (sizeof(DataPage))); idx++)
  {
    DataPage* pPage = &(((DataPage*)pBase_)[idx]);
    if (pPage->pageNo_ != idx)
    {
      //写入数据到空页
      pPage->pageNo_ = idx;
      pPage->recCnt_ = 1;
      pPage->freeBytes_ = (DATA_PAGE_DATASIZE - 2 - recLen);
      pPage->chipBytes_ = 0;

      pTmp = pPage->data_ + DATA_PAGE_DATASIZE - recLen;
      memcpy(pTmp, recBuf, recLen);
      ((uint16_t*)(pPage->data_))[0] = static_cast<uint16_t>(pTmp - reinterpret_cast<uint8_t*>(pPage));

      return ER_OK;
    }
    else if ((pPage->freeBytes_ + pPage->chipBytes_) >= (recLen + 2))
    {
      if (pPage->freeBytes_ < recLen)
      {
        //需要对页中的数据进行整理
        CleanPage(pPage);
      }

      //整理后空闲空间大于记录长度
      if (pPage->freeBytes_ >= recLen)
      {
        //计算写入的起始位置
        pTmp = pPage->data_ + pPage->recCnt_ * 2 + pPage->freeBytes_ - recLen;
        memcpy(pTmp, recBuf, recLen);
        ((uint16_t*)(pPage->data_))[pPage->recCnt_] = static_cast<uint16_t>(pTmp - reinterpret_cast<uint8_t*>(pPage));
        pPage->recCnt_++;
        pPage->freeBytes_ -= ((int)recLen + 2);

        return ER_OK;
      }
    }

  }

  return ER_TABLE_FULL;
}

int Table::ExecQuery(const QueryParam* pQueryParam, Query** ppQuery, int* pErrPos)
{
  Query* pQuery = nullptr;
  int result = BuildQuery(pQueryParam, &pQuery, pErrPos);
  if (result != 0)
    return result;

  MetaPage* pMeta = (MetaPage*)pBase_;
  std::vector<DBVal> recVal(pMeta->colCnt_);
  
  for (int pageIdx = 1; pageIdx < (TABLE_FILE_SIZE / (sizeof(DataPage))); pageIdx++)
  {
    DataPage* pPage = &(((DataPage*)pBase_)[pageIdx]);
  
    if (pPage->pageNo_ == pageIdx && pPage->recCnt_ > 0)
    {
      for (int recIdx = 0; recIdx < pPage->recCnt_; recIdx++)
      {
        const uint8_t* pRec = ((const uint8_t*)pPage) + ((uint16_t*)(pPage->data_))[recIdx];
  
        LoadRecord(pRec, recVal);
        pQuery->AppendData(recVal);
      }
    }
  }

  *ppQuery = pQuery;
  return ER_OK;
}

int Table::ExecDelete(const DeleteParam* pDeleteParam, int* pErrPos)
{
  int retVal = ER_OK;
  Condition delCondition;

  retVal = BuildCondition(pDeleteParam->pCondition_, &delCondition, pErrPos);
  if (retVal != ER_OK)
    return retVal;

  MetaPage* pMeta = (MetaPage*)pBase_;
  std::vector<DBVal> recVal(pMeta->colCnt_);
  for (int pageIdx = 1; pageIdx < (TABLE_FILE_SIZE / (sizeof(DataPage))); pageIdx++)
  {
    DataPage* pPage = &(((DataPage*)pBase_)[pageIdx]);

    if (pPage->pageNo_ == pageIdx && pPage->recCnt_ > 0)
    {
      for (int recIdx = (pPage->recCnt_ - 1); recIdx >= 0; recIdx--)
      {
        uint8_t* pRecOffset = (pPage->data_ + recIdx * 2);
        uint8_t* pRec = ((uint8_t*)pPage) + *((uint16_t*)pRecOffset);
        LoadRecord(pRec, recVal);
        if (delCondition.RunCondition(recVal))
        {
          memmove(pRecOffset, (pRecOffset + 2), ((pPage->recCnt_ - recIdx - 1) * 2));

          pPage->recCnt_--;
          pPage->freeBytes_ += sizeof(uint16_t);
          pPage->chipBytes_ += (*pRec);
        }
      }
    }
  }

  return ER_OK;
}

const std::string& Table::GetTableName() const
{
  return this->tabName_;
}

void Table::GetColumnsInfo(std::vector<ColInfo>& colVec) const
{
  MetaPage* pMeta = (MetaPage*)pBase_;
  for (int i = 0; i < pMeta->colCnt_; i++)
  {
    colVec.push_back(pMeta->cols_[i]);
  }
}

int Table::BuildQuery(const QueryParam* pQueryParam, Query** ppQuery, int* pErrPos)
{
  Query* pQuery = nullptr;
  MetaPage* pMeta = (MetaPage*)pBase_;
  int groupFieldPos = -1;

  const std::vector<ExprItem*>& targetVec = pQueryParam->pTargetList_->GetExprList();

  //判断是否是聚合查询
  bool isGroup = pQueryParam->pGroup_ != nullptr;
  if (!isGroup)
  {
    //对应于 pQueryParam->pGroup_ == nullptr 的情形
    //可能的SQL为: SELECT count(*) FROM <tabname>
    for (auto targetIt = targetVec.begin(); targetIt != targetVec.end(); targetIt++)
    {
      //若出现过ID和*以外的字段则是聚合查询
      int targetType = (*targetIt)->GetOp();
      if (targetType != TK_STRING && targetType != TK_ID && targetType != TK_STAR)
      {
        isGroup = true;
        break;
      }
    }
  }

  if (!isGroup)
  {
    pQuery = new RawQuery();
  }
  else
  {
    if (pQueryParam->pGroup_ != nullptr)
    {
      //将所有数据分为一组进行查询
      const std::string& groupName = pQueryParam->pGroup_->GetGroupField();
      groupFieldPos = GetFieldPos(groupName.c_str());
      if (groupFieldPos < 0)
      {
        //分组字段不存在
        if (pErrPos != nullptr)
          *pErrPos = pQueryParam->pGroup_->GetSqlPos();

        return ER_FIELD_NOT_FOUND;
      }

      if (pMeta->cols_[groupFieldPos].colType_ != VALUE_TYPE::VAL_INT
        && pMeta->cols_[groupFieldPos].colType_ != VALUE_TYPE::VAL_STRING)
      {
        if (pErrPos != nullptr)
          *pErrPos = pQueryParam->pGroup_->GetSqlPos();

        return ER_SQL_ERR;
      }
    }
    pQuery = new GroupQuery(groupFieldPos);
  }

  int retVal = ER_OK;

  do {
    //构建条件
    Condition* pCondition = new Condition();
    retVal = BuildCondition(pQueryParam->pCondition_, pCondition, pErrPos);
    if (retVal != ER_OK)
    {
      delete pCondition;
      break;
    }
    pQuery->SetCondition(pCondition);

    if (pQueryParam->pLimit_ != nullptr)
    {
      pQuery->SetQueryOffset(pQueryParam->pLimit_->GetOffset());
      pQuery->SetQueryCnt(pQueryParam->pLimit_->GetQueryCnt());
    }

    //构建查询结果
    for (auto targetIt = targetVec.begin(); targetIt != targetVec.end(); targetIt++)
    {
      retVal = BuildTarget(pQuery, isGroup, groupFieldPos, (*targetIt));
      if (retVal != ER_OK)
      {

        break;
      }
    }

    //初始化结果集
    //为了解决 select count(*) from tab 有且只有一条数据
    pQuery->InitResult();
  } while (false);

  if (retVal != ER_OK)
  {
    delete pQuery;
  }
  else
  {
    *ppQuery = pQuery;
  }

  return retVal;
}

int Table::BuildCondition(const ExprItem* pCondiExpr, Condition* pCondition, int* pErrPos)
{
  int retVal = 0;
  MetaPage* pMeta = (MetaPage*)pBase_;
  if (pCondiExpr == nullptr)
    return ER_OK;

  const ExprItem* pLeftExpr = pCondiExpr->GetLeftExpr();
  const ExprItem* pRightExpr = pCondiExpr->GetRightExpr();

  DBVal rightVal;
  DBVAL_SET_NULL(&rightVal);

  int op = pCondiExpr->GetOp();
  if (op == TK_AND)
  {
    if (pLeftExpr == nullptr || pRightExpr == nullptr)
    {
      if (pErrPos != nullptr)
        *pErrPos = pCondiExpr->GetSqlPos();

      return ER_SQL_ERR;
    }

    retVal = BuildCondition(pLeftExpr, pCondition, pErrPos);
    if (retVal != ER_OK)
      return retVal;

    retVal = BuildCondition(pRightExpr, pCondition, pErrPos);
    if (retVal != ER_OK)
      return retVal;
  }
  else
  {
    //条件表达式左边必须是字段
    if (pLeftExpr == nullptr)
    {
      if (pErrPos != nullptr)
        *pErrPos = pCondiExpr->GetSqlPos();

      return ER_SQL_ERR;
    }

    if (pLeftExpr->GetOp() != TK_ID)
    {
      if (pErrPos != nullptr)
        *pErrPos = pCondiExpr->GetSqlPos();

      return ER_SQL_ERR;
    }

    const std::string& fieldName = pLeftExpr->GetTokenStr();
    int fieldPos = GetFieldPos(fieldName.c_str());
    if (fieldPos < 0)
    {
      if (pErrPos != nullptr)
        *pErrPos = pCondiExpr->GetSqlPos();

      return ER_FIELD_NOT_FOUND;
    }

    if (pRightExpr == nullptr)
    {
      //一个操作数的条件， 如： <field> is not null  ,   <field> is null
      if (op == TK_ISNOTNULL)
      {
        pCondition->AddCondition(new IsNotNullCondition(fieldPos));
      }
      else if (op == TK_ISNULL)
      {
        pCondition->AddCondition(new IsNullCondition(fieldPos));
      }
      else
      {
        if (pErrPos != nullptr)
          *pErrPos = pCondiExpr->GetSqlPos();

        return ER_SQL_ERR;
      }
    }
    else
    {
      //两个操作数的条件，如: num > 5 等等
      const std::string& valueStr = pRightExpr->GetTokenStr();
      int32_t rightOp = pRightExpr->GetOp();

      switch (rightOp)
      {
      case TK_INTEGER:
      case TK_UINTEGER:
      {
        if (!StringTool::StrToInt64(valueStr.c_str(), valueStr.size(), &(rightVal.val_.intVal_)))
        {
          if (pErrPos != nullptr)
            *pErrPos = pRightExpr->GetSqlPos();

          return ER_SQL_ERR;
        }

        if (rightOp == TK_UINTEGER)
          rightVal.val_.intVal_ *= -1;

        rightVal.valType_ = VALUE_TYPE::VAL_INT;
        rightVal.dataLen_ = sizeof(int64_t);
        break;
      }
      case TK_DOUBLE:
      case TK_UDOUBLE:
      {
        if (!StringTool::StrToDouble(valueStr.c_str(), valueStr.size(), &(rightVal.val_.doubleVal_)))
        {
          if (pErrPos != nullptr)
            *pErrPos = pRightExpr->GetSqlPos();

          return ER_SQL_ERR;
        }

        if (rightOp == TK_UDOUBLE)
          rightVal.val_.doubleVal_ *= -1;

        rightVal.valType_ = VALUE_TYPE::VAL_DOUBLE;
        rightVal.dataLen_ = sizeof(double);
        break;
      }
      case TK_STRING:
      {
        DBVAL_SET_STRING(&rightVal, valueStr.c_str(), valueStr.size());
        break;
      }
      default:
        if (pErrPos != nullptr)
          *pErrPos = pRightExpr->GetSqlPos();

        return ER_SQL_ERR;
      }

      //如果表达式两边的类型不匹配
      if (pMeta->cols_[fieldPos].colType_ == VALUE_TYPE::VAL_DOUBLE
        && rightVal.valType_ == VALUE_TYPE::VAL_INT)
      {
        //int64 可以转 double, 例如 double_type >= 5
        rightVal.val_.doubleVal_ = static_cast<double>(rightVal.val_.intVal_);
        rightVal.valType_ = VALUE_TYPE::VAL_DOUBLE;
        rightVal.dataLen_ = sizeof(double);
      }

      if (pMeta->cols_[fieldPos].colType_ != rightVal.valType_)
      {
        if (pErrPos != nullptr)
          *pErrPos = pLeftExpr->GetSqlPos();

        return ER_SQL_ERR;
      }

      ConditionItem* pCondItem = nullptr;
      switch (rightVal.valType_)
      {
      case VALUE_TYPE::VAL_INT:
        pCondItem = BuildIntCondition(fieldPos, op, rightVal.val_.intVal_);
        break;
      case VALUE_TYPE::VAL_DOUBLE:
        pCondItem = BuildDoubleCondition(fieldPos, op, rightVal.val_.doubleVal_);
        break;
      case VALUE_TYPE::VAL_STRING:
        pCondItem = BuildStringCondition(fieldPos, op, rightVal.val_.strVal_);
        break;
      }
      if (pCondItem == nullptr)
      {
        if (pErrPos != nullptr)
          *pErrPos = pLeftExpr->GetSqlPos();

        return ER_SQL_ERR;
      }

      pCondition->AddCondition(pCondItem);
    }
  }

  return ER_OK;
}

int Table::GetFieldPos(const char* pName)
{
  uint64_t nameCrc = StringTool::Crc64NoCase(pName);
  auto colIt = colPosMap_.find(nameCrc);
  if (colIt != colPosMap_.end())
    return colIt->second;

  return -1;
}

ConditionItem* Table::BuildIntCondition(int fieldPos, int op, int64_t val)
{
  switch (op)
  {
  case TK_LT:
    return new IntCondition<TK_LT>(fieldPos, val);
  case TK_LE:
    return new IntCondition<TK_LE>(fieldPos, val);
  case TK_GT:
    return new IntCondition<TK_GT>(fieldPos, val);
  case TK_GE:
    return new IntCondition<TK_GE>(fieldPos, val);
  case TK_EQ:
    return new IntCondition<TK_EQ>(fieldPos, val);
  case TK_NE:
    return new IntCondition<TK_NE>(fieldPos, val);
  }

  return nullptr;
}

ConditionItem* Table::BuildDoubleCondition(int fieldPos, int op, double val)
{
  switch (op)
  {
  case TK_LT:
    return new DoubleCondition<TK_LT>(fieldPos, val);
  case TK_LE:
    return new DoubleCondition<TK_LE>(fieldPos, val);
  case TK_GT:
    return new DoubleCondition<TK_GT>(fieldPos, val);
  case TK_GE:
    return new DoubleCondition<TK_GE>(fieldPos, val);
  case TK_EQ:
    return new DoubleCondition<TK_EQ>(fieldPos, val);
  case TK_NE:
    return new DoubleCondition<TK_NE>(fieldPos, val);
  }
  return nullptr;
}

ConditionItem* Table::BuildStringCondition(int fieldPos, int op, const std::string& val)
{
  switch (op)
  {
  case TK_EQ:
    return new StringCondition<TK_EQ>(fieldPos, val);
  case TK_NE:
    return new StringCondition<TK_NE>(fieldPos, val);
  case TK_LIKE:
    return new StringCondition<TK_LIKE>(fieldPos, val);
  }

  return nullptr;
}

int Table::BuildTarget(Query* pQuery, bool isGroup, int groupFieldPos, const ExprItem* pTarget)
{
  const MetaPage* pMeta = (const MetaPage*)pBase_;
  int op = pTarget->GetOp();

  if (TK_ID == op)
  {
    const std::string& fieldName = pTarget->GetTokenStr();
    int fieldPos = GetFieldPos(fieldName.c_str());
    if (fieldPos < 0)
      return ER_FIELD_NOT_FOUND;

    if (isGroup && groupFieldPos != fieldPos)
      return ER_SQL_ERR;

    pQuery->AddTarget(fieldName.c_str(), new RawResultField(fieldPos));

    return ER_OK;
  }
  else if (TK_STAR == op)
  {
    if (isGroup)
      return ER_SQL_ERR;

    for (int idx = 0; idx < pMeta->colCnt_; idx++)
    {
      pQuery->AddTarget(pMeta->cols_[idx].colName_, new RawResultField(idx));
    }

    return ER_OK;
  }
  else if (TK_FUNC == op)
  {
    std::string funcName = pTarget->GetTokenStr();
    std::transform(funcName.begin(), funcName.end(), funcName.begin(), ::tolower);

    const ExprList* pArgs = pTarget->GetArgList();
    if (pArgs == nullptr)
      return ER_SQL_ERR;

    const std::vector<ExprItem*>& argVec = pArgs->GetExprList();
    if (argVec.size() != 1) //目前所有的聚合函数都只有一个参数
      return ER_SQL_ERR;

    int argOp = argVec[0]->GetOp();

    if (funcName.compare("count") == 0 && argOp == TK_STAR)
    {
      // count(*) 
      pQuery->AddTarget("count(*)", new CountField(-1));
      return ER_OK;
    }
    
    if (argOp != TK_ID)
      return ER_SQL_ERR;

    const std::string& argName = argVec[0]->GetTokenStr();
    int argPos = GetFieldPos(argName.c_str());
    if (argPos < 0)
      return ER_FIELD_NOT_FOUND;

    int argType = pMeta->cols_[argPos].colType_;

    std::string targetName = funcName + "(" + argName + ")";

    if (funcName.compare("count") == 0)
    {
      pQuery->AddTarget(targetName.c_str(), new CountField(argPos));
    }
    else if (funcName.compare("avg") == 0)
    {
      switch (argType)
      {
      case VALUE_TYPE::VAL_INT:
        pQuery->AddTarget(targetName.c_str(), new AvgIntField(argPos));
        return ER_OK;
      case VALUE_TYPE::VAL_DOUBLE:
        pQuery->AddTarget(targetName.c_str(), new AvgDoubleField(argPos));
        return ER_OK;
      }
    }
    else if (funcName.compare("max") == 0)
    {
      switch (argType)
      {
      case VALUE_TYPE::VAL_INT:
        pQuery->AddTarget(targetName.c_str(), new MaxField<VALUE_TYPE::VAL_INT>(argPos));
        return ER_OK;
      case VALUE_TYPE::VAL_DOUBLE:
        pQuery->AddTarget(targetName.c_str(), new MaxField<VALUE_TYPE::VAL_DOUBLE>(argPos));
        return ER_OK;
      }
    }
    else if (funcName.compare("min") == 0)
    {
      switch (argType)
      {
      case VALUE_TYPE::VAL_INT:
        pQuery->AddTarget(targetName.c_str(), new MinField<VALUE_TYPE::VAL_INT>(argPos));
        return ER_OK;
      case VALUE_TYPE::VAL_DOUBLE:
        pQuery->AddTarget(targetName.c_str(), new MinField<VALUE_TYPE::VAL_DOUBLE>(argPos));
        return ER_OK;
      }
    }
    else if (funcName.compare("sum") == 0)
    {
      switch (argType)
      {
      case VALUE_TYPE::VAL_INT:
        pQuery->AddTarget(targetName.c_str(), new SumIntField(argPos));
        return ER_OK;
      case VALUE_TYPE::VAL_DOUBLE:
        pQuery->AddTarget(targetName.c_str(), new SumDoubleField(argPos));
        return ER_OK;
      }
    }
  }

  return ER_SQL_ERR;
}

void Table::LoadRecord(const uint8_t* pRec, std::vector<DBVal>& recVal)
{
  //第一个字节是记录长度
  pRec++;

  for (size_t colIdx = 0; colIdx < recVal.size(); colIdx++)
  {
    int type = *pRec;
    int strLen = 0;
    pRec++;
    switch (type)
    {
    case VALUE_TYPE::VAL_NULL:
      DBVAL_SET_NULL(&(recVal[colIdx]));
      break;
    case VALUE_TYPE::VAL_INT:
      DBVAL_SET_INT(&(recVal[colIdx]), *((int64_t*)pRec));
      pRec += sizeof(int64_t);
      break;
    case VALUE_TYPE::VAL_DOUBLE:
      DBVAL_SET_DOUBLE(&(recVal[colIdx]), *((double*)pRec));
      pRec += sizeof(double);
      break;
    case VALUE_TYPE::VAL_STRING:
      strLen = *((uint8_t*)pRec);
      pRec++;
      DBVAL_SET_STRING(&(recVal[colIdx]), (const char*)pRec, strLen);
      pRec += strLen;
      break;
    }
  }
}

void Table::CleanPage(DataPage* pPage)
{
  DataPage tmpPage;
  ZeroMemory(&tmpPage, sizeof(DataPage));

  uint8_t* pTmp = tmpPage.data_ + DATA_PAGE_DATASIZE;
  for (int recIdx = 0; recIdx < pPage->recCnt_; recIdx++)
  {
    uint8_t* pRecOffset = (pPage->data_ + recIdx * 2);
    uint8_t* pRec = pPage->data_ + *((uint16_t*)pRecOffset);

    pTmp -= *(pRec);
    memcpy(pTmp, pRec, *(pRec));
    ((uint16_t*)tmpPage.data_)[recIdx] = static_cast<uint16_t>(pTmp - tmpPage.data_);
  }

  tmpPage.pageNo_ = pPage->pageNo_;
  tmpPage.recCnt_ = pPage->recCnt_;
  tmpPage.chipBytes_ = 0;
  tmpPage.freeBytes_ = static_cast<int>(pTmp - tmpPage.data_ - (pPage->recCnt_ * 2));
  
  memcpy(pPage, &tmpPage, sizeof(DataPage));
}