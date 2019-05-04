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

#include <stdio.h>

#define LOG_OUT(FORMAT, ...) do { printf("%s, line: %d, ", __FUNCTION__, __LINE__); printf(FORMAT, ## __VA_ARGS__); printf("\n");} while(false)