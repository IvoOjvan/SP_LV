#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define N 5
#define NUM_THREADS 4

HANDLE threadHandles[NUM_THREADS];
DWORD threadIDs[NUM_THREADS];
HANDLE columnSemaphores[N];
double matrix[N][N];

typedef struct {
    int rowStart;
    int rowEnd;
} ThreadArgs;

double calculateElement(int i, int j) {
    double sum = 0.0;
    double j_rad = j * M_PI / 180.0;
    for (int k = 0; k <= i; k++) {
        sum += k * sin(j_rad) - j * cos(k * M_PI / 180.0);
    }
    return sum;
}

DWORD WINAPI calculateThreadElement(LPVOID arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int rowStart = args->rowStart;
    int rowEnd = args->rowEnd;

    for (int i = rowStart; i < rowEnd; i++) {
        for (int j = 0; j < N; j++) {
            // Calculate prevColumnAverage for the previous column for the current row
            double prevColumnAverage = 0.0;
            if (j > 0) {
                WaitForSingleObject(columnSemaphores[j - 1], INFINITE);
                prevColumnAverage = matrix[i][j - 1];
                ReleaseSemaphore(columnSemaphores[j - 1], 1, NULL);
            }

            // Calculate element of the current column
            double sum = calculateElement(i, j);

            // Update matrix element with the computed sum and prevColumnAverage
            WaitForSingleObject(columnSemaphores[j], INFINITE);
            matrix[i][j] = sum + prevColumnAverage;
            ReleaseSemaphore(columnSemaphores[j], 1, NULL);
        }
    }

    return 0;
}

int main() {
    clock_t t1, t2;
    double sum = 0;

    // Sequential part
    double seqMatrix[N][N];
    t1 = clock();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            seqMatrix[i][j] = calculateElement(i, j);
        }
    }

    double columnAverage = 0.0;
    for (int j = 1; j < N; j++) {
        columnAverage = 0.0;
        for (int i = 0; i < N; i++) {
            columnAverage += seqMatrix[i][j - 1];
        }

        // After calculating add to current column
        for (int k = 0; k < N; k++) {
            seqMatrix[k][j] += columnAverage / N;
        }
    }
    t2 = clock();

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sum += seqMatrix[i][j];
        }
    }

    printf("Sequential: %.5f[ms]\n", (float)(t2 - t1) * 1000.0 / CLOCKS_PER_SEC);
    printf("Sum: %.4f\n", sum);

    // Initialize semaphores
    for (int i = 0; i < N; i++) {
        columnSemaphores[i] = CreateSemaphore(NULL, 1, 1, NULL);
    }

    // Calculate number of rows each thread will handle
    int rowsPerThread = N / NUM_THREADS;
    int remainingRows = N % NUM_THREADS;

    // Create threads
    ThreadArgs threadArgs[NUM_THREADS];
    int rowStart = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        int rowEnd = rowStart + rowsPerThread + (i < remainingRows ? 1 : 0);
        threadArgs[i].rowStart = rowStart;
        threadArgs[i].rowEnd = rowEnd;
        rowStart = rowEnd;

        threadHandles[i] = CreateThread(NULL, 0, calculateThreadElement, &threadArgs[i], 0, &threadIDs[i]);
        if (threadHandles[i] == NULL) {
            printf("Error creating thread %d\n", i);
            return 1;
        }
    }

    // Wait for all threads to complete
    WaitForMultipleObjects(NUM_THREADS, threadHandles, TRUE, INFINITE);

    double checkSum = 0.0;
    // Output the matrix
    printf("Matrix:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%.4f\t", matrix[i][j]);
            checkSum += matrix[i][j];
        }
        printf("\n");
    }

    printf("Sum: %.4f\n", checkSum);

    // Close handles
    for (int i = 0; i < N; i++) {
        CloseHandle(columnSemaphores[i]);
    }

    return 0;
}
