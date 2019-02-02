# sensorso

PROYECTO II PARCIAL SISTEMAS OPERATIVOS

Para compilar: 
-main.c : gcc -Wall -o ../exe/main main.c -lpthread 
-sensor_Dist.c : gcc -Wall -o ../exe/sensor_Dist sensor_Dist.c -lpthread 
-sensor_Giro.c : gcc -Wall -o ../exe/sensor_Giro sensor_Giro.c -lpthread 
-set_params.c : gcc -Wall -o ../exe/params set_params.c

Para utilizar el sistema:
1) Abrir 2 terminales
2) ejecutar ./params (Programa para ingresar parametros)
3) ejecutar ./main (Programa para ejecutar sistema)

Configuracion Inicial
El archivo conf.h contiene todos los parametros iniciales por si desean ser cambiados.