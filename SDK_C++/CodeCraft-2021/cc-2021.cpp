#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>
#include<unordered_set>
#include<set>
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
int migrateaccount = 0;
#endif

// 用来生成服务器编号
int localServerNum = 0;
int globalServerNumber = 0;
int MAX_VM_CORE = 0;
int MAX_VM_MEMORY = 0;
int MAX_SERVERMODEL_CORE = 0;
int MAX_SERVERMODEL_MEMORY = 0;
int MAXSOURCE = 0;
int MIGRATEUPLIMIT ;
int vmsingle,vmdouble;
// 权重值
const double choseWeight = 0.1; //迁移时选择虚拟机的比例
const double DayWeight = 0.1;  //允许单节点向双节点迁移的天数比例
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
        bool single;
        Server(string type, int core, int memory, int deviceCost, int dailyCost, int id,bool single): ServerModel(type, core, memory, deviceCost, dailyCost) {
            this->id = id;
            this->single = single;
        }

        Server(){}

        Server(string type, int id,bool single){
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
            this->single = single;

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
                + ", issingle: " + to_string(this->single)
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
        bool getSingle(){
            return this->single;
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
//***************************END DEFINITION**********************************


//***************************FUNCTION DEFINITION**********************************

int canPut(Server server, VirtualMachineModel vmd);

pair<int,int> selectServer(VirtualMachineModel vmd);

pair<int,int> makePurchase(VirtualMachineModel vmd, int today, int T);

string makeDeploymentOutput(int serverId, int node);

void updateResource(Server &server, int node, int vmcore, int vmmemory, bool isDel);

void putVirtualMachineToServer(VirtualMachineModel vmd, int vmid, pair<int,int> serverAndNode,bool change);

void migrateVirtualMachineToServer(VirtualMachineModel vmd, int vmid, pair<int,int> serverAndNode,bool change);

void allocateServer(OP addop, int today, int T);

void releaseRes(OP delop);

void releaseRes(int vmid);

void doOutput();

void migrate(int today,int T);

void solve(int day, int T);

void initializeOperationVector();

int getNextServerId();

int getNextGlobalServerId();

pair<double,int> selectServerCal(Server &currServer, VirtualMachineModel &vmd,int choice);

void migrateMachine(pair<int,int> from,int vmid,pair<int,int> to);

bool checkServer(Server server);

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
map<int, VirtualMachine> mVmidVirtualMachine;       //虚拟机编号到虚拟机的映射
map<pair<int,int>,set<pair<int,int> > > ServerHaveVirtualMachine; //各服务器(编号)和节点拥有的虚拟机(编号)集合,按碎片大小排序，第一个pair为（服务器编号，节点）第二个pair为（碎片大小，虚拟机编号）
set<pair<int,pair<int,int> > > singleDeployServer,doubleDeployServer;  //单双节点部署服务器编号及节点，（碎片大小的相反数（服务器编号，节点))
//输出数据
vector<string> vPurchases;
vector<pair<int, int>> vDeployments;

vector<int> vPurchasedServers;
vector<pair<int,pair<int,int> > > vMigrations;

void initializeOperationVector(){
    vOperations.clear();
    vOperations.resize(0);
    vPurchases.clear();
    vPurchases.resize(0);
    vDeployments.clear();
    vDeployments.resize(0);
    vPurchasedServers.clear();
    vPurchasedServers.resize(0);
    vMigrations.clear();
    vMigrations.resize(0);
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
//计算选择值
double selectServerFun(Server &currServer,int occupyACore,int occupyAMem,int occupyBCore,int occupyBMem,int node){
    int leftACore = currServer.nodeA.coreRem - occupyACore, leftBCore = currServer.nodeB.coreRem - occupyBCore;
    int leftAMem = currServer.nodeA.memoryRem - occupyAMem,leftBMem = currServer.nodeB.memoryRem - occupyBMem;
    //计算碎片参数
    double chipF;
    if(node == A)
        chipF = (1.0 * MAXSOURCE- leftBCore - leftBMem) / MAXSOURCE;
    else if(node == B)
        chipF = (1.0 * MAXSOURCE- leftACore - leftAMem) / MAXSOURCE;
    else
        chipF = (1.0 * MAXSOURCE- leftACore - leftAMem - leftBCore - leftBMem) / MAXSOURCE;
    //cout<<"chipF:"<<chipF<<" "<<MAXSOURCE<<" "<<leftACore<<" "<<leftAMem<<" "<<leftBCore<<" "<<leftBMem<<endl;
    //返回碎片参数
    return chipF;
}

pair<double,int> selectServerCal(Server &currServer, VirtualMachineModel &vmd,int choice){
    double res = 0;
    int choseNode = 0;
    double cal;
    if(vmd.single){
        if(choice == A || choice == BOTH){
            res = selectServerFun(currServer,vmd.core,vmd.memory,0,0,A);
            choseNode = A;
        }
        if(choice == B || choice == BOTH){
            cal = selectServerFun(currServer,0,0,vmd.core,vmd.memory,B);
            if(cal > res){
                res = cal;
                choseNode = B;
            }
        }
    }else{
        res = selectServerFun(currServer,vmd.core >> 1,vmd.memory >> 1,vmd.core >> 1,vmd.memory >> 1,BOTH);
        choseNode = BOTH;
    }
    return make_pair(res,choseNode);
}

inline bool ServerEmpty(const Server &nowServer){
    return nowServer.nodeA.coreUsed == 0 && nowServer.nodeA.memoryUsed == 0 && nowServer.nodeB.coreUsed == 0 && nowServer.nodeB.memoryUsed == 0;
}
pair<int,int> selectServer(VirtualMachineModel vmd){
    double maxn = 0; //函数获得的最大值
    pair<double,int> res;//返回值
    int finalChoice;
    int targetServerIdx = -1;
    for(int i = 0; i < vServers.size(); i++){
        Server &currServer = vServers[i];
        if(!(currServer.single == vmd.single || (currServer.single != vmd.single && ServerEmpty(currServer)))) // 允许放入相同属性服务器或者空服务器中
            continue;
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

//计算服务器的碎片量
inline int calChip(Server &server,int nodes){
    return nodes == BOTH ? (server.nodeA.coreRem + server.nodeA.memoryRem + server.nodeB.coreRem + server.nodeB.memoryRem) :
        (nodes == A ? server.nodeA.coreRem + server.nodeA.memoryRem :
            server.nodeB.coreRem + server.nodeB.memoryRem) ;
}

//计算虚拟机的总容量
inline int calCap(VirtualMachineModel &vmd){
    return vmd.core + vmd.memory;
}

inline int calCap(VirtualMachine &vm){
    return vm.getCore() + vm.getMemory();
}
pair<int,int> makePurchase(VirtualMachineModel vmd, int today, int T){
    int newServerId = getNextServerId();

    //当前虚拟机需要的core和内存大小
    int neededCore = vmd.core, neededMem = vmd.memory;
    if(vmd.single){
        neededCore *= 2;
        neededMem *= 2;
    }
    int devicePrice,dayPrice,PriceF;
    int k = -1;
    int priceF;
    int minx = 0x3f3f3f3f;
    for(int i=0;i<vServerModels.size();i++){
         ServerModel p = vServerModels[i];
        if(canBuy(p,neededCore,neededMem)){
            devicePrice = p.deviceCost;
            dayPrice = p.dailyCost * (T - today);
            PriceF = devicePrice + dayPrice;
            if(k == -1 || minx > PriceF) k = i,minx = PriceF;
        }
    }

#ifdef DEBUG
    assert(k != -1);
#endif

    ServerModel sm = vServerModels[k];
    Server purchasedServer(sm.type, newServerId,vmd.single);
    if(purchasedServer.single)
        singleDeployServer.insert(make_pair(-calChip(purchasedServer,A),make_pair(newServerId,A))),
        singleDeployServer.insert(make_pair(-calChip(purchasedServer,B),make_pair(newServerId,B)));
    else
        doubleDeployServer.insert(make_pair(-calChip(purchasedServer,BOTH),make_pair(newServerId,BOTH)));
    ServerHaveVirtualMachine[make_pair(newServerId,A)] = set<pair<int,int> > {};
    ServerHaveVirtualMachine[make_pair(newServerId,B)] = set<pair<int,int> > {};
    ServerHaveVirtualMachine[make_pair(newServerId,BOTH)] = set<pair<int,int> > {};
    vPurchasedServers.push_back(newServerId);

    vServers.push_back(purchasedServer);
    mServerIdVectorPos[newServerId] = vServers.size() - 1;
    mSeverIdServer[newServerId] = purchasedServer;

    if(vmd.single) return make_pair(newServerId,A);
    else return make_pair(newServerId,BOTH);

#ifdef DEBUG
    if(!checkServer(purchasedServer)) cout<<"purchase wrong :"<<purchasedServer.tostring()<<endl;
#endif // DEBUG
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

void putVirtualMachineToServer(VirtualMachineModel vmd, int vmid, pair<int,int> serverAndNode,bool change){
    //找到对应的server
    int serverId = serverAndNode.first;
    int node = serverAndNode.second;
    Server server = mSeverIdServer[serverId];

#ifdef DEBUG
    assert(canPut(server, vmd));
#endif

    if(server.single)
        singleDeployServer.erase(make_pair(-calChip(server,A),make_pair(serverId,A))),
        singleDeployServer.erase(make_pair(-calChip(server,B),make_pair(serverId,B)));
    else
        doubleDeployServer.erase(make_pair(-calChip(server,BOTH),make_pair(serverId,BOTH)));

    if(change){
        server.single = vmd.single;
    }

    updateResource(server, node, vmd.core, vmd.memory, false);
    mVirtualMachineServer[vmid] = {serverId, node};
    mSeverIdServer[serverId] = server;
    vServers[mServerIdVectorPos[serverId]] = server;
    vDeployments.push_back({serverId, node});

    if(server.single)
        singleDeployServer.insert(make_pair(-calChip(server,A),make_pair(serverId,A))),
        singleDeployServer.insert(make_pair(-calChip(server,B),make_pair(serverId,B)));
    else
        doubleDeployServer.insert(make_pair(-calChip(server,BOTH),make_pair(serverId,BOTH)));

    ServerHaveVirtualMachine[make_pair(serverId,node)].insert(make_pair(calCap(vmd),vmid));

#ifdef DEBUG
    if(!checkServer(server)) cout<<"put machine wrong :"<<server.tostring()<<endl;

#endif // DEBUG

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
    if(vmd.single != mSeverIdServer[serverAndNode.first].single) putVirtualMachineToServer(vmd, vmid, serverAndNode,true);
    else putVirtualMachineToServer(vmd, vmid, serverAndNode,false);
    if(vm.getSingle()) vmsingle ++;
    else vmdouble ++;
}

void releaseRes(OP delop){
    int vmid = delop.id;
    VirtualMachine vm = mVmidVirtualMachine[vmid];

    pair<int, int> t = mVirtualMachineServer[vmid];
    int serverId = t.first, loc = t.second;
    Server server = mSeverIdServer[serverId];

    if(server.single)
        singleDeployServer.erase(make_pair(-calChip(server,loc),t));
    else
        doubleDeployServer.erase(make_pair(-calChip(server,loc),t));

    updateResource(server, loc, vm.getCore(), vm.getMemory(), true);
    vServers[mServerIdVectorPos[serverId]] = server;
    mSeverIdServer[serverId] = server;

    mVmidVirtualMachine.erase(vmid);
    mVirtualMachineServer.erase(vmid);

    pair<int,int> serverAndNode = t;
    pair<int,int> capAndVm = make_pair(calCap(vm),vmid);
    ServerHaveVirtualMachine[serverAndNode].erase(capAndVm);

    if(server.single)
        singleDeployServer.insert(make_pair(-calChip(server,loc),t));
    else
        doubleDeployServer.insert(make_pair(-calChip(server,loc),t));

    if(vm.getSingle()) vmsingle --;
    else vmdouble --;
    return ;
#ifdef DEBUG
    if(!checkServer(server)) cout<<"release wrong :"<<server.tostring()<<endl;
#endif // DEBUG
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

    //输出migration
    cout << "(migration, " << vMigrations.size() << ")" << endl;
    for(auto p : vMigrations){
        if(p.second.second == BOTH){
            cout<< "(" << p.first << ", " << p.second.first << ")"<<endl;
        }else{
            cout<< "(" << p.first << ", " << p.second.first  << ", " << (p.second.second == A ? "A" : "B") << ")" <<endl;

        }
    }
    //输出deployment
    for(auto &p : vDeployments){
        cout << makeDeploymentOutput(p.first, p.second) << endl;
    }
}
//单节点canput
inline bool canPut2(int ServerId,int Node,VirtualMachine &vm){
    int leftCore,leftMem;
    switch (Node){
    case A :
        leftCore = mSeverIdServer[ServerId].nodeA.coreRem;
        leftMem = mSeverIdServer[ServerId].nodeA.memoryRem;
        break;
    case B :
        leftCore = mSeverIdServer[ServerId].nodeB.coreRem;
        leftMem = mSeverIdServer[ServerId].nodeB.memoryRem;
        break;
    }
    return leftCore >= vm.getCore() && leftMem >= vm.getMemory() ;
}
//双节点canPut
inline bool canPut2(int ServerId,VirtualMachine &vm){
    int &leftACore = mSeverIdServer[ServerId].nodeA.coreRem;
    int &leftBCore = mSeverIdServer[ServerId].nodeB.coreRem;
    int &leftAMem = mSeverIdServer[ServerId].nodeA.memoryRem;
    int &leftBMem = mSeverIdServer[ServerId].nodeB.memoryRem;
    return leftACore >= vm.getCore() / 2 && leftBCore >= vm.getCore() / 2 && leftAMem >= vm.getMemory() / 2 && leftBMem >= vm.getMemory() / 2;
}
void migrateMachine(pair<int,int> from,int vmid,pair<int,int> to){
    VirtualMachine vm = mVmidVirtualMachine[vmid];


#ifdef DEBUG
    if((from.second == A || from.second == B) && to.second == BOTH){
        cout << "node wrong" << endl;
    }
    if((to.second == A || to.second == B) && from.second == BOTH){
        cout << "node wrong" << endl;
    }
    if(to.second == BOTH){
        if(!canPut2(to.first,vm)) cout<<"mig select wrong!"<<endl;
    }else{
        if(!canPut2(to.first,to.second,vm)) cout<<"mig select wrong!"<<endl;
    }

#endif // DEBUG
    int fromServerId = from.first;
    int fromServerNode = from.second;
    int toServerId = to.first;
    int toServerNode = to.second;

    Server fromServer = mSeverIdServer[fromServerId];
    Server toServer = mSeverIdServer[toServerId];

    if(fromServer.single)
        singleDeployServer.erase(make_pair(-calChip(fromServer,fromServerNode),from));
    else
        doubleDeployServer.erase(make_pair(-calChip(fromServer,fromServerNode),from));

    updateResource(fromServer, fromServerNode, vm.getCore(), vm.getMemory(), true);
    vServers[mServerIdVectorPos[fromServerId]] = fromServer;
    mSeverIdServer[fromServerId] = fromServer;

    pair<int,int> capAndVm = make_pair(calCap(vm),vmid);
    ServerHaveVirtualMachine[from].erase(capAndVm);

    if(fromServer.single)
        singleDeployServer.insert(make_pair(-calChip(fromServer,fromServerNode),from));
    else
        doubleDeployServer.insert(make_pair(-calChip(fromServer,fromServerNode),from));


    if(toServer.single)
        singleDeployServer.erase(make_pair(-calChip(toServer,toServerNode),to));
    else
        doubleDeployServer.erase(make_pair(-calChip(toServer,toServerNode),to));

    updateResource(toServer, toServerNode, vm.getCore(), vm.getMemory(), false);
    mVirtualMachineServer[vmid] = to;
    mSeverIdServer[toServerId] = toServer;
    vServers[mServerIdVectorPos[toServerId]] = toServer;

    if(toServer.single)
        singleDeployServer.insert(make_pair(-calChip(toServer,toServerNode),to));
    else
        doubleDeployServer.insert(make_pair(-calChip(toServer,toServerNode),to));

    ServerHaveVirtualMachine[to].insert(make_pair(calCap(vm),vmid));

    vMigrations.push_back(make_pair(vmid,to));

#ifdef DEBUG
    if(!checkServer(fromServer)) cout<<"migrate fromserver wrong :"<<fromServer.tostring()<<endl;
    if(!checkServer(toServer)) cout<<"migrate toserver wrong :"<<toServer.tostring()<<endl;
    migrateaccount ++;
#endif // DEBUG

    return ;
}

void migrate(int today,int T){


    int migrateSingle = vmsingle * 5 / 1000;
    int migrateDouble = vmdouble * 5 / 1000;

    //单节点备选虚拟机数目
    int singleCt = 20 * migrateSingle ;

    //获得单节点备选服务器
    vector<pair<int,int> > migSingleServerAndNode;
    for(auto migpair : singleDeployServer){
        auto ServerAndNode = migpair.second;
        //if(ServerAndNode.second == BOTH) cout<<"single1 mig wrong!"<<endl;
        singleCt -= ServerHaveVirtualMachine[ServerAndNode].size();
        migSingleServerAndNode.push_back(ServerAndNode);
        if(singleCt <= 0) break;
    }

    //将单节点备选服务器上的虚拟机迁移到其余较大的服务器中
    for(pair<int,int> ServerAndNode : migSingleServerAndNode){
        if(migrateSingle <= 0) break;
        //if(ServerAndNode.second == BOTH) cout<<"single mig wrong!"<<endl;
        int ServerId = ServerAndNode.first;
        int node = ServerAndNode.second;
        vector<int> Findvmd;
        for(auto p : ServerHaveVirtualMachine[ServerAndNode]){
            if(p.first >= MIGRATEUPLIMIT) break;
            Findvmd.push_back(p.second);
        }
        for(int vmId : Findvmd){
            VirtualMachine &vm = mVmidVirtualMachine[vmId];
            auto p = singleDeployServer.upper_bound(make_pair(-calCap(vm),make_pair(0,0)));
            while(p != singleDeployServer.begin() && p->second != ServerAndNode && !canPut2(p->second.first,p->second.second,vm)) p --;
            if(p->second == ServerAndNode ) continue;
            pair<int,int> to = p->second;
            if(canPut2(p->second.first, p->second.second, vm)){
                migrateMachine(ServerAndNode,vmId,to);
            }
            migrateSingle --;
            if(migrateSingle <= 0) break;
        }
    }

    //双备选虚拟机数目
    int doubleCt = 20 * migrateDouble ;

    //获得双节点备选服务器
    vector<pair<int,int> > migDoubleServerAndNode;
    for(auto migpair : doubleDeployServer){
        auto ServerAndNode = migpair.second;
        //if(ServerAndNode.second != BOTH) cout<<"double1 mig wrong!"<<endl;
        doubleCt -= ServerHaveVirtualMachine[ServerAndNode].size();
        migDoubleServerAndNode.push_back(ServerAndNode);
        if(doubleCt <= 0) break;
    }

    //将双节点备选服务器上的虚拟机迁移到其余较大的服务器中
    for(pair<int,int> ServerAndNode : migDoubleServerAndNode){
        if(migrateDouble <= 0) break;
        //if(ServerAndNode.second != BOTH) cout<<"double mig wrong!"<<endl;
        int ServerId = ServerAndNode.first;
        int node = ServerAndNode.second;
        vector<int> Findvmd;
        for(auto p : ServerHaveVirtualMachine[ServerAndNode]){
            if(p.first >= MIGRATEUPLIMIT) break;
            Findvmd.push_back(p.second);
        }
        for(int vmId : Findvmd){
            VirtualMachine &vm = mVmidVirtualMachine[vmId];
            auto p = doubleDeployServer.upper_bound(make_pair(-calCap(vm),make_pair(0,0)));
            while(p != doubleDeployServer.begin() && p->second != ServerAndNode && !canPut2(p->second.first,vm)) p --;
            if(p->second == ServerAndNode) continue;
            pair<int,int> to = p->second;
            if(canPut2(p->second.first, vm)){
                migrateMachine(ServerAndNode,vmId,to);
            }
            migrateDouble --;
            if(migrateDouble <= 0) break;
        }
    }
    return ;
}
void solve(int today, int T){

    //首先进行迁移
    migrate(today,T);
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
    vmsingle = vmdouble = 0;
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
    vector<int> vmChip;
    for(int i = 0; i < vVirtualMachineModels.size(); i++){
        MAX_VM_CORE = max(MAX_VM_CORE,vVirtualMachineModels[i].core);
        MAX_VM_MEMORY = max(MAX_VM_CORE,vVirtualMachineModels[i].memory);
        vmChip.push_back(calCap(vVirtualMachineModels[i]));
    }

    sort(vmChip.begin(),vmChip.end());
    int upPos = vmChip.size() * 0.9;
    MIGRATEUPLIMIT = vmChip[upPos];

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
    cout << "migrate count: " << migrateaccount << endl;
    int unfilledSize = 0;
    int totalPrice = 0;
    for(auto &s : vServers){
        totalPrice += s.getDeviceCost();
        if(!checkServer(s)){
            cout << s.tostring() << endl;
            continue;
        }
        if(!checkUsage(s)){
            //cout << s.tostring() << endl;
            unfilledSize += 1;
        }
    }
    cout << "total price: " << totalPrice << " " << vServers.size() << " " << unfilledSize << endl;
#endif
    return 0;
}
