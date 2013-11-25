#!/usr/bin/python
import os
import subprocess
import re

global results

def getTime(time):
  p = re.compile('[0-9]+')
  m = p.search(time)
  return m.group(0)

def matrixSizeSpeedup(prog):
    results.write('Description: Time vs. Matrix Size for '+prog+'\n')
    results.write('------------------------------------------------\n')
    results.write('Matrix,Block,Time\n')
    for i in range(500,3500+1,500):
      proc = subprocess.Popen([prog, str(i), '100'], stdout=subprocess.PIPE)    
      out = proc.stdout.read()
      time = getTime(out)
      results.write(str(i)+',100,'+time+'\n')
    results.write('\n\n')

def blockSizeSpeedup(prog):
    results.write('Description: Time vs Block Size for '+prog+'\n')
    results.write('------------------------------------------------\n')
    results.write('Matrix,Block,Time\n')
    for i in range(100,1000+1,100):
        proc = subprocess.Popen([prog, '3000', str(i)], stdout=subprocess.PIPE)
        out = proc.stdout.read()
        time = getTime(out)
        results.write('3000,'+str(i)+','+time+'\n')
    results.write('\n\n')

def threadSpeedup(prog):
    results.write('Description: Time vs Threads for '+prog+'\n')
    results.write('------------------------------------------------\n')
    results.write('Matrix,Block,Thread,Time\n')
    for i in range (500, 2000+1, 500):
        for j in range (1,10+1,1):
            proc = subprocess.Popen([prog, str(i), '100', str(j)], stdout=subprocess.PIPE)
            out = proc.stdout.read()
            time = getTime(out)
            results.write(str(i)+',100,'+str(j)+','+time+'\n')
    results.write('\n\n')

if __name__ == '__main__':
    results = open('./time', 'w+')
    matrixSizeSpeedup('./serial')
    blockSizeSpeedup('./serial')
    matrixSizeSpeedup('./openmp')
    blockSizeSpeedup('./openmp')
    threadSpeedup('./openmp_threads')
    results.close()
