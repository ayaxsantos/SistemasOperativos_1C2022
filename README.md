# TP Sistemas Operativos - 1° Cuatrimestre 2022 (KISS)

Grupo: CarpinchOS Reloaded 😂

[Enunciado](https://docs.google.com/document/d/17WP76Vsi6ZrYlpYT8xOPXzLf42rQgtyKsOdVkyL5Jj0) | 
[Documento de pruebas](https://docs.google.com/document/d/1SBBTCweMCiBg6TPTt7zxdinRh4ealRasbu0bVlkty5o)

## Introduccion:

Trabajo practico correspondiente a la asignatura obligatoria "Sistemas Operativos", perteneciente al tercer año de la carrera Ingenieria en Sistemas de Informacion de la UTN FRBA.

El objetivo del presente, es realizar una simulacion de un sistema distribuido, implementando de forma practica diferentes conceptos y estrategias (con ciertas simplificaciones acordes al tiempo de desarrollo) que utilizan los Sistemas Operativos. 

## Despliegue:

- Ejecutar el script 'cambiar_ips.sh' para actualizar las IPs de los archivos de configuracion correspondientes. El formato a respetar es:

```bash
./cambiar_ips.sh IP_MEMORIA [memoria_ip] IP_CPU [cpu_ip] IP_KERNEL [kernel_ip]
```

- Ejecutar cada uno de los modulos en el siguiente orden:
    1) Memoria
    2) CPU
    3) Kernel
    4) Consola (instancias deseadas)

## Modulo MEMORIA:

Recibe los pedidos para lectura/escritura en tablas de paginas o porciones del espacio de usuario de la memoria. Utiliza paginacion jerarquica de dos niveles, e implementa memoria virtual haciendo uso de paginacion bajo demanda.

Algoritmos de reemplazo:
- Clock
- Clock modificado

Para ejecutar:

```bash
./bin/memoria.out [archivo_de_configuracion] 
```

## Modulo CPU:

Interpreta y ejecuta las instrucciones del proceso correspondiente a ejecutar, simulando un ciclo de instruccion tradicional. Ademas, simula las operaciones de una MMU desponiendo de una TLB para la traduccion de direcciones logicas a fisicas. 

Algoritmos de reemplazo:
- FIFO
- LRU

Para ejecutar:

```bash
./bin/cpu.out [archivo_de_configuracion] 
```

## Modulo KERNEL:

Encargado de realizar la planificacion de los distintos procesos que ingresan al sistema. Por tal motivo, esta compuesto de:
- Planificador de largo plazo
- Planificador de corto plazo (algoritmos FIFO o SJF con desalojo)
- Planificador de mediano plazo

Para ejecutar:

```bash
./bin/kernel.out [archivo_de_configuracion] 
```

## Modulo CONSOLA:

Es el punto de entrada del sistema, parseando un archivo con codigo fuente. Las operaciones disponibles, representadas por mnemonicos, son las siguientes:

- `NO_OP [CantidadRepeticiones]`
- `I/O [TiempoIO]`
- `READ [DireccionLogica]`
- `WRITE [DireccionLogica] [Valor]`
- `COPY [DireccionLogicaDestino] [DireccionLogicaOrigen]`
- `EXIT -> Sin parametros`

Para ejecutar:

```bash
./bin/consola.out [archivo_codigo_fuente] [tamaño_proceso]
```

## Agradecimientos:

- [Catedra UTN SO](https://github.com/sisoputnfrba) - Autores de la biblioteca [so-commons-library](https://github.com/sisoputnfrba/so-commons-library), utilizada a lo largo del proyecto.
- [RaniAgus](https://github.com/RaniAgus) - Autor del template utilizado para la estructura del proyecto, disponible en [so-project-template](https://github.com/RaniAgus/so-project-template).
