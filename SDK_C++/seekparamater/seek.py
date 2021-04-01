import subprocess

train1 = open('../training-1.txt', 'r')
train2 = open('../training-2.txt', 'r')

completedProcess1 = subprocess.run(["./../bin/CodeCraft-2021"], stdin=train1, capture_output=True, encoding="utf-8")
completedProcess2 = subprocess.run(["./../bin/CodeCraft-2021"], stdin=train2, capture_output=True, encoding="utf-8")
output1 = completedProcess1.stdout
output2 = completedProcess2.stdout

print(output1, output2)
