#!/bin/bash


# Algunas validaciones iniciales
if [ -z "$1" ]
then
	echo Falta un parametro indicando cuantas ejecuciones se desean, chamaco/a!
	echo Ejemplo: ./batch-runner 10 programas/programa1
	exit 1
fi

if [ -z "$2" ]
then
	echo Falta un segundo parametro indicando el programa a ejecutar, chamaco/a!
	echo Ejemplo: ./batch-runner 10 programas/programa1
	exit 1
fi

if [ ! -f "$2" ]
then
	echo "El archivo $2 no existe :("
	exit 1
fi

# Los parametros en bash se referencian segun el orden, $1, $2, etc
SCRIPTS_QTY=$1
PROG_TO_RUN=$2 

INTERVAL=0.25 # Para esperar, en segundos, entre cada ejecucion de la consola

for i in `seq 1 $SCRIPTS_QTY`
do
	LOG_PATH=console_$i.log
	echo Iniciando consola numero $i en background y enviando log a $LOG_PATH...

	# Reemplazar "dummy-console" por el nombre del binario posta de la consola
	# Cada ejecucion redirecciona la salida estandar (la terminal) a un archivo propio 
	./bin/consola.out $PROG_TO_RUN 80 >$LOG_PATH &

	sleep $INTERVAL
done 
