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
double change_buyWeight = 1.0;

// work
void solve(int day, int T);

// migrate
void migrateVirtualMachineToServer(int vmid, pair<int, int> serverAndNode);

void optimized_migrate(int today,int T);

void small_virtual_machine_migrate(int today,int T);

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
    double priceF = ((double)currServer.deviceCost + currServer.dailyCost * (T - today)) / (MAX_DEVICE_COST + MAX_DAILY_COST * T);
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

//最优化性价比购买策略
pair<int, int> makePurchase3(VirtualMachineModel vmd, int today, int T){
    int newServerId = getNextServerId();
    //当前虚拟机需要的core和内存大小
    int neededCore = vmd.core, neededMem = vmd.memory;
    if(vmd.single){
        neededCore *= 2;
        neededMem *= 2;
    }
    //计算满足k天情况服务器最少新增最优解
    int expert_core = 0, expert_mem = 0;

    pair<int, int> expert = getBetterServerResource(vOperationKdays, vServerModel);
    expert_core = expert.first;
    expert_mem = expert.second;

    //遍历服务器，选择满足k天情况最优的服务器
    int k = -1;
    int distance = 1000000000;
    for(int i = 0; i < vServerModel.size(); i ++){
        //寻找最接近expert资源的服务器(大于最好, 但是大于尚未写)
        int new_distance = abs(vServerModel[i].core - expert_core) + abs(vServerModel[i] - expert_mem);
        if(new_distance < distance){
            distance = new_distance;
            k = i;
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

//计算满足k天情况下的最优服务器(返回核心和内存)
pair<int, int> getBetterServerResource(vector<vector<OP>> &vOperationKdays, vector<ServerModel> vServerModel){
    //vServerModel 按性价比排序
    //按性价比排序所以服务器(价格比上性能，越小越好)
    sort(vServerModel.begin(), vServerModel.end(), [&](ServerModel&a, ServerModel&b){
        double priceServerA = a.getDailyCost() * (T - today) + a.getDeviceCost();
        double priceServerB = b.getDailyCost() * (T - today) + b.getDeviceCost();
        double priceWithCapacityA =  priceServerA / a.totalMem + priceServerA / a.totalCore;
        double priceWithCapacityB =  priceServerB / b.totalMem + priceServerB / b.totalCore;
        return priceWithCapacityA < priceWithCapacityB;
    });
    int expert_core = 0, expert_mem = 0;
    for(int i = 0; i < vOperationKdays.size(); i ++){
        for(int j = 0;j < vOperationKdays[i].size(); j ++){
            //判断这个操作对于当前情况需不需要购买服务器，如果需要，则将当前操作虚拟机需要资源加进expert

        }
    }
    return make_pair(expert_core, expert_mem)
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

void optimized_migrate(int today,int T){
// 在这一轮总共可以迁移的次数
#ifdef DEBUG
  assert(mVmidToVirtualMachine.size() == VM_AMOUNT);
#endif
    int totalOperation = 3 * VM_AMOUNT / 100;
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
           server.nodeB.coreRem - server.nodeB.memoryRem) /
          MAXSOURCE;
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

void solve(int today, int T){
#ifdef DEBUG
  cout << today << " " << vOperation.size() << endl;
#endif
    // 顺序遍历每次操作
    optimized_migrate(today,T);
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
    doOutput();
}
