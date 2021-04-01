
#pragma once

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


unordered_map<string, ServerModel> mTypeToServerModel; // ���з������ͺţ� key: type -> value: �������ͺ�
unordered_map<string, VirtualMachineModel> mTypeToVirtualMachineModel; // ����������ͺţ� key: type -> value:

vector<ServerModel> vServerModel;
vector<VirtualMachineModel> vVirtualMachineModel;
vector<unordered_set<int> > sortedVirtualMachine; //�±��Ӧ�����vVitrualMachineModel,set�ڰ������и�����model�������id;
unordered_map<string,int > VirtualMachineModeltoPos ; //���������ģ��type��Ӧ���������±�
unordered_map<int, int> mLocalServerIdGlobalServerId; //serverId�����id��ӳ��

unordered_map<int, pair<int, int>> mVirtualMachineInServer; // �������ŵ�����������Լ��ڵ��ӳ��
unordered_map<int, VirtualMachine> mVmidToVirtualMachine; //�������ŵ��������ӳ��
unordered_map<int, set<pair<int, int>>> mServerHasVirtualMachine;

vector<Server> vAllServer;                    //�����Ѿ������ķ�����
unordered_map<int, Server> mServerIdToServer; //��������ŵ���������ӳ��
unordered_map<int, int> mServerIdVectorPos;    //��������ŵ�vector�±��ӳ��

//�������
vector<string> vMigration;
vector<pair<int, int>> vDeployment;
vector<int> vPurchasedServer;

vector<OP> vOperation;
