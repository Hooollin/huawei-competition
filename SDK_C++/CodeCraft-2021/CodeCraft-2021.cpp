#include "function/strategy.h"
using namespace std;

int main(int argc, char *argv[])
{
#ifdef SEEK_PARAMETER
    buy_PriceWeight = atof(argv[1]);//按价格购买权重
    buy_BalanceWeight = atof(argv[2]);//两个节点使用资源比例平衡参数
    buy_LeftSpaceWeight = atof(argv[3]);//剩余空间
    buy_PriceWithCapacityWeight = atof(argv[4]);//性价比
    buy_SmWeight = atof(argv[5]);//相似性购买
    // double DayWeight = 0.8;

    //放置权值(和为1)
    put_SelectWeight = atof(argv[6]);//碎片选择权重
    put_NodeBlanceWeight = atof(argv[7]);//负载均衡参数
    put_SimWeight = atof(argv[8]);//相似放置
    put_PriceWithCapacityWeight = atof(argv[9]);
    put_BalanceWeight = atof(argv[10]);//两个节点使用资源比例平衡参数
    change_buyWeight = atof(argv[11]);
    bigServer = atof(argv[12]);
#endif
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int N, M, T, K;
    cin >> N;
    cin.ignore(); //忽略回车
    for(int i = 0; i < N; i++){
      readServerModel();
    }
    for (int i = 0; i < vServerModel.size(); i++) {
      MAX_SERVERMODEL_CORE = max(MAX_SERVERMODEL_CORE, vServerModel[i].core);
      MAX_SERVERMODEL_MEMORY =
          max(MAX_SERVERMODEL_MEMORY, vServerModel[i].memory);
      MAX_DEVICE_COST = max(MAX_DEVICE_COST, vServerModel[i].deviceCost);
      MAX_DAILY_COST = max(MAX_DAILY_COST, vServerModel[i].dailyCost);
    }

    cin >> M;
    cin.ignore(); //忽略回车
    for(int i = 0; i < M; i++){
      readVirtualMachineModel();
    }

    //将虚拟机模型按照大小排序
    sort(vVirtualMachineModel.begin(),vVirtualMachineModel.end(),[&](const VirtualMachineModel &a,const VirtualMachineModel &b){
        return a.core + a.memory < b.core + b.memory;
         });
    //并对应type：下标
    for(int i=0;i<vVirtualMachineModel.size();i++){
        VirtualMachineModel &p = vVirtualMachineModel[i];
        sortedVirtualMachine.push_back(unordered_set<int>{});
        VirtualMachineModeltoPos[p.type] = i;
    }

        //将虚拟机模型按照大小排序
    sort(vServerModel.begin(),vServerModel.end(),[&](const ServerModel &a,const ServerModel &b){
        return a.core + a.memory < b.core + b.memory;
         });
    //并对应type：下标
    for(int i=0;i<vServerModel.size();i++){
        ServerModel &p = vServerModel[i];
        sortedServer.push_back(unordered_set<int>{});
        ServerModeltoPos[p.type] = i;
    }

    long long vmcoresum = 0, vmmemsum = 0;
    for (int i = 0; i < vVirtualMachineModel.size(); i++) {
      vmcoresum += vVirtualMachineModel[i].core;
      vmmemsum += vVirtualMachineModel[i].memory;
      MAX_VM_CORE = max(MAX_VM_CORE, vVirtualMachineModel[i].core);
      MAX_VM_MEMORY = max(MAX_VM_CORE, vVirtualMachineModel[i].memory);
    }
    MEAN_VM_CORE = (double)vmcoresum / M;
    MEAN_VM_MEMORY = (double)vmmemsum / M;
    MAXSOURCE = MAX_SERVERMODEL_CORE + MAX_SERVERMODEL_MEMORY;
    for (auto &vmd : vVirtualMachineModel) {
      for (auto &sm : vServerModel) {
        MAX_SIMI = max(MAX_SIMI, similarity(vmd, sm));
        MIN_SIMI = min(MIN_SIMI, similarity(vmd, sm));
      }
    }
    //计算相似度
    cin >> T >> K;

    // 下标0为空
    vAllOperation.push_back({});
    int l = 1, r = 1;
    while(l <= T){
        while(r - l + 1 <= K){
            int R;
            cin >> R;
            cin.ignore();
            initializeOperationVector();
            while(R-- > 0){
                readOperation();
            }
            vAllOperation.push_back(vOperation);
            r += 1;
        }
        solve(l, r, T);
        l += 1;
    }
#ifdef DEBUG
    statiInformation();
#endif // DEBUG
    return 0;
}
