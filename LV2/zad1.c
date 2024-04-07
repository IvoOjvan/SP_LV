#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#undef UNICODE
#undef UNICODE_

#include <Windows.h>

void countFiles(const char* directory, int* files, int* dirs, int* largeFiles, FILETIME* oldestFileTime);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <directory path>\n", argv[0]);
        return 1;
    }

    int files = 0;
    int dirs = 0;
    int largeFiles = 0;

    FILETIME oldestFileTime;
    GetSystemTimeAsFileTime(&oldestFileTime);

    countFiles(argv[1], &files, &dirs, &largeFiles, &oldestFileTime);

    // Convert FILETIME to SYSTEMTIME for printing
    SYSTEMTIME sysTime;
    FileTimeToSystemTime(&oldestFileTime, &sysTime);

    printf("Dirs: %d\n", dirs);
    printf("Files: %d\n", files);
    printf("Big files: %d\n", largeFiles);
    printf("Oldest file: %02d/%02d/%d %02d:%02d:%02d\n", sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

    return 0;
}

void countFiles(const char* directory, int* files, int* dirs, int* largeFiles, FILETIME* oldestFileTime)
{
    WIN32_FIND_DATA fileInfo;
    char searchPath[MAX_PATH];
    sprintf(searchPath, "%s\\*.*", directory);

    HANDLE hFind = FindFirstFile(searchPath, &fileInfo);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error: Unable to find directory %s.\n", directory);
        return;
    }

    do {
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(fileInfo.cFileName, ".") != 0 && strcmp(fileInfo.cFileName, "..") != 0) {
                (*dirs)++;
                char subdirPath[MAX_PATH];
                sprintf(subdirPath, "%s\\%s", directory, fileInfo.cFileName);
                countFiles(subdirPath, files, dirs, largeFiles, oldestFileTime); // Recursive call for subdirectory
            }
        }
        else {
            (*files)++;

            LARGE_INTEGER fileSize;
            fileSize.LowPart = fileInfo.nFileSizeLow;
            fileSize.HighPart = fileInfo.nFileSizeHigh;

            if (fileSize.QuadPart >= (1LL * 1024 * 1024 * 1024)) {
                (*largeFiles)++;
            }

            // Check if the current file's last write time is older than the oldestFileTime
            if (CompareFileTime(&fileInfo.ftLastWriteTime, oldestFileTime) == -1) {
                *oldestFileTime = fileInfo.ftLastWriteTime;
            }

        }
    } while (FindNextFile(hFind, &fileInfo) != 0);

    FindClose(hFind);
}
