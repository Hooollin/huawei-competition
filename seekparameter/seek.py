import subprocess
import numpy as np
from sko.GA import GA
from subprocess import TimeoutExpired
from sko.tools import set_run_mode

x = [0.4,0.3,0,0.3,0.1, 0,0.4,0,0.4]
precision = [1e-3, 1e-3, 1e-3, 1e-3, 1e-3, 1e-3, 1e-3, 1e-3, 1e-3]
lowerbound = [0,0,0,0, 0,0,0,0,0]
upperbound = [1,1,1,1, 1,1,1,1,1]
def tar_func(x):
    inf = 1e40
    timeout = 180
    train1 = open('../training-1.txt', 'r')
    train2 = open('../training-2.txt', 'r')

    cmd = ["./../bin/CodeCraft-2021"]
    for x_i in x:
        cmd.append(str(x_i))
    try:
        #completedProcess1 = subprocess.run(cmd, stdin=train1, capture_output=True, encoding="utf-8", timeout=timeout)

        completedProcess2 = subprocess.run(cmd, stdin=train2, capture_output=True, encoding="utf-8", timeout=timeout)

        #output1 = int(completedProcess1.stdout)
        output2 = int(completedProcess2.stdout)
    except TimeoutExpired:
        output1 = inf
        output2 = inf
    output1 = 0
    print(x, output1, output2, output1 + output2)
    return output1 + output2


set_run_mode(tar_func, 'multithreading')
ga = GA(func=tar_func, n_dim=9, size_pop=6, max_iter=24, lb=lowerbound, ub=upperbound,precision=precision)

best_x, best_y = ga.run()
print('best_x:', best_x, '\n', 'best_y:', best_y)
