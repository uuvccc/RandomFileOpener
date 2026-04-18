#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#define VERSION "1.0.0"

typedef struct {
    BOOL guiMode;
    BOOL helpMode;
    BOOL versionMode;
    char targetDir[MAX_PATH];
    BOOL imagesOnly;
} AppOptions;

HWND g_hButton = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnButtonClick(HWND hwnd);
BOOL ScanDirectory(const char* lpPath, char*** fileList, int* fileCount, int* maxCount);
void FreeFileList(char*** fileList, int fileCount);
BOOL IsImageFile(const char* lpFileName);
void PrintUsage(const char* programName);
void PrintVersion(const char* programName);
BOOL ParseCommandLine(int argc, char* argv[], AppOptions* options);
BOOL OpenRandomFile(const char* lpPath, BOOL imagesOnly);

int main(int argc, char* argv[])
{
    AppOptions options = {FALSE, FALSE, FALSE, {0}, FALSE};

    if (!ParseCommandLine(argc, argv, &options))
    {
        return 1;
    }

    if (options.helpMode)
    {
        PrintUsage(argv[0]);
        return 0;
    }

    if (options.versionMode)
    {
        PrintVersion(argv[0]);
        return 0;
    }

    if (options.guiMode)
    {
        return WinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOW);
    }

    BOOL needsConsole = (GetConsoleWindow() != NULL);
    if (!needsConsole)
    {
        AllocConsole();
    }

    char targetPath[MAX_PATH] = {0};
    if (strlen(options.targetDir) > 0)
    {
        strcpy(targetPath, options.targetDir);
    }
    else
    {
        if (GetCurrentDir(targetPath, MAX_PATH) == NULL)
        {
            fprintf(stderr, "Error: Failed to get current directory\n");
            if (!needsConsole) FreeConsole();
            return 1;
        }
    }

    printf("Scanning directory: %s\n", targetPath);
    if (options.imagesOnly)
    {
        printf("Filter: Images only\n");
    }
    else
    {
        printf("Filter: All files\n");
    }

    if (!OpenRandomFile(targetPath, options.imagesOnly))
    {
        fprintf(stderr, "Error: Failed to open random file\n");
        if (!needsConsole) FreeConsole();
        return 1;
    }

    if (!needsConsole)
    {
        printf("Press any key to exit...\n");
        getchar();
        FreeConsole();
    }

    return 0;
}

BOOL ParseCommandLine(int argc, char* argv[], AppOptions* options)
{
    if (argc < 2)
    {
        options->guiMode = TRUE;
        return TRUE;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            options->helpMode = TRUE;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            options->versionMode = TRUE;
        }
        else if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gui") == 0)
        {
            options->guiMode = TRUE;
        }
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--images-only") == 0)
        {
            options->imagesOnly = TRUE;
        }
        else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--directory") == 0)
        {
            if (i + 1 < argc)
            {
                i++;
                strncpy(options->targetDir, argv[i], MAX_PATH - 1);
            }
            else
            {
                fprintf(stderr, "Error: -d/--directory requires a path argument\n");
                return FALSE;
            }
        }
        else if (argv[i][0] != '-')
        {
            strncpy(options->targetDir, argv[i], MAX_PATH - 1);
        }
        else
        {
            fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
            return FALSE;
        }
    }

    return TRUE;
}

void PrintUsage(const char* programName)
{
    printf("Usage: %s [OPTIONS] [DIRECTORY]\n", programName);
    printf("\n");
    printf("Open a random file from the specified directory or current directory.\n");
    printf("If no options or -g/--gui is specified, launches the GUI window.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help           Show this help message and exit\n");
    printf("  -v, --version        Show version information and exit\n");
    printf("  -g, --gui            Launch GUI window (default when no args provided)\n");
    printf("  -i, --images-only    Only select from image files\n");
    printf("  -d, --directory DIR  Specify target directory (alternative to positional arg)\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  DIRECTORY            Target directory to scan (default: current directory)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s                   Launch GUI window\n", programName);
    printf("  %s -g                Launch GUI window\n", programName);
    printf("  %s -i                Open a random image from current directory\n", programName);
    printf("  %s /path/to/dir      Open a random file from specified directory\n", programName);
    printf("  %s -d /path/to/dir   Open a random file from specified directory\n", programName);
    printf("  %s -i -d ./images    Open a random image from ./images directory\n", programName);
}

void PrintVersion(const char* programName)
{
    printf("%s version %s\n", programName, VERSION);
    printf("Pure Win32 API implementation\n");
    printf("Copyright (C) 2024\n");
}

BOOL OpenRandomFile(const char* lpPath, BOOL imagesOnly)
{
    char** fileList = NULL;
    int fileCount = 0;
    int maxCount = 100;

    if (!ScanDirectory(lpPath, &fileList, &fileCount, &maxCount))
    {
        return FALSE;
    }

    if (fileCount == 0)
    {
        printf("No files found in directory: %s\n", lpPath);
        FreeFileList(&fileList, fileCount);
        return FALSE;
    }

    printf("Found %d files\n", fileCount);

    int imageCount = 0;
    if (imagesOnly)
    {
        for (int i = 0; i < fileCount; i++)
        {
            if (IsImageFile(fileList[i]))
            {
                imageCount++;
            }
        }
        if (imageCount == 0)
        {
            printf("No image files found\n");
            FreeFileList(&fileList, fileCount);
            return FALSE;
        }
        printf("Found %d image files\n", imageCount);
    }

    char* selectedFile = NULL;
    srand((unsigned int)time(NULL));

    if (imagesOnly && imageCount > 0)
    {
        int randomIndex = rand() % imageCount;
        int imageIndex = 0;

        for (int i = 0; i < fileCount; i++)
        {
            if (IsImageFile(fileList[i]))
            {
                if (imageIndex == randomIndex)
                {
                    selectedFile = fileList[i];
                    break;
                }
                imageIndex++;
            }
        }
    }
    else
    {
        int randomIndex = rand() % fileCount;
        selectedFile = fileList[randomIndex];
    }

    if (selectedFile)
    {
        printf("Opening: %s\n", selectedFile);
        ShellExecute(NULL, "open", selectedFile, NULL, NULL, SW_SHOWNORMAL);
    }

    FreeFileList(&fileList, fileCount);
    return TRUE;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "RandomFileOpener";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, "窗口类注册失败", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowEx(
        0,
        "RandomFileOpener",
        "随机文件打开器",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 150,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
    {
        MessageBox(NULL, "窗口创建失败", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }

    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        {
            g_hButton = CreateWindowEx(
                0,
                "BUTTON",
                "随机打开文件",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                50, 50, 200, 30,
                hwnd,
                (HMENU)1,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );
        }
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1 && HIWORD(wParam) == BN_CLICKED)
        {
            OnButtonClick(hwnd);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void OnButtonClick(HWND hwnd)
{
    char szExePath[MAX_PATH] = {0};
    char szDirPath[MAX_PATH] = {0};

    if (GetModuleFileName(NULL, szExePath, MAX_PATH) == 0)
    {
        MessageBox(hwnd, "获取可执行文件路径失败", "错误", MB_OK | MB_ICONERROR);
        return;
    }

    char* pLastSlash = strrchr(szExePath, '\\');
    if (pLastSlash)
    {
        *pLastSlash = '\0';
        strcpy(szDirPath, szExePath);
    }
    else
    {
        MessageBox(hwnd, "提取目录路径失败", "错误", MB_OK | MB_ICONERROR);
        return;
    }

    char** fileList = NULL;
    int fileCount = 0;
    int maxCount = 100;

    if (!ScanDirectory(szDirPath, &fileList, &fileCount, &maxCount))
    {
        MessageBox(hwnd, "扫描目录失败", "错误", MB_OK | MB_ICONERROR);
        return;
    }

    if (fileCount == 0)
    {
        MessageBox(hwnd, "目录中没有找到文件", "提示", MB_OK | MB_ICONINFORMATION);
        FreeFileList(&fileList, fileCount);
        return;
    }

    int imageCount = 0;
    for (int i = 0; i < fileCount; i++)
    {
        if (IsImageFile(fileList[i]))
        {
            imageCount++;
        }
    }

    char* selectedFile = NULL;
    srand((unsigned int)time(NULL));

    if (imageCount > 0)
    {
        int randomIndex = rand() % imageCount;
        int imageIndex = 0;

        for (int i = 0; i < fileCount; i++)
        {
            if (IsImageFile(fileList[i]))
            {
                if (imageIndex == randomIndex)
                {
                    selectedFile = fileList[i];
                    break;
                }
                imageIndex++;
            }
        }
    }
    else
    {
        int randomIndex = rand() % fileCount;
        selectedFile = fileList[randomIndex];
    }

    if (selectedFile)
    {
        ShellExecute(NULL, "open", selectedFile, NULL, NULL, SW_SHOWNORMAL);
    }

    FreeFileList(&fileList, fileCount);
}

BOOL ScanDirectory(const char* lpPath, char*** fileList, int* fileCount, int* maxCount)
{
    char szSearchPath[MAX_PATH] = {0};
    sprintf(szSearchPath, "%s\\*", lpPath);

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(szSearchPath, &findData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    do
    {
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
        {
            continue;
        }

        char szFullPath[MAX_PATH] = {0};
        sprintf(szFullPath, "%s\\%s", lpPath, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            ScanDirectory(szFullPath, fileList, fileCount, maxCount);
        }
        else
        {
            if (*fileList == NULL || *fileCount >= *maxCount)
            {
                *maxCount = (*fileList == NULL) ? 100 : *maxCount * 2;
                char** newList = (char**)realloc(*fileList, *maxCount * sizeof(char*));
                if (!newList)
                {
                    FindClose(hFind);
                    return FALSE;
                }
                *fileList = newList;
            }

            (*fileList)[*fileCount] = (char*)malloc(strlen(szFullPath) + 1);
            if (!(*fileList)[*fileCount])
            {
                FindClose(hFind);
                return FALSE;
            }
            strcpy((*fileList)[*fileCount], szFullPath);
            (*fileCount)++;
        }

    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
    return TRUE;
}

void FreeFileList(char*** fileList, int fileCount)
{
    if (*fileList)
    {
        for (int i = 0; i < fileCount; i++)
        {
            if ((*fileList)[i])
            {
                free((*fileList)[i]);
            }
        }
        free(*fileList);
        *fileList = NULL;
    }
}

BOOL IsImageFile(const char* lpFileName)
{
    const char* extensions[] = {".jpg", ".jpeg", ".png", ".bmp", ".gif", ".tiff", ".webp", NULL};

    const char* pExt = strrchr(lpFileName, '.');
    if (!pExt)
    {
        return FALSE;
    }

    for (int i = 0; extensions[i]; i++)
    {
        if (_stricmp(pExt, extensions[i]) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}
