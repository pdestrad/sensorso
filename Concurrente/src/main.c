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

#define SHMSZ     27
#define PI 3.14159265

extern int errno;

static float tiempo_prom;
static float tiempo_actual;


pid_t pid_sensor_d, pid_sensor_g;

//DISTANCIA
int shmidd_1,shmidd_2,shmidd_3;
char *shmd_1, *shmd_2, *shmd_3; 
key_t keyd_1,keyd_2,keyd_3;

//GIROSCOPIO
int shmidt_1,shmidt_2,shmidt_3;
char *shmt_1,*shmt_2,*shmt_3;
key_t keyt_1,keyt_2,keyt_3;

//ESTRUCTURA PARA UN PAR DE SENSORES
struct sensor {
    char *shmd;
    char *shmt;
    float valor;
}par1,par2,par3;

void iniciar_sensores();
void set_sharedMem();
int procesar();
int getTimestamp();

float distanciaReal(char* ang_str, char* dist_str);
float getTiempoPromedio(float tiempo_actual, float tiempo_prom);

void *leerParSensor(void *vargp);


int main() {
	iniciar_sensores();
    sleep(2);
	procesar();

	return 0;
}

int procesar() {
	pid_t hilo_par1;
	pid_t hilo_par2;
	pid_t hilo_par3;

	//SET MEMORIA COMPARTIDA
	set_sharedMem();

	//INICIAR ESTRUCTURAS
	par1.shmd = shmd_1;
	par1.shmt = shmt_1;
	par1.valor = 0.0;

	par2.shmd = shmd_2;
	par2.shmt = shmt_2;
	par2.valor = 0.0;

	par3.shmd = shmd_3;
	par3.shmt = shmt_3;
	par3.valor = 0.0;

	//INICIANDO HILOS QUE LEEN LOS PARES DE SENSORES
	pthread_create(&hilo_par1, NULL, leerParSensor, (void *)&par1); 
	pthread_create(&hilo_par2, NULL, leerParSensor, (void *)&par2);
	pthread_create(&hilo_par3, NULL, leerParSensor, (void *)&par3);

	//PROCESAR VALORES DE DISTANCIA
	while(1) {
		printf("\r %f | %f | %f    ",par1.valor, par2.valor, par3.valor);
		fflush(stdout);
		sleep(1);
	}
    return(0);
}

void *leerParSensor(void *vargp) { 
	struct sensor *s = vargp;
	char tmpdist[SHMSZ];
	char tmpgiro[SHMSZ];
	int mutex = 0;
	while(1) {
		strcpy(tmpgiro,s->shmt); //LEYENDO GIROSCOPIO 1

		if (atoi(tmpgiro) == 0) {
			mutex = 0;
		}

		if ((atoi(tmpgiro) != 0) && (mutex == 0)) {
			mutex = 1;
		}

		if (mutex == 1) {
			strcpy(tmpdist,s->shmd); //LEYENDO DISTANCIA 1
			s->valor = distanciaReal(tmpgiro, tmpdist);
			mutex = 2;
		}
	}
}


void set_sharedMem() {
 
    //SENSOR DISTANCIA 1 
    keyd_1 = 1111;
    if ((shmidd_1 = shmget(keyd_1, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmd_1 = shmat(shmidd_1, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    //SENSOR DISTANCIA 2 
    keyd_2 = 2222;
    if ((shmidd_2 = shmget(keyd_2, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmd_2 = shmat(shmidd_2, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    //SENSOR DISTANCIA 3 
    keyd_3 = 3333;
    if ((shmidd_3 = shmget(keyd_3, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmd_3 = shmat(shmidd_3, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    //GIROSCOPIO 1 
    keyt_1 = 4444;
    if ((shmidt_1 = shmget(keyt_1, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmt_1 = shmat(shmidt_1, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    //GIROSCOPIO 2 
    keyt_2 = 5555;
    if ((shmidt_2 = shmget(keyt_2, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmt_2 = shmat(shmidt_2, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    //GIROSCOPIO 3 
    keyt_3 = 6666;
    if ((shmidt_3 = shmget(keyt_3, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmt_3 = shmat(shmidt_3, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

}

void iniciar_sensores() {
	if ((pid_sensor_d = fork()) == 0) {
		char *const args[] = {"../exe/sensor_Dist", 0};
		execv(args[0], args);
	} else if ((pid_sensor_d = fork()) == 0) {
		char *const args[] = {"../exe/sensor_Giro", 0};
		execv(args[0], args);
	}
}

float distanciaReal(char* ang_str, char* dist_str) {
    float ang = atof(ang_str);
    float dist = atof(dist_str);
    float rad = (ang * PI)/180;
    return dist * cos(rad);
}

int getTimestamp() {
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_usec;
}
float getTiempoPromedio(float tiempo_actual, float tiempo_prom) {
	return roundf(((tiempo_actual + tiempo_prom)/2.0) * 100) / 100;
}


