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
    for i in [10, 100, 500, 1000, 1500, 2000, 2500, 3000]:
      if (prog != './mpi'):
        proc = subprocess.Popen([prog, str(i), '100'], stdout=subprocess.PIPE)    
      else:
        proc = subprocess.Popen(['openmpirun', '-np', '4', './mpi', str(i), '100'], stdout=subprocess.PIPE)
      out = proc.stdout.read()
      time = getTime(out)
      results.write(str(i)+',100,'+time+'\n')
    results.write('\n\n')

def blockSizeSpeedup(prog):
    results.write('Description: Time vs Block Size for '+prog+'\n')
    results.write('------------------------------------------------\n')
    results.write('Matrix,Block,Time\n')
    for i in [10, 20, 30, 40, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000]:
      if(prog != './mpi'):
        proc = subprocess.Popen([prog, '1000', str(i)], stdout=subprocess.PIPE)
      else:
        proc = subprocess.Popen(['openmpirun', '-np', '4', './mpi', '3000', str(i)], stdout=subprocess.PIPE)
      out = proc.stdout.read()
      time = getTime(out)
      results.write('1000,'+str(i)+','+time+'\n')
    results.write('\n\n')

def threadSpeedup(prog):
    results.write('Description: Time vs Threads for '+prog+'\n')
    results.write('------------------------------------------------\n')
    results.write('Matrix,Block,Thread,Time\n')
    for i in [2,4,8,12,16,20,100]:
      for j in range (500, 2000+1, 500):
        proc = subprocess.Popen([prog, str(j), '100', str(i)], stdout=subprocess.PIPE)
        out = proc.stdout.read()
        time = getTime(out)
        results.write(str(j)+',100,'+str(i)+','+time+'\n')
      results.write('\n')

def createExecs():
  os.system('gcc -o serial lu_tile_serial.c')
  os.system('gcc -o openmp -fopenmp lu_tile_openmp.c')
  os.system('openmpicc lu_tile_openmpi.c -o mpi')
  os.system('gcc -o threads -fopenmp lu_tile_openmp_threads.c')

if __name__ == '__main__':
    results = open('./time', 'w+')
    createExecs()
    matrixSizeSpeedup('./serial')
    blockSizeSpeedup('./serial')
    matrixSizeSpeedup('./openmp')
    blockSizeSpeedup('./openmp')
    matrixSizeSpeedup('./mpi')
    blockSizeSpeedup('./mpi')
    threadSpeedup('./threads')
    results.close()
