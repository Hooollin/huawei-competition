#pragma once
#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include<unordered_set>

#include "ServerModel.h"
#include "VirtualMachineModel.h"
#include "VirtualMachine.h"
#include "Server.h"
#include "OP.h"

using namespace std;

vector<vector<OP>> vAllOperation; // 所有操作序列的数据
unordered_map<string, ServerModel> mTypeToServerModel; // 所有服务器型号； key: type -> value: 服务器型号
unordered_map<string, VirtualMachineModel> mTypeToVirtualMachineModel; // 所有虚拟机型号； key: type -> value:

vector<ServerModel> vServerModel;
vector<unordered_set<int> > sortedServer; //下标对应排序的vServerModel,set内包含所有该类型model的服务器id;
unordered_map<string,int > ServerModeltoPos ; //各个服务器模型type对应的排序后的下标

vector<VirtualMachineModel> vVirtualMachineModel;
vector<unordered_set<int> > sortedVirtualMachine; //下标对应排序的vVitrualMachineModel,set内包含所有该类型model的虚拟机id;
unordered_map<string,int > VirtualMachineModeltoPos ; //各个虚拟机模型type对应的排序后的下标
unordered_map<int, int> mLocalServerIdGlobalServerId; //serverId到输出id的映射

unordered_map<int, pair<int, int>> mVirtualMachineInServer; // 虚拟机编号到服务器编号以及节点的映射
unordered_map<int, VirtualMachine> mVmidToVirtualMachine; //虚拟机编号到虚拟机到映射
unordered_map<int, set<pair<int, int>>> mServerHasVirtualMachine;

vector<Server> vAllServer;                    //所有已经创建的服务器
unordered_map<int, Server> mServerIdToServer; //服务器编号到服务器的映射
unordered_map<int, int> mServerIdVectorPos;    //服务器编号到vector下标的映射


//输出数据
vector<string> vMigration;
vector<pair<int, int>> vDeployment;
vector<int> vPurchasedServer;

vector<OP> vOperation;

//dp迁移相关
unordered_set<int> dpMigrateServerId; //可能会被dp迁移的serverid
