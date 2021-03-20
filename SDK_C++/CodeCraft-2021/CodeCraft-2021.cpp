#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>

//最大CPU + RAM

// 操作类型
#define ADD 1
#define DEL 2

// 虚拟机所在节点：A、B、BOTH,虚拟机可部署节点：1.单节点下:A,B,BOTH,NONE2.双节点下:BOTH,NONE
#define A 3
#define B 4
#define BOTH 5
#define NONE 0
//#define DEBUG

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
int MAXSOURCE = 0;

// 权重值
const double SelectWeight = 0.8;
const double PurchaseWeight = 0.8;

using namespace std;



//***************************DEFINITION**********************************


class Node{
    public:
        int coreUsed;
        int coreRem;            //还可以用的核心数
        int memoryUsed;
        int memoryRem;          //还可以用的内存大小

        string tostring(){
            return "Node: {coreUsed: " + to_string(coreUsed)
                + ", coreRem: " + to_string(coreRem)
                + ", memoryUsed: " + to_string(memoryUsed)
                + ", memoryRem: " + to_string(memoryRem)
                + "}";
        }
};

class ServerModel{
    public:
        string type;    // 型号
        int core;		// CPU数量
        int memory;		// 内存大小

        int deviceCost; // 硬件成本
        int dailyCost;  // 日常成本

        ServerModel(){}

        ServerModel(string type, int core, int memory, int deviceCost, int dailyCost){
            this->type = type;
            this->core = core;
            this->memory = memory;
            this->deviceCost = deviceCost;
            this->dailyCost = dailyCost;
        }

        string tostring(){
            return "ServerModel: {type: " + this->type
                + ", core: " + to_string(this->core)
                + ", memory: " + to_string(this->memory)
                + ", deviceCost: " + to_string(this->deviceCost)
                + ", dailyCost: " + to_string(this->dailyCost) + "}";
        }

};

class VirtualMachineModel{
    public:
        string type;	//虚拟机型号
        int core;		//所需内核数
        int memory;		//所需内存大小
        bool single;	//是否单节点布置

        VirtualMachineModel(){

        }

        VirtualMachineModel(string type, int core, int memory, bool single){
            this->type = type;
            this->core = core;
            this->memory = memory;
            this->single = single;
        }

        string tostring(){
            return "VirtualMachineModel: {type: " + this->type
                + ", core: " + to_string(this->core)
                + ", memory: " + to_string(this->memory)
                + ", single: " + (this->single ? "True" : "False")
                + "}";
        }
};

// 这四行放在sever和virtual machine之前
map<string, ServerModel> mServerModels; // 所有服务器型号； key: type -> value: 服务器型号
map<string, VirtualMachineModel> mVirtualMachineModels; // 所有虚拟机型号； key: type -> value: 虚拟机型号

vector<ServerModel> vServerModels;
vector<VirtualMachineModel> vVirtualMachineModels;

class Server : ServerModel{
    public:
        int globalServerId;         //输出时候的serverId
        int id;			            //本地服务器编号
        Node nodeA, nodeB;          //节点A，B

        Server(string type, int core, int memory, int deviceCost, int dailyCost, int id): ServerModel(type, core, memory, deviceCost, dailyCost) {
            this->id = id;
        }

        Server(){}

        Server(string type, int id){
#ifdef DEBUG
            assert(mServerModels.find(type) != mServerModels.end()); // debug：确保type存在
#endif
            ServerModel model = mServerModels[type];
            this->type = type;
            this->id = id;
            this->core = model.core;
            this->memory = model.memory;
            this->deviceCost = model.deviceCost;
            this->dailyCost = model.dailyCost;

            // 节点初始化
            nodeA.coreUsed = 0;
            nodeA.memoryUsed = 0;
            nodeB.coreUsed = 0;
            nodeB.memoryUsed = 0;

            nodeA.coreRem = model.core / 2;
            nodeA.memoryRem = model.memory / 2;
            nodeB.coreRem = model.core / 2;
            nodeB.memoryRem = model.memory / 2;
        }

        string getType(){
            return this->type;
        }
        int getCore(){
            return this->core;
        }

        int getMemory(){
            return this->memory;
        }

        int getDeviceCost(){
            return this->deviceCost;
        }

        int getDailyCost(){
            return this->dailyCost;
        }

        string tostring(){
            return "Server: {id: " + to_string(this->id)
                + ", type: " + this->type
                + ", " + nodeA.tostring()
                + ", " + nodeB.tostring()
                + ", core: " + to_string(this->core)
                + ", memory: " + to_string(this->memory)
                + ", deviceCost: " + to_string(this->deviceCost)
                + ", dailyCost: " + to_string(this->dailyCost)
                + "}";
        }
};

class VirtualMachine : VirtualMachineModel{
    public:
        int id;
        VirtualMachine(){}
        VirtualMachine(string type, int id){
#ifdef DEBUG
            assert(mVirtualMachineModels.find(type) != mVirtualMachineModels.end());
#endif
            VirtualMachineModel vmd = mVirtualMachineModels[type];
            this->type = type;
            this->core = vmd.core;
            this->memory = vmd.memory;
            this->single = vmd.single;
            this->id = id;
        }

        string getType(){
            return this->type;
        }

        int getCore(){
            return this->core;
        }
        int getMemory(){
            return this->memory;
        }

        string tostring(){
            return "VirtualMachine: {id: " + to_string(this->id)
                + ", type: " + this->type
                + ", core: " + to_string(this->core)
                + ", memory: " + to_string(this->memory)
                + ", single: " + (this->single ? "True" : "False")
                + "}";
        }
};

class OP{
    public:
        int opType;
        int id;
        string machineType;
        OP(){}
};

struct cmp1{
    bool operator()(ServerModel &a, ServerModel &b){
        return abs(a.core - a.memory) > abs(b.core - b.memory);
    }
};

struct cmp2{
    bool operator()(ServerModel &a, ServerModel &b){
        return a.core + a.memory < b.core + b.memory;
    }
};

//***************************END DEFINITION**********************************


//***************************FUNCTION DEFINITION**********************************

int canPut(Server server, VirtualMachineModel vmd);

float leftProportion(Server server);

pair<int,int> selectServer(VirtualMachineModel vmd);

pair<int,int> makePurchase(VirtualMachineModel vmd, int today, int T);

string makeDeploymentOutput(int serverId, int node);

void updateResource(Server &server, int node, int vmcore, int vmmemory, bool isDel);

void putVirtualMachineToServer(VirtualMachineModel vmd, int vmid, pair<int,int> serverAndNode);

void allocateServer(OP addop, int today, int T);

void releaseRes(OP delop);

void doOutput();

void migrate();

void solve(int day, int T);

void initializeOperationVector();

int getNextServerId();

int getNextGlobalServerId();

pair<double,int> selectServerCal(Server &currServer, VirtualMachineModel &vmd,int choice);

//***************************END FUNCTION DEFINITION**********************************


//***************************INPUT********************************

vector<OP> vOperations;

void readServerModel(){
    ServerModel sm;

    stringstream ss;
    string line;
    getline(cin, line);
    ss << line;

    // 解析每行
    char lp, comma, rp;
    ss >> lp;
    string type;
    int core, memory, deviceCost, dailyCost;
    ss >> type;
    type.pop_back();
    sm.type = type;
    ss >> sm.core >> comma;
    ss >> sm.memory >> comma;
    ss >> sm.deviceCost >> comma;
    ss >> sm.dailyCost;
    ss >> rp;

#ifdef DEBUG
    //cout << sm.tostring() << endl;
#endif

    // 分别添加到map和vector
    mServerModels[sm.type] = sm;
    vServerModels.push_back(sm);
}

void readVirtualMachineModel(){
    VirtualMachineModel vmd;

    stringstream ss;
    string line;
    getline(cin, line);

    ss << line;
    char lp, comma, rp;

    ss >> lp;
    string type;
    ss >> type;
    type.pop_back();
    vmd.type = type;
    ss >> vmd.core >> comma;
    ss >> vmd.memory >> comma;
    ss >> vmd.single;
    vmd.single = !vmd.single;
    ss >> rp;

#ifdef DEBUG
    //cout << vmd.tostring() << endl;
#endif

    vVirtualMachineModels.push_back(vmd);
    mVirtualMachineModels[vmd.type] = vmd;
}

void readOperation(){
    stringstream ss;

    string line;
    getline(cin, line); ss << line;

    string type;
    char lp, comma, rp;

    ss >> lp;
    ss >> type;

    type.pop_back();
    OP op;
    if(type == "add"){
        op.opType = ADD;
        string vmtype;
        ss >> vmtype;
        vmtype.pop_back();
        op.machineType = vmtype;
        ss >> op.id;
#ifdef DEBUG
        addcount += 1;
#endif
    }else{
        op.opType = DEL;
        ss >> op.id;
#ifdef DEBUG
        delcount += 1;
#endif
    }

    ss >> rp;
    vOperations.push_back(op);
}

//***************************END INPUT********************************



// ******************************ALGO*********************************

map<int, int> mLocalServerIdGlobalServerId; //serverId到输出id的映射
map<int, pair<int, int> > mVirtualMachineServer; // 虚拟机编号到服务器编号以及节点的映射
vector<Server> vServers;        //所有已经创建的服务器
map<int, Server> mSeverIdServer;     //服务器编号到服务器的映射
map<int, int> mServerIdVectorPos;    //服务器编号到vector下标的映射
map<int, VirtualMachine> mVmidVirtualMachine;       //虚拟机编号到虚拟机到映射

//输出数据
vector<string> vPurchases;
vector<string> vMigrations;
vector<pair<int, int>> vDeployments;

vector<int> vPurchasedServers;

void initializeOperationVector(){
    vOperations.clear();
    vOperations.resize(0);
    vPurchases.clear();
    vPurchases.resize(0);
    vMigrations.clear();
    vMigrations.resize(0);
    vDeployments.clear();
    vDeployments.resize(0);
    vPurchasedServers.clear();
    vPurchasedServers.resize(0);
}

// purchase时获取服务器编号
int getNextServerId(){
    return localServerNum++;
}

int getNextGlobalServerId(){
    return globalServerNumber++;
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

float leftProportion(Server server){
    int totalCoreRem = server.nodeA.coreRem + server.nodeB.coreRem;
    int totalMemRem = server.nodeA.memoryRem + server.nodeB.memoryRem;
    return (float)totalCoreRem / server.getCore() + (float)totalMemRem / server.getMemory();
}

double similarity(VirtualMachineModel vmd, Server server){
    return abs((double)vmd.core / vmd.memory - (double)server.getMemory() / server.getMemory());
}

//计算选择值
double selectServerFun(Server &currServer,int occupyACore,int occupyAMem,int occupyBCore,int occupyBMem){
    int leftACore = currServer.nodeA.coreRem - occupyACore, leftBCore = currServer.nodeB.coreRem - occupyBCore;
    int leftAMem = currServer.nodeA.memoryRem - occupyAMem,leftBMem = currServer.nodeB.memoryRem - occupyBMem;
    int totalCore = currServer.getCore() >> 1;
    int totalMem = currServer.getMemory() >> 1;
    //计算碎片参数
    double chipF = (1.0 * MAXSOURCE - leftACore - leftAMem - leftBCore - leftBMem) / MAXSOURCE;
    //cout<<"chipF:"<<chipF<<" "<<MAXSOURCE<<" "<<leftACore<<" "<<leftAMem<<" "<<leftBCore<<" "<<leftBMem<<endl;
    //计算平衡参数
    double balanceF =(abs(1 - abs(1 - (1.0 * leftACore / totalCore ) / (1.0 * leftAMem / totalMem))) + abs(1 - abs(1 - (1.0 * leftBCore / totalCore) / (1.0 * leftBMem / totalMem)))) / 2;
    //cout<<"balanceF:"<<balanceF<<endl;
    //返回加权值
    return SelectWeight * chipF + (1 - SelectWeight) * balanceF;
}

pair<double,int> selectServerCal(Server &currServer, VirtualMachineModel &vmd,int choice){
    double res = 0;
    int choseNode = 0;
    double cal;
    if(vmd.single){
        if(choice == A || choice == BOTH){
            res = selectServerFun(currServer,vmd.core,vmd.memory,0,0);
            choseNode = A;
        }
        if(choice == B || choice == BOTH){
            cal = selectServerFun(currServer,0,0,vmd.core,vmd.memory);
            if(cal > res){
                res = cal;
                choseNode = B;
            }
        }
    }else{
        res = selectServerFun(currServer,vmd.core >> 1,vmd.memory >> 1,vmd.core >> 1,vmd.memory >> 1);
        choseNode = BOTH;
    }
    return make_pair(res,choseNode);
}

pair<int,int> selectServer(VirtualMachineModel vmd){
    double maxn = 0; //函数获得的最大值
    pair<double,int> res;//返回值
    int choice;
    int targetServerIdx = -1;
    for(int i = 0; i < vServers.size(); i++){
        Server currServer = vServers[i];
        choice = canPut(currServer,vmd);
        if(choice > 0){
            res = selectServerCal(currServer,vmd,choice);
            if(res.first > maxn)  maxn = res.first,targetServerIdx = i,choice = res.second;
        }
    }
    return targetServerIdx == -1 ? make_pair(-1,-1) : make_pair(vServers[targetServerIdx].id,choice);
}

bool canBuy(ServerModel sm, int neededCore, int neededMem){
    return sm.core >= neededCore && sm.memory >= neededMem;
}

bool isHugeAndBalanced(int core, int memory, int maxcore, int maxmem){
    if(core * 5 < maxcore && memory * 5 < maxmem){
        return false;
    }
    //所需资源不均衡
    //if((double)core / memory > 5. || (double) memory / core > 5.){
    //    return true;
    //}
    return false;
}

double maxmin(double maxv, double minv, double val){
    if(maxv == 0){
        return 1;
    }
    return (val - minv) / maxv;
}

//返回server所在vServers的下标
int buyServer(int day, int T, int neededCore, int neededMem){
    vector<int> costs;
    vector<int> balances;
    vector<int> dailyCosts;
    vector<int> deviceCosts;

    for(int i = 0; i < vServerModels.size(); i++){
        ServerModel server = vServerModels[i];
        balances.push_back(abs(server.core - server.memory));
        costs.push_back(server.dailyCost * (T - day) + server.deviceCost);
  //      dailyCosts.push_back(server.dailyCost * (T - day));
  //      deviceCosts.push_back(server.deviceCost);
    }
  //int maxdac = *max_element(dailyCosts.begin(), dailyCosts.end());
  //int mindac = *min_element(dailyCosts.begin(), dailyCosts.end());

  //int maxdec = *max_element(deviceCosts.begin(), deviceCosts.end());
  //int mindec = *min_element(deviceCosts.begin(), deviceCosts.end());
  //
  //for(int i = 0; i < vServerModels.size(); i++){
  //    double mmdaily = maxmin(maxdac, mindac, dailyCosts[i]);
  //    double mmdevice = maxmin(maxdec, mindec, deviceCosts[i]);
  //    costs.push_back((mmdaily * 0.3 + mmdevice * 0.7));
  //}
    int maxc = *max_element(costs.begin(), costs.end());
    int minc = *min_element(costs.begin(), costs.end());
    int maxb = *max_element(balances.begin(), balances.end());
    int minb = *min_element(balances.begin(), balances.end());

    vector<double> priority(vServerModels.size());


    for(int i = 0; i < priority.size(); i++){
        priority[i] = maxmin(maxc, minc, costs[i]) * 0.3 + (1 - maxmin(maxb, minb, balances[i])) * 0.7;
    }

    int k = -1;
    for(int i = 0; i < priority.size(); i++){
        if(canBuy(vServerModels[i], neededCore, neededMem) && (k == -1 || priority[k] < priority[i])){
            k = i;
        }
    }
#ifdef DEBUG
//    assert(k != -1);
//    for(int i = 0; i < priority.size(); i++){
//        cout << priority[i] << endl;
//    }
//
//    cout << maxc << " " << minc << " " << maxb << " " << minb << endl;
    cout << k << " " << priority[k] << " " << day << " " << vServerModels[k].tostring() << endl;
#endif
    return k;
}

pair<int,int> makePurchase(VirtualMachineModel vmd, int today, int T){
    int newServerId = getNextServerId();

    //当前虚拟机需要的core和内存大小
    int neededCore = vmd.core, neededMem = vmd.memory;
    if(vmd.single){
        neededCore *= 2;
        neededMem *= 2;
    }

    int k = -1;
//    if(today <= 1 * T / 100 && !isHugeAndBalanced(neededCore, neededMem, MAX_VM_CORE, MAX_VM_MEMORY)){
//        k = buyServer(today, T, neededCore, neededMem);
//    }else{
        //找最合适的那个
        for(int i = 0; i < vServerModels.size(); i++){
            ServerModel sm = vServerModels[i];
            if(canBuy(sm, neededCore, neededMem) && (k == -1 || vServerModels[k].deviceCost > sm.deviceCost)){
                k = i;
            }
        }
//    }

#ifdef DEBUG
    assert(k != -1);
#endif

    ServerModel sm = vServerModels[k];
    Server purchasedServer(sm.type, newServerId);

    vPurchasedServers.push_back(newServerId);

    vServers.push_back(purchasedServer);
    mServerIdVectorPos[newServerId] = vServers.size() - 1;
    mSeverIdServer[newServerId] = purchasedServer;

    if(vmd.single) return make_pair(newServerId,A);
    else return make_pair(newServerId,BOTH);
}

string makeDeploymentOutput(int serverId, int node){
    string output;
    if(node == A){
        output = "(" + to_string(mLocalServerIdGlobalServerId[serverId]) + ", A" + ")";
    }else if(node == B){
        output = "(" + to_string(mLocalServerIdGlobalServerId[serverId]) + ", B" + ")";
    }else{
        output = "(" + to_string(mLocalServerIdGlobalServerId[serverId]) + ")";
    }
    return output;
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

void putVirtualMachineToServer(VirtualMachineModel vmd, int vmid, pair<int,int> serverAndNode){
    //找到对应的server
    int serverId = serverAndNode.first;
    int node = serverAndNode.second;
    Server server = mSeverIdServer[serverId];
#ifdef DEBUG
    assert(canPut(server, vmd));
#endif
    updateResource(server, node, vmd.core, vmd.memory, false);
    mVirtualMachineServer[vmid] = {serverId, node};
    mSeverIdServer[serverId] = server;
    vServers[mServerIdVectorPos[serverId]] = server;
    vDeployments.push_back({serverId, node});
}


void allocateServer(OP addop, int today, int T){
    int vmid = addop.id;
    // 需要新增虚拟机的型号
    VirtualMachineModel vmd = mVirtualMachineModels[addop.machineType];
    //新增的虚拟机实例
    VirtualMachine vm(addop.machineType, vmid);
    mVmidVirtualMachine[vmid] = vm;
    pair<int,int> serverAndNode = selectServer(vmd);
    if(serverAndNode.first == -1){       // 没有合适的服务器，需要新购买服务器
        serverAndNode = makePurchase(vmd, today, T);
    }
    putVirtualMachineToServer(vmd, vmid, serverAndNode);
}



void releaseRes(OP delop){
    int vmid = delop.id;
    VirtualMachine vm = mVmidVirtualMachine[vmid];

    pair<int, int> t = mVirtualMachineServer[vmid];
    int serverId = t.first, loc = t.second;
    Server server = mSeverIdServer[serverId];

    updateResource(server, loc, vm.getCore(), vm.getMemory(), true);
    vServers[mServerIdVectorPos[serverId]] = server;
    mSeverIdServer[serverId] = server;

    mVmidVirtualMachine.erase(vmid);
    mVirtualMachineServer.erase(vmid);
}

void doOutput(){
    // 输出purchase
    map<string, vector<int>> cnt;
    for(int serverId : vPurchasedServers){
        string type = mSeverIdServer[serverId].getType();
        if(cnt.find(type) == cnt.end()){
            cnt[type] = vector<int>();
        }
        cnt[type].push_back(serverId);
    }
    cout << "(purchase, " << to_string(cnt.size()) << ")" << endl;
    for(auto &p : cnt){
        for(int serverid : p.second){
            mLocalServerIdGlobalServerId[serverid] = getNextGlobalServerId();
        }
        string curr = "(" + p.first + ", " + to_string(p.second.size()) + ")";
        cout << curr << endl;
    }

    //暂时没有migration
    cout << "(migration, 0)" << endl;

    //输出deployment
    for(auto &p : vDeployments){
        cout << makeDeploymentOutput(p.first, p.second) << endl;
    }
}

void migrate(){

}


void solve(int today, int T){
    // 顺序遍历每次操作
    for(auto & op : vOperations){
        switch(op.opType){
            case ADD:
                allocateServer(op, today, T);
                break;
            case DEL:
                releaseRes(op);
                break;
        }
    }
    migrate();
#ifndef DEBUG
    doOutput();
#endif
}

//测试用，判断server的资源是否合法
bool checkServer(Server server){
    if(server.nodeA.coreRem < 0 || server.nodeB.coreRem < 0){
        return false;
    }
    if(server.nodeA.memoryRem < 0 || server.nodeB.memoryRem < 0){
        return false;
    }
    if(server.nodeA.coreUsed + server.nodeB.coreUsed + server.nodeA.coreRem + server.nodeB.coreRem != server.getCore()){
        return false;
    }
    if(server.nodeA.memoryUsed + server.nodeA.memoryRem + server.nodeB.memoryRem + server.nodeB.memoryUsed != server.getMemory()){
        return false;
    }
    return true;
}
// 检查服务器是否到了使用度
bool checkUsage(Server server){
    float minimumUseage = .7;
    int usedCore = server.nodeA.coreUsed + server.nodeB.coreUsed;
    int usedMem = server.nodeA.memoryUsed + server.nodeB.memoryUsed;
    if((float)usedCore / server.getCore() < minimumUseage){
        return false;
    }
    if((float)usedMem / server.getMemory() < minimumUseage){
        return false;
    }
    return true;
}
// ***********************END ALGO*************************
//
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int N, M, T;
    cin >> N;
    cin.ignore(); //忽略回车
    for(int i = 0; i < N; i++){
        readServerModel();
    }
    for(int i = 0; i < vServerModels.size(); i++){
        MAX_SERVERMODEL_CORE = max(MAX_SERVERMODEL_CORE, vServerModels[i].core);
        MAX_SERVERMODEL_MEMORY = max(MAX_SERVERMODEL_MEMORY, vServerModels[i].memory);
    }

    cin >> M;
    cin.ignore(); //忽略回车
    for(int i = 0; i < M; i++){
        readVirtualMachineModel();
    }
    for(int i = 0; i < vVirtualMachineModels.size(); i++){
        MAX_VM_CORE = max(MAX_VM_CORE,vVirtualMachineModels[i].core);
        MAX_VM_MEMORY = max(MAX_VM_CORE,vVirtualMachineModels[i].memory);
    }
    MAXSOURCE = MAX_SERVERMODEL_CORE + MAX_SERVERMODEL_MEMORY ;
    cin >> T;

    for(int i = 1; i <= T; i++){
        int R;
        cin >> R;
        cin.ignore(); //忽略回车

        initializeOperationVector();
        while(R-- > 0){
            readOperation();
        }
        solve(i, T);
    }

#ifdef DEBUG
    cout << "N: " << vServerModels.size() << endl;
    cout << "M: " << vVirtualMachineModels.size() << endl;
    cout << "add count: " << addcount << endl;
    cout << "del count: " << delcount << endl;
    int unfilledSize = 0;
    for(auto &s : vServers){
        if(!checkUsage(s)){
            cout << s.tostring() << endl;
            unfilledSize += 1;
        }
    }
    cout << vServers.size() << " " << unfilledSize << endl;

//    for(auto &s : vServers){
//        int sid = s.id;
//        for(auto p : mVirtualMachineServer){
//            if(p.second.first == sid){
//                cout << p.first << " ";
//            }
//        }
//        cout << endl;
//        cout << s.tostring() << endl;
//    }

//    for(auto &s : vServers){
//        if(!checkServer(s)){
//            cout << s.tostring() << endl;
//        }
//    }

//    for(auto &s : vServerModels){
//        cout << s.tostring() << endl;
//    }
//    for(auto &s : vServers){
//        cout << s.tostring() << endl;
//    }
#endif
    return 0;
}
