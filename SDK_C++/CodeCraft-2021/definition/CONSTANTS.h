#pragma once

// 操作类型
#define ADD 1
#define DEL 2

// 虚拟机所在节点：A、B、BOTH,虚拟机可部署节点：1.单节点下:A,B,BOTH,NONE2.双节点下:BOTH,NONE
#define A 3
#define B 4
#define BOTH 5
#define NONE 0
#define DEBUG

#ifdef DEBUG
extern int addcount;
extern int delcount;
#endif

// 用来生成服务器编号
extern int localServerNum;
extern int globalServerNumber;

extern int MAX_VM_CORE;
extern int MAX_VM_MEMORY;
extern int MAX_SERVERMODEL_CORE;
extern int MAX_SERVERMODEL_MEMORY;
extern int MEAN_VM_CORE;
extern int MEAN_VM_MEMORY;
extern int VM_AMOUNT;
extern double MAX_SIMI;
extern double MIN_SIMI;

extern int MAXSOURCE;

//购买权重（和为1）
extern double buy_PriceWeight;//按价格购买权重
extern double buy_BalanceWeight;//两个节点使用资源比例平衡参数
extern double buy_leftSpaceWeight;//剩余空间
extern double buy_PriceWithCapacituWeight;//性价比
extern double buy_SmWeight;//相似性购买
// double DayWeight = 0.8;

//放置权值(和为1)
extern double put_SelectWeight;//碎片选择权重
extern double put_NodeBlanceWeight;//负载均衡参数
extern double put_SimWeight;//相似放置
extern double put_BalanceWeight;//两个节点使用资源比例平衡参数
extern double put_NearWeight;//相近放置
