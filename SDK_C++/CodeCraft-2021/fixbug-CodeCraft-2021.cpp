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

#include <chrono>
//最大CPU + RAM

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
int addcount = 0;
int delcount = 0;
#endif

// 用来生成服务器编号
int localServerNum = 0;
double timeLeft = 0.0;
int globalServerNumber = 0;

long long MAX_TIME;

int MAX_VM_CORE = 0;
int MAX_VM_MEMORY = 0;
int MAX_SERVERMODEL_CORE = 0;
int MAX_SERVERMODEL_MEMORY = 0;
int MEAN_VM_CORE = 0;
int MEAN_VM_MEMORY = 0;
int VM_AMOUNT = 0;
double MAX_SIMI = -1;
double MIN_SIMI = 0x3f3f3f3f;

int MAXSOURCE = 0;

// 权重值
//购买权重（和为1）
double buy_PriceWeight = 0.9;//按价格购买权重
double buy_BalanceWeight = 0.0;//购买平衡权重
double buy_leftSpaceWeight = 0.0;//剩余空间
double buy_PriceWithCapacituWeight = 0.1;//性价比
double buy_SmWeight = 0.0;//相似性购买
// double DayWeight = 0.8;

//放置权值(和为1)
double put_SelectWeight = 0.9;//碎片选择权重
double put_NodeBlanceWeight = 0.0;//节点平衡
double put_SimWeight = 0.1;//相似放置
double put_BalanceWeight = 0.0;//平衡参数

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
unordered_map<string, ServerModel> mServerModels; // 所有服务器型号； key: type -> value: 服务器型号
unordered_map<string, VirtualMachineModel> mVirtualMachineModels; // 所有虚拟机型号； key: type -> value: 虚拟机型号

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

        int getSingle(){
            return this->single;
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

struct cmp3{
    bool operator()(pair<double, int> &a, pair<double, int> &b){
        return a.first > b.first;
    }
};

struct cmp4{
    bool operator()(pair<double, int> &a, pair<double, int> &b){
        return a.first < b.first;
    }
};

//***************************END DEFINITION**********************************


//***************************FUNCTION DEFINITION**********************************

int canPut(Server server, VirtualMachineModel vmd);

int canPut(Server server, VirtualMachine vm);

float leftProportion(Server server);

pair<int,int> selectServer(VirtualMachineModel vmd);

pair<int,int> makePurchase(VirtualMachineModel vmd, int today, int T);

string makeDeploymentOutput(int serverId, int node);

void updateResource(Server &server, int node, int vmcore, int vmmemory, bool isDel);

void putVirtualMachineToServer(VirtualMachineModel vmd, int vmid, pair<int,int> serverAndNode);

void migrateVirtualMachineToServer(int vmid, pair<int, int> serverAndNode);

void allocateServer(OP addop, int today, int T);

void releaseRes(OP delop);

void doOutput();

void migrate();

void solve(int day, int T);

void initializeOperationVector();

int getNextServerId();

int getNextGlobalServerId();

double maxmin(double maxv, double minv, double val);

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

unordered_map<int, int> mLocalServerIdGlobalServerId; //serverId到输出id的映射
unordered_map<int, pair<int, int>> mVirtualMachineServer; // 虚拟机编号到服务器编号以及节点的映射
unordered_map<int, VirtualMachine> mVmidVirtualMachine;       //虚拟机编号到虚拟机到映射
unordered_map<int, set<int>> mServerVirtualMachine;            // 服务器到虚拟机到映射
unordered_map<int, set<pair<int, int>>> mServerVirtualMachineEn; 

unordered_map<int, VirtualMachineModel> mVmidVirtualMachineModel;

vector<Server> vServers;        //所有已经创建的服务器
unordered_map<int, Server> mSeverIdServer;     //服务器编号到服务器的映射
unordered_map<int, int> mServerIdVectorPos;    //服务器编号到vector下标的映射

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

//void updateBuyFactor(int today, int T){
//    if(today * 3 < T){
//        buy_PriceWeight = 0.1;//按价格购买权重
//        buy_BalanceWeight = 0.1;//购买平衡权重
//        buy_leftSpaceWeight = 0.3;//剩余空间
//        buy_PriceWithCapacituWeight = .4;//性价比
//        buy_SmWeight = 0.1;//相似性购买
//    }else if(today * 3 < 2 * T){
//        buy_PriceWeight = 0.4;
//        buy_BalanceWeight = 0.2;
//        buy_leftSpaceWeight = 0.1;
//        buy_PriceWithCapacituWeight = .3;
//        buy_SmWeight = 0.2;
//    }else{
//        buy_PriceWeight = 0.6;
//        buy_BalanceWeight = 0.0;
//        buy_leftSpaceWeight = 0.0;
//        buy_PriceWithCapacituWeight = 0.4;
//        buy_SmWeight = 0.0;
//    }
//}

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

float leftProportion(Server server){
    int totalCoreRem = server.nodeA.coreRem + server.nodeB.coreRem;
    int totalMemRem = server.nodeA.memoryRem + server.nodeB.memoryRem;
    return (float)totalCoreRem / server.getCore() + (float)totalMemRem / server.getMemory();
}

double similarity(VirtualMachineModel vmd, Server server){
    return abs((double)vmd.core / vmd.memory - (double)server.getCore() / server.getMemory());
}

double similarity(VirtualMachineModel vmd, ServerModel server){
    return abs((double)vmd.core / vmd.memory - (double)server.core / server.memory);
}

// 没用
double getFragmentRatio(Server &currServer, VirtualMachineModel vmd, int occupyACore, int occupyAMem, int occupyBCore, int occupyBMem){
    int leftACore = currServer.nodeA.coreRem - occupyACore, leftBCore = currServer.nodeB.coreRem - occupyBCore;
    int leftAMem = currServer.nodeA.memoryRem - occupyAMem,leftBMem = currServer.nodeB.memoryRem - occupyBMem;
    int totalCore = currServer.getCore() >> 1;
    int totalMem = currServer.getMemory() >> 1;
    //计算碎片参数
    double chipF;
    if(occupyACore == 0){
        chipF = (1.0 * MAXSOURCE - leftBCore - leftBMem) / MAXSOURCE;
    } else if(occupyBCore == 0){
        chipF = (1.0 * MAXSOURCE - leftACore - leftAMem) / MAXSOURCE;
    } else{
        chipF = (1.0 * MAXSOURCE - leftACore - leftAMem - leftBCore - leftBMem) / MAXSOURCE;
    }
    return chipF;
}

//计算选择值
double selectServerFun(Server &currServer,VirtualMachineModel vmd, int occupyACore,int occupyAMem,int occupyBCore,int occupyBMem){
    int leftACore = currServer.nodeA.coreRem - occupyACore, leftBCore = currServer.nodeB.coreRem - occupyBCore;
    int leftAMem = currServer.nodeA.memoryRem - occupyAMem, leftBMem = currServer.nodeB.memoryRem - occupyBMem;
    int totalCore = currServer.getCore() / 2;
    int totalMem = currServer.getMemory() / 2;
    //计算碎片参数
    double chipF;// = getFragmentRatio(currServer, vmd, occupyACore, occupyAMem, occupyBCore, occupyBMem);
    double total = currServer.getCore() + currServer.getMemory();
    double Sim = 0.;
    if(occupyACore == 0){
        chipF = ((1.0 * totalCore - leftBCore) / currServer.getCore() + (1.0 * totalMem - leftBMem) / currServer.getMemory());
    } else if(occupyBCore == 0){
        chipF = ((1.0 * totalCore - leftACore) / currServer.getCore() / (total / 2) + (1.0 * totalMem - leftAMem) / currServer.getMemory()) / (total / 2);
    } else{
        chipF = (1.0 * total- leftACore - leftAMem - leftBCore - leftBMem) / total;
    }
    //Sim = 1 - ((double)abs(currServer.getCore() - occupyBCore - occupyACore) + (double)abs(currServer.getMemory() - occupyBMem - occupyAMem)) / MAXSOURCE;
    Sim = (similarity(vmd, currServer) - MIN_SIMI) / (MAX_SIMI - MIN_SIMI);
    //cout<<"chipF:"<<chipF<<" "<<MAXSOURCE<<" "<<leftACore<<" "<<leftAMem<<" "<<leftBCore<<" "<<leftBMem<<endl;
    //计算平衡参数
    double balanceF =(abs(1 - abs(1.0 * leftACore / totalCore  - 1.0 * leftAMem / totalMem)) + abs(1 - abs(1.0 * leftBCore / totalCore - 1.0 * leftBMem / totalMem))) / 2;

    double balanceNode = 1 - ((double)abs(leftACore-leftBCore) / totalCore + abs(leftAMem - leftBMem)/totalMem) / total;
    //cout<<"balanceF:"<<balanceF<<endl;
    //返回加权值
    return put_SelectWeight * chipF + put_SimWeight * Sim + balanceF * put_BalanceWeight + put_NodeBlanceWeight * balanceNode;
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
        res = selectServerFun(currServer,vmd, vmd.core / 2,vmd.memory / 2,vmd.core / 2,vmd.memory / 2);
        choseNode = BOTH;
    }
    return make_pair(res,choseNode);
}

pair<int,int> selectServer(VirtualMachineModel vmd){
    double maxn = 0; //函数获得的最大值
    pair<double,int> res;//返回值
    int finalChoice;
    int targetServerIdx = -1;
    for(int i = 0; i < vServers.size(); i++){
        Server currServer = vServers[i];
        int choice = canPut(currServer,vmd);
        if(choice > 0){
            res = selectServerCal(currServer,vmd,choice);
            if(res.first > maxn)  maxn = res.first,targetServerIdx = i,finalChoice = res.second;
        }
    }
    return targetServerIdx == -1 ? make_pair(-1,-1) : make_pair(vServers[targetServerIdx].id,finalChoice);
}

bool canBuy(ServerModel sm, int neededCore, int neededMem){
    return sm.core >= neededCore && sm.memory >= neededMem;
}

double maxmin(double maxv, double minv, double val){
    if(maxv - minv <= 1e-5){
        return 1;
    }
    return (val - minv) / maxv - minv;
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

    // [12, 15]
    //if(12 * today >= T * 20 && 15 * today <= T * 20){
    //    PriceWeight = .7;
    //    leftSpaceWeight = 0.0;
    //}else{
    //    PriceWeight = 1.0;
    //    leftSpaceWeight = 0.0;
    //}

    double maxP = -1,minP = 0x3f3f3f3f;
    double maxC = -1,minC = 0x3f3f3f3f;
    double maxSm = -1, minSm = 0x3f3f3f3f;
    int leftCore,leftMem,devicePrice,dayPrice,totalCore,totalMem,leftNodeCore,leftNodeMem,totalNodeCore,totalNodeMem;
    //价格
    vector<double> PriceF(vServerModels.size(),-1);
    //性价比
    vector<double> PriceC(vServerModels.size(),-1);
    vector<double> Sm(vServerModels.size(),-1);

    for(int i=0; i<vServerModels.size(); i++){
        ServerModel p = vServerModels[i];
        if(canBuy(p,neededCore, neededMem)){
            devicePrice = p.deviceCost;
            dayPrice = p.dailyCost * (T - today);
            totalCore = p.core;
            totalMem = p.memory;
            PriceF[i] = devicePrice + dayPrice;
            PriceC[i] = ((devicePrice + dayPrice) / totalMem) + (devicePrice + dayPrice) / totalCore;
            //*((1.0*devicePrice / p.core) / (1.0*devicePrice / p.memory))
            //Sm[i] = abs(totalCore - MEAN_VM_CORE) + abs(totalMem - MEAN_VM_MEMORY);
            Sm[i] = similarity(vmd, p);
            // Sm[i] = abs(totalCore - vmd.core) / totalCore + abs(totalMem - vmd.memory) / totalMem;
            maxP = max(maxP,PriceF[i]), minP = min(minP,PriceF[i]);
            maxC = max(maxC,PriceC[i]), minC = min(minC,PriceC[i]);
            maxSm = max(maxSm,Sm[i]), minSm = min(minSm,Sm[i]);
        }
    }
    int k = -1;
    double balanceF,spaceF,newPriceF,choseF, newPriceC, newSm;
    double maxn = -1;
    for(int i = 0; i < vServerModels.size(); i++){
        ServerModel p = vServerModels[i];
        if(canBuy(p,neededCore,neededMem)){
            leftCore = p.core - vmd.core;
            leftMem = p.memory - vmd.memory;
            totalCore = p.core;
            totalMem = p.memory;
            totalNodeCore = totalCore >> 1;
            totalNodeMem = totalMem >> 1;
            if(vmd.single){
                leftNodeCore = (totalCore >> 1) - vmd.core;
                leftNodeMem = (totalMem >> 1) - vmd.memory;
            }else{
                leftNodeCore = (totalCore >> 1) - (vmd.core >> 1);
                leftNodeMem = (totalMem >> 1) - (vmd.memory >> 1);
            }
            balanceF = 1 - abs(1 - (1.0 * leftNodeCore / totalNodeCore) / (1.0 * leftNodeMem / totalNodeMem));
            spaceF = (1.0 * leftCore + leftMem ) / MAXSOURCE;
            newPriceF = 1 - (PriceF[i] - minP) / (maxP - minP);
            newPriceC = 1 - (PriceC[i] - minC) / (maxC - minC);
            newSm = 1 - (Sm[i] - minSm) / (maxSm - minSm);
            // choseF = (PriceWeight-PriceWithCapacituWeight-SmWeight) * newPriceF + (1 - PriceWeight - leftSpaceWeight) * balanceF + leftSpaceWeight * spaceF + PriceWithCapacituWeight*newPriceC + newSm*SmWeight;
            choseF = buy_PriceWeight * newPriceF + buy_BalanceWeight * balanceF + buy_leftSpaceWeight * spaceF + buy_PriceWithCapacituWeight * newPriceC + newSm * buy_SmWeight;
            // cout<<choseF<<endl;
            if(maxn < choseF) k = i,maxn = choseF;
        }
    }

#ifdef DEBUG
    assert(k != -1);
#endif

    ServerModel sm = vServerModels[k];
    Server purchasedServer(sm.type, newServerId);

    //cout << purchasedServer.getCore() << " " << purchasedServer.getMemory() << endl;

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
    if(mServerVirtualMachine.find(serverId) == mServerVirtualMachine.end()){
        mServerVirtualMachine[serverId] = set<int>();
    }
    if(mServerVirtualMachineEn.find(serverId) == mServerVirtualMachineEn.end()){
        mServerVirtualMachineEn[serverId] = set<pair<int, int>>();
    }
#ifdef DEBUG
    assert(vmid != 0);
#endif

    mVirtualMachineServer[vmid] = {serverId, node};
    mServerVirtualMachine[serverId].insert(vmid);
    mServerVirtualMachineEn[serverId].insert(make_pair(vmd.core + vmd.memory, vmid));
    mSeverIdServer[serverId] = server;
    vServers[mServerIdVectorPos[serverId]] = server;
    vDeployments.push_back({serverId, node});
}


void migrateVirtualMachineToServer(int vmid, pair<int, int> serverAndNode){
    Server fromServer = mSeverIdServer[mVirtualMachineServer[vmid].first];
    int fromnode = mVirtualMachineServer[vmid].second;

    Server toServer = mSeverIdServer[serverAndNode.first];
    int tonode = serverAndNode.second;

    updateResource(fromServer, fromnode, mVmidVirtualMachine[vmid].getCore(), mVmidVirtualMachine[vmid].getMemory(), true);

    updateResource(toServer, tonode, mVmidVirtualMachine[vmid].getCore(), mVmidVirtualMachine[vmid].getMemory(), false);

    mSeverIdServer[fromServer.id] = fromServer;
    vServers[mServerIdVectorPos[fromServer.id]] = fromServer;

    mSeverIdServer[toServer.id] = toServer;
    vServers[mServerIdVectorPos[toServer.id]] = toServer;

    mVirtualMachineServer[vmid] = serverAndNode;

    mServerVirtualMachine[toServer.id].insert(vmid);
}

void _migrateVirtualMachineToServer(int vmid, pair<int, int> serverAndNode){
    Server fromServer = mSeverIdServer[mVirtualMachineServer[vmid].first];
    int fromnode = mVirtualMachineServer[vmid].second;

    Server toServer = mSeverIdServer[serverAndNode.first];
    int tonode = serverAndNode.second;

    updateResource(fromServer, fromnode, mVmidVirtualMachine[vmid].getCore(), mVmidVirtualMachine[vmid].getMemory(), true);

    updateResource(toServer, tonode, mVmidVirtualMachine[vmid].getCore(), mVmidVirtualMachine[vmid].getMemory(), false);

    mSeverIdServer[fromServer.id] = fromServer;
    vServers[mServerIdVectorPos[fromServer.id]] = fromServer;

    mSeverIdServer[toServer.id] = toServer;
    vServers[mServerIdVectorPos[toServer.id]] = toServer;

    mVirtualMachineServer[vmid] = serverAndNode;

    mServerVirtualMachineEn[toServer.id].insert({mVmidVirtualMachine[vmid].getCore() + mVmidVirtualMachine[vmid].getMemory(), vmid});
}


void allocateServer(OP addop, int today, int T){
    int vmid = addop.id;
    VM_AMOUNT += 1;
    // 需要新增虚拟机的型号
    VirtualMachineModel vmd = mVirtualMachineModels[addop.machineType];
    //新增的虚拟机实例
    pair<int,int> serverAndNode = selectServer(vmd);
    if(serverAndNode.first == -1){       // 没有合适的服务器，需要新购买服务器
        serverAndNode = makePurchase(vmd, today, T);
    }

    VirtualMachine vm(addop.machineType, vmid);
    mVmidVirtualMachine[vmid] = vm;

    mVmidVirtualMachineModel[vmid] = vmd;
    putVirtualMachineToServer(vmd, vmid, serverAndNode);
}


void releaseRes(OP delop){
    VM_AMOUNT -= 1;
    int vmid = delop.id;
    VirtualMachine vm = mVmidVirtualMachine[vmid];

    pair<int, int> t = mVirtualMachineServer[vmid];
    int serverId = t.first, loc = t.second;
    Server server = mSeverIdServer[serverId];

    updateResource(server, loc, vm.getCore(), vm.getMemory(), true);
    vServers[mServerIdVectorPos[serverId]] = server;
    mSeverIdServer[serverId] = server;

    mVmidVirtualMachine.erase(vmid);
    mVmidVirtualMachineModel.erase(vmid);
    mVirtualMachineServer.erase(vmid);

    mServerVirtualMachine[serverId].erase(vmid);
    mServerVirtualMachineEn[serverId].erase({vm.getCore() + vm.getMemory(), vmid});
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
#ifndef DEBUG
        cout << curr << endl;
#endif
    }

    //migration
    cout << "(" << "migration, " << vMigrations.size() << ")" << endl;
    for(auto &s : vMigrations){
#ifndef DEBUG
        cout << s << endl;
#endif
    }

    //输出deployment
    for(auto &p : vDeployments){
#ifndef DEBUG
        cout << makeDeploymentOutput(p.first, p.second) << endl;
#endif
    }
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

void slow_optimized_migrate(){
// 在这一轮总共可以迁移的次数
#ifdef DEBUG
    assert(mVmidVirtualMachine.size() == VM_AMOUNT);
#endif
    int totalOperation = 5 * VM_AMOUNT / 1000;
    //int totalOperation = 3 * mVmidVirtualMachine.size() / 1000;
    // int totalOperation = 2 * mVmidVirtualMachine.size() / 1000;

    // 存储所有server的chipF
    vector<double> fragments;
    // vServers的下标，用于排序
    vector<int> serverIdxs;
    int idx = 0;
    for(auto &server : vServers){
        double fragment = (1.0 * MAXSOURCE - server.nodeA.coreRem - server.nodeA.memoryRem - server.nodeB.coreRem - server.nodeB.memoryRem) / MAXSOURCE;
        fragments.push_back(fragment);
        serverIdxs.push_back(idx++);
    }
    // 通过chipF排序，将排序后下标小的服务器中虚拟机往下标大的服务器中放
    sort(serverIdxs.begin(), serverIdxs.end(), [&](int a, int b){
            return fragments[a] < fragments[b];
            });
    int end = serverIdxs.size() - 1;
    while(end > 0 && abs(fragments[serverIdxs[end]] - 1.) < 1e-3){
        end -= 1;
    }
    bool timeUp = false;
    std::chrono::time_point<chrono::high_resolution_clock> start = chrono::high_resolution_clock::now();
    for(int i = 0; i < end && totalOperation > 0 && !timeUp; i++){
        // fromServer: 当前需要移除虚拟机的服务器
        int fromServerIdx = serverIdxs[i];
        // 需要移除虚拟机的服务器在vServers中的下标
        int fromServerId = vServers[fromServerIdx].id;

        // 存储循环中被移动到了新的服务器的虚拟机的vmid
        vector<pair<int, int>> modified;

        int last = end;
        for(auto p : mServerVirtualMachineEn[fromServerId]){
            int vmid = p.second;
#ifdef DEBUG
            assert(mVmidVirtualMachine.find(vmid) != mVmidVirtualMachine.end());
#endif
            // 要被迁移的虚拟机
            VirtualMachine vm = mVmidVirtualMachine[vmid];

            //std::chrono::time_point<chrono::high_resolution_clock> now = chrono::high_resolution_clock::now();
             //从碎片小的服务器往下找，直到第一个可以放下这个虚拟机的服务器
            while(last > i && totalOperation > 0){
                //long long findDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - now).count();
                //if(findDuration > 1000){
                //    break;
                //}
                // toServer：被迁移虚拟机的去向
                //int toServerIdx = serverIdxs[j];
                int toServerIdx = serverIdxs[last];
                // toServer在vServers中的下标
                int toServerId = vServers[toServerIdx].id;

                // choice可以放在哪个结点
                int choice = canPut(vServers[toServerId], vm);
                if(choice > 0){
                    string output;
                    output += "(" + to_string(vmid) + ", " + to_string(mLocalServerIdGlobalServerId[toServerId]);
                    if(vm.getSingle()){
                        // 如果是单节点并且两个都能放，那么放在剩余资源多的结点上
                        if(choice == A){
                            output += ", A";
                        }else if(choice == B){
                            output += ", B";
                        }else if(compareNode(mSeverIdServer[toServerId].nodeA, mSeverIdServer[toServerId].nodeB, vm)){
                            output += ", A";
                            choice = A;
                        }else{
                            output += ", B";
                            choice = B;
                        }
                    }
                    output += ")";
                    vMigrations.push_back(output);
                    // 迁移到新的服务器上
                    _migrateVirtualMachineToServer(vmid, {toServerId, choice});
                    // 在for loop中修改集合中的元素会导致bug
                    modified.push_back({vm.getCore() + vm.getMemory(), vmid});
                    // 可迁移次数减1
                    totalOperation -= 1;
                    break;
                }else{
                    last -= 1;
                }
            }
            long long totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
            if(totalDuration > MAX_TIME){
                timeUp = true;
            }
            break;
        }
        // 更新数据结构
        for(auto &p : modified){
            mServerVirtualMachineEn[fromServerId].erase(p);
        }

    }
}

void optimized_migrate(){
// 在这一轮总共可以迁移的次数
#ifdef DEBUG
    assert(mVmidVirtualMachine.size() == VM_AMOUNT);
#endif
    int totalOperation = 5 * VM_AMOUNT / 1000;
    //int totalOperation = 3 * mVmidVirtualMachine.size() / 1000;
    // int totalOperation = 2 * mVmidVirtualMachine.size() / 1000;

    // 存储所有server的chipF
    vector<double> fragments;
    // vServers的下标，用于排序
    vector<int> serverIdxs;
    int idx = 0;
    for(auto &server : vServers){
        double fragment = (1.0 * MAXSOURCE - server.nodeA.coreRem - server.nodeA.memoryRem - server.nodeB.coreRem - server.nodeB.memoryRem) / MAXSOURCE;
        fragments.push_back(fragment);
        serverIdxs.push_back(idx++);
    }
    // 通过chipF排序，将排序后下标小的服务器中虚拟机往下标大的服务器中放
    sort(serverIdxs.begin(), serverIdxs.end(), [&](int a, int b){
            return fragments[a] < fragments[b];
            });
    sort(fragments.begin(),fragments.end());
    int end = serverIdxs.size() - 1;
    while(end > 0 && abs(fragments[serverIdxs[end]] - 1.) < 1e-3){
        end -= 1;
    }
    for(int i = 0; i < end && totalOperation > 0; i++){
        // fromServer: 当前需要移除虚拟机的服务器
        int fromServerIdx = serverIdxs[i];
        // 需要移除虚拟机的服务器在vServers中的下标
        int fromServerId = vServers[fromServerIdx].id;

        // 存储循环中被移动到了新的服务器的虚拟机的vmid
        vector<pair<int, int>> modified;

        int last = end;
        for(auto p : mServerVirtualMachineEn[fromServerId]){
            int vmid = p.second;
            VirtualMachine &vm = mVmidVirtualMachine[vmid];
            double chipF = (1.0 * MAXSOURCE - vm.getCore() - vm.getMemory()) / MAXSOURCE;
            last = upper_bound(fragments.begin(),fragments.end(),chipF) - fragments.begin();
            last --;
#ifdef DEBUG
            assert(mVmidVirtualMachine.find(vmid) != mVmidVirtualMachine.end());
#endif
            //甩头d::chrono::time_point<chrono::high_resolution_clock> now = chrono::high_resolution_clock::now();
             //从碎片小的服务器往下找，直到第一个可以放下这个虚拟机的服务器
            while(last > i && totalOperation > 0){
                //long long findDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - now).count();
                //if(findDuration > 1000){
                //    break;
                //}
                // toServer：被迁移虚拟机的去向
                //int toServerIdx = serverIdxs[j];
                int toServerIdx = serverIdxs[last];
                // toServer在vServers中的下标
                int toServerId = vServers[toServerIdx].id;

                // choice可以放在哪个结点
                int choice = canPut(vServers[toServerId], vm);
                if(choice > 0){
                    string output;
                    output += "(" + to_string(vmid) + ", " + to_string(mLocalServerIdGlobalServerId[toServerId]);
                    if(vm.getSingle()){
                        // 如果是单节点并且两个都能放，那么放在剩余资源多的结点上
                        if(choice == A){
                            output += ", A";
                        }else if(choice == B){
                            output += ", B";
                        }else if(compareNode(mSeverIdServer[toServerId].nodeA, mSeverIdServer[toServerId].nodeB, vm)){
                            output += ", A";
                            choice = A;
                        }else{
                            output += ", B";
                            choice = B;
                        }
                    }
                    output += ")";
                    vMigrations.push_back(output);
                    // 迁移到新的服务器上
                    _migrateVirtualMachineToServer(vmid, {toServerId, choice});
                    // 在for loop中修改集合中的元素会导致bug
                    modified.push_back({vm.getCore() + vm.getMemory(), vmid});
                    // 可迁移次数减1
                    totalOperation -= 1;
                    break;
                }else{
                    last -= 1;
                }
            }
            //break;
        }
        // 更新数据结构
        for(auto &p : modified){
            mServerVirtualMachineEn[fromServerId].erase(p);
        }

    }
}

void migrate(){
    // 在这一轮总共可以迁移的次数
#ifdef DEBUG
    assert(mVmidVirtualMachine.size() == VM_AMOUNT);
#endif
    int totalOperation = 2 * VM_AMOUNT / 1000;
    //int totalOperation = 3 * mVmidVirtualMachine.size() / 1000;
    // int totalOperation = 2 * mVmidVirtualMachine.size() / 1000;

    // 存储所有server的chipF
    vector<double> fragments;
    // vServers的下标，用于排序
    vector<int> serverIdxs;
    int idx = 0;
    for(auto &server : vServers){
        double fragment = (1.0 * MAXSOURCE - server.nodeA.coreRem - server.nodeA.memoryRem - server.nodeB.coreRem - server.nodeB.memoryRem) / MAXSOURCE;
        fragments.push_back(fragment);
        serverIdxs.push_back(idx++);
    }
    // 通过chipF排序，将排序后下标小的服务器中虚拟机往下标大的服务器中放
    sort(serverIdxs.begin(), serverIdxs.end(), [&](int a, int b){
            return fragments[a] < fragments[b];
            });

    int last = serverIdxs.size() - 1;
    while(last > 0 && abs(fragments[last] - 1) <= 1e-3){
        last -= 1;
    }
    for(int i = 0; i < last && totalOperation > 0; i++){
        // fromServer: 当前需要移除虚拟机的服务器
        int fromServerIdx = serverIdxs[i];
        // 需要移除虚拟机的服务器在vServers中的下标
        int fromServerId = vServers[fromServerIdx].id;

        // 存储循环中被移动到了新的服务器的虚拟机的vmid
        vector<int> modified;
        for(auto vmid : mServerVirtualMachine[fromServerId]){
#ifdef DEBUG
            assert(mVmidVirtualMachine.find(vmid) != mVmidVirtualMachine.end());
#endif
            // 要被迁移的虚拟机
            VirtualMachine vm = mVmidVirtualMachine[vmid];
            // 从碎片小的服务器往下找，直到第一个可以放下这个虚拟机的服务器
            for(int j = last; j > i && totalOperation > 0; j--){
                // toServer：被迁移虚拟机的去向
                int toServerIdx = serverIdxs[j];
                // toServer在vServers中的下标
                int toServerId = vServers[toServerIdx].id;

                // choice可以放在哪个结点
                int choice = canPut(vServers[toServerId], vm);
                if(choice > 0){
                    string output;
                    output += "(" + to_string(vmid) + ", " + to_string(mLocalServerIdGlobalServerId[toServerId]);
                    if(vm.getSingle()){
                        // 如果是单节点并且两个都能放，那么放在剩余资源多的结点上
                        if(choice == A){
                            output += ", A";
                        }else if(choice == B){
                            output += ", B";
                        }else if(compareNode(mSeverIdServer[toServerId].nodeA, mSeverIdServer[toServerId].nodeB, vm)){
                            output += ", A";
                            choice = A;
                        }else{
                            output += ", B";
                            choice = B;
                        }
                    }
                    output += ")";
                    vMigrations.push_back(output);
                    // 迁移到新的服务器上
                    migrateVirtualMachineToServer(vmid, {toServerId, choice});
                    // 在for loop中修改集合中的元素会导致bug
                    modified.push_back(vmid);
                    // 可迁移次数减1
                    totalOperation -= 1;
                    break;
                }
            }
        }
        // 更新数据结构
        for(auto &vmid : modified){
            mServerVirtualMachine[fromServerId].erase(vmid);
        }
    }
}

void solve(int today, int T){
#ifdef DEBUG
    cout << MAX_TIME << " " << today << " " << vOperations.size() << endl;
#endif
    // 顺序遍历每次操作
    optimized_migrate();
    //slow_optimized_migrate();
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
    doOutput();
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
    float minimumUseage = .8;
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
    long long vmcoresum = 0, vmmemsum = 0;
    for(int i = 0; i < vVirtualMachineModels.size(); i++){
        vmcoresum += vVirtualMachineModels[i].core;
        vmmemsum += vVirtualMachineModels[i].memory;
        MAX_VM_CORE = max(MAX_VM_CORE,vVirtualMachineModels[i].core);
        MAX_VM_MEMORY = max(MAX_VM_CORE,vVirtualMachineModels[i].memory);
    }

    MEAN_VM_CORE = (double)vmcoresum / M;
    MEAN_VM_MEMORY = (double)vmmemsum / M;
    MAXSOURCE = MAX_SERVERMODEL_CORE + MAX_SERVERMODEL_MEMORY;

    for(auto &vmd : vVirtualMachineModels){
        for(auto &sm : vServerModels){
            MAX_SIMI = max(MAX_SIMI, similarity(vmd, sm));
            MIN_SIMI = min(MIN_SIMI, similarity(vmd, sm));
        }
    }

    //计算相似度
    cin >> T;
    MAX_TIME = 90 * 1000 * 1000 / T;
    timeLeft = (double)90 / T;
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
    int totalPrice = 0;
    for(auto &s : vServers){
        totalPrice += s.getDeviceCost();
        if(!checkServer(s)){
            cout << s.tostring() << endl;
            return 0;
        }
        if(!checkUsage(s)){
            //cout << s.tostring() << endl;
            unfilledSize += 1;
        }
    }
    cout << "total price: " << totalPrice << " " << vServers.size() << " " << unfilledSize << endl;

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

