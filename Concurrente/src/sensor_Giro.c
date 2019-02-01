/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Daniel Ochoa Donoso 2010 <dochoa@fiec.espol.edu.ec>
 * 
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h> 

#define SHMSZ     27
#define MAX_SAMPLES 100
#define MAX_SAMPLES_THETA 50
#define DIST 10
#define PI 3.14159265
        /* ranf() is uniform in 0..1 */

float box_muller(float m, float s);	/* normal random variate generator */
void *iniciarSensor(void *vargp);

int main(int argc,char * argv[]) {
	pthread_t tid1,tid2,tid3; 
	int key1 = 4444;
	int key2 = 5555;
	int key3 = 6666;
    pthread_create(&tid1, NULL, iniciarSensor, (void *)&key1); 
    pthread_create(&tid2, NULL, iniciarSensor, (void *)&key2); 
    pthread_create(&tid3, NULL, iniciarSensor, (void *)&key3); 

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

	return(0);
}

void *iniciarSensor(void *vargp) { 
    int shmidt;
    char *shmt;
	int i,j;
	float distances[MAX_SAMPLES];
    float angles[MAX_SAMPLES_THETA];
    float anglesD[MAX_SAMPLES];
	float mu,sigma,delta_theta;

    struct timespec tim, tim2;
    tim.tv_sec = 1;
    tim.tv_nsec = 0;

    key_t keyt = *(int *)vargp;
    if ((shmidt = shmget(keyt, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
    }    
    if ((shmt = shmat(shmidt, NULL, 0)) == (char *) -1) {
        perror("shmat");
    }    

	mu=0;
	sigma=25;    

	sleep(3);

    for(i=0;i<MAX_SAMPLES_THETA;i++)	  
	{
		angles[i]=box_muller(mu,sigma);
	}
	
	j=-1;
    for(i=0;i<MAX_SAMPLES_THETA-1;i++)	  
	{        
		if (j++<MAX_SAMPLES) anglesD[j]=angles[i];
    	if (j++<MAX_SAMPLES){
			delta_theta=fabsf(angles[i+1]-angles[i])/2;
			if (angles[i+1]>angles[i]) 
				anglesD[j]= angles[i]+delta_theta;
			else
				anglesD[j]= angles[i]-delta_theta;
		}
	}
   
    for(i=0;i<j;i++){	
		distances[i]=DIST/cos(anglesD[i]/180*PI);		
	}

	for(i=0;i<j;i++){	
	  if(nanosleep(&tim , &tim2) < 0 ) {
  			printf("Nano sleep failed \n");
		}	
	   if (i%2==0){
			sprintf(shmt,"%f",anglesD[i]); 
	   }else{
			strcpy(shmt,"--");
	   }
	}
	return 0;
}

float box_muller(float m, float s)	/* normal random variate generator */
{				        /* mean m, standard deviation s */
	float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * ((double)(rand())/RAND_MAX)- 1.0;
			x2 = 2.0 * ((double)(rand())/RAND_MAX) - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( m + y1 * s );
}

