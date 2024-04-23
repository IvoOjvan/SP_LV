#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
/*
#define N 5
#define NTHRD 2

HANDLE threadHandles[NTHRD]; // HANDLE-ovi niti
long ids[NTHRD];  // ID-ovi od niti (kreæu od 0,1,2,3,...)
float matrix[N][N];

typedef struct {
	int start;
	int end;
}ThreadArgs;

float calculateElement(int i, int j) {
	float sum = 0.0;
	float j_rad = j * M_PI / 180; // Convert j from degrees to radians
	for (int k = 0; k <= i; k++) {
		sum += k * sin(j_rad) - j * cos(k * M_PI / 180); // Convert k from degrees to radians
	}
	return sum;
}

VOID WINAPI calculateThreadElement(LPVOID arg){
	ThreadArgs* args = (ThreadArgs*)arg;
	for (int i = args->start; i <= args->end; i++) {
		for (int j = 0; j < N; j++) {
			float sum = 0.0;
			float j_rad = j * (float)M_PI / 180;
			for (int k = 0; k <= i; k++) {
				sum += k * sin(j_rad) - j * cos(k * (float)M_PI / 180);
			}
			matrix[i][j] = sum;
		}
	}
}


int main() {

	clock_t t1, t2;
	float sum = 0;

    t1 = clock();
	// Sekvencijalni dio
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			matrix[i][j] = calculateElement(i, j);
			sum += matrix[i][j];
		}
	}
	t2 = clock();

	printf("Sek: %.5f[ms]\n", (float)(t2 - t1) * 1000.0 / CLOCKS_PER_SEC);
	printf("Sum: %.4f\n", sum);


	// Paralelni dio
	ThreadArgs threadArgs[NTHRD];

	t1 = clock();
	for (int i = 0; i < NTHRD; i++) {
		threadArgs[i].start = i * (N / NTHRD);
		threadArgs[i].end = (i + 1) * (N / NTHRD) - 1;
		threadHandles[i] = CreateThread(NULL, 0, calculateThreadElement, &threadArgs[i], 0, NULL);
		
		if (threadHandles[i] == NULL) {
			printf("Error creating thread %d\n", i);
			return 1;
		}
	}

	WaitForMultipleObjects(NTHRD, threadHandles, TRUE, INFINITE);
	t2 = clock();

	printf("Multithreaded: %.2f[ms]\n", (float)(t2 - t1) * 1000.0 / CLOCKS_PER_SEC);
	
	float checkSum = 0.0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			checkSum += matrix[i][j];
		}
	}

	printf("Thread sum: %.4f", checkSum);
	return 0;
}*/