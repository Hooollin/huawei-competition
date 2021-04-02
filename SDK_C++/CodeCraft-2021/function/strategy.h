#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>
#include <queue>
#include <set>
#include <unordered_map>

#include "./interaction.h"

//购买权重（和为1）

double buy_PriceWeight = 0.53;//按价格购买权重
double buy_BalanceWeight = 0.59;//两个节点使用资源比例平衡参数
double buy_LeftSpaceWeight = 0.25;        //剩余空间
double buy_PriceWithCapacityWeight = 0.99; //性价比
double buy_SmWeight = 0.07;//相似性购买
// double DayWeight = 0.8;

//放置权值(和为1)
double put_SelectWeight = 0.07;//碎片选择权重
double put_NodeBlanceWeight = 0.91;//负载均衡参数
double put_SimWeight = 0.05;//相似放置
double put_BalanceWeight = 0.53;//两个节点使用资源比例平衡参数
double put_PriceWithCapacityWeight = 0.5;
double put_NearWeight = 0.0;//相近放置

//迁移权值（和为1）
double migrate_chipWeight = 1.0;//碎片选择权重
double migrate_balanceWeight = 0;//平衡权重

//策略改变的天数权值
const double change_buyWeight = 1.0;

//改变迁移策略的数据量
const double migrate_changeData = 9000;

//dp策略填充机器量

const int dp_machineNumber = 70;

//前一天购买服务器量

int preDayPurchase = -1;

// work
void solve(int day, int T);

// migrate
void optimized_migrate(int today,int T,int totalOperation);

void migrate(int today,int T,int opNum,int prePurchase);

int dp_migrate(int today,int T,int totalOperation);

void small_virtual_machine_migrate(int today,int T);

void clear_small_server_migrate(int today,int T,int totalOperation);

// purchase
pair<int,int> makePurchase(VirtualMachineModel vmd, int today, int T);

pair<int,int> makePurchase1(VirtualMachineModel vmd, int today, int T);

pair<int,int> makePurchase2(VirtualMachineModel vmd, int today, int T);

int getNextServerId();

// deployment
pair<int,int> selectServer(int today, int T, VirtualMachineModel vmd);

double selectServerFun(int today, int T, Server &currServer, VirtualMachineModel vmd, int occupyACore,int occupyAMem,int occupyBCore,int occupyBMem);

pair<double,int> selectServerCal(int today, int T, Server &currServer, VirtualMachineModel &vmd,int choice);

void putVirtualMachineToServer(VirtualMachineModel vmd, int vmid, pair<int,int> serverAndNode);

void allocateServer(OP addop, int today, int T);

void releaseRes(OP delop);

//inner function
void updateResource(Server &server, int node, int vmcore, int vmmemory, bool isDel);

double similarity(VirtualMachineModel &vmd, Server &server);

double similarity(VirtualMachineModel &vmd, ServerModel &server);

int canPut(Server server, VirtualMachineModel vmd);

int canPut(Server server, VirtualMachine vm);

bool canBuy(ServerModel sm, int neededCore, int neededMem);

bool compareNode(Node nodeA, Node nodeB, VirtualMachine vm);


//**********************************DECLEAR END*****************************

// purchase时获取服务器编号
int getNextServerId(){
    return localServerNum++;
}

void updateResource(Server &server, int node, int vmcore, int vmmemory, bool isDel){
    int sign = isDel ? 1 : -1;
    int corediff = sign * vmcore;
    int memdiff = sign * vmmemory;
    switch(node){
        case A:
            server.nodeA.coreRem += corediff;
            server.nodeA.coreUsed -= corediff;
            server.nodeA.memoryRem += memdiff;
            server.nodeA.memoryUsed -= memdiff;
            break;
        case B:
            server.nodeB.coreRem += corediff;
            server.nodeB.coreUsed -= corediff;
            server.nodeB.memoryRem += memdiff;
            server.nodeB.memoryUsed -= memdiff;
            break;
        case BOTH:
            corediff /= 2;
            memdiff /= 2;

            server.nodeA.coreRem += corediff;
            server.nodeA.coreUsed -= corediff;
            server.nodeA.memoryRem += memdiff;
            server.nodeA.memoryUsed -= memdiff;

            server.nodeB.coreRem += corediff;
            server.nodeB.coreUsed -= corediff;
            server.nodeB.memoryRem += memdiff;
            server.nodeB.memoryUsed -= memdiff;
            break;
    }
}

double similarity(VirtualMachineModel &vmd, Server &server){
    return abs((double)vmd.core / vmd.memory - (double)server.getCore() / server.getMemory());
}

double similarity(VirtualMachineModel &vmd, ServerModel &server){
    return abs((double)vmd.core / vmd.memory - (double)server.core / server.memory);
}

// 判断虚拟机是否可以放在服务器中
int canPut(Server server, VirtualMachineModel vmd){
    int neededCore = vmd.core, neededMem = vmd.memory;
    if(vmd.single){
        int choice = NONE;
        // 可以放在A节点
        if(server.nodeA.coreRem >= neededCore && server.nodeA.memoryRem >= neededMem){
            choice = A;
        }
        // 可以放在B节点
        if(server.nodeB.coreRem >= neededCore && server.nodeB.memoryRem >= neededMem){
            choice = (choice == NONE) ? B : BOTH;
        }
        return choice;
    }else{
        neededCore /= 2; neededMem /= 2;
        if(server.nodeA.coreRem < neededCore){
            return NONE;
        }
        if(server.nodeB.coreRem < neededCore){
            return NONE;
        }
        if(server.nodeA.memoryRem < neededMem){
            return NONE;
        }
        if(server.nodeB.memoryRem < neededMem){
            return NONE;
        }
        return BOTH;
    }
}

int canPut(Server server, VirtualMachine vm){
    int neededCore = vm.getCore(), neededMem = vm.getMemory();
    if(vm.getSingle()){
        int choice = NONE;
        // 可以放在A节点
        if(server.nodeA.coreRem >= neededCore && server.nodeA.memoryRem >= neededMem){
            choice = A;
        }
        // 可以放在B节点
        if(server.nodeB.coreRem >= neededCore && server.nodeB.memoryRem >= neededMem){
            choice = (choice == NONE) ? B : BOTH;
        }
        return choice;
    }else{
        neededCore /= 2; neededMem /= 2;
        if(server.nodeA.coreRem < neededCore){
            return NONE;
        }
        if(server.nodeB.coreRem < neededCore){
            return NONE;
        }
        if(server.nodeA.memoryRem < neededMem){
            return NONE;
        }
        if(server.nodeB.memoryRem < neededMem){
            return NONE;
        }
        return BOTH;
    }
}

//计算选择值
double selectServerFun(int today, int T, Server &currServer, VirtualMachineModel vmd, int occupyACore, int occupyAMem, int occupyBCore, int occupyBMem){
    int leftACore = currServer.nodeA.coreRem - occupyACore, leftBCore = currServer.nodeB.coreRem - occupyBCore;
    int leftAMem = currServer.nodeA.memoryRem - occupyAMem, leftBMem = currServer.nodeB.memoryRem - occupyBMem;
    int totalCore = currServer.getCore() >> 1;
    int totalMem = currServer.getMemory() >> 1;
    //计算碎片参数
    double chipF;
    double total = currServer.getCore() + currServer.getMemory();
    double Sim = 0.0, Near = 0.0;

    if(occupyACore == 0 && occupyAMem == 0){
        chipF = ((1.0 * totalCore - leftBCore) / currServer.getCore() + (1.0 * totalMem - leftBMem) / currServer.getMemory()) / (total / 2);
        Near = 1 - (abs(currServer.getCore() - occupyBCore) + abs(currServer.getMemory() - occupyBMem)) / MAXSOURCE;
        Sim = 1.0 - abs(1.0 * occupyBCore / occupyBMem - 1.0 * currServer.getCore() / currServer.getMemory()) / (MAX_SIMI - MIN_SIMI);
    } else if(occupyBCore == 0 && occupyBMem == 0){
        chipF = ((1.0 * totalCore - leftACore) / currServer.getCore() + (1.0 * totalMem - leftAMem) / currServer.getMemory()) / (total / 2);
        Near = 1 - (abs(currServer.getCore() - occupyACore) + abs(currServer.getMemory() - occupyAMem)) / MAXSOURCE;
        Sim = 1. - abs(1.0 * occupyACore / occupyAMem - 1.0 * currServer.getCore() / currServer.getMemory()) / (MAX_SIMI - MIN_SIMI);
    } else{
        chipF = (1.0 * total- leftACore - leftAMem - leftBCore - leftBMem) / total;
        Near = 1 - (abs((currServer.getCore()>>1) - occupyBCore) + abs((currServer.getMemory()>>1) - occupyBMem)) / MAXSOURCE;
        Sim = 1.0 - abs(1.0 * occupyBCore / occupyBMem - 1.0 * currServer.getCore() / currServer.getMemory()) / (MAX_SIMI - MIN_SIMI);
    }
    //计算平衡参数
    double balanceF = (abs(1 - abs(1.0 * leftACore / totalCore - 1.0 * leftAMem / totalMem)) + abs(1 - abs(1.0 * leftBCore / totalCore - 1.0 * leftBMem / totalMem))) / 2;
    double balanceNode = 1 - ((double)abs(leftACore - leftBCore) / totalCore + (double)abs(leftAMem - leftBMem) / totalMem) / total;
    double priceF = (MAX_DEVICE_COST + MAX_DAILY_COST * T - (double)currServer.deviceCost + currServer.dailyCost * (T - today)) / (MAX_DEVICE_COST + MAX_DAILY_COST * T);
    //返回加权值
    return put_PriceWithCapacityWeight * priceF + put_SelectWeight * chipF + put_SimWeight * Sim + put_NearWeight * Near + balanceF * put_BalanceWeight + put_NodeBlanceWeight * balanceNode;
}
inline bool serverEmpty(const Server &server){
    return server.nodeA.coreUsed == 0 && server.nodeA.memoryUsed == 0 && server.nodeB.coreUsed == 0 && server.nodeB.memoryUsed == 0;

}

pair<double,int> selectServerCal(int today, int T, Server &currServer, VirtualMachineModel &vmd, int choice){
    double res = 0;
    int choseNode = 0;
    double cal;
    if(vmd.single){
        if(choice == A || choice == BOTH){
            res = selectServerFun(today, T, currServer, vmd, vmd.core,vmd.memory,0,0);
            choseNode = A;
        }
        if(choice == B || choice == BOTH){
            cal = selectServerFun(today, T, currServer, vmd, 0,0,vmd.core,vmd.memory);
            if(cal > res){
                res = cal;
                choseNode = B;
            }
        }
    }else{
        res = selectServerFun(today, T, currServer,vmd, vmd.core >> 1,vmd.memory >> 1,vmd.core >> 1,vmd.memory >> 1);
        choseNode = BOTH;
    }
    return make_pair(res + (serverEmpty(currServer) ? 0 : 1),choseNode);
}

pair<int,int> selectServer(int today, int T, VirtualMachineModel vmd){
    double maxn = 0; //函数获得的最大值
    pair<double,int> res;//返回值
    int finalChoice;
    int targetServerIdx = -1;
    for (int i = 0; i < vAllServer.size(); i++) {
      Server currServer = vAllServer[i];
      int choice = canPut(currServer, vmd);
      if (choice > 0) {
        res = selectServerCal(today, T, currServer, vmd, choice);
        if (res.first > maxn)
          maxn = res.first, targetServerIdx = i, finalChoice = res.second;
      }
    }
    return targetServerIdx == -1
               ? make_pair(-1, -1)
               : make_pair(vAllServer[targetServerIdx].id, finalChoice);
}

bool canBuy(ServerModel sm, int neededCore, int neededMem){
    return sm.core >= neededCore && sm.memory >= neededMem;
}

pair<int,int> makePurchase(VirtualMachineModel vmd, int today, int T){
    if(today < T * change_buyWeight) return makePurchase1(vmd,today,T);
    else return makePurchase2(vmd,today,T);
}

pair<int,int> makePurchase1(VirtualMachineModel vmd, int today, int T){
    int newServerId = getNextServerId();
    //当前虚拟机需要的core和内存大小
    int neededCore = vmd.core, neededMem = vmd.memory;
    if(vmd.single){
        neededCore *= 2;
        neededMem *= 2;
    }
    double maxP = -1,minP = 0x3f3f3f3f;
    double maxC = -1,minC = 0x3f3f3f3f;
    double maxSm = -1, minSm = 0x3f3f3f3f;
    double maxNear = -1, minNear = 0x3f3f3f3f;
    int leftCore,leftMem,devicePrice,dayPrice,totalCore,totalMem,leftNodeCore,leftNodeMem,totalNodeCore,totalNodeMem;

    vector<double> PriceF(vServerModel.size(), -1);
    vector<double> PriceC(vServerModel.size(), -1);
    vector<double> Sm(vServerModel.size(), -1);
    vector<double> Near(vServerModel.size(), -1);

    for (int i = 0; i < vServerModel.size(); i++) {
      ServerModel p = vServerModel[i];
      if (canBuy(p, neededCore, neededMem)) {
        devicePrice = p.deviceCost;
        dayPrice = p.dailyCost * (T - today);
        totalCore = p.core;
        totalMem = p.memory;
        PriceF[i] = devicePrice + dayPrice;
        PriceC[i] = 1.0 * (devicePrice + dayPrice) / totalMem +
                    1.0 * (devicePrice + dayPrice) / totalCore;
        Near[i]   = abs(1. * totalCore - neededCore) + abs(1. * totalMem - neededMem);
        Sm[i]     = abs((1. * MEAN_VM_CORE / MEAN_VM_MEMORY) -
                    (1. * totalCore / totalMem));

        maxP = max(maxP, PriceF[i]), minP = min(minP, PriceF[i]);
        maxC = max(maxC, PriceC[i]), minC = min(minC, PriceC[i]);
        maxSm = max(maxSm, Sm[i]), minSm = min(minSm, Sm[i]);
        maxNear = max(maxNear, Near[i]), minNear = min(minNear, Near[i]);
      }
    }

    int k = -1;
    double balanceF,spaceF,newPriceF,choseF, newPriceC, newSm, newNear;
    double maxn = -1;
    for (int i = 0; i < vServerModel.size(); i++) {
      ServerModel p = vServerModel[i];
      if (canBuy(p, neededCore, neededMem)) {
        leftCore = p.core - vmd.core;
        leftMem = p.memory - vmd.memory;
        totalCore = p.core;
        totalMem = p.memory;
        totalNodeCore = totalCore >> 1;
        totalNodeMem = totalMem >> 1;
        if (vmd.single) {
          leftNodeCore = (totalCore >> 1) - vmd.core;
          leftNodeMem = (totalMem >> 1) - vmd.memory;
        } else {
          leftNodeCore = (totalCore >> 1) - (vmd.core >> 1);
          leftNodeMem = (totalMem >> 1) - (vmd.memory >> 1);
        }

        balanceF = 1 - abs((1.0 * leftNodeCore / totalNodeCore) - (1.0 * leftNodeMem / totalNodeMem));

        spaceF = (1.0 * leftCore + leftMem) / MAXSOURCE;
        newPriceF = 1 - (PriceF[i] - minP) / (maxP - minP);
        newPriceC = 1 - (PriceC[i] - minC) / (maxC - minC);
        newSm = 1 - (Sm[i] - minSm) / (maxSm - minSm);
        newNear = 1 - (Near[i] - minNear) / (maxNear - minNear);

        choseF = buy_PriceWeight * newPriceF + buy_BalanceWeight * balanceF +
                 buy_LeftSpaceWeight * spaceF +
                 buy_PriceWithCapacityWeight * newPriceC + newSm * buy_SmWeight;

        if (maxn < choseF)
          k = i, maxn = choseF;
      }
    }
#ifdef DEBUG
    assert(k != -1);
#endif

    ServerModel sm = vServerModel[k];
    Server purchasedServer(sm.type, newServerId, sm.core, sm.memory, sm.deviceCost, sm.dailyCost);
    vPurchasedServer.push_back(newServerId);

    vAllServer.push_back(purchasedServer);
    mServerIdVectorPos[newServerId] = vAllServer.size() - 1;
    mServerIdToServer[newServerId] = purchasedServer;
    sortedServer[k].insert(newServerId);

    if(vmd.single) return make_pair(newServerId,A);
    else return make_pair(newServerId,BOTH);
}

pair<int,int> makePurchase2(VirtualMachineModel vmd, int today, int T){
    int newServerId = getNextServerId();
    //当前虚拟机需要的core和内存大小
    int neededCore = vmd.core, neededMem = vmd.memory;
    if(vmd.single){
        neededCore *= 2;
        neededMem *= 2;
    }
    int k = -1;
    int leftCore,leftMem,devicePrice,dayPrice,totalCore,totalMem,leftNodeCore,leftNodeMem,totalNodeCore,totalNodeMem,priceF;
    int minPriceF = 0x3f3f3f3f;
    for (int i = 0; i < vServerModel.size(); i++) {
      ServerModel p = vServerModel[i];
      if (canBuy(p, neededCore, neededMem)) {
        devicePrice = p.deviceCost;
        dayPrice = p.dailyCost * (T - today);
        priceF = devicePrice + dayPrice;
        if(priceF < minPriceF) minPriceF = priceF,k = i;
      }
    }

#ifdef DEBUG
    assert(k != -1);
#endif

    ServerModel sm = vServerModel[k];
    Server purchasedServer(sm.type, newServerId, sm.core, sm.memory, sm.deviceCost, sm.dailyCost);
    vPurchasedServer.push_back(newServerId);

    vAllServer.push_back(purchasedServer);
    mServerIdVectorPos[newServerId] = vAllServer.size() - 1;
    mServerIdToServer[newServerId] = purchasedServer;

    if(vmd.single) return make_pair(newServerId,A);
    else return make_pair(newServerId,BOTH);
}

void putVirtualMachineToServer(VirtualMachineModel vmd, int vmid, pair<int,int> serverAndNode){
    //找到对应的server
    int serverId = serverAndNode.first, node = serverAndNode.second;
    Server server = mServerIdToServer[serverId];
#ifdef DEBUG
    assert(canPut(server, vmd));
#endif
    updateResource(server, node, vmd.core, vmd.memory, false);
    if (mServerHasVirtualMachine.find(serverId) ==
        mServerHasVirtualMachine.end()) {
      mServerHasVirtualMachine[serverId] = set<pair<int, int>>();
    }
#ifdef DEBUG
    assert(vmid != 0);
#endif

    //更新数据结构
    mVirtualMachineInServer[vmid] = {serverId, node};
    mServerHasVirtualMachine[serverId].insert(
        make_pair(vmd.core + vmd.memory, vmid));
    mServerIdToServer[serverId] = server;
    vAllServer[mServerIdVectorPos[serverId]] = server;

    sortedVirtualMachine[VirtualMachineModeltoPos[vmd.type]].insert(vmid);

    //输出
    vDeployment.push_back({serverId, node});
}


void migrateVirtualMachineToServer(int vmid, pair<int, int> serverAndNode){
  Server fromServer = mServerIdToServer[mVirtualMachineInServer[vmid].first];
  int fromnode = mVirtualMachineInServer[vmid].second;

  Server toServer = mServerIdToServer[serverAndNode.first];
  int tonode = serverAndNode.second;

  updateResource(fromServer, fromnode, mVmidToVirtualMachine[vmid].getCore(),
                 mVmidToVirtualMachine[vmid].getMemory(), true);

  updateResource(toServer, tonode, mVmidToVirtualMachine[vmid].getCore(),
                 mVmidToVirtualMachine[vmid].getMemory(), false);

  mServerIdToServer[fromServer.id] = fromServer;
  vAllServer[mServerIdVectorPos[fromServer.id]] = fromServer;

  mServerIdToServer[toServer.id] = toServer;
  vAllServer[mServerIdVectorPos[toServer.id]] = toServer;

  mVirtualMachineInServer[vmid] = serverAndNode;

  mServerHasVirtualMachine[toServer.id].insert(
      {mVmidToVirtualMachine[vmid].getCore() +
           mVmidToVirtualMachine[vmid].getMemory(),
       vmid});
}


void allocateServer(OP addop, int today, int T){
    int vmid = addop.id;
    VM_AMOUNT += 1;
    // 需要新增虚拟机的型号
    VirtualMachineModel vmd = mTypeToVirtualMachineModel[addop.machineType];
    //新增的虚拟机实例
    pair<int,int> serverAndNode = selectServer(today, T, vmd);
    if(serverAndNode.first == -1){       // 没有合适的服务器，需要新购买服务器
        serverAndNode = makePurchase(vmd, today, T);
    }

    VirtualMachine vm(addop.machineType, vmid, vmd.core, vmd.memory, vmd.single);
    mVmidToVirtualMachine[vmid] = vm;

    putVirtualMachineToServer(vmd, vmid, serverAndNode);
}


void releaseRes(OP delop){
    VM_AMOUNT -= 1;
    int vmid = delop.id;
    VirtualMachine vm = mVmidToVirtualMachine[vmid];

    pair<int, int> t = mVirtualMachineInServer[vmid];
    int serverId = t.first, loc = t.second;
    Server server = mServerIdToServer[serverId];

    updateResource(server, loc, vm.getCore(), vm.getMemory(), true);
    vAllServer[mServerIdVectorPos[serverId]] = server;
    mServerIdToServer[serverId] = server;

    //更新数据数据结构
    sortedVirtualMachine[VirtualMachineModeltoPos[mVmidToVirtualMachine[vmid].getType()]].erase(vmid);
    mVmidToVirtualMachine.erase(vmid);
    mVirtualMachineInServer.erase(vmid);
    mServerHasVirtualMachine[serverId].erase(
        {vm.getCore() + vm.getMemory(), vmid});
}

bool compareNode(Node nodeA, Node nodeB, VirtualMachine vm){
    //只能放在A、B中的一个
    // A节点剩下的资源不够
    if(vm.getCore() > nodeA.coreRem || vm.getMemory() > nodeA.memoryRem){
        return false;
    }
    // B节点剩下的资源不够
    if(vm.getCore() > nodeB.coreRem || vm.getMemory() > nodeB.memoryRem){
        return true;
    }

    //A, B节点都可以放时
    //如果A可以放并且放后core和mem都比B多
    if(nodeA.coreRem - vm.getCore() >= nodeB.coreRem && nodeA.memoryRem - vm.getMemory() >= nodeB.memoryRem){
        return true;
    }
    //如果B可以放并且放后core和mem都比A多
    if(nodeB.coreRem - vm.getCore() >= nodeA.coreRem && nodeB.memoryRem - vm.getMemory() >= nodeA.memoryRem){
        return false;
    }
    // 其他的情况取决于vmd所需的core和memory
    if(vm.getCore() > vm.getMemory()){
        return nodeA.coreRem > nodeB.coreRem;
    }
    return nodeA.memoryRem > nodeB.memoryRem;
}
void small_virtual_machine_migrate(int today,int T){
// 在这一轮总共可以迁移的次数
#ifdef DEBUG
  assert(mVmidToVirtualMachine.size() == VM_AMOUNT);
#endif
    int totalOperation = 3 * VM_AMOUNT / 100;
    // 存储所有server计算生成的函数值
    vector<double> choseFs;
    // vServers的下标，用于排序
    vector<int> serverIdxs;
    int idx = 0;
    for (auto &server : vAllServer) {
      double chipF =
          (1.0 * MAXSOURCE - server.nodeA.coreRem - server.nodeA.memoryRem -
           server.nodeB.coreRem - server.nodeB.memoryRem) /
          MAXSOURCE;
          double choseF = chipF;
      choseFs.push_back(choseF);
      serverIdxs.push_back(idx++);
    }
    //vServers 下标对应的位置，用于查找
    vector<int> serverPos(serverIdxs.size());
    // 通过choseF排序，将排序后下标小的服务器中虚拟机往下标大的服务器中放
    sort(serverIdxs.begin(), serverIdxs.end(), [&](int a, int b){
            return choseFs[a] < choseFs[b];
            });
    sort(choseFs.begin(),choseFs.end());
    for(int i=0;i<serverIdxs.size();i++){
        serverPos[serverIdxs[i]] = i;
    }
    //需要迁移的虚拟机id
    vector<int> migrateVmid;
    for(int i=0;i<sortedVirtualMachine.size();i++){
        for(int vmid : sortedVirtualMachine[i]){
            migrateVmid.push_back(vmid);
        }
    }
    for(int vmid : migrateVmid){
        VirtualMachine &vm = mVmidToVirtualMachine[vmid];
        double chipF =
          (1.0 * MAXSOURCE - vm.getCore() - vm.getMemory()) / MAXSOURCE;
        int last = upper_bound(choseFs.begin(), choseFs.end(), chipF) -
             choseFs.begin();
        last--;
        int fromServerId = mVirtualMachineInServer[vmid].first;
        int i = serverPos[mServerIdVectorPos[fromServerId]];
#ifdef DEBUG
      assert(mVmidToVirtualMachine.find(vmid) !=
             mVmidToVirtualMachine.end());
#endif
         //从碎片小的服务器往下找，直到第一个可以放下这个虚拟机的服务器
        while(last > i && totalOperation > 0){
            // toServer：被迁移虚拟机的去向
            int toServerIdx = serverIdxs[last];
            // toServer在vServers中的下标
            int toServerId = vAllServer[toServerIdx].id;

            // choice可以放在哪个结点
            int choice = canPut(vAllServer[toServerId], vm);
            if(choice > 0){
                if(vm.getSingle() && choice == BOTH){
                    if(compareNode(mServerIdToServer[toServerId].nodeA,mServerIdToServer[toServerId].nodeB,vm)) {
                      choice = A;
                    } else {
                      choice = B;
                    }
                }
                vMigration.push_back(makeMigrateOutput(vmid,mLocalServerIdGlobalServerId[toServerId],choice));
                // 迁移到新的服务器上
                migrateVirtualMachineToServer(vmid, {toServerId, choice});
                //更新数据结构
                mServerHasVirtualMachine[fromServerId].erase({vm.getCore() + vm.getMemory(), vmid});
                // 可迁移次数减1
                totalOperation -= 1;
                break;
            }else{
                last -= 1;
            }
        }
    }
    return ;
}

void migrate_by_costperf(int today,int T){
// 在这一轮总共可以迁移的次数
#ifdef DEBUG
  assert(mVmidToVirtualMachine.size() == VM_AMOUNT);
#endif
    int totalOperation = 3 * VM_AMOUNT / 100;
    //int totalOperation = 3 * mVmidVirtualMachine.size() / 1000;
    // int totalOperation = 2 * mVmidVirtualMachine.size() / 1000;

    // 存储所有server计算生成的函数值
    vector<int> storage;
    vector<double> costperf;
    // vServers的下标，用于排序
    vector<int> storageIdx;
    vector<int> costperfIdx;
    int idx = 0;
    for (auto &server : vAllServer) {
      int chipF = MAXSOURCE - server.nodeA.coreRem - server.nodeA.memoryRem -
           server.nodeB.coreRem - server.nodeB.memoryRem;
      double costperfF = (double)(server.dailyCost * (T - today) + server.deviceCost) / (server.getCore() + server.getMemory());
      storage.push_back(chipF);
      costperf.push_back(costperfF);
      storageIdx.push_back(idx);
      costperfIdx.push_back(idx);
      idx++;
    }
    // 通过storage排序，将排序后下标小的服务器中虚拟机往下标大的服务器中放
    sort(storageIdx.begin(), storageIdx.end(),
         [&](int a, int b) { return storage[a] < storage[b]; });
    // 通过性价比排序，江排序后性价比低的往性价比高的服务器中放
    sort(costperfIdx.begin(), costperfIdx.end(),
            [&](int a, int b){ return costperf[a] > costperf[b]; });

    sort(storage.begin(), storage.end());
    int end = storageIdx.size() - 1;
    for(int i = 0; i < end && totalOperation > 0; i++){
        // fromServer: 当前需要移除虚拟机的服务器
        int fromServerIdx = storageIdx[i];
        // 需要移除虚拟机的服务器在vServers中的下标
        int fromServerId = vAllServer[fromServerIdx].id;

        // 存储循环中被移动到了新的服务器的虚拟机的vmid
        vector<pair<int, int>> modified;

        int last = end;
        for (auto p : mServerHasVirtualMachine[fromServerId]) {
          int vmid = p.second;
          VirtualMachine &vm = mVmidToVirtualMachine[vmid];
          double chipF =
              (1.0 * MAXSOURCE - vm.getCore() - vm.getMemory()) / MAXSOURCE;
          last = upper_bound(storage.begin(), storage.end(), chipF) -
                 storage.begin();
          last--;
#ifdef DEBUG
          assert(mVmidToVirtualMachine.find(vmid) !=
                 mVmidToVirtualMachine.end());
#endif
             //从碎片小的服务器往下找，直到第一个可以放下这个虚拟机的服务器
            while(last > i && totalOperation > 0){
                // toServer：被迁移虚拟机的去向
                int toServerIdx = storageIdx[last];
                // toServer在vServers中的下标
                int toServerId = vAllServer[toServerIdx].id;

                // choice可以放在哪个结点
                int choice = canPut(vAllServer[toServerId], vm);
                if(choice > 0 && !serverEmpty(vAllServer[toServerId])){
                    if(vm.getSingle() && choice == BOTH){
                        if(compareNode(mServerIdToServer[toServerId].nodeA,mServerIdToServer[toServerId].nodeB,vm)) {
                          choice = A;
                        } else {
                          choice = B;
                        }
                    }
                    vMigration.push_back(makeMigrateOutput(vmid,mLocalServerIdGlobalServerId[toServerId],choice));
                    // 迁移到新的服务器上
                    migrateVirtualMachineToServer(vmid, {toServerId, choice});
                    // 在for loop中修改集合中的元素会导致bug
                    modified.push_back({vm.getCore() + vm.getMemory(), vmid});
                    // 可迁移次数减1
                    totalOperation -= 1;
                    break;
                }else{
                    last -= 1;
                }
            }
        }
        // 更新数据结构
        for(auto &p : modified){
          mServerHasVirtualMachine[fromServerId].erase(p);
        }

    }
}

void migrate(int today,int T,int opNum,int prePurchase){
// 在这一轮总共可以迁移的次数
#ifdef DEBUG
  if(mVmidToVirtualMachine.size() != VM_AMOUNT){
    cout<<mVmidToVirtualMachine.size()<<" "<<VM_AMOUNT<<endl;
    assert(mVmidToVirtualMachine.size() == VM_AMOUNT);
  }
#endif
    int totalOperation = 3 * VM_AMOUNT / 100;
    clear_small_server_migrate(today,T,totalOperation);

/*
    if(opNum > migrate_changeData && prePurchase == 0){
        int leftOperation = dp_migrate(today,T,totalOperation);
        optimized_migrate(today,T,leftOperation);
    }
    optimized_migrate(today,T,totalOperation);
*/

    return ;
}

inline int getServerChip(Server &server){
    return server.nodeA.coreRem + server.nodeA.memoryRem + server.nodeB.coreRem + server.nodeB.memoryRem;
}
inline int getVMCap(VirtualMachine &vm){
    return vm.getCore() +  vm.getMemory();
}
inline int getVMModelCap(VirtualMachineModel &vmd){
    return vmd.core +  vmd.memory;
}

int dp_migrate(int today,int T,int totalOperation){//返回最终迁移次数
    //存储需要dp的服务器Id
    vector<int> storeServerId;
    //可以动态增删，查找符合要求碎片大小的服务器的set

    set<pair<int,int> > fragments;//(碎片大小，serverId)
    for(pair<int,Server> p : mServerIdToServer){
        Server &server = p.second;
        fragments.insert(make_pair(getServerChip(server),p.first));
    }
    //现存可用的虚拟机
    vector<unordered_set<int> > leftVM = sortedVirtualMachine;

    int migrateServerNum = dp_machineNumber;
    //获取小服务器进行dp迁移
    for(int i=0;i<sortedServer.size();i++){
        for(int serverId : sortedServer[i]){
            storeServerId.push_back(serverId);
            migrateServerNum --;
            if(migrateServerNum <= 0) break;
        }
        if(migrateServerNum <= 0) break;
    }
    //获得各虚拟机类型的数量，单节点服务器成对放,双节点为虚拟机个数，单节点为虚拟机对数
    vector<int> virtualMachineNum;
    for(int i=0;i<sortedVirtualMachine.size();i++){
        if(vVirtualMachineModel[i].single) virtualMachineNum.push_back(sortedVirtualMachine[i].size() / 2);
        else virtualMachineNum.push_back(sortedVirtualMachine[i].size());
    }
    for(int serverId : storeServerId){
        Server &server = mServerIdToServer[serverId];
        if(totalOperation <= 0) break;

        //仅考虑两节点平衡放置的情况，即双节点部署的虚拟机单个放入，单节点同一类型的虚拟机成对放入，容量为单个节点的容量
        int serverCoreNum = mServerIdToServer[serverId].getCore() / 2;
        int serverMemNum = mServerIdToServer[serverId].getMemory() / 2;
        //二维dp，第一维为core，第二维为mem，值为放的虚拟机模型下标
        vector<vector<int> > VMPos(serverCoreNum + 1,vector<int> (serverMemNum + 1,-2));

        //将虚拟机物品进行二进制优化
        //第一维为体积，第二维为下标

        vector<pair<pair<int,int>,int> > vmgoods;

        for(int i=0;i<virtualMachineNum.size();i++){
            int x = 1,y = virtualMachineNum[i];
            int VMcoreNum,VMMemNum;
            if(vVirtualMachineModel[i].single){
                VMcoreNum = vVirtualMachineModel[i].core;
                VMMemNum = vVirtualMachineModel[i].memory;
            }else{
                VMcoreNum = vVirtualMachineModel[i].core / 2;
                VMMemNum = vVirtualMachineModel[i].memory / 2;
            }
            while(y >= x){
                y -= x;
                vmgoods.push_back(make_pair(make_pair(VMcoreNum * x,VMMemNum * x),i));
                x *= 2;
            }
            if(y > 0){
                vmgoods.push_back(make_pair(make_pair(VMcoreNum * y,VMMemNum * y),i));
            }
        }
        //开始二进制优化多重背包
        VMPos[0][0] = -1;
        for(int i=0;i<vmgoods.size();i++){
            int oriCore = vmgoods[i].first.first,oriMem = vmgoods[i].first.second,type = i;
            for(int j=serverCoreNum;j>=oriCore;j--){
                for(int k=serverMemNum;k>=oriMem;k--){
                    if(VMPos[j - oriCore][k - oriMem] >= -1) VMPos[j][k] = type;
                }
            }
        }
        //获得剩余碎片最小的位置(posx，posy),且希望剩余空间尽可能平衡
        int minChip = serverCoreNum + serverMemNum,minbalance = max(serverCoreNum,serverMemNum);
        int posx = -1,posy = -1;
        for(int i=0;i<serverCoreNum;i++){
            for(int j=0;j<=serverMemNum;j++){
                if(i == 0 && j == 0) continue;
                if(VMPos[i][j] == -2) continue;
                int chip = serverCoreNum - i + serverMemNum - j, balance = abs((serverCoreNum - i) - (serverMemNum - j));
                if(chip < minChip || (chip == minChip && balance < minbalance)){
                    minChip = chip;
                    balance = minbalance;
                    posx = i;
                    posy = j;
                }
            }
        }
        //没有这样的位置,去做下个服务器迁移
        if(posx == -1 && posy == -1) continue;

        //获得背包策略下各种虚拟机真正数量
        map<int,int> migrateNum;

        //
        while(posx >0 || posy > 0){
            int pos = VMPos[posx][posy];
            pair<pair<int,int>,int > &good = vmgoods[pos];
            int xsub = good.first.first, ysub = good.first.second;
            int findpos = good.second;
            int num = xsub / vVirtualMachineModel[findpos].core;
            if(vVirtualMachineModel[findpos].single) num *= 2;
            migrateNum[findpos] += num;
            posx -= xsub,posy -= ysub;
        }
        //获得需要迁出的虚拟机,优先迁移双节点虚拟机出去
        vector<int> migrateOut;

        for(pair<int,int> p : mServerHasVirtualMachine[serverId]){
            int vmid = p.second;
#ifdef DEBUG
        if(mVmidToVirtualMachine.find(vmid) == mVmidToVirtualMachine.end()){
            cout<<vmid<<endl;
            assert(mVmidToVirtualMachine.find(vmid) != mVmidToVirtualMachine.end());
        }
#endif // DEBUG
            if(!mVmidToVirtualMachine[vmid].getSingle()) migrateOut.push_back(vmid);
        }
        for(pair<int,int> p : mServerHasVirtualMachine[serverId]){
            int vmid = p.second;
#ifdef DEBUG
                    assert(mVmidToVirtualMachine.find(vmid) != mVmidToVirtualMachine.end());
#endif // DEBUG
            if(mVmidToVirtualMachine[vmid].getSingle())migrateOut.push_back(vmid);
        }
        //开始做迁移 先迁出去，再迁进来。优先先迁出去小的，迁进来大的
        int i = 0;//迁移出去的migrateout 下标
        bool flag = true;//迁移标记，若一轮都没有任何迁移发生，则跳出
        map<int,int>::iterator migrateit = migrateNum.begin(); //迁移进来的Num迭代器
        while(totalOperation > 0 && (i < migrateOut.size() || migrateit != migrateNum.end()) && flag == true){
            flag = false;
            while(totalOperation > 0 && i < migrateOut.size()){
                int &fromServerId = serverId;
                Server &fromServer = server;
                int vmid = migrateOut[i];
#ifdef DEBUG
                    assert(mVmidToVirtualMachine.find(vmid) != mVmidToVirtualMachine.end());
#endif // DEBUG
                VirtualMachine &vm = mVmidToVirtualMachine[vmid];
                set<pair<int,int> >::iterator p = fragments.lower_bound(make_pair(getVMCap(vm),0));
                while(p != fragments.end()  && (!canPut(mServerIdToServer[p->second],vm) || p->second == fromServerId )) p ++;
                if(p == fragments.end()) break;
                int choice = canPut(mServerIdToServer[p->second],vm);
                if(choice == NONE) break;
                int toServerId = p->second;
                if(vm.getSingle() && choice == BOTH){
                    if(compareNode(mServerIdToServer[toServerId].nodeA,mServerIdToServer[toServerId].nodeB,vm)) {
                      choice = A;
                    } else {
                      choice = B;
                    }
                }
                vMigration.push_back(makeMigrateOutput(vmid,mLocalServerIdGlobalServerId[toServerId],choice));
                //更新fragment
                fragments.erase(make_pair(getServerChip(mServerIdToServer[fromServerId]),fromServerId));
                fragments.erase(make_pair(getServerChip(mServerIdToServer[toServerId]),toServerId));
                // 迁移到新的服务器上
                migrateVirtualMachineToServer(vmid, {toServerId, choice});
                //更新fragment
                fragments.insert(make_pair(getServerChip(mServerIdToServer[fromServerId]),fromServerId));
                fragments.insert(make_pair(getServerChip(mServerIdToServer[toServerId]),toServerId));
                //更新数据结构
                mServerHasVirtualMachine[fromServerId].erase({vm.getCore() + vm.getMemory(), vmid});
                //标记为成功迁移过
                flag = true;
                //总迁移次数-1
                totalOperation --;
                i++;//指向下一个迁移虚拟机
            }
            while(totalOperation > 0 && migrateit != migrateNum.end()){

                int &toServerId = serverId;
                Server &toServer = server;
                while(migrateit != migrateNum.end() && migrateit->second == 0) migrateit ++;
                if(migrateit == migrateNum.end()) break;
                int pos = migrateit->first;

                //单节点部署虚拟机情况
                if(vVirtualMachineModel[pos].single){
                    if(totalOperation < 2) break;//不能迁移两次,跳出
                    if(vVirtualMachineModel[pos].core > toServer.nodeA.coreRem || vVirtualMachineModel[pos].core > toServer.nodeA.memoryRem ||
                       vVirtualMachineModel[pos].core > toServer.nodeB.coreRem || vVirtualMachineModel[pos].core > toServer.nodeB.memoryRem)
                        break;//两个单节点虚拟机无法同时迁入，跳出
                    migrateNum[pos] -= 2;
                    virtualMachineNum[pos] -= 1;

                    auto p = leftVM[pos].begin();
                    int vmid = *p;
#ifdef DEBUG
                    assert(p != leftVM[pos].end());
                    assert(mVmidToVirtualMachine.find(vmid) != mVmidToVirtualMachine.end());
#endif // DEBUG
                    leftVM[pos].erase(p);
                    VirtualMachine vm = mVmidToVirtualMachine[vmid];
                    pair<int,int> ServerAndNode = mVirtualMachineInServer[vmid];
                    int fromServerId = ServerAndNode.first;
                    Server fromServer = mServerIdToServer[fromServerId];
                    int choice = A;
                    vMigration.push_back(makeMigrateOutput(vmid,mLocalServerIdGlobalServerId[toServerId],choice));
                    //更新fragment
                    fragments.erase(make_pair(getServerChip(mServerIdToServer[fromServerId]),fromServerId));
                    fragments.erase(make_pair(getServerChip(mServerIdToServer[toServerId]),toServerId));
                    // 迁移到新的服务器上
                    migrateVirtualMachineToServer(vmid, {toServerId, choice});
                    //更新fragment
                    fragments.insert(make_pair(getServerChip(mServerIdToServer[fromServerId]),fromServerId));
                    fragments.insert(make_pair(getServerChip(mServerIdToServer[toServerId]),toServerId));
                    //更新数据结构
                    mServerHasVirtualMachine[fromServerId].erase({vm.getCore() + vm.getMemory(), vmid});

                    p = leftVM[pos].begin();
                    vmid = *p;
#ifdef DEBUG
                    assert(p != leftVM[pos].end());
                    assert(mVmidToVirtualMachine.find(vmid) != mVmidToVirtualMachine.end());
#endif // DEBUG
                    leftVM[pos].erase(p);
                    vm = mVmidToVirtualMachine[vmid];
                    ServerAndNode = mVirtualMachineInServer[vmid];
                    fromServerId = ServerAndNode.first;
                    fromServer = mServerIdToServer[fromServerId];
                    choice = B;
                    vMigration.push_back(makeMigrateOutput(vmid,mLocalServerIdGlobalServerId[toServerId],choice));
                    //更新fragment
                    fragments.erase(make_pair(getServerChip(mServerIdToServer[fromServerId]),fromServerId));
                    fragments.erase(make_pair(getServerChip(mServerIdToServer[toServerId]),toServerId));
                    // 迁移到新的服务器上
                    migrateVirtualMachineToServer(vmid, {toServerId, choice});
                    //更新fragment
                    fragments.insert(make_pair(getServerChip(mServerIdToServer[fromServerId]),fromServerId));
                    fragments.insert(make_pair(getServerChip(mServerIdToServer[toServerId]),toServerId));
                    //更新数据结构
                    mServerHasVirtualMachine[fromServerId].erase({vm.getCore() + vm.getMemory(), vmid});

                    //标记为成功迁移过
                    flag = true;
                    //总迁移次数-2
                    totalOperation -= 2;
                }else{
                //双节点迁移
                    if(vVirtualMachineModel[pos].core / 2 > toServer.nodeA.coreRem || vVirtualMachineModel[pos].core / 2> toServer.nodeA.memoryRem ||
                       vVirtualMachineModel[pos].core / 2> toServer.nodeB.coreRem || vVirtualMachineModel[pos].core / 2> toServer.nodeB.memoryRem)
                        break;//双节点虚拟机无法迁入，跳出
                    migrateNum[pos] --;
                    virtualMachineNum[pos] -= 1;

                    auto p = leftVM[pos].begin();
                    int vmid = *p;
#ifdef DEBUG
                    assert(p != leftVM[pos].end());
                    assert(mVmidToVirtualMachine.find(vmid) != mVmidToVirtualMachine.end());
#endif // DEBUG
                    leftVM[pos].erase(p);
                    VirtualMachine vm = mVmidToVirtualMachine[vmid];
                    pair<int,int> ServerAndNode = mVirtualMachineInServer[vmid];
                    int fromServerId = ServerAndNode.first;
                    Server fromServer = mServerIdToServer[fromServerId];
                    int choice = ServerAndNode.second;
                    vMigration.push_back(makeMigrateOutput(vmid,mLocalServerIdGlobalServerId[toServerId],choice));
                    //更新fragment
                    fragments.erase(make_pair(getServerChip(mServerIdToServer[fromServerId]),fromServerId));
                    fragments.erase(make_pair(getServerChip(mServerIdToServer[toServerId]),toServerId));
                    // 迁移到新的服务器上
                    migrateVirtualMachineToServer(vmid, {toServerId, choice});
                    //更新fragment
                    fragments.insert(make_pair(getServerChip(mServerIdToServer[fromServerId]),fromServerId));
                    fragments.insert(make_pair(getServerChip(mServerIdToServer[toServerId]),toServerId));
                    //更新数据结构
                    mServerHasVirtualMachine[fromServerId].erase({vm.getCore() + vm.getMemory(), vmid});
                    //标记为成功迁移过
                    flag = true;
                    //总迁移次数-1
                    totalOperation --;
                }

            }
        }
    }
    return totalOperation;
}

void optimized_migrate(int today,int T,int totalOperation){
// 在这一轮总共可以迁移的次数
#ifdef DEBUG
  assert(mVmidToVirtualMachine.size() == VM_AMOUNT);
#endif
    //int totalOperation = 3 * mVmidVirtualMachine.size() / 1000;
    // int totalOperation = 2 * mVmidVirtualMachine.size() / 1000;

    // 存储所有server计算生成的函数值
    vector<double> choseFs;
    // vServers的下标，用于排序
    vector<int> serverIdxs;
    int idx = 0;
    for (auto &server : vAllServer) {
        int leftACore = server.nodeA.coreRem , leftBCore = server.nodeB.coreRem;
        int leftAMem = server.nodeA.memoryRem, leftBMem = server.nodeB.memoryRem;
        int totalCore = server.getCore() >> 1;
        int totalMem = server.getMemory() >> 1;
      double chipF =
          (1.0 * MAXSOURCE - server.nodeA.coreRem - server.nodeA.memoryRem -
           server.nodeB.coreRem - server.nodeB.memoryRem) /  MAXSOURCE;
      double balanceF = (abs(1 - abs(1.0 * leftACore / totalCore - 1.0 * leftAMem / totalMem)) + abs(1 - abs(1.0 * leftBCore / totalCore - 1.0 * leftBMem / totalMem))) / 2;
      double choseF = migrate_balanceWeight * balanceF + migrate_chipWeight * chipF;
      choseFs.push_back(choseF);
      serverIdxs.push_back(idx++);
    }
    // 通过choseF排序，将排序后下标小的服务器中虚拟机往下标大的服务器中放
    sort(serverIdxs.begin(), serverIdxs.end(), [&](int a, int b){
            return choseFs[a] < choseFs[b];
            });
    sort(choseFs.begin(),choseFs.end());
    int end = serverIdxs.size() - 1;
    for(int i = 0; i < end && totalOperation > 0; i++){
        // fromServer: 当前需要移除虚拟机的服务器
        int fromServerIdx = serverIdxs[i];
        // 需要移除虚拟机的服务器在vServers中的下标
        int fromServerId = vAllServer[fromServerIdx].id;

        // 存储循环中被移动到了新的服务器的虚拟机的vmid
        vector<pair<int, int>> modified;

        int last = end;
        for (auto p : mServerHasVirtualMachine[fromServerId]) {
          int vmid = p.second;
          VirtualMachine &vm = mVmidToVirtualMachine[vmid];
          double chipF =
              (1.0 * MAXSOURCE - vm.getCore() - vm.getMemory()) / MAXSOURCE;
          last = upper_bound(choseFs.begin(), choseFs.end(), chipF) -
                 choseFs.begin();
          last--;
#ifdef DEBUG
          assert(mVmidToVirtualMachine.find(vmid) !=
                 mVmidToVirtualMachine.end());
#endif
             //从碎片小的服务器往下找，直到第一个可以放下这个虚拟机的服务器
            while(last > i && totalOperation > 0){
                // toServer：被迁移虚拟机的去向
                int toServerIdx = serverIdxs[last];
                // toServer在vServers中的下标
                int toServerId = vAllServer[toServerIdx].id;

                // choice可以放在哪个结点
                int choice = canPut(vAllServer[toServerId], vm);
                if(choice > 0 && !serverEmpty(vAllServer[toServerId])){
                    if(vm.getSingle() && choice == BOTH){
                        if(compareNode(mServerIdToServer[toServerId].nodeA,mServerIdToServer[toServerId].nodeB,vm)) {
                          choice = A;
                        } else {
                          choice = B;
                        }
                    }
                    vMigration.push_back(makeMigrateOutput(vmid,mLocalServerIdGlobalServerId[toServerId],choice));
                    // 迁移到新的服务器上
                    migrateVirtualMachineToServer(vmid, {toServerId, choice});
                    // 在for loop中修改集合中的元素会导致bug
                    modified.push_back({vm.getCore() + vm.getMemory(), vmid});
                    // 可迁移次数减1
                    totalOperation -= 1;
                    break;
                }else{
                    last -= 1;
                }
            }
        }
        // 更新数据结构
        for(auto &p : modified){
          mServerHasVirtualMachine[fromServerId].erase(p);
        }
    }
}
void clear_small_server_migrate(int today,int T,int totalOperation){

    //按照服务器小为第一优先级，虚拟机少为第二优先级，腾出服务器空间
    vector<int> serverIdsq;//处理的服务器Id序列

    for(auto p : sortedServer){
        vector<int> tmp;
        for(int serverId : p){
            tmp.push_back(serverId);
        }
        sort(tmp.begin(),tmp.end(),[&](const int &a,const int &b){
             return mServerHasVirtualMachine[a].size() < mServerHasVirtualMachine[b].size();
             });
        for(int serverId : p){
            serverIdsq.push_back(serverId);
        }
    }
    reverse(serverIdsq.begin(),serverIdsq.end());
    set<pair<int,int> > fragments;//(碎片大小，serverId)
    for(pair<int,Server> p : mServerIdToServer){
        Server &server = p.second;
        fragments.insert(make_pair(getServerChip(server),p.first));
    }
    for(int i=0;i<serverIdsq.size();i++){
        if(totalOperation <= 0) break;
        int fromServerId = serverIdsq[i];
        int fromServerIdx = i;
        Server &server = mServerIdToServer[fromServerId];
        set<pair<int,int> >::iterator last;
        fragments.erase(make_pair(getServerChip(mServerIdToServer[fromServerId]),fromServerId));
        for (auto p : mServerHasVirtualMachine[fromServerId]) {
            if(totalOperation <= 0) break;
            int vmid = p.second;
            VirtualMachine &vm = mVmidToVirtualMachine[vmid];
            last = fragments.upper_bound(make_pair(getVMCap(vm),0));
            while(last != fragments.end() && !canPut(mServerIdToServer[last->second],vm)) last ++;
            if(last == fragments.end()) continue;
            int choice = canPut(mServerIdToServer[last->second],vm);
            if(choice == NONE) continue;
            int toServerId = last->second;
            if(vm.getSingle() && choice == BOTH){
                if(compareNode(mServerIdToServer[toServerId].nodeA,mServerIdToServer[toServerId].nodeB,vm)) {
                  choice = A;
                } else {
                  choice = B;
                }
            }
            vMigration.push_back(makeMigrateOutput(vmid,mLocalServerIdGlobalServerId[toServerId],choice));
            //更新fragment
            fragments.erase(make_pair(getServerChip(mServerIdToServer[toServerId]),toServerId));
            // 迁移到新的服务器上
            migrateVirtualMachineToServer(vmid, {toServerId, choice});
            //更新fragment
            fragments.insert(make_pair(getServerChip(mServerIdToServer[toServerId]),toServerId));
            //更新数据结构
            mServerHasVirtualMachine[fromServerId].erase({vm.getCore() + vm.getMemory(), vmid});
            //总迁移次数-1
            totalOperation --;
        }
    }
    return ;
}
void solve(int today, int T){
#ifdef DEBUG
#ifndef SEEK_PARAMETER
  cout << today << " " << vOperation.size() << endl;
#endif
#endif
    // 顺序遍历每次操作
    migrate(today,T,vOperation.size(),preDayPurchase);
    //small_virtual_machine_migrate(today,T);
    //migrate();
    for (auto &op : vOperation) {
      switch (op.opType) {
      case ADD:
        allocateServer(op, today, T);
        break;
      case DEL:
        releaseRes(op);
        break;
      }
    }
#ifdef DEBUG
    for(Server p : vAllServer){
        if(p.nodeA.coreUsed > 0 || p.nodeB.coreUsed > 0 || p.nodeA.memoryUsed > 0 || p.nodeB.memoryUsed > 0){
            dailycost += p.getDailyCost();
        }
    }
#endif // DEBUG
    preDayPurchase = vPurchasedServer.size();
    doOutput();
}
