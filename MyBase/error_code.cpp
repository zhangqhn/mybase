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
#include "error_code.h"

static const char* errMsg[] = {
  "ERROR 10000 (ER_INIT_ERR) 初始化失败",   
  "ERROR 10001 (ER_INVALID_PARAM) 无效的参数", 
  "ERROR 10002 (ER_FIELD_NOT_FOUND) 字段不存在", 
  "ERROR 10003 (ER_SQL_ERR) 错误的SQL",  
  "ERROR 10004 (ER_TABLE_NOT_FOUND) 表不存在",
  "ERROR 10005 (ER_TABLE_EXISTS) 表已存在",
  "ERROR 10006 (ER_NOMEM) 申请内存失败",
  "ERROR 10007 (ER_DATA_TOO_LONG) 数据太长",
  "ERROR 10008 (ER_TABLE_FULL) 表已满",
  "ERROR 10009 (ER_HANDLE_OPEND) 句柄已打开",
  "ERROR 10010 (ER_INVALID_FIELD_NAME) 无效的字段名",
  "ERROR 10011 (ER_FILE_EXISTS) 文件已存在",
  "ERROR 10012 (ER_IO_ERR) IO失败",
  "ERROR 10013 (ER_TOO_LESS_COLUMN) 字段太少",
  "ERROR 10014 (ER_TOO_MANY_COLUMN) 字段太多",
  "ERROR 10015 (ER_TYPE_MISTAKE) 类型不匹配"
};

const char* GetErrMsg(int errCode)
{
  if (errCode == ER_OK)
    return "ERROR 0 (ER_OK) 操作成功";

  if (errCode >= ER_MIN_VAL && errCode <= ER_MAX_VAL)
  {
    return errMsg[(errCode - ER_MIN_VAL)];
  }

  return "未知的错误码";
}