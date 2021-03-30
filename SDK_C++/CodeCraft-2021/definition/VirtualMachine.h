#pragma once
#include <string>

using namespace std;

class VirtualMachine{
    public:
        string type;
        int id;
        int core;
        int memory;
        bool single;

        VirtualMachine(){}
        VirtualMachine(string type, int id, int core, int memory, bool single){
            this->type = type;
            this->core = core;
            this->memory = memory;
            this->single = single;
            this->id = id;
        }

        int getId(){
            return this->id;
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
