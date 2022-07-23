#!/bin/bash

# Algunas validaciones iniciales
if [ $# != 6 ]
then
	echo Faltan setear algunas IPS
    echo EJ: $0 IP_MEMORIA 192.168.0.1 IP_CPU 192.168.0.1 IP_KERNEL 192.168.0.1
	exit 1
fi

actual_path=$(pwd)
configs=$(find $actual_path -iname "*.config")
echo $actual_path
for config in $configs
do
    if [ $1 == "IP_MEMORIA" ]
    then
        echo $1;
        sed -i "/$1=/c\IP_MEMORIA=$2" $config
    fi
    if [ $3 == "IP_CPU" ]
    then
        echo $3;
        sed -i "/$3=/c\IP_CPU=$4" $config
    fi
    if [ $5 == "IP_KERNEL" ]
    then
        echo $5;
        sed -i "/$5=/c\IP_KERNEL=$6" $config
    fi
done