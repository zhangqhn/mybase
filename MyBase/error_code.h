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


#define ER_OK                             0
#define ER_INIT_ERR                   10000
#define ER_INVALID_PARAM              10001
#define ER_FIELD_NOT_FOUND            10002
#define ER_SQL_ERR                    10003
#define ER_TABLE_NOT_FOUND            10004
#define ER_TABLE_EXISTS               10005
#define ER_NOMEM                      10006
#define ER_DATA_TOO_LONG              10007 
#define ER_TABLE_FULL                 10008
#define ER_HANDLE_OPEND               10009
#define ER_INVALID_FIELD_NAME         10010
#define ER_FILE_EXISTS                10011
#define ER_IO_ERR                     10012
#define ER_TOO_LESS_COLUMN            10013
#define ER_TOO_MANY_COLUMN            10014
#define ER_TYPE_MISTAKE               10015


#define ER_MIN_VAL                    10000
#define ER_MAX_VAL                    10015

const char* GetErrMsg(int errCode);