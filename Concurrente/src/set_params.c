#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "conf.h"

char parametro[INPUT_MAX_LEN];
char valor[INPUT_MAX_LEN];
char texto[INPUT_MAX_LEN];

int shmid_val, shmid_type;
char *shmd_val, *shmd_type;

int leerParametros();
void set_sharedMem();

int main (int argc, char *argv[]) {
	set_sharedMem();
	leerParametros();
	return 0;
}

int leerParametros() {
	while(1) {
		strcpy(texto,"");
		printf ("Ingrese parametro tipo parametro(I | Q | W | T | EXIT): ");
		scanf ("%s", parametro);

		if (strcmp(parametro,"I") == 0) {
			printf ("Ingrese I (I >= 1): ");
			scanf ("%s", valor);
			if (atoi(valor) >= 1) {
				sprintf(shmd_type,"%s",parametro);
				sprintf(shmd_val,"%s",valor);
			} else {
				printf ("ERROR: I fuera de rango\n");
			}
		} else if (strcmp(parametro,"Q") == 0) {
			printf ("Ingrese Q (Q >= 1): ");
			scanf ("%s", valor);
			if (atoi(valor) >= 1) {
				sprintf(shmd_type,"%s",parametro);
				sprintf(shmd_val,"%s",valor);
			} else {
				printf ("ERROR: Q fuera de rango\n");
			}
		} else if (strcmp(parametro,"W") == 0) {
			printf ("Ingrese W (W > 1): ");
			scanf ("%s", valor);
			if (atoi(valor) >= 1) {
				sprintf(shmd_type,"%s",parametro);
				sprintf(shmd_val,"%s",valor);
			} else {
				printf ("ERROR: W fuera de rango\n");
			}
		} else if (strcmp(parametro,"T") == 0) {
			printf ("Ingrese T (0 < Q < 1): ");
			scanf ("%s", valor);
			if ((atof(valor) < 1) && (atof(valor) > 0))  {
				sprintf(shmd_type,"%s",parametro);
				sprintf(shmd_val,"%s",valor);
			} else {
				printf ("ERROR: T fuera de rango\n");
			}
		} else if (strcmp(parametro,"EXIT") == 0) {
			return 0;
		} else {
			printf ("Parametro no reconocido.\n");
		}
	}
}

//OBTENER PUNTERO A BLOQUES DE MEMORIA COMPARTIDA
void set_sharedMem() {
    if ((shmid_val = shmget(KEY_PARAMS_VAL, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmd_val = shmat(shmid_val, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }

    if ((shmid_type = shmget(KEY_PARAMS_TYPE, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return;
    }
    if ((shmd_type = shmat(shmid_type, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return;
    }
}





