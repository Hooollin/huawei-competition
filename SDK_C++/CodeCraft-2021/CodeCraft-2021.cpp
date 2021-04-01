#include "function/strategy.h"
using namespace std;

int main()
{
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
    statiInformation();
#endif // DEBUG
    return 0;
}
