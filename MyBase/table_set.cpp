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
#include "table_set.h"
#include "string_tool.h"
#include "log.h"
#include "error_code.h"
#include <Windows.h>
#include <algorithm>

extern std::string glbDataPath;

TableSet::TableSet()
{

}

TableSet::~TableSet()
{
  for (auto tabIt = tabSet_.begin(); tabIt != tabSet_.end(); tabIt++)
  {
    delete (tabIt->second);
  }
}

int TableSet::LoadTables()
{
  WIN32_FIND_DATA findFileData;
  HANDLE findHandle = INVALID_HANDLE_VALUE;
  std::vector<std::string> nameVec;

  std::string findStr = glbDataPath + "\\*.dat";
  findHandle = FindFirstFile(findStr.c_str(), &findFileData);
  if (findHandle == INVALID_HANDLE_VALUE)
  {
    //没有数据文件
    return ER_OK;
  }

  std::string fileName = findFileData.cFileName;
  nameVec.push_back(fileName.substr(0, fileName.find_last_of('.')));
  while (FindNextFile(findHandle, &findFileData) == TRUE)
  {
    fileName = findFileData.cFileName;
    nameVec.push_back(fileName.substr(0, fileName.find_last_of('.')));
  }
  FindClose(findHandle);

  for (auto nameIt = nameVec.begin(); nameIt != nameVec.end(); nameIt++)
  {
    Table* pTab = new Table();
    if (pTab->Open(nameIt->c_str()) != 0)
    {
      LOG_OUT("打开表 %s 失败", nameIt->c_str());
      delete pTab;
      continue;
    }

    std::string tabName = *nameIt;
    std::transform(tabName.begin(), tabName.end(), tabName.begin(), ::tolower);
    if (tabSet_.find(tabName) != tabSet_.end())
    {
      LOG_OUT("打开表 %s 失败, 该表已存在", tabName.c_str());
      delete pTab;
      continue;
    }
    tabSet_.insert(std::pair<std::string, Table*>(tabName, pTab));
  }

  return ER_OK;
}

int TableSet::ShowTables(std::vector<std::string>& tabVec)
{
  for (auto tabIt = tabSet_.begin(); tabIt != tabSet_.end(); tabIt++)
  {
    tabVec.push_back(tabIt->second->GetTableName());
  }
  return ER_OK;
}

int TableSet::ShowTableColumns(const std::string& tabName, std::vector<ColInfo>& colVec)
{
  Table* pTable = GetTable(tabName.c_str());
  if (pTable == nullptr)
    return ER_TABLE_NOT_FOUND;

  pTable->GetColumnsInfo(colVec);
  return ER_OK;
}
int TableSet::CreateTable(const std::string& tabName, const CreateTableParam* pCreateParam, int* pErrPos)
{
  std::string tmpName = tabName;
  std::transform(tmpName.begin(), tmpName.end(), tmpName.begin(), ::tolower);
  if (tabSet_.find(tmpName) != tabSet_.end())
  {
    return ER_TABLE_EXISTS;
  }

  Table* pTable = new Table();
  int retVal = pTable->Create(tmpName, pCreateParam, pErrPos);
  if (retVal != ER_OK)
  {
    delete pTable;
    return retVal;
  }

  tabSet_.insert(std::pair<std::string, Table*>(tmpName, pTable));
  return ER_OK;
}
int TableSet::DropTable(const std::string& tabName)
{
  std::string tmpName = tabName;
  std::transform(tmpName.begin(), tmpName.end(), tmpName.begin(), ::tolower);
  auto tabIt = tabSet_.find(tmpName);
  if (tabIt == tabSet_.end())
    return ER_TABLE_NOT_FOUND;

  delete tabIt->second;
  tabSet_.erase(tabIt);

  std::string filePath = glbDataPath + "\\" + tabName.c_str() + ".dat";
  DeleteFile(filePath.c_str());

  return ER_OK;
}
int TableSet::Insert(const std::string& tabName, const InsertParam* pInsertParam, int* pErrPos)
{
  Table* pTable = GetTable(tabName.c_str());
  if (pTable == nullptr)
    return ER_TABLE_NOT_FOUND;

  return pTable->Insert(pInsertParam, pErrPos);
}

int TableSet::ExecQuery(const std::string& tabName, const QueryParam* pQueryParam, Query** ppQuery, int* pErrPos)
{
  Table* pTable = GetTable(tabName.c_str());
  if (pTable == nullptr)
    return ER_TABLE_NOT_FOUND;

  return pTable->ExecQuery(pQueryParam, ppQuery, pErrPos);
}
int TableSet::Delete(const std::string& tabName, const DeleteParam* pDeleteParam, int* pErrPos)
{
  Table* pTable = GetTable(tabName.c_str());
  if (pTable == nullptr)
    return ER_TABLE_NOT_FOUND;

  return pTable->ExecDelete(pDeleteParam, pErrPos);
}

Table* TableSet::GetTable(const char* pName)
{
  std::string tmpName = pName;
  std::transform(tmpName.begin(), tmpName.end(), tmpName.begin(), ::tolower);
  auto tabIt = tabSet_.find(tmpName);
  if (tabIt == tabSet_.end())
    return nullptr;

  return tabIt->second;
}