#include <Windows.h>
#include <iostream>

// 全局变量
SERVICE_STATUS        g_ServiceStatus = {0};
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE                g_ServiceStopEvent = INVALID_HANDLE_VALUE;

// 服务名称和显示名称
#define SERVICE_NAME    L"MyService"
#define DISPLAY_NAME    L"My Service"

// 服务控制处理函数
void WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
    switch(CtrlCode)
    {
        case SERVICE_CONTROL_STOP:
            g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
            SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
            // 设置停止事件
            SetEvent(g_ServiceStopEvent);
            return;

        default:
            break;
    }
}

// 服务主函数
void WINAPI ServiceMain(DWORD argc, LPWSTR *argv)
{
    // 注册服务控制处理函数
    g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
    if(g_StatusHandle == NULL)
    {
        return;
    }

    // 设置服务状态
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwWin32ExitCode = NO_ERROR;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    // 向 SCM 报告服务状态
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    // 初始化服务
    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if(g_ServiceStopEvent == NULL)
    {
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = GetLastError();
        SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
        return;
    }

    // 服务已启动
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    // 执行服务逻辑，这里简单地打印一条消息
    std::wcout << L"MyService is running..." << std::endl;

    // 等待停止事件
    WaitForSingleObject(g_ServiceStopEvent, INFINITE);

    // 服务停止
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    // 清理资源
    CloseHandle(g_ServiceStopEvent);
}

// 服务安装函数
BOOL InstallService()
{
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(schSCManager == NULL)
    {
        return FALSE;
    }

    wchar_t szPath[MAX_PATH];
    if(!GetModuleFileName(NULL, szPath, MAX_PATH))
    {
        CloseServiceHandle(schSCManager);
        return FALSE;
    }

    SC_HANDLE schService = CreateService(schSCManager, SERVICE_NAME, DISPLAY_NAME,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL, szPath, NULL, NULL, NULL, NULL, NULL);

    if(schService == NULL)
    {
        CloseServiceHandle(schSCManager);
        return FALSE;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return TRUE;
}

// 服务卸载函数
BOOL UninstallService()
{
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(schSCManager == NULL)
    {
        return FALSE;
    }

    SC_HANDLE schService = OpenService(schSCManager, SERVICE_NAME, SERVICE_ALL_ACCESS);
    if(schService == NULL)
    {
        CloseServiceHandle(schSCManager);
        return FALSE;
    }

    BOOL result = DeleteService(schService);
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return result;
}

int main(int argc, char *argv[])
{
    if(argc > 1 && strcmp(argv[1], "install") == 0)
    {
        if(InstallService())
        {
            std::wcout << L"Service installed successfully." << std::endl;
        }
        else
        {
            std::wcerr << L"Failed to install service." << std::endl;
        }
    }
    else if(argc > 1 && strcmp(argv[1], "uninstall") == 0)
    {
        if(UninstallService())
        {
            std::wcout << L"Service uninstalled successfully." << std::endl;
        }
        else
        {
            std::wcerr << L"Failed to uninstall service." << std::endl;
        }
    }
    else
    {
        // 以服务方式运行
        SERVICE_TABLE_ENTRY ServiceTable[] =
        {
            {SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
            {NULL, NULL}
        };

        if(!StartServiceCtrlDispatcher(ServiceTable))
        {
            std::wcerr << L"Failed to start service dispatcher." << std::endl;
        }
    }

    return 0;
}
