
#pragma once

#include <string>
#include <set>
#include <unordered_map>

#include "ServerModel.h"
#include "VirtualMachineModel.h"
#include "VirtualMachine.h"
#include "Server.h"
#include "OP.h"

using namespace std;


unordered_map<string, ServerModel> mTypeToServerModel; // 所有服务器型号； key: type -> value: 服务器型号
unordered_map<string, VirtualMachineModel> mTypeToVirtualMachineModel; // 所有虚拟机型号； key: type -> value:

vector<ServerModel> vServerModel;
vector<VirtualMachineModel> vVirtualMachineModel;

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
