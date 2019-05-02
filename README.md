# 什么是MyBase
MyBase是一个数据库实现，大约包含4000行c++代码，它极其简单，确切地说算不上一个数据库。
仅仅具备数据库的部分特点，但是您可以基于MyBase去实现一些数据库的功能，比如：主键、索引、
B+树的存储、排序、网络等等。

MyBase的目标是提供一个基本的、简单的数据库实现平台，让软件开发的初学者在学习完 
c++ 、 数据结构 、 数据库系统等基础课程后不光能使用数据库还能按照自己的想法实现一个数据库

# 编译MyBase
MyBase仅支持Windows平台，项目中已经包含vs2017的项目文件，不使用任何第三方的库，直接编译即可。
其中 parse.h 和 parse.cpp 由Lemon根据语法文件（Lemon\parse.y）生成。 Lemon的介绍见下一节。

# 关于Lemon
Lemon是Sqlite中对SQL语法分析的工具，更详细的内容请参考Sqlite中的相关部分。本项目使用Sqlite 3.0 对应的Lemon代码。
若修改了语法文件，只需执行以下步骤即可：
1. 使用lemon.c 生成一个执行程序，比如：lemon.exe。

2. 将语法文件(parse.y) 、 lempar.c 与 lemon.exe 放在同一个目录下。

3. 执行 lemon.exe parse.y ，若无错误会生成 parse.h 和 parse.c ， 
此时要注意：parse.y中使用了c++的代码，所以需要将parse.c修改为parse.cpp加入项目，否则会编译出错。

# 运行MyBase
MyBase是服务程序和客户程序的混合，运行后会以运行目录为库目录，读取后缀为 .dat 的文件加入当前系统。
创建的每个表作为一个独立的文件保存在当前目录下。MyBase的表有固定大小，写满后不会增长。

MyBase功能列表

* 打印帮助信息

  help
  
* 清屏

  clear
  
* 退出

  exit
  
* 执行外部的sql文件

  source \<sql-file-path\>

* 显示所有的表

  show tables
  
* 显示指定表的列信息

  show columns from \<tab-name\>
  
* 创建表
  
  仅支持 int, double, string 类型
  
  create table \<tab-name\> (\<col-name\> int|double|string, ...)
  
* 删除表
  
  drop table \<tab-name\>
  
* 插入数据

  insert into \<tab-name\> (\<col-name-list\>) values (\<val-list\>)

* 查询数据

  支持 max, min, sum, avg, count 聚合函数
  
  select \<target-list\> from \<tab-name\> [where-expr] [group-by-expr] [limit-expr]
  
* 删除数据

  delete from \<tab-name\> [where-expr]

# 联系我

  有任何问题欢迎与我联系，zhangqhn@foxmail.com


