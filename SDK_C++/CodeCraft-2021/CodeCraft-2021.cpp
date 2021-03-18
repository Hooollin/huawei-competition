#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <sstream>
#include <string>

// 操作类型
#define ADD 1
#define DEL 2

// 虚拟机所在节点：A、B、BOTH
#define A 3
#define B 4
#define BOTH 5

//#define DEBUG

#ifdef DEBUG
int addcount = 0;
int delcount = 0;
#endif

// 用来生成服务器编号
int serverNum = 0;

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
        int id;			    //服务器编号
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


//***************************END DEFINITION**********************************

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
    vServerModels.push_back(sm);
    mServerModels[sm.type] = sm;
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

map<int, pair<int, int> > mVirtualMachineServer; // 虚拟机编号到服务器编号以及节点的映射
vector<Server> vServers;        //所有已经创建的服务器
map<int, Server> mSeverIdServer;     //服务器编号到服务器的映射
map<int, int> mServerIdVectorPos;    //服务器编号到vector下标的映射
map<int, VirtualMachine> mVmidVirtualMachine;       //虚拟机编号到虚拟机到映射

//输出数据
vector<string> vPurchases;
vector<string> vMigrations;
vector<string> vDeployments;

void initializeOperationVector(){
    vOperations.clear();
    vOperations.resize(0);
    vPurchases.clear();
    vPurchases.resize(0);
    vMigrations.clear();
    vMigrations.resize(0);
    vDeployments.clear();
    vDeployments.resize(0);
}

// purchase时获取服务器编号
int getNextServerId(){
    return serverNum++;
}

// 判断虚拟机是否可以放在服务器中
bool canPut(Server server, VirtualMachineModel vmd){
    if(vmd.single){
        return (server.nodeA.coreRem >= vmd.core && server.nodeA.memoryRem >= vmd.memory) || (server.nodeB.coreRem >= vmd.core && server.nodeB.memoryRem >= vmd.memory);
    }
    return server.nodeA.coreRem >= (vmd.core / 2) && server.nodeB.coreRem >= (vmd.core / 2) && server.nodeA.memoryRem >= (vmd.memory / 2) && server.nodeB.memoryRem >= (vmd.memory / 2);
}

float leftProportion(Server server){
    int totalCoreRem = server.nodeA.coreRem + server.nodeB.coreRem;
    int totalMemRem = server.nodeA.memoryRem + server.nodeB.memoryRem;
    return (float)totalCoreRem / server.getCore() + (float)totalMemRem / server.getMemory();
}

int selectServer(VirtualMachineModel vmd){
    float left = 2.; // 可能的最大值
    int targetServerIdx = -1;
    for(int i = 0; i < vServers.size(); i++){
        Server currServer = vServers[i];
        float currLeft = leftProportion(currServer);
        if(canPut(currServer, vmd) && currLeft < left){
            left = currLeft;
            targetServerIdx = i;
        }
    }
    return targetServerIdx == -1 ? -1 : vServers[targetServerIdx].id;
}

void makePurchaseOutput(string type, int amount){
    string output = "(" + type + ", " + to_string(amount) + ")";
    vPurchases.push_back(output);
}

int makePurchase(VirtualMachineModel vmd, int today, int T){
    int newServerId = getNextServerId();

    // 测试，只买最便宜的
    ServerModel sm = vServerModels[0];
    for(ServerModel currsm : vServerModels){
        if(currsm.core > vmd.core && currsm.memory > vmd.memory && currsm.deviceCost < sm.deviceCost){
            sm = currsm;
        }
    }
    Server purchasedServer(sm.type, newServerId);
    vServers.push_back(purchasedServer);
    mServerIdVectorPos[newServerId] = vServers.size() - 1;
    mSeverIdServer[newServerId] = purchasedServer;

    makePurchaseOutput(purchasedServer.getType(), 1);
    return newServerId;
}

//当虚拟机是单节点放置时，比较将vm放在A节点还是B节点；true->A, false->B;
bool compareNode(Node nodeA, Node nodeB, VirtualMachineModel vmd){
    //只能放在A、B中的一个
    // A节点剩下的资源不够
    if(vmd.core > nodeA.coreRem || vmd.memory > nodeA.memoryRem){
        return false;
    }
    // B节点剩下的资源不够
    if(vmd.core > nodeB.coreRem || vmd.memory > nodeB.memoryRem){
        return true;
    }

    //A, B节点都可以放时
    //如果A可以放并且放后core和mem都比B多
    if(nodeA.coreRem - vmd.core >= nodeB.coreRem && nodeA.memoryRem -vmd.memory > nodeB.memoryRem){
        return true;
    }
    //如果B可以放并且放后core和mem都比A多
    if(nodeB.coreRem - vmd.core >= nodeA.coreRem && nodeB.memoryRem - vmd.memory > nodeA.memoryRem){
        return false;
    }
    // 其他的情况取决于vmd所需的core和memory
    if(vmd.core > vmd.memory){
        return nodeA.coreRem > nodeB.coreRem;
    }
    return nodeA.memoryRem > nodeB.memoryRem;
}

void makeDeploymentOutPut(int serverId, int node){
    string output;
    if(node == A){
        output = "(" + to_string(serverId) + ", A" + ")";
    }else if(node == B){
        output = "(" + to_string(serverId) + ", B" + ")";
    }else{
        output = "(" + to_string(serverId) + ")";
    }
    vDeployments.push_back(output);
}

void putVirtualMachineToServer(VirtualMachineModel vmd, int vmid, int serverId){
    Server &server = mSeverIdServer[serverId];
    if(vmd.single){
        //尽可能使A、B节点负载均衡
        if(compareNode(server.nodeA, server.nodeB, vmd)){
            server.nodeA.coreRem -= vmd.core;
            server.nodeA.coreUsed += vmd.core;
            server.nodeA.memoryRem -= vmd.memory;
            server.nodeA.memoryUsed += vmd.memory;

            mVirtualMachineServer[vmid] = {serverId, A};

            makeDeploymentOutPut(serverId, A);
        }else{
            server.nodeB.coreUsed += vmd.core;
            server.nodeB.coreRem -= vmd.core;
            server.nodeB.memoryRem -= vmd.memory;
            server.nodeB.memoryUsed += vmd.memory;

            mVirtualMachineServer[vmid] = {serverId, B};

            makeDeploymentOutPut(serverId, B);
        }
    }else{
        server.nodeA.coreUsed += vmd.core / 2;
        server.nodeA.coreRem -= vmd.core / 2;
        server.nodeA.memoryUsed += vmd.memory / 2;
        server.nodeA.memoryRem -= vmd.memory / 2;

        server.nodeB.coreUsed += vmd.core / 2;
        server.nodeB.coreRem -= vmd.core / 2;
        server.nodeB.memoryUsed += vmd.memory / 2;
        server.nodeB.memoryRem -= vmd.memory / 2;

        makeDeploymentOutPut(serverId, BOTH);

        mVirtualMachineServer[vmid] = {serverId, BOTH};
    }

    vServers[mServerIdVectorPos[serverId]] = server;
}


void allocateServer(OP addop, int today, int T){
    int vmid = addop.id;
    // 找到一个最合适的server放虚拟机
    // 新增虚拟机的型号
    VirtualMachineModel vmd = mVirtualMachineModels[addop.machineType];

    VirtualMachine vm(addop.machineType, vmid);
    mVmidVirtualMachine[vmid] = vm;

    int serverId = selectServer(vmd);
    if(serverId == -1){       // 没有可以放下的虚拟机，需要新购买服务器
        serverId = makePurchase(vmd, today, T);
    }
    putVirtualMachineToServer(vmd, vmid, serverId);
}

void releaseRes(OP delop){
    int vmid = delop.id;
    VirtualMachine vm = mVmidVirtualMachine[vmid];
    mVmidVirtualMachine.erase(vmid);

    pair<int, int> t = mVirtualMachineServer[vmid];
    int serverId = t.first, loc = t.second;

    Server &server = mSeverIdServer[serverId];
    if(loc == A){
        server.nodeA.coreRem += vm.getCore();
        server.nodeA.coreUsed -= vm.getCore();
        server.nodeA.memoryRem += vm.getMemory();
        server.nodeA.memoryUsed -= vm.getMemory();
    }else if(loc == B){
        server.nodeB.coreRem += vm.getCore();
        server.nodeB.coreUsed -= vm.getCore();
        server.nodeB.memoryRem += vm.getMemory();
        server.nodeB.memoryUsed -= vm.getMemory();
    }else{
        server.nodeA.coreRem += vm.getCore() / 2;
        server.nodeA.coreUsed -= vm.getCore() / 2;
        server.nodeA.memoryRem += vm.getMemory() / 2;
        server.nodeA.memoryUsed -= vm.getMemory() / 2;

        server.nodeB.coreRem += vm.getCore() / 2;
        server.nodeB.coreUsed -= vm.getCore() / 2;
        server.nodeB.memoryRem += vm.getMemory() / 2;
        server.nodeB.memoryUsed -= vm.getMemory() / 2;
    }
    mVirtualMachineServer.erase(vmid);
}

void doOutput(){
    cout << "(purchase, " << vPurchases.size() << ")" << endl;
    for(auto &str : vPurchases){
        cout << str << endl;
    }
    //暂时没有migration
    cout << "(migration, 0)" << endl;
    for(auto &str : vDeployments){
        cout << str << endl;
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
        migrate();
    }   
    doOutput();
}

// ***********************END ALGO*************************
//
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    char ret;

    int N, M, T;
    cin >> N;
    cin.ignore(); //忽略回车
    for(int i = 0; i < N; i++){
        readServerModel();
    }
    cin >> M;
    cin.ignore(); //忽略回车
    for(int i = 0; i < M; i++){
        readVirtualMachineModel();
    }

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
    for(auto &s : vServerModels){
        cout << s.tostring() << endl;
    }
    for(auto &s : vServers){
        cout << s.tostring() << endl;
    }
#endif
    return 0;
}
