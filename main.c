#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 全局变量
HWND g_hButton = NULL;

// 函数声明
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnButtonClick(HWND hwnd);
BOOL ScanDirectory(const char* lpPath, char*** fileList, int* fileCount, int* maxCount);
void FreeFileList(char*** fileList, int fileCount);
BOOL IsImageFile(const char* lpFileName);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 注册窗口类
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
    
    // 创建窗口
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
    
    // 设置窗口置顶
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    
    // 显示窗口
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // 消息循环
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
            // 创建按钮
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
    
    // 获取可执行文件路径
    if (GetModuleFileName(NULL, szExePath, MAX_PATH) == 0)
    {
        MessageBox(hwnd, "获取可执行文件路径失败", "错误", MB_OK | MB_ICONERROR);
        return;
    }
    
    // 提取目录路径
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
    
    // 扫描目录
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
    
    // 优先选择图片文件
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
        // 随机选择图片文件
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
        // 随机选择任意文件
        int randomIndex = rand() % fileCount;
        selectedFile = fileList[randomIndex];
    }
    
    // 使用系统默认程序打开文件
    if (selectedFile)
    {
        ShellExecute(NULL, "open", selectedFile, NULL, NULL, SW_SHOWNORMAL);
    }
    
    // 释放文件列表
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
        // 跳过 . 和 ..
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
        {
            continue;
        }
        
        char szFullPath[MAX_PATH] = {0};
        sprintf(szFullPath, "%s\\%s", lpPath, findData.cFileName);
        
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // 递归扫描子目录
            ScanDirectory(szFullPath, fileList, fileCount, maxCount);
        }
        else
        {
            // 添加文件到列表
            if (*fileList == NULL || *fileCount >= *maxCount)
            {
                // 分配或扩展文件列表
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
