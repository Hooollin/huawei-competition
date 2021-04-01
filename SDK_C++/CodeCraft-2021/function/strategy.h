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

<<<<<<< HEAD
//¹ºÂòÈ¨ÖØ£¨ºÍÎª1£©
double buy_PriceWeight = 0.15;//°´¼Û¸ñ¹ºÂòÈ¨ÖØ
double buy_BalanceWeight = 0.3;//Á½¸ö½ÚµãÊ¹ÓÃ×ÊÔ´±ÈÀıÆ½ºâ²ÎÊı
double buy_leftSpaceWeight = 0.0;//Ê£Óà¿Õ¼ä
double buy_PriceWithCapacituWeight = 0.45;//ĞÔ¼Û±È
double buy_SmWeight = 0.1;//ÏàËÆĞÔ¹ºÂò
=======
//è´­ä¹°æƒé‡ï¼ˆå’Œä¸º1ï¼‰

double buy_PriceWeight = 0.16;//æŒ‰ä»·æ ¼è´­ä¹°æƒé‡
double buy_BalanceWeight = 0.91;//ä¸¤ä¸ªèŠ‚ç‚¹ä½¿ç”¨èµ„æºæ¯”ä¾‹å¹³è¡¡å‚æ•°
double buy_leftSpaceWeight = 0.49;//å‰©ä½™ç©ºé—´
double buy_PriceWithCapacituWeight = 0.29;//æ€§ä»·æ¯”
double buy_SmWeight = 0.036;//ç›¸ä¼¼æ€§è´­ä¹°
>>>>>>> 668095262391c7038268353c54e82a3707fee0cc
// double DayWeight = 0.8;

//æ”¾ç½®æƒå€¼(å’Œä¸º1)
double put_SelectWeight = 0.39;//ç¢ç‰‡é€‰æ‹©æƒé‡
double put_NodeBlanceWeight = 0.86;//è´Ÿè½½å‡è¡¡å‚æ•°
double put_SimWeight = 0.7;//ç›¸ä¼¼æ”¾ç½®
double put_BalanceWeight = 0.67;//ä¸¤ä¸ªèŠ‚ç‚¹ä½¿ç”¨èµ„æºæ¯”ä¾‹å¹³è¡¡å‚æ•°
double put_NearWeight = 0.0;//ç›¸è¿‘æ”¾ç½®

//è¿ç§»æƒå€¼ï¼ˆå’Œä¸º1ï¼‰
double migrate_chipWeight = 1.0;//ç¢ç‰‡é€‰æ‹©æƒé‡
double migrate_balanceWeight = 0;//å¹³è¡¡æƒé‡

//ç­–ç•¥æ”¹å˜çš„å¤©æ•°æƒå€¼
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

// purchaseæ—¶è·å–æœåŠ¡å™¨ç¼–å·
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

// åˆ¤æ–­è™šæ‹Ÿæœºæ˜¯å¦å¯ä»¥æ”¾åœ¨æœåŠ¡å™¨ä¸­
int canPut(Server server, VirtualMachineModel vmd){
    int neededCore = vmd.core, neededMem = vmd.memory;
    if(vmd.single){
        int choice = NONE;
        // å¯ä»¥æ”¾åœ¨AèŠ‚ç‚¹
        if(server.nodeA.coreRem >= neededCore && server.nodeA.memoryRem >= neededMem){
            choice = A;
        }
        // å¯ä»¥æ”¾åœ¨BèŠ‚ç‚¹
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
        // å¯ä»¥æ”¾åœ¨AèŠ‚ç‚¹
        if(server.nodeA.coreRem >= neededCore && server.nodeA.memoryRem >= neededMem){
            choice = A;
        }
        // å¯ä»¥æ”¾åœ¨BèŠ‚ç‚¹
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

//è®¡ç®—é€‰æ‹©å€¼
double selectServerFun(Server &currServer, VirtualMachineModel vmd, int occupyACore,int occupyAMem,int occupyBCore,int occupyBMem){
    int leftACore = currServer.nodeA.coreRem - occupyACore, leftBCore = currServer.nodeB.coreRem - occupyBCore;
    int leftAMem = currServer.nodeA.memoryRem - occupyAMem, leftBMem = currServer.nodeB.memoryRem - occupyBMem;
    int totalCore = currServer.getCore() >> 1;
    int totalMem = currServer.getMemory() >> 1;
    //è®¡ç®—ç¢ç‰‡å‚æ•°
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

    //è®¡ç®—å¹³è¡¡å‚æ•°
    double balanceF = (abs(1 - abs(1.0 * leftACore / totalCore - 1.0 * leftAMem / totalMem)) + abs(1 - abs(1.0 * leftBCore / totalCore - 1.0 * leftBMem / totalMem))) / 2;
    double balanceNode = 1 - ((double)abs(leftACore - leftBCore) / totalCore + (double)abs(leftAMem - leftBMem) / totalMem) / total;

    //è¿”å›åŠ æƒå€¼
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
    double maxn = 0; //å‡½æ•°è·å¾—çš„æœ€å¤§å€¼
    pair<double,int> res;//è¿”å›å€¼
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
    //å½“å‰è™šæ‹Ÿæœºéœ€è¦çš„coreå’Œå†…å­˜å¤§å°
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
    //å½“å‰è™šæ‹Ÿæœºéœ€è¦çš„coreå’Œå†…å­˜å¤§å°
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
    //æ‰¾åˆ°å¯¹åº”çš„server
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

    //æ›´æ–°æ•°æ®ç»“æ„
    mVirtualMachineInServer[vmid] = {serverId, node};
    mServerHasVirtualMachine[serverId].insert(
        make_pair(vmd.core + vmd.memory, vmid));
    mServerIdToServer[serverId] = server;
    vAllServer[mServerIdVectorPos[serverId]] = server;

<<<<<<< HEAD
    sortedVirtualMachine[VirtualMachineModeltoPos[vmd.type]].insert(vmid);

    //Êä³ö
=======
    //è¾“å‡º
>>>>>>> 668095262391c7038268353c54e82a3707fee0cc
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
    // éœ€è¦æ–°å¢è™šæ‹Ÿæœºçš„å‹å·
    VirtualMachineModel vmd = mTypeToVirtualMachineModel[addop.machineType];
    //æ–°å¢çš„è™šæ‹Ÿæœºå®ä¾‹
    pair<int,int> serverAndNode = selectServer(vmd);
    if(serverAndNode.first == -1){       // æ²¡æœ‰åˆé€‚çš„æœåŠ¡å™¨ï¼Œéœ€è¦æ–°è´­ä¹°æœåŠ¡å™¨
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

<<<<<<< HEAD
    //¸üĞÂÊı¾İÊı¾İ½á¹¹
    sortedVirtualMachine[VirtualMachineModeltoPos[mVmidToVirtualMachine[vmid].getType()]].erase(vmid);
=======
    //æ›´æ–°æ•°æ®æ•°æ®ç»“æ„
>>>>>>> 668095262391c7038268353c54e82a3707fee0cc
    mVmidToVirtualMachine.erase(vmid);
    mVirtualMachineInServer.erase(vmid);
    mServerHasVirtualMachine[serverId].erase(
        {vm.getCore() + vm.getMemory(), vmid});
}

bool compareNode(Node nodeA, Node nodeB, VirtualMachine vm){
    //åªèƒ½æ”¾åœ¨Aã€Bä¸­çš„ä¸€ä¸ª
    // AèŠ‚ç‚¹å‰©ä¸‹çš„èµ„æºä¸å¤Ÿ
    if(vm.getCore() > nodeA.coreRem || vm.getMemory() > nodeA.memoryRem){
        return false;
    }
    // BèŠ‚ç‚¹å‰©ä¸‹çš„èµ„æºä¸å¤Ÿ
    if(vm.getCore() > nodeB.coreRem || vm.getMemory() > nodeB.memoryRem){
        return true;
    }

    //A, BèŠ‚ç‚¹éƒ½å¯ä»¥æ”¾æ—¶
    //å¦‚æœAå¯ä»¥æ”¾å¹¶ä¸”æ”¾åcoreå’Œmeméƒ½æ¯”Bå¤š
    if(nodeA.coreRem - vm.getCore() >= nodeB.coreRem && nodeA.memoryRem - vm.getMemory() >= nodeB.memoryRem){
        return true;
    }
    //å¦‚æœBå¯ä»¥æ”¾å¹¶ä¸”æ”¾åcoreå’Œmeméƒ½æ¯”Aå¤š
    if(nodeB.coreRem - vm.getCore() >= nodeA.coreRem && nodeB.memoryRem - vm.getMemory() >= nodeA.memoryRem){
        return false;
    }
    // å…¶ä»–çš„æƒ…å†µå–å†³äºvmdæ‰€éœ€çš„coreå’Œmemory
    if(vm.getCore() > vm.getMemory()){
        return nodeA.coreRem > nodeB.coreRem;
    }
    return nodeA.memoryRem > nodeB.memoryRem;
}
<<<<<<< HEAD
void small_virtual_machine_migrate(int today,int T){
// ÔÚÕâÒ»ÂÖ×Ü¹²¿ÉÒÔÇ¨ÒÆµÄ´ÎÊı
#ifdef DEBUG
  assert(mVmidToVirtualMachine.size() == VM_AMOUNT);
#endif
    int totalOperation = 3 * VM_AMOUNT / 100;
    // ´æ´¢ËùÓĞserver¼ÆËãÉú³ÉµÄº¯ÊıÖµ
    vector<double> choseFs;
    // vServersµÄÏÂ±ê£¬ÓÃÓÚÅÅĞò
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
    //vServers ÏÂ±ê¶ÔÓ¦µÄÎ»ÖÃ£¬ÓÃÓÚ²éÕÒ
    vector<int> serverPos(serverIdxs.size());
    // Í¨¹ıchoseFÅÅĞò£¬½«ÅÅĞòºóÏÂ±êĞ¡µÄ·şÎñÆ÷ÖĞĞéÄâ»úÍùÏÂ±ê´óµÄ·şÎñÆ÷ÖĞ·Å
    sort(serverIdxs.begin(), serverIdxs.end(), [&](int a, int b){
            return choseFs[a] < choseFs[b];
            });
    sort(choseFs.begin(),choseFs.end());
    for(int i=0;i<serverIdxs.size();i++){
        serverPos[serverIdxs[i]] = i;
    }
    //ĞèÒªÇ¨ÒÆµÄĞéÄâ»úid
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
         //´ÓËéÆ¬Ğ¡µÄ·şÎñÆ÷ÍùÏÂÕÒ£¬Ö±µ½µÚÒ»¸ö¿ÉÒÔ·ÅÏÂÕâ¸öĞéÄâ»úµÄ·şÎñÆ÷
        while(last > i && totalOperation > 0){
            // toServer£º±»Ç¨ÒÆĞéÄâ»úµÄÈ¥Ïò
            int toServerIdx = serverIdxs[last];
            // toServerÔÚvServersÖĞµÄÏÂ±ê
            int toServerId = vAllServer[toServerIdx].id;

            // choice¿ÉÒÔ·ÅÔÚÄÄ¸ö½áµã
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
                // Ç¨ÒÆµ½ĞÂµÄ·şÎñÆ÷ÉÏ
                migrateVirtualMachineToServer(vmid, {toServerId, choice});
                //¸üĞÂÊı¾İ½á¹¹
                mServerHasVirtualMachine[fromServerId].erase({vm.getCore() + vm.getMemory(), vmid});
                // ¿ÉÇ¨ÒÆ´ÎÊı¼õ1
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
// ÔÚÕâÒ»ÂÖ×Ü¹²¿ÉÒÔÇ¨ÒÆµÄ´ÎÊı
=======
void optimized_migrate(){
// åœ¨è¿™ä¸€è½®æ€»å…±å¯ä»¥è¿ç§»çš„æ¬¡æ•°
>>>>>>> 668095262391c7038268353c54e82a3707fee0cc
#ifdef DEBUG
  assert(mVmidToVirtualMachine.size() == VM_AMOUNT);
#endif
    int totalOperation = 3 * VM_AMOUNT / 100;
    //int totalOperation = 3 * mVmidVirtualMachine.size() / 1000;
    // int totalOperation = 2 * mVmidVirtualMachine.size() / 1000;

    // å­˜å‚¨æ‰€æœ‰serverè®¡ç®—ç”Ÿæˆçš„å‡½æ•°å€¼
    vector<double> choseFs;
    // vServersçš„ä¸‹æ ‡ï¼Œç”¨äºæ’åº
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
<<<<<<< HEAD
    // Í¨¹ıchoseFÅÅĞò£¬½«ÅÅĞòºóÏÂ±êĞ¡µÄ·şÎñÆ÷ÖĞĞéÄâ»úÍùÏÂ±ê´óµÄ·şÎñÆ÷ÖĞ·Å
=======
    // é€šè¿‡chipFæ’åºï¼Œå°†æ’åºåä¸‹æ ‡å°çš„æœåŠ¡å™¨ä¸­è™šæ‹Ÿæœºå¾€ä¸‹æ ‡å¤§çš„æœåŠ¡å™¨ä¸­æ”¾
>>>>>>> 668095262391c7038268353c54e82a3707fee0cc
    sort(serverIdxs.begin(), serverIdxs.end(), [&](int a, int b){
            return choseFs[a] < choseFs[b];
            });
    sort(choseFs.begin(),choseFs.end());
    int end = serverIdxs.size() - 1;
    for(int i = 0; i < end && totalOperation > 0; i++){
        // fromServer: å½“å‰éœ€è¦ç§»é™¤è™šæ‹Ÿæœºçš„æœåŠ¡å™¨
        int fromServerIdx = serverIdxs[i];
        // éœ€è¦ç§»é™¤è™šæ‹Ÿæœºçš„æœåŠ¡å™¨åœ¨vServersä¸­çš„ä¸‹æ ‡
        int fromServerId = vAllServer[fromServerIdx].id;

        // å­˜å‚¨å¾ªç¯ä¸­è¢«ç§»åŠ¨åˆ°äº†æ–°çš„æœåŠ¡å™¨çš„è™šæ‹Ÿæœºçš„vmid
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
             //ä»ç¢ç‰‡å°çš„æœåŠ¡å™¨å¾€ä¸‹æ‰¾ï¼Œç›´åˆ°ç¬¬ä¸€ä¸ªå¯ä»¥æ”¾ä¸‹è¿™ä¸ªè™šæ‹Ÿæœºçš„æœåŠ¡å™¨
            while(last > i && totalOperation > 0){
                // toServerï¼šè¢«è¿ç§»è™šæ‹Ÿæœºçš„å»å‘
                int toServerIdx = serverIdxs[last];
                // toServeråœ¨vServersä¸­çš„ä¸‹æ ‡
                int toServerId = vAllServer[toServerIdx].id;

                // choiceå¯ä»¥æ”¾åœ¨å“ªä¸ªç»“ç‚¹
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
                    // è¿ç§»åˆ°æ–°çš„æœåŠ¡å™¨ä¸Š
                    migrateVirtualMachineToServer(vmid, {toServerId, choice});
                    // åœ¨for loopä¸­ä¿®æ”¹é›†åˆä¸­çš„å…ƒç´ ä¼šå¯¼è‡´bug
                    modified.push_back({vm.getCore() + vm.getMemory(), vmid});
                    // å¯è¿ç§»æ¬¡æ•°å‡1
                    totalOperation -= 1;
                    break;
                }else{
                    last -= 1;
                }
            }
        }
        // æ›´æ–°æ•°æ®ç»“æ„
        for(auto &p : modified){
          mServerHasVirtualMachine[fromServerId].erase(p);
        }

    }
}

void solve(int today, int T){
#ifdef DEBUG 
#ifndef SEEK_PARAMETER
  cout << today << " " << vOperation.size() << endl;
#endif
<<<<<<< HEAD
    // Ë³Ğò±éÀúÃ¿´Î²Ù×÷
    //optimized_migrate(today,T);
    small_virtual_machine_migrate(today,T);
=======
#endif
    // é¡ºåºéå†æ¯æ¬¡æ“ä½œ
    optimized_migrate();
>>>>>>> 668095262391c7038268353c54e82a3707fee0cc
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

