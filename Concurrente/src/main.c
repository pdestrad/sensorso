#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include "conf.h"

extern int errno;

pid_t pid_sensor_d, pid_sensor_g;

//DISTANCIA
int shmidd_1,shmidd_2,shmidd_3;
char *shmd_1, *shmd_2, *shmd_3;

//GIROSCOPIO
int shmidt_1,shmidt_2,shmidt_3;
char *shmt_1,*shmt_2,*shmt_3;

//PARAMS
int shmid_val, shmid_type, I, W, Q;
float T;
char *shm_val, *shm_type;


//ESTRUCTURA PARA UN PAR DE SENSORES
struct sensor {
    char *shmd;
    char *shmt;
    float valor;
    int disponible;
}par1,par2,par3;

void iniciar_sensores();
void set_sharedMem();
void distinguirObjeto(float dist1, float dist2, float dist3);
void *leerParSensor(void *vargp);
void *leerParams(void *vargp);
void inicializar_params();

int procesar();

float getDesviacionEstandar(float data[]);
float distanciaReal(char* ang_str, char* dist_str);


int main() {
    inicializar_params();
	iniciar_sensores();
    sleep(2);
	procesar();

	return 0;
}

int procesar() {
	pthread_t hilo_par1,hilo_par2,hilo_par3;

	//SET MEMORIA COMPARTIDA
	set_sharedMem();

	//INICIAR ESTRUCTURAS
	par1.shmd = shmd_1;
	par1.shmt = shmt_1;
	par1.valor = 0.0;
    par1.disponible = 0;

	par2.shmd = shmd_2;
	par2.shmt = shmt_2;
	par2.valor = 0.0;
    par2.disponible = 0;

	par3.shmd = shmd_3;
	par3.shmt = shmt_3;
	par3.valor = 0.0;
    par3.disponible = 0;

	//INICIANDO HILOS QUE LEEN LOS PARES DE SENSORES
	pthread_create(&hilo_par1, NULL, leerParSensor, (void *)&par1); 
	pthread_create(&hilo_par2, NULL, leerParSensor, (void *)&par2);
	pthread_create(&hilo_par3, NULL, leerParSensor, (void *)&par3);

	//PROCESAR VALORES DE DISTANCIA
    float intervalo;
    float acu_par1;
    float acu_par2;
    float acu_par3;
	while(1) {
        intervalo = I/Q;
        acu_par1 = acu_par2 = acu_par3 = 0.0;
        for (int i=0; i<Q; i++) {
            acu_par1 += par1.valor;
            acu_par2 += par2.valor;
            acu_par3 += par3.valor;

            if (intervalo < 1) {
                usleep(intervalo);
            } else {
                sleep(intervalo);
            }
        }

        distinguirObjeto(acu_par1/Q, acu_par2/Q, acu_par3/Q);
	}
    return(0);
}

//INICIA LOS PROCESOS DE SENSORES
void iniciar_sensores() {
    if ((pid_sensor_d = fork()) == 0) {
        char *const args[] = {"../exe/sensor_Dist", 0};
        execv(args[0], args);
    } else if ((pid_sensor_d = fork()) == 0) {
        char *const args[] = {"../exe/sensor_Giro", 0};
        execv(args[0], args);
    }
}

//OBTENER PUNTERO A BLOQUES DE MEMORIA COMPARTIDA
void set_sharedMem() {
    //SENSOR DISTANCIA 1 
    if ((shmidd_1 = shmget(KEY_DIST_1, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmd_1 = shmat(shmidd_1, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    //SENSOR DISTANCIA 2 
    if ((shmidd_2 = shmget(KEY_DIST_2, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmd_2 = shmat(shmidd_2, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    //SENSOR DISTANCIA 3 
    if ((shmidd_3 = shmget(KEY_DIST_3, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmd_3 = shmat(shmidd_3, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    //GIROSCOPIO 1
    if ((shmidt_1 = shmget(KEY_GIRO_1, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmt_1 = shmat(shmidt_1, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    //GIROSCOPIO 2
    if ((shmidt_2 = shmget(KEY_GIRO_2, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmt_2 = shmat(shmidt_2, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    //GIROSCOPIO 3
    if ((shmidt_3 = shmget(KEY_GIRO_3, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmt_3 = shmat(shmidt_3, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    //PARAMS
    if ((shmid_val = shmget(KEY_PARAMS_VAL, SHMSZ, 0666)) < 0) {
        perror("shmget");
        printf("%s\n", "errro aqui1");
        return;
    }
    if ((shm_val = shmat(shmid_val, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    if ((shmid_type = shmget(KEY_PARAMS_TYPE, SHMSZ, 0666)) < 0) {
        perror("shmget");
        printf("%s\n", "errro aqui1");
        return;
    }
    if ((shm_type = shmat(shmid_type, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }
}

//METODO PARA LEER UN PAR DE SENSORES (DISTANCIA Y GIROSCOPIO), USADO POR CADA HILO
void *leerParSensor(void *vargp) { 
    struct sensor *s = vargp;
    char tmpdist[SHMSZ];
    char tmpgiro[SHMSZ];
    int mutex = 0;
    while(1) {
        strcpy(tmpgiro,s->shmt); //LEYENDO GIROSCOPIO 

        //CUANDO GIROSCOPIO ES CERO SIGNIFICA QUE EN EL SIGUIENTE CICLO VENDRA UN NUEVO VALOR
        // Y SE HABILITA EL SEMAFORO PARA LEER EL PROXIMO VALOR 
        if (atoi(tmpgiro) == 0) {
            mutex = 0;
        }
        //VERIFICAR QUE GIROSCOPIO DIFERENTE DE CERO (NUEVO VALOR) Y SEMAFORO DISPONIBLE
        //SE HABILITA SEMAFORO PARA LEER DISTANCIA
        if ((atoi(tmpgiro) != 0) && (mutex == 0)) {
            mutex = 1;
        }
        //VERIFICAR QUE SEMAFORO DISPONIBLE PARA LECTURA DE DISTANCIA
        //LEER DISTANCIA Y SET SEMAFORO PARA QUE NO LEA DISTANCIA HASTA QUE EXISTA NUEVO VALOR DE GIROSCOPIO
        if (mutex == 1) {
            strcpy(tmpdist,s->shmd); //LEYENDO DISTANCIA 
            s->valor = distanciaReal(tmpgiro, tmpdist);
            mutex = 2;
        }
    }
}

void distinguirObjeto(float s_izq, float s_cen, float s_der) {
    float data[] = {s_izq, s_cen, s_der};
    float sd = getDesviacionEstandar(data);
    
    //VERIFICANDO SI ES VEHICULO
    if ((fabsf(s_izq - s_cen) > T*sd) && (fabsf(s_der - s_cen) > T*sd)) {
        printf("\r %s              ","VEHICULO");
        fflush(stdout);
    } else if ((fabsf(s_izq - s_cen) > W*sd) || (fabsf(s_der - s_cen) > W*sd) || (fabsf(s_der - s_izq) > W*sd)) {
        printf("\r %s              ","OBSTACULO");
        fflush(stdout);
    } else {
        printf("\r %s              ","OBJETO NO IDENTIFICADO");
        fflush(stdout);
    }
}

void *leerParams(void *vargp) {
    char old[SHMSZ];
    while(1) {
        if ((shm_type != NULL) && (strcmp(shm_type,old) != 0)) {
            if (shm_type != NULL) {
                if (strcmp(shm_type, "I") == 0) {
                    I = atoi(shm_val);
                } else if (strcmp(shm_type, "Q") == 0) {
                    Q = atoi(shm_val);
                }
                 else if (strcmp(shm_type, "W") == 0) {
                    W = atoi(shm_val);
                }
                 else if (strcmp(shm_type, "T") == 0) {
                    T = atof(shm_val);
                }
            }
            strcpy(old, shm_val);
        }
    }
}

//OBTENER DESVIACION ESTANDAR
float getDesviacionEstandar(float data[]) {
    float sum = 0.0, prom, sd = 0.0; 
    int i;
    for(i=0; i<CANTIDAD_PAR_SENSORES; ++i){sum += data[i];}
    prom = sum/CANTIDAD_PAR_SENSORES;
    for(i=0; i<CANTIDAD_PAR_SENSORES; ++i){sd += pow(data[i] - prom, 2);}
    return sqrt(sd/10);
}

//OBTIENE EL ESTIMADO DE LA DISTANCIA REAL EN BASE A LA DISTANCIA Y EL GIROSCOPIO
float distanciaReal(char* ang_str, char* dist_str) {
    float ang = atof(ang_str);
    float dist = atof(dist_str);
    float rad = (ang * PI)/180;
    return dist * cos(rad);
}

//INICIALIZAR PARAMETROS
void inicializar_params() {
    I = I_ini;
    T = T_ini;
    W = W_ini;
    Q = Q_ini;
    pthread_t hilo_lectorParams;
    pthread_create(&hilo_lectorParams, NULL, leerParams, NULL); 
}



