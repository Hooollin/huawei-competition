#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <sstream>
#include <string>

#define ADD 1
#define DEL 2

#define DEBUG

#ifdef DEBUG
    int addcount = 0;
    int delcount = 0;
#endif

using namespace std;

class ServerModel{
    public:
        string type;    // 型号
        int core;		// CPU数量
        int memory;		// 内存大小

        int deviceCost; // 硬件成本
        int dailyCost;  // 日常成本

        ServerModel(){

        }

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
                + ", single" + (this->single ? "True" : "False")
                + "}";
        }
};

map<string, ServerModel> mServerModels; // 所有服务器型号； key: type -> value: 服务器型号
map<string, VirtualMachineModel> mVirtualMachineModels; // 所有虚拟机型号； key: type -> value: 虚拟机型号


vector<ServerModel> vServerModels; 
vector<VirtualMachineModel> vVirtualMachineModels;

class Server : ServerModel{
    public:
        int id;			    //服务器编号
        int ACoreUsed; 	    //A节点已经被使用的核心数
        int AMemoryUsed;	//A节点已经被使用的内存大小
        int ACoreRemain; 	    //A节点还可被使用的核心数
        int AMemoryRemain;	//A节点已经还可被使用的内存大小

        int BCoreUsed;      //B节点已经被使用的核心数
        int BMemoryUsed;    //B节点已经被使用的内存大小
        int BCoreRemain; 	    //B节点还可被使用的核心数
        int BMemoryRemain;	//B节点已经还可被使用的内存大小

        Server(string type, int core, int memory, bool single, int deviceCost, int dailyCost, int id): ServerModel(type, core, memory, deviceCost, dailyCost) {
            this->id = id;
        }

        Server(string type, int id){
#ifdef DEBUG
            assert(mServerModels.find(type) != mServerModels.end()); // debug：确保type存在
#endif
            ServerModel model = mServerModels[type];
            this->type = type;
            this->id = id;
            this->core = model.core;
            this->memory = model.memory;
            this->dailyCost = model.dailyCost;
            this->deviceCost = deviceCost;

            this->ACoreUsed = 0;
            this->AMemoryUsed = 0;
            this->BCoreUsed = 0;
            this->BMemoryUsed = 0;

            this->ACoreRemain = this->core / 2;
            this->AMemoryRemain = this->memory / 2;
            this->BCoreRemain = this->core / 2;
            this->BMemoryRemain = this->memory / 2;
        }

        string tostring(){	
            return "Server: {id: " + to_string(this->id)
                + ", type: " + this->type
                + ", core: " + to_string(this->core)
                + ", ACoreUsed: " + to_string(this->ACoreUsed)
                + ", ACoreRemain: " + to_string(this->ACoreRemain)
                + ", BCoreUsed: " + to_string(this->BCoreUsed)
                + ", BCoreRemain: " + to_string(this->BCoreRemain)
                + ", memory: " + to_string(this->memory) 
                + ", AMemoryUsed: " + to_string(this->AMemoryUsed) 
                + ", AMemoryRemain: " + to_string(this->AMemoryRemain) 
                + ", BMemoryUsed: " + to_string(this->BMemoryUsed) 
                + ", BMemoryRemain: " + to_string(this->BMemoryRemain) 
                + ", deviceCost: " + to_string(this->deviceCost) 
                + ", dailyCost: " + to_string(this->dailyCost) + "}";
        }
};

class VirtualMachine : VirtualMachineModel{
    public:
        int id;
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

vector<OP> vOperations;
map<int, int> mVirtualMachineServer; //key: virtual machine id; value: server id;

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


void initializeOperationVector(){
    vOperations.clear();
    vOperations.resize(0);
}

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
    while(T-- > 0){
        int R;
        cin >> R;
        cin.ignore(); //忽略回车

        initializeOperationVector();
        while(R-- > 0){
            readOperation();
        }
        //TODO:algo
    }
    // TODO:fflush(stdout);

#ifdef DEBUG
    cout << "N: " << vServerModels.size() << endl;
    cout << "M: " << vVirtualMachineModels.size() << endl;
    cout << "add count: " << addcount << endl;
    cout << "del count: " << delcount << endl;
#endif
    return 0;
}
