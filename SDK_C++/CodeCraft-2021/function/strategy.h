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

double buy_PriceWeight = 0.5376;//按价格购买权重
double buy_BalanceWeight = 0.5914;//两个节点使用资源比例平衡参数
double buy_leftSpaceWeight = 0.2532;//剩余空间
double buy_PriceWithCapacituWeight = 0.9921;//性价比
double buy_SmWeight = 0.0752;//相似性购买
// double DayWeight = 0.8;

//放置权值(和为1)
double put_SelectWeight = 0.0713;//碎片选择权重
double put_NodeBlanceWeight = 0.913;//负载均衡参数
double put_SimWeight = 0.0596;//相似放置
double put_BalanceWeight = 0.5347;//两个节点使用资源比例平衡参数
double put_NearWeight = 0.0;//相近放置

double migrate_chipWeight = 1.0;
double migrate_balanceWeight = 0;

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
pair<int,int> selectServer(VirtualMachineModel vmd);

double selectServerFun(Server &currServer, VirtualMachineModel vmd, int occupyACore,int occupyAMem,int occupyBCore,int occupyBMem);

pair<double,int> selectServerCal(Server &currServer, VirtualMachineModel &vmd,int choice);

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

// purchaseʱ��ȡ���������
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

// �ж�������Ƿ���Է��ڷ�������
int canPut(Server server, VirtualMachineModel vmd){
    int neededCore = vmd.core, neededMem = vmd.memory;
    if(vmd.single){
        int choice = NONE;
        // ���Է���A�ڵ�
        if(server.nodeA.coreRem >= neededCore && server.nodeA.memoryRem >= neededMem){
            choice = A;
        }
        // ���Է���B�ڵ�
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
        // ���Է���A�ڵ�
        if(server.nodeA.coreRem >= neededCore && server.nodeA.memoryRem >= neededMem){
            choice = A;
        }
        // ���Է���B�ڵ�
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

//����ѡ��ֵ
double selectServerFun(Server &currServer, VirtualMachineModel vmd, int occupyACore,int occupyAMem,int occupyBCore,int occupyBMem){
    int leftACore = currServer.nodeA.coreRem - occupyACore, leftBCore = currServer.nodeB.coreRem - occupyBCore;
    int leftAMem = currServer.nodeA.memoryRem - occupyAMem, leftBMem = currServer.nodeB.memoryRem - occupyBMem;
    int totalCore = currServer.getCore() >> 1;
    int totalMem = currServer.getMemory() >> 1;
    //������Ƭ����
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

    //����ƽ�����
    double balanceF = (abs(1 - abs(1.0 * leftACore / totalCore - 1.0 * leftAMem / totalMem)) + abs(1 - abs(1.0 * leftBCore / totalCore - 1.0 * leftBMem / totalMem))) / 2;
    double balanceNode = 1 - ((double)abs(leftACore - leftBCore) / totalCore + (double)abs(leftAMem - leftBMem) / totalMem) / total;

    //���ؼ�Ȩֵ
    return put_SelectWeight * chipF + put_SimWeight * Sim + put_NearWeight * Near + balanceF * put_BalanceWeight + put_NodeBlanceWeight * balanceNode;
}
inline bool serverEmpty(const Server &server){
    return server.nodeA.coreUsed == 0 && server.nodeA.memoryUsed == 0 && server.nodeB.coreUsed == 0 && server.nodeB.memoryUsed == 0;
}

pair<double,int> selectServerCal(Server &currServer, VirtualMachineModel &vmd,int choice){
    double res = 0;
    int choseNode = 0;
    double cal;
    if(vmd.single){
        if(choice == A || choice == BOTH){
            res = selectServerFun(currServer, vmd, vmd.core,vmd.memory,0,0);
            choseNode = A;
        }
        if(choice == B || choice == BOTH){
            cal = selectServerFun(currServer, vmd, 0,0,vmd.core,vmd.memory);
            if(cal > res){
                res = cal;
                choseNode = B;
            }
        }
    }else{
        res = selectServerFun(currServer,vmd, vmd.core >> 1,vmd.memory >> 1,vmd.core >> 1,vmd.memory >> 1);
        choseNode = BOTH;
    }
    return make_pair(res + (serverEmpty(currServer) ? 0 : 1),choseNode);
}

pair<int,int> selectServer(VirtualMachineModel vmd){
    double maxn = 0; //������õ����ֵ
    pair<double,int> res;//����ֵ
    int finalChoice;
    int targetServerIdx = -1;
    for (int i = 0; i < vAllServer.size(); i++) {
      Server currServer = vAllServer[i];
      int choice = canPut(currServer, vmd);
      if (choice > 0) {
        res = selectServerCal(currServer, vmd, choice);
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
    //��ǰ�������Ҫ��core���ڴ��С
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
                 buy_leftSpaceWeight * spaceF +
                 buy_PriceWithCapacituWeight * newPriceC + newSm * buy_SmWeight;

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
    //��ǰ�������Ҫ��core���ڴ��С
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
    //�ҵ���Ӧ��server
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

    //�������ݽṹ
    mVirtualMachineInServer[vmid] = {serverId, node};
    mServerHasVirtualMachine[serverId].insert(
        make_pair(vmd.core + vmd.memory, vmid));
    mServerIdToServer[serverId] = server;
    vAllServer[mServerIdVectorPos[serverId]] = server;

    sortedVirtualMachine[VirtualMachineModeltoPos[vmd.type]].insert(vmid);

    //���
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
    // ��Ҫ������������ͺ�
    VirtualMachineModel vmd = mTypeToVirtualMachineModel[addop.machineType];
    //�����������ʵ��
    pair<int,int> serverAndNode = selectServer(vmd);
    if(serverAndNode.first == -1){       // û�к��ʵķ���������Ҫ�¹��������
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

    //�����������ݽṹ
    sortedVirtualMachine[VirtualMachineModeltoPos[mVmidToVirtualMachine[vmid].getType()]].erase(vmid);
    mVmidToVirtualMachine.erase(vmid);
    mVirtualMachineInServer.erase(vmid);
    mServerHasVirtualMachine[serverId].erase(
        {vm.getCore() + vm.getMemory(), vmid});
}

bool compareNode(Node nodeA, Node nodeB, VirtualMachine vm){
    //ֻ�ܷ���A��B�е�һ��
    // A�ڵ�ʣ�µ���Դ����
    if(vm.getCore() > nodeA.coreRem || vm.getMemory() > nodeA.memoryRem){
        return false;
    }
    // B�ڵ�ʣ�µ���Դ����
    if(vm.getCore() > nodeB.coreRem || vm.getMemory() > nodeB.memoryRem){
        return true;
    }

    //A, B�ڵ㶼���Է�ʱ
    //���A���ԷŲ��ҷź�core��mem����B��
    if(nodeA.coreRem - vm.getCore() >= nodeB.coreRem && nodeA.memoryRem - vm.getMemory() >= nodeB.memoryRem){
        return true;
    }
    //���B���ԷŲ��ҷź�core��mem����A��
    if(nodeB.coreRem - vm.getCore() >= nodeA.coreRem && nodeB.memoryRem - vm.getMemory() >= nodeA.memoryRem){
        return false;
    }
    // ���������ȡ����vmd�����core��memory
    if(vm.getCore() > vm.getMemory()){
        return nodeA.coreRem > nodeB.coreRem;
    }
    return nodeA.memoryRem > nodeB.memoryRem;
}
void small_virtual_machine_migrate(int today,int T){
// ����һ���ܹ�����Ǩ�ƵĴ���
#ifdef DEBUG
  assert(mVmidToVirtualMachine.size() == VM_AMOUNT);
#endif
    int totalOperation = 3 * VM_AMOUNT / 100;
    // �洢����server�������ɵĺ���ֵ
    vector<double> choseFs;
    // vServers���±꣬��������
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
    //vServers �±��Ӧ��λ�ã����ڲ���
    vector<int> serverPos(serverIdxs.size());
    // ͨ��choseF���򣬽�������±�С�ķ���������������±��ķ������з�
    sort(serverIdxs.begin(), serverIdxs.end(), [&](int a, int b){
            return choseFs[a] < choseFs[b];
            });
    sort(choseFs.begin(),choseFs.end());
    for(int i=0;i<serverIdxs.size();i++){
        serverPos[serverIdxs[i]] = i;
    }
    //��ҪǨ�Ƶ������id
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
         //����ƬС�ķ����������ң�ֱ����һ�����Է������������ķ�����
        while(last > i && totalOperation > 0){
            // toServer����Ǩ���������ȥ��
            int toServerIdx = serverIdxs[last];
            // toServer��vServers�е��±�
            int toServerId = vAllServer[toServerIdx].id;

            // choice���Է����ĸ����
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
                // Ǩ�Ƶ��µķ�������
                migrateVirtualMachineToServer(vmid, {toServerId, choice});
                //�������ݽṹ
                mServerHasVirtualMachine[fromServerId].erase({vm.getCore() + vm.getMemory(), vmid});
                // ��Ǩ�ƴ�����1
                totalOperation -= 1;
                break;
            }else{
                last -= 1;
            }
        }
    }
    return ;
}

void optimized_migrate(int today,int T){
// ����һ���ܹ�����Ǩ�ƵĴ���
#ifdef DEBUG
  assert(mVmidToVirtualMachine.size() == VM_AMOUNT);
#endif
    int totalOperation = 3 * VM_AMOUNT / 100;
    //int totalOperation = 3 * mVmidVirtualMachine.size() / 1000;
    // int totalOperation = 2 * mVmidVirtualMachine.size() / 1000;

    // �洢����server�������ɵĺ���ֵ
    vector<double> choseFs;
    // vServers���±꣬��������
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
    // ͨ��choseF���򣬽�������±�С�ķ���������������±��ķ������з�
    sort(serverIdxs.begin(), serverIdxs.end(), [&](int a, int b){
            return choseFs[a] < choseFs[b];
            });
    sort(choseFs.begin(),choseFs.end());
    int end = serverIdxs.size() - 1;
    for(int i = 0; i < end && totalOperation > 0; i++){
        // fromServer: ��ǰ��Ҫ�Ƴ�������ķ�����
        int fromServerIdx = serverIdxs[i];
        // ��Ҫ�Ƴ�������ķ�������vServers�е��±�
        int fromServerId = vAllServer[fromServerIdx].id;

        // �洢ѭ���б��ƶ������µķ��������������vmid
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
             //����ƬС�ķ����������ң�ֱ����һ�����Է������������ķ�����
            while(last > i && totalOperation > 0){
                // toServer����Ǩ���������ȥ��
                int toServerIdx = serverIdxs[last];
                // toServer��vServers�е��±�
                int toServerId = vAllServer[toServerIdx].id;

                // choice���Է����ĸ����
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
                    // Ǩ�Ƶ��µķ�������
                    migrateVirtualMachineToServer(vmid, {toServerId, choice});
                    // ��for loop���޸ļ����е�Ԫ�ػᵼ��bug
                    modified.push_back({vm.getCore() + vm.getMemory(), vmid});
                    // ��Ǩ�ƴ�����1
                    totalOperation -= 1;
                    break;
                }else{
                    last -= 1;
                }
            }
        }
        // �������ݽṹ
        for(auto &p : modified){
          mServerHasVirtualMachine[fromServerId].erase(p);
        }

    }
}

void solve(int today, int T){
#ifdef DEBUG
  cout << today << " " << vOperation.size() << endl;
#endif
    // ˳�����ÿ�β���
    //optimized_migrate(today,T);
    small_virtual_machine_migrate(today,T);
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

