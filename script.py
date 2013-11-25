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
    results.write('Time vs. Matrix Size (Start=500, End=3500, Increment=500) for '+prog+'\n')
    for i in range(500,3500+1,500):
      proc = subprocess.Popen([prog, str(i), '100'], stdout=subprocess.PIPE)    
      out = proc.stdout.read()
      time = getTime(out)
      results.write(time+'\n')
    results.write('\n\n')

def blockSizeSpeedup(prog):
    results.write('Time vs Block Size (Start=100, End=1000, Increment=100) for '+prog+'\n')
    for i in range(100,1000+1,100):
        proc = subprocess.Popen([prog, '3000', str(i)], stdout=subprocess.PIPE)
        out = proc.stdout.read()
        time = getTime(out)
        results.write(time+'\n')
    results.write('\n\n')
    
if __name__ == '__main__':
    results = open('./time', 'w+')
    matrixSizeSpeedup('./serial')
    blockSizeSpeedup('./serial')
    matrixSizeSpeedup('./openmp')
    blockSizeSpeedup('./openmp')
    results.close()
