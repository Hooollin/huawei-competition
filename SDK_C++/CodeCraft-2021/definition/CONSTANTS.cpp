#include "CONSTANTS.h"

#ifdef DEBUG
int addcount = 0;
int delcount = 0;
#endif

// 用来生成服务器编号
int localServerNum = 0;
int globalServerNumber = 0;

int MAX_VM_CORE = 0;
int MAX_VM_MEMORY = 0;
int MAX_SERVERMODEL_CORE = 0;
int MAX_SERVERMODEL_MEMORY = 0;
int MEAN_VM_CORE = 0;
int MEAN_VM_MEMORY = 0;
int VM_AMOUNT = 0;
double MAX_SIMI = -1, MIN_SIMI = 0x3f3f3f3f;

int MAXSOURCE = 0;

//购买权重（和为1）
double buy_PriceWeight = 0.4;//按价格购买权重
double buy_BalanceWeight = 0.3;//两个节点使用资源比例平衡参数
double buy_leftSpaceWeight = 0.0;//剩余空间
double buy_PriceWithCapacituWeight = 0.3;//性价比
double buy_SmWeight = 0.1;//相似性购买
// double DayWeight = 0.8;

//放置权值(和为1)
double put_SelectWeight = 0.0;//碎片选择权重
double put_NodeBlanceWeight = 0.2;//负载均衡参数
double put_SimWeight = 0.;//相似放置
double put_BalanceWeight = 0.3;//两个节点使用资源比例平衡参数
double put_NearWeight = 0.0;//相近放置
