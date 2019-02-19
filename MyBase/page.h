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

#define TABLE_HEAD_STR_LEN 16
#define TABLE_HEAD_STR   "MYBASE 1"

#define COL_NAME_LEN   60
#define TABLE_MAX_COL  15

#define DATA_PAGE_DATASIZE     (4080)
#define DATA_PAGE_HEADSIZE     (4096 - DATA_PAGE_DATASIZE)

#define MAX_REC_LEN    255

//ColInfo 256
typedef struct _ColInfo
{
  char colName_[COL_NAME_LEN];     // 列名
  int  colType_;                   // 列类型
  char  padding_[192];             // 填充到256字节                   
}ColInfo;


typedef struct _MetaPage
{
  char headStr_[TABLE_HEAD_STR_LEN];  // 字符串，MYBASE 1
  int colCnt_;                        // 列数量
  char padding_[236];                 // 填充到256字节                    

  ColInfo cols_[TABLE_MAX_COL];
}MetaPage;


typedef struct _DataPage
{
  int pageNo_;     // 页号，数据页从1开始
  int recCnt_;     // 记录条数
  int freeBytes_;  // 空闲字节数量
  int chipBytes_;  // 碎片字节数量
  uint8_t data_[DATA_PAGE_DATASIZE];
}DataPage;