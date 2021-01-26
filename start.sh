#!/bin/bash
#变量
buildvar=build
execvar=TCPDemo

rm -rf $buildvar
mkdir $buildvar
cd $buildvar

cmake ..
make

#根据进程名杀死进程
PROCESS=`ps -ef|grep $execvar |grep -v grep|grep -v PPID|awk '{ print $2}'`
for i in $PROCESS
do
  echo "Kill the $1 process [ $i ]"
  sudo kill -9 $i
done


sudo ./$execvar
