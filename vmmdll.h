
// vmmdll.h�����ڰ�����ʹ��vmm.dll����Ŀ�е�ͷ�ļ�����������Ϊ������Ŀ������Ϊvmm.dll�ı��ز����
//
// (c) Ulf Frisk, 2018-2019
// ���ߣ�Ulf Frisk, pcileech@frizk.net
//
// ͷ�ļ��汾��3.0

#include <windows.h>
#include "leechcore.h"

#pragma warning(disable : 4200)

#ifndef __VMMDLL_H__
#define __VMMDLL_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//-----------------------------------------------------------------------------
         // �����ǳ�ʼ�����ܣ�
        // ѡ��һ�ֳ�ʼ��VMM / �ڴ�����ļ�ϵͳ�ķ�ʽ��
//-----------------------------------------------------------------------------


/*

ʹ�������в�����ʼ�� VMM.DLL���йز����ĸ���ϸ��Ϣ������� MemProcFS �� LeechCore �� GitHub Wiki��
ע�⣡LeechCore �ĳ�ʼ�������Ըú���Ҳ����Ч�ġ�
��Ҫ����������
-printf = ��ʾ printf ��ʽ�������
-v -vv -vvv = �������ϸ�̶ȼ���
-device = �豸����ʽ�� LeechCore ��ʾ - ����� leechcore.h �� GitHub �ĵ��Ի�ȡ������Ϣ��һЩ���ܵ�ֵ������<file>��fpga��usb3380��hvsavedstate��totalmeltdown��pmem
-remote = Զ�̵� LeechCore ʵ�� - ����� leechcore.h �� GitHub �ĵ��Ի�ȡ������Ϣ��
-norefresh = ���ú�̨ˢ�£���ʹ���ڴ��Ƿ���ʧ���ڴ棩��
-symbolserverdisable = ���÷��ŷ�������ֱ���û�����Ϊֹ���˲�����������ע������á�
-pagefile[0-9] = �������ڴ�֮��Ҫʹ�õ�ҳ���ļ���ͨ�� pagefile.sys ������Ϊ 0��swapfile.sys ������Ϊ 1��ҳ���ļ��ڲ��ϱ仯�� - ����ڴ�ת����ҳ���ļ�֮���ʱ���������ӣ��벻Ҫʹ�á�
*-waitinitialize = �ڷ���֮ǰ�ȴ���ʼ����ɡ�ͨ������£�ĳЩ��ʼ�����첽���еģ�����ʼ���������ʱ������δ��ɡ�����������ڴ�ѹ����ע���ȡ�
* -- argc
* -- argv
* -- ����ֵ = �ɹ�/ʧ��
*/

_Success_(return)
BOOL VMMDLL_Initialize(_In_ DWORD argc, _In_ LPSTR argv[]);

/*
�ر��ѳ�ʼ����VMM.DLLʵ�����������з������Դ��������������ӵ�PCILeech.DLL�������ڴ���Դ��

����ֵ���ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_Close();

/*
ִ�ж������ڲ������ǿ��ˢ�£�������

�����б�
�ڴ滺��
ҳ����
���棺�˺���������ҪһЩʱ����ִ�У�

dwReserved������������ʹ�ã�����Ϊ�㡣
����ֵ���ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_Refresh(_In_ DWORD dwReserved);

/*
�ͷ���VMMDLL������ڴ档

pvMem��Ҫ�ͷŵ��ڴ�ָ�롣
*/
VOID VMMDLL_MemFree(_Frees_ptr_opt_ PVOID pvMem);


//-----------------------------------------------------------------------------
/// �������������ã�
// �����ڴ�����ļ�ϵͳ��ײ���ڴ��ȡ�豸��
//-----------------------------------------------------------------------------

/*
// �뺯��һ��ʹ�õ�ѡ�VMMDLL_ConfigGet �� VMMDLL_ConfigSet
// ѡ�����ʹ�ô�ͷ�ļ��е� VMMDLL_OPT_* �� leechcore.h �е� LEECHCORE_OPT_* ���ж���
// �йظ���ϸ����Ϣ������ĸ����豸���͵�Դ���롣
*/
#define VMMDLL_OPT_CORE_PRINTF_ENABLE                   0x80000001  // RW
#define VMMDLL_OPT_CORE_VERBOSE                         0x80000002  // RW
#define VMMDLL_OPT_CORE_VERBOSE_EXTRA                   0x80000003  // RW
#define VMMDLL_OPT_CORE_VERBOSE_EXTRA_TLP               0x80000004  // RW
#define VMMDLL_OPT_CORE_MAX_NATIVE_ADDRESS              0x80000005  // R
#define VMMDLL_OPT_CORE_SYSTEM                          0x80000007  // R
#define VMMDLL_OPT_CORE_MEMORYMODEL                     0x80000008  // R

#define VMMDLL_OPT_CONFIG_IS_REFRESH_ENABLED            0x40000001  // R - 1/0
#define VMMDLL_OPT_CONFIG_TICK_PERIOD                   0x40000002  // RW - base tick period in ms
#define VMMDLL_OPT_CONFIG_READCACHE_TICKS               0x40000003  // RW - memory cache validity period (in ticks)
#define VMMDLL_OPT_CONFIG_TLBCACHE_TICKS                0x40000004  // RW - page table (tlb) cache validity period (in ticks)
#define VMMDLL_OPT_CONFIG_PROCCACHE_TICKS_PARTIAL       0x40000005  // RW - process refresh (partial) period (in ticks)
#define VMMDLL_OPT_CONFIG_PROCCACHE_TICKS_TOTAL         0x40000006  // RW - process refresh (full) period (in ticks)
#define VMMDLL_OPT_CONFIG_VMM_VERSION_MAJOR             0x40000007  // R
#define VMMDLL_OPT_CONFIG_VMM_VERSION_MINOR             0x40000008  // R
#define VMMDLL_OPT_CONFIG_VMM_VERSION_REVISION          0x40000009  // R
#define VMMDLL_OPT_CONFIG_STATISTICS_FUNCTIONCALL       0x4000000A  // RW - enable function call statistics (.status/statistics_fncall file)
#define VMMDLL_OPT_CONFIG_IS_PAGING_ENABLED             0x4000000B  // RW - 1/0

#define VMMDLL_OPT_WIN_VERSION_MAJOR                    0x40000101  // R
#define VMMDLL_OPT_WIN_VERSION_MINOR                    0x40000102  // R
#define VMMDLL_OPT_WIN_VERSION_BUILD                    0x40000103  // R

static LPCSTR VMMDLL_MEMORYMODEL_TOSTRING[4] = { "N/A", "X86", "X86PAE", "X64" };

typedef enum tdVMMDLL_MEMORYMODEL_TP {
    VMMDLL_MEMORYMODEL_NA       = 0,
    VMMDLL_MEMORYMODEL_X86      = 1,
    VMMDLL_MEMORYMODEL_X86PAE   = 2,
    VMMDLL_MEMORYMODEL_X64      = 3
} VMMDLL_MEMORYMODEL_TP;

typedef enum tdVMMDLL_SYSTEM_TP {
    VMMDLL_SYSTEM_UNKNOWN_X64   = 1,
    VMMDLL_SYSTEM_WINDOWS_X64   = 2,
    VMMDLL_SYSTEM_UNKNOWN_X86   = 3,
    VMMDLL_SYSTEM_WINDOWS_X86   = 4
} VMMDLL_SYSTEM_TP;

/*
�����豸�ض���ѡ��ֵ������� VMMDLL_OPT_* �������˽���Ч��ѡ��ֵ��Ϣ����ע�⣬��ͬ�豸����֮���ѡ��ֵ�����ص������в�ͬ�ĺ��塣
-- fOption
-- pqwValue = ָ�� ULONG64 ��ָ�룬���ڽ���ѡ��ֵ��
-- ����ֵ = �ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_ConfigGet(_In_ ULONG64 fOption, _Out_ PULONG64 pqwValue);

/*
�����豸�ض���ѡ��ֵ������� VMMDLL_OPT_* �������˽���Ч��ѡ��ֵ��Ϣ����ע�⣬��ͬ�豸����֮���ѡ��ֵ�����ص������в�ͬ�ĺ��塣
-- fOption
-- qwValue
-- ����ֵ = �ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_ConfigSet(_In_ ULONG64 fOption, _In_ ULONG64 qwValue);



//-----------------------------------------------------------------------------
// VFS - �����ļ�ϵͳ�������£�
// �����ڴ�����ļ�ϵͳ�ĺ��ġ�ͨ��ʹ������Ĺ��ܣ����Զ��ļ�ϵͳ�������е�ʵ�ֺͷ�����
//-----------------------------------------------------------------------------

#define VMMDLL_STATUS_SUCCESS                       ((NTSTATUS)0x00000000L)
#define VMMDLL_STATUS_UNSUCCESSFUL                  ((NTSTATUS)0xC0000001L)
#define VMMDLL_STATUS_END_OF_FILE                   ((NTSTATUS)0xC0000011L)
#define VMMDLL_STATUS_FILE_INVALID                  ((NTSTATUS)0xC0000098L)
#define VMMDLL_STATUS_FILE_SYSTEM_LIMITATION        ((NTSTATUS)0xC0000427L)

#define VMMDLL_VFS_FILELIST_EXINFO_VERSION          1
#define VMMDLL_VFS_FILELIST_VERSION                 1

typedef struct tdVMMDLL_VFS_FILELIST_EXINFO {
    DWORD dwVersion;
    BOOL fCompressed;                   // set flag FILE_ATTRIBUTE_COMPRESSED - (no meaning but shows gui artifact in explorer.exe)
    union {
        FILETIME ftCreationTime;        // 0 = default time
        QWORD qwCreationTime;
    };
    union {
        FILETIME ftLastAccessTime;      // 0 = default time
        QWORD qwLastAccessTime;
    };
    union {
        FILETIME ftLastWriteTime;       // 0 = default time
        QWORD qwLastWriteTime;
    };
} VMMDLL_VFS_FILELIST_EXINFO, *PVMMDLL_VFS_FILELIST_EXINFO;

typedef struct tdVMMDLL_VFS_FILELIST {
    DWORD dwVersion;
    VOID(*pfnAddFile)     (_Inout_ HANDLE h, _In_opt_ LPSTR szName, _In_opt_ LPWSTR wszName, _In_ ULONG64 cb, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo);
    VOID(*pfnAddDirectory)(_Inout_ HANDLE h, _In_opt_ LPSTR szName, _In_opt_ LPWSTR wszName, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo);
    HANDLE h;
} VMMDLL_VFS_FILELIST, *PVMMDLL_VFS_FILELIST;

/*
* ���ڻص��� VMM_VFS_FILELIST �ṹ�ĸ�������������
*/
inline VOID VMMDLL_VfsList_AddFile(_In_ HANDLE pFileList, _In_opt_ LPSTR szName, _In_ ULONG64 cb)
{
    ((PVMMDLL_VFS_FILELIST)pFileList)->pfnAddFile(((PVMMDLL_VFS_FILELIST)pFileList)->h, szName, NULL, cb, NULL);
}

inline VOID VMMDLL_VfsList_AddFileEx(_In_ HANDLE pFileList, _In_opt_ LPSTR szName, _In_opt_ LPWSTR wszName, _In_ ULONG64 cb, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo)
{
    ((PVMMDLL_VFS_FILELIST)pFileList)->pfnAddFile(((PVMMDLL_VFS_FILELIST)pFileList)->h, szName, wszName, cb, pExInfo);
}

inline VOID VMMDLL_VfsList_AddDirectory(_In_ HANDLE pFileList, _In_opt_ LPSTR szName)
{
    ((PVMMDLL_VFS_FILELIST)pFileList)->pfnAddDirectory(((PVMMDLL_VFS_FILELIST)pFileList)->h, szName, NULL, NULL);
}

inline VOID VMMDLL_VfsList_AddDirectoryEx(_In_ HANDLE pFileList, _In_opt_ LPSTR szName, _In_opt_ LPWSTR wszName, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo)
{
    ((PVMMDLL_VFS_FILELIST)pFileList)->pfnAddDirectory(((PVMMDLL_VFS_FILELIST)pFileList)->h, szName, wszName, pExInfo);
}

inline BOOL VMMDLL_VfsList_IsHandleValid(_In_ HANDLE pFileList)
{
    return ((PVMMDLL_VFS_FILELIST)pFileList)->dwVersion == VMMDLL_VFS_FILELIST_VERSION;
}

/*
* ���ڴ�����ļ�ϵͳ���г�һ��Ŀ¼�е��ļ���ͨ���ص�������Ŀ¼���ļ������ṩ�� pFileList �����С������Ҫ��ȡ�����ļ�����Ϣ�������г�������Ŀ¼�е������ļ���
* -- wcsPath��Ŀ¼��·��
* -- pFileList�����ڽ����ļ��б�Ļص�����ָ��
* -- ����ֵ���ɹ�/ʧ��
*/
_Success_(return)
BOOL VMMDLL_VfsList(_In_ LPCWSTR wcsPath, _Inout_ PVMMDLL_VFS_FILELIST pFileList);

/*
* ���ڴ�����ļ�ϵͳ�ж�ȡ�ļ���ָ�����֡�
* -- wcsFileName���ļ���·��
* -- pb�����ڽ��ն�ȡ���ݵĻ�����ָ��
* -- cb��Ҫ��ȡ�����ݴ�С
* -- pcbRead��ʵ�ʶ�ȡ�����ݴ�С�����������
* -- cbOffset����ȡ����ʼƫ����
* -- ����ֵ���ɹ�/ʧ��
*
*/
NTSTATUS VMMDLL_VfsRead(_In_ LPCWSTR wcsFileName, _Out_ LPVOID pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset);

/*
* ���ڴ�����ļ�ϵͳ�е��ļ�д��ָ���������ݡ�
* -- wcsFileName���ļ���·��
* -- pb��Ҫд������ݵĻ�����ָ��
* -- cb��Ҫд������ݴ�С
* -- pcbWrite��ʵ��д������ݴ�С�����������
* -- cbOffset��д�����ʼƫ����
* -- ����ֵ���ɹ�/ʧ��
*/
NTSTATUS VMMDLL_VfsWrite(_In_ LPCWSTR wcsFileName, _In_ LPVOID pb, _In_ DWORD cb, _Out_ PDWORD pcbWrite, _In_ ULONG64 cbOffset);

/*
* ���ڶԲ�ͬ�ײ����ݱ�ʾ���ж�ȡ��д����ڴ�����ļ�ϵͳ��ʵ�ú�����
*/
NTSTATUS VMMDLL_UtilVfsReadFile_FromPBYTE(_In_ PBYTE pbFile, _In_ ULONG64 cbFile, _Out_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset);
NTSTATUS VMMDLL_UtilVfsReadFile_FromQWORD(_In_ ULONG64 qwValue, _Out_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset, _In_ BOOL fPrefix);
NTSTATUS VMMDLL_UtilVfsReadFile_FromDWORD(_In_ DWORD dwValue, _Out_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset, _In_ BOOL fPrefix);
NTSTATUS VMMDLL_UtilVfsReadFile_FromBOOL(_In_ BOOL fValue, _Out_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset);
NTSTATUS VMMDLL_UtilVfsWriteFile_BOOL(_Inout_ PBOOL pfTarget, _In_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbWrite, _In_ ULONG64 cbOffset);
NTSTATUS VMMDLL_UtilVfsWriteFile_DWORD(_Inout_ PDWORD pdwTarget, _In_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbWrite, _In_ ULONG64 cbOffset, _In_ DWORD dwMinAllow);


//-----------------------------------------------------------------------------
// ������������£�
// ���ڳ�ʼ����ʹ���ڴ�����ļ�ϵͳ������ܵĺ����ͽṹ��ͨ�����ú���VMM_VfsInitializePlugins�����������������ÿ�����ò�����ⲿ�����
// ��DLL������m_*.dllƥ��Ĳ������������InitializeVmmPlugin���������/ģ����Ծ�������pfnPluginManager_Register��ע���Բ�ͬ������ʽ�Ĳ��һ�λ��Ρ�
// �����ǲ��DLL�е�ע�ắ��ʾ����
// 'VOID InitializeVmmPlugin(_In_ PVMM_PLUGIN_REGINFO pRegInfo)'
//-----------------------------------------------------------------------------

/*
* ��ʼ�����п��ܵĲ�����������ò�����ⲿ�������Щ���ӳ�䵽�ڴ�����ļ�ϵͳ����ע�⣬Ĭ������²�����ز�� - 
   ����ͨ�����ô˺�����ʽ���ز�������ǽ��� VMM DLL �Ĺر�ʱ��ж�ء�
*/
_Success_(return)
BOOL VMMDLL_VfsInitializePlugins();

#define VMMDLL_PLUGIN_CONTEXT_MAGIC             0xc0ffee663df9301c
#define VMMDLL_PLUGIN_CONTEXT_VERSION           3
#define VMMDLL_PLUGIN_REGINFO_MAGIC             0xc0ffee663df9301d
#define VMMDLL_PLUGIN_REGINFO_VERSION           4

#define VMMDLL_PLUGIN_EVENT_VERBOSITYCHANGE     0x01
#define VMMDLL_PLUGIN_EVENT_TOTALREFRESH        0x02

typedef struct tdVMMDLL_PLUGIN_CONTEXT {
    ULONG64 magic;
    WORD wVersion;
    WORD wSize;
    DWORD dwPID;
    PVOID pProcess;
    LPWSTR wszModule;
    LPWSTR wszPath;
    PVOID pvReserved1;
    PVOID pvReserved2;
} VMMDLL_PLUGIN_CONTEXT, *PVMMDLL_PLUGIN_CONTEXT;

typedef struct tdVMMDLL_PLUGIN_REGINFO {
    ULONG64 magic;
    WORD wVersion;
    WORD wSize;
    VMMDLL_MEMORYMODEL_TP tpMemoryModel;
    VMMDLL_SYSTEM_TP tpSystem;
    HMODULE hDLL;
    HMODULE hReservedDllPython3X;   // not for general use (only used for python).
    BOOL(*pfnPluginManager_Register)(struct tdVMMDLL_PLUGIN_REGINFO *pPluginRegInfo);
    HMODULE hReservedDllPython3;   // not for general use (only used for python).
    PVOID pvReserved2;
    // general plugin registration info to be filled out by the plugin below:
    struct {
        WCHAR wszModuleName[32];
        BOOL fRootModule;
        BOOL fProcessModule;
        PVOID pvReserved1;
        PVOID pvReserved2;
    } reg_info;
    // function plugin registration info to be filled out by the plugin below:
    struct {
        BOOL(*pfnList)(_In_ PVMMDLL_PLUGIN_CONTEXT ctx, _Inout_ PHANDLE pFileList);
        NTSTATUS(*pfnRead)(_In_ PVMMDLL_PLUGIN_CONTEXT ctx, _Out_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead,  _In_ ULONG64 cbOffset);
        NTSTATUS(*pfnWrite)(_In_ PVMMDLL_PLUGIN_CONTEXT ctx, _In_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbWrite, _In_ ULONG64 cbOffset);
        VOID(*pfnNotify)(_In_ DWORD fEvent, _In_opt_ PVOID pvEvent, _In_opt_ DWORD cbEvent);
        VOID(*pfnClose)();
        PVOID pvReserved1;
        PVOID pvReserved2;
    } reg_fn;
} VMMDLL_PLUGIN_REGINFO, *PVMMDLL_PLUGIN_REGINFO;

//-----------------------------------------------------------------------------
// VMM ���Ĺ������ȡ����д�룩�����ڴ�������ڴ档ע�⣡д��ֻ��Ŀ����֧�ֵ�����²���Ч���������ڴ�ת���ļ�...
// ��Ҫ��ȡ�����ڴ棬�뽫 dwPID ָ��Ϊ (DWORD)-1

// �����ڵ��� VMM_MemReadEx() ʱ��ֹĬ�϶�ȡ����ı�־���⽫���¶�ȡ���Ǵ�Ŀ��ϵͳ��ȡ��
// ��Ȼʹ�û����ҳ�����������ַ�������ַ��ת������
#define VMMDLL_FLAG_NOCACHE                        0x0001  // ��ʹ�����ݻ��棨ǿ�ƴ��ڴ��ȡ�豸��ȡ��
#define VMMDLL_FLAG_ZEROPAD_ON_FAIL                0x0002  // ʧ�ܵ������ڴ��ȡ��������䣬���������ȡ�������ڴ淶Χ�ڣ�����ɹ���
#define VMMDLL_FLAG_FORCECACHE_READ                0x0008  // ǿ��ʹ�û��� - �Էǻ���ҳ�����ʧ�ܴ��� - �������ڶ�ȡ���� VMMDLL_FLAG_NOCACHE/VMMDLL_FLAG_ZEROPAD_ON_FAIL �����ݡ�
#define VMMDLL_FLAG_NOPAGING                       0x0010  // �����Դӷ�ҳ�ļ�/ѹ���м�����ҳ�����ڴ棨��ʹ���ܣ�
#define VMMDLL_FLAG_NOPAGING_IO                    0x0020  // �����Դӷ�ҳ�����ڴ��м����ڴ棬�����ȡ����������� I/O����ʹ���ܣ���

/*
* ���� ppMEMs �����е���Ŀָ��ָ���ĸ���������λ�ö�ȡ�ڴ档ÿ����Ԫ�Ľ�������������û�ж�ȡ��Ŀ���������ޣ�
   ���������Ӳ�����ƣ��������ṩ����������ÿ����Ԫ������СΪ4KBҳ�棨4096�ֽڣ���
* -- dwPID - Ŀ����̵� PID��(DWORD)-1 ���ڶ�ȡ�����ڴ档
* -- ppMEMs = ��ɢ��ȡͷ�����顣
* -- cpMEMs = ppMEMs ��������
* -- flags = ��ѡ�ı�־���� VMMDLL_FLAG_* ��ָ����
* -- return = �ɹ���ȡ����Ŀ������
*/
DWORD VMMDLL_MemReadScatter(_In_ DWORD dwPID, _Inout_ PPMEM_IO_SCATTER_HEADER ppMEMs, _In_ DWORD cpMEMs, _In_ DWORD flags);

/*
* ��ȡһ��4096�ֽڵ��ڴ�ҳ�档
* -- dwPID - Ŀ����̵�PID��(DWORD)-1 ���ڶ�ȡ�����ڴ档
* -- qwA
* -- pbPage
* -- return = �ɹ�/ʧ�ܣ�ȡ�����Ƿ��ȡ������������ֽڣ���
*/
_Success_(return)
BOOL VMMDLL_MemReadPage(_In_ DWORD dwPID, _In_ ULONG64 qwA, _Inout_bytecount_(4096) PBYTE pbPage);

/*
����������ڶ�ȡָ����ַ��ʼ�������ڴ����ݡ�

- `dwPID` ��Ŀ����̵�PID�������������Ϊ(DWORD)-1�����ʾ��ȡ�����ڴ�����ض����̵������ڴ档
- `qwA` ��Ҫ��ȡ����ʼ��ַ��
- `pb` �����ڽ��ն�ȡ���ݵĻ�������ָ�롣
- `cb` ��ʾҪ��ȡ���ֽ�����

��������һ������ֵ����ʾ��ȡ�����Ƿ�ɹ����������������ֽڶ��ɹ���ȡ���򷵻سɹ������򷵻�ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_MemRead(_In_ DWORD dwPID, _In_ ULONG64 qwA, _Out_writes_(cb) PBYTE pb, _In_ DWORD cb);

/*
����������ڴ�ָ����ַ��ʼ������ȡһ���ڴ����ݣ�����ʵ�ʶ�ȡ���ֽ���ͨ�� pcbRead �������ء�
dwPID ��Ŀ����̵�PID�������������Ϊ(DWORD)-1�����ʾ��ȡ�����ڴ�����ض����̵������ڴ档
qwA ��Ҫ��ȡ����ʼ��ַ��
pb �����ڽ��ն�ȡ���ݵĻ�������ָ�롣
cb ��ʾҪ��ȡ���ֽ�����
pcbRead ��һ��ָ�룬���ڽ���ʵ�ʶ�ȡ���ֽ�����
flags ��һ���־������ʹ�� VMMDLL_FLAG_* �ж���ı�־�������á�
��������һ������ֵ����ʾ��ȡ�����Ƿ�ɹ�����ע�⣬��ʹ��ȡ��0���ֽڣ���ȡ�������ܻᱨ��ɹ�����˽���ʹ�� pcbRead ��������֤ʵ�ʶ�ȡ���ֽ�����
*/
_Success_(return)
BOOL VMMDLL_MemReadEx(_In_ DWORD dwPID, _In_ ULONG64 qwA, _Out_writes_(cb) PBYTE pb, _In_ DWORD cb, _Out_opt_ PDWORD pcbReadOpt, _In_ ULONG64 flags);

/*
* Ԥȡһ���ַ���� pA ������ָ�������ڴ滺���С��ú������ڽ��ϴ����֪��ȡ����Ԥȡ�����ػ����У�Ȼ��ӻ����н��ж�ν�С�Ķ�ȡ��
�ú����Ĵ�����Ϊ��������ܡ�
* -- dwPID = Ŀ����̵�PID��(DWORD)-1 ���������ڴ档
* -- pPrefetchAddresses = ҪԤȡ�������еĵ�ַ���顣
* -- cPrefetchAddresses = ��ַ������
*/
_Success_(return)
BOOL VMMDLL_MemPrefetchPages(_In_ DWORD dwPID, _In_reads_(cPrefetchAddresses) PULONG64 pPrefetchAddresses, _In_ DWORD cPrefetchAddresses);

/*
* д�������������������ڴ档��ע�⣬ĳЩ�����ڴ棨�����ִ���ļ���ҳ�棬��DLL�������ڲ�ͬ����֮�乲����Ϊʾ������һ�������е�
kernel32.dll��д����ܻ�Ӱ������ϵͳ�����н����е�kernel32���ѡ�ջ�������ڴ�ͨ���ǿ��԰�ȫд��ġ�����д���ڴ�ʱС�Ĳ�����

������
- dwPID: Ŀ����̵�PID��ʹ��(DWORD)-1����ʾ��ȡ�����ڴ档
- qwA: д�����ʼ��ַ��
- pb: Ҫд������ݻ�������
- cb: Ҫд����ֽ�����
- ����ֵ: �ɹ�ʱ����TRUE�����ֻ���д��ʱ����FALSE��
*/
_Success_(return)
BOOL VMMDLL_MemWrite(_In_ DWORD dwPID, _In_ ULONG64 qwA, _In_reads_(cb) PBYTE pb, _In_ DWORD cb);

/*
* ͨ������ָ�����̵�ҳ�������ַת��Ϊ�����ַ��
* -- dwPID: Ŀ����̵�PID��
* -- qwVA: Ҫת���������ַ��
* -- pqwPA: ����ת����������ַ��ָ�롣
* -- ����ֵ: �ɹ�ʱ����TRUE��ʧ��ʱ����FALSE��
*/
_Success_(return)
BOOL VMMDLL_MemVirt2Phys(_In_ DWORD dwPID, _In_ ULONG64 qwVA, _Out_ PULONG64 pqwPA);



//-----------------------------------------------------------------------------
// VMM ����ӳ�书�����£�
// ���ڼ����������ص���Ŀ���ϵĹ��ܣ�����ҳ��ӳ�䣨PTE���������ַ������ӳ�䣨VAD�������ص�ģ�飬�Ѻ��̡߳�
//-----------------------------------------------------------------------------

#define VMMDLL_MAP_PTE_VERSION              1
#define VMMDLL_MAP_VAD_VERSION              1
#define VMMDLL_MAP_MODULE_VERSION           1
#define VMMDLL_MAP_HEAP_VERSION             1
#define VMMDLL_MAP_THREAD_VERSION           1
#define VMMDLL_MAP_HANDLE_VERSION           1

// flags to check for existence in the fPage field of VMMDLL_MAP_PTEENTRY
#define VMMDLL_MEMMAP_FLAG_PAGE_W          0x0000000000000002
#define VMMDLL_MEMMAP_FLAG_PAGE_NS         0x0000000000000004
#define VMMDLL_MEMMAP_FLAG_PAGE_NX         0x8000000000000000
#define VMMDLL_MEMMAP_FLAG_PAGE_MASK       0x8000000000000006

typedef struct tdVMMDLL_MAP_PTEENTRY {
    QWORD vaBase;
    QWORD cPages;
    QWORD fPage;
    BOOL  fWoW64;
    DWORD cwszText;
    LPWSTR wszText;
    DWORD _Reserved1[2];
} VMMDLL_MAP_PTEENTRY, *PVMMDLL_MAP_PTEENTRY;

typedef struct tdVMMDLL_MAP_VADENTRY {
    QWORD vaStart;
    QWORD vaEnd;
    QWORD vaVad;
    // DWORD 0
    DWORD VadType           : 3;   // Pos 0
    DWORD Protection        : 5;   // Pos 3
    DWORD fImage            : 1;   // Pos 8
    DWORD fFile             : 1;   // Pos 9
    DWORD fPageFile         : 1;   // Pos 10
    DWORD fPrivateMemory    : 1;   // Pos 11
    DWORD fTeb              : 1;   // Pos 12
    DWORD fStack            : 1;   // Pos 13
    DWORD fSpare            : 2;   // Pos 14
    DWORD HeapNum           : 7;   // Pos 16
    DWORD fHeap             : 1;   // Pos 23
    DWORD cwszDescription   : 8;   // Pos 24
    // DWORD 1
    DWORD CommitCharge      : 31;   // Pos 0
    DWORD MemCommit         : 1;    // Pos 31
    DWORD u2;
    DWORD cbPrototypePte;
    QWORD vaPrototypePte;
    QWORD vaSubsection;
    LPWSTR wszText;                 // optional LPWSTR pointed into VMMDLL_MAP_VAD.wszMultiText
    DWORD cwszText;                 // WCHAR count not including terminating null
    DWORD _Reserved1;
} VMMDLL_MAP_VADENTRY, *PVMMDLL_MAP_VADENTRY;

typedef struct tdVMMDLL_MAP_MODULEENTRY {
    QWORD vaBase;//ģ�����ַ
    QWORD vaEntry;//ģ����ڵ�
    DWORD cbImageSize;//ͼ���С
    BOOL  fWoW64;//�Ƿ�ΪWoW64����
    LPWSTR wszText;//ģ���ı�
    DWORD cwszText;                 // wchar count not including terminating null
    DWORD _Reserved1[7];
} VMMDLL_MAP_MODULEENTRY, *PVMMDLL_MAP_MODULEENTRY;

typedef struct tdVMMDLL_MAP_HEAPENTRY {
    QWORD vaHeapSegment;
    DWORD cPages;
    DWORD cPagesUnCommitted : 24;
    DWORD HeapId : 7;
    DWORD fPrimary : 1;
} VMMDLL_MAP_HEAPENTRY, *PVMMDLL_MAP_HEAPENTRY;

typedef struct tdVMMDLL_MAP_THREADENTRY {
    DWORD dwTID;
    DWORD dwPID;
    DWORD dwExitStatus;
    UCHAR bState;
    UCHAR bRunning;
    UCHAR bPriority;
    UCHAR bBasePriority;
    QWORD vaETHREAD;
    QWORD vaTeb;
    QWORD ftCreateTime;
    QWORD ftExitTime;
    QWORD vaStartAddress;
    QWORD vaStackBaseUser;
    QWORD vaStackLimitUser;
    QWORD vaStackBaseKernel;
    QWORD vaStackLimitKernel;
    DWORD _FutureUse[10];
} VMMDLL_MAP_THREADENTRY, *PVMMDLL_MAP_THREADENTRY;

typedef struct tdVMMDLL_MAP_HANDLEENTRY {
    QWORD vaObject;
    DWORD dwHandle;
    DWORD dwGrantedAccess : 24;
    DWORD iType : 8;
    QWORD qwHandleCount;
    QWORD qwPointerCount;
    QWORD vaObjectCreateInfo;
    QWORD vaSecurityDescriptor;
    LPWSTR wszText;
    DWORD cwszText;
    DWORD dwPID;
    DWORD dwPoolTag;
    DWORD _FutureUse[4];
    DWORD cwszType;
    LPWSTR wszType;
} VMMDLL_MAP_HANDLEENTRY, *PVMMDLL_MAP_HANDLEENTRY;

typedef struct tdVMMDLL_MAP_PTE {
    DWORD dwVersion;
    DWORD _Reserved1[5];
    LPWSTR wszMultiText;            // NULL or multi-wstr pointed into by VMMDLL_MAP_VADENTRY.wszText
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_PTEENTRY pMap[];     // map entries.
} VMMDLL_MAP_PTE, *PVMMDLL_MAP_PTE;

typedef struct tdVMMDLL_MAP_VAD {
    DWORD dwVersion;
    DWORD _Reserved1[5];
    LPWSTR wszMultiText;            // NULL or multi-wstr pointed into by VMMDLL_MAP_VADENTRY.wszText
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_VADENTRY pMap[];     // map entries.
} VMMDLL_MAP_VAD, *PVMMDLL_MAP_VAD;

typedef struct tdVMMDLL_MAP_MODULE {
    DWORD dwVersion;
    DWORD _Reserved1[5];
    LPWSTR wszMultiText;            // multi-wstr pointed into by VMMDLL_MAP_MODULEENTRY.wszText
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_MODULEENTRY pMap[];  // map entries.
} VMMDLL_MAP_MODULE, *PVMMDLL_MAP_MODULE; 

typedef struct tdVMMDLL_MAP_HEAP {
    DWORD dwVersion;
    DWORD _Reserved1[8];
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_HEAPENTRY pMap[];    // map entries.
} VMMDLL_MAP_HEAP, *PVMMDLL_MAP_HEAP;

typedef struct tdVMMDLL_MAP_THREAD {
    DWORD dwVersion;
    DWORD _Reserved[8];
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_THREADENTRY pMap[];  // map entries.
} VMMDLL_MAP_THREAD, *PVMMDLL_MAP_THREAD;

typedef struct tdVMMDLL_MAP_HANDLE {
    DWORD dwVersion;
    DWORD _Reserved1[5];
    LPWSTR wszMultiText;            // multi-wstr pointed into by VMMDLL_MAP_HANDLEENTRY.wszText
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_HANDLEENTRY pMap[];  // map entries.
} VMMDLL_MAP_HANDLE, *PVMMDLL_MAP_HANDLE;

/*
* ���ݽ��̵�Ӳ��ҳ��PTE�������ڴ�ӳ����Ŀ������� pPteMap ����Ϊ NULL���򽫷���������ֽ����ڲ��� pcbPteMap �С�
* ���ص���Ŀ���� VMMDLL_MAP_PTEENTRY.vaBase ��������
* -- dwPID
* -- pPteMap = ��С�ֽڳ���Ϊ *pcbPteMap �Ļ�����������Ϊ NULL��
* -- pcbPteMap = ָ�� pPteMap ���������ֽ�����ָ�롣
* -- fIdentifyModules = ����ʶ��ģ�飨= ������
* -- return = �ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetPte(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbPteMap) PVMMDLL_MAP_PTE pPteMap, _Inout_ PDWORD pcbPteMap, _In_ BOOL fIdentifyModules);

/*
* ���ݽ��̵������ַ��������VAD�������ڴ�ӳ����Ŀ������� pVadMap ����Ϊ NULL���򽫷���������ֽ����ڲ��� pcbVadMap �С�
* ���ص���Ŀ���� VMMDLL_MAP_VADENTRY.vaStart ��������
* -- dwPID
* -- pVadMap = ��С�ֽڳ���Ϊ *pcbVadMap �Ļ�����������Ϊ NULL��
* -- pcbVadMap = ָ�� pVadMap ���������ֽ�����ָ�롣
* -- fIdentifyModules = ����ʶ��ģ�飨= ������
* -- return = �ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetVad(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbVadMap) PVMMDLL_MAP_VAD pVadMap, _Inout_ PDWORD pcbVadMap, _In_ BOOL fIdentifyModules);

/*
* ����ָ�����̵�ģ�飨.dll��������� pModuleMap ����Ϊ NULL���򽫷���������ֽ����ڲ��� pcbModuleMap �С�
* -- dwPID
* -- pModuleMap = ��С�ֽڳ���Ϊ *pcbModuleMap �Ļ�����������Ϊ NULL��
* -- pcbModuleMap = ָ�� pModuleMap ���������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetModule(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbModuleMap) PVMMDLL_MAP_MODULE pModuleMap, _Inout_ PDWORD pcbModuleMap);

/*
* ���ݽ��̺�ģ�����Ƽ���ģ��ӳ����Ŀ��.exe / .dll����
* ע�⣡�˺����������� PVMMDLL_MAP_MODULEENTRY->wszText��
* �����Ҫģ�����ƣ���ʹ�� VMMDLL_ProcessMap_GetModule()��
* -- dwPID
* -- szModuleName
* -- pModuleMapEntry
* -- return = �ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetModuleFromName(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _Out_ PVMMDLL_MAP_MODULEENTRY pModuleMapEntry);

/*
* ����ָ�����̵Ķ���Ϣ����� pHeapMap ����Ϊ NULL�����ڲ��� pcbHeapMap �з���������ֽ�����
* -- dwPID
* -- pHeapMap = ���ٳ���Ϊ *pcbHeapMap �Ļ����������Ϊ NULL���򷵻�������ֽ�����
* -- pcbHeapMap = ָ�� pHeapMap ���������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetHeap(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbHeapMap) PVMMDLL_MAP_HEAP pHeapMap, _Inout_ PDWORD pcbHeapMap);

/*
* ����ָ�����̵��߳���Ϣ����� pThreadMap ����Ϊ NULL�����ڲ��� pcbThreadMap �з���������ֽ�����
* ���ص���Ŀ�� VMMDLL_MAP_THREADENTRY.dwTID ����
* -- dwPID
* -- pThreadMap = ���ٳ���Ϊ *pcbThreadMap �Ļ����������Ϊ NULL���򷵻�������ֽ�����
* -- pcbThreadMap = ָ�� pThreadMap ���������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetThread(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbThreadMap) PVMMDLL_MAP_THREAD pThreadMap, _Inout_ PDWORD pcbThreadMap);

/*
* ����ָ�����̵ľ����Ϣ����� pHandleMap ����Ϊ NULL�����ڲ��� pcbHandleMap �з���������ֽ�����
* ���ص���Ŀ�� VMMDLL_MAP_HANDLEENTRY.dwHandle ����
* -- dwPID
* -- pHandleMap = ���ٳ���Ϊ *pcbHandleMap �Ļ����������Ϊ NULL���򷵻�������ֽ�����
* -- pcbHandleMap = ָ�� pHandleMap ���������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetHandle(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbHandleMap) PVMMDLL_MAP_HANDLE pHandleMap, _Inout_ PDWORD pcbHandleMap);



//-----------------------------------------------------------------------------
/*
* ���ݽ������Ƽ�������̡���ע�⣬������ڶ��������ͬ���ƵĽ��̣�ֻ�᷵��һ�����̡�
* �����Ҫ����������ͬ���Ƶ����н��̣���ͨ������ VMMDLL_PidList ����� VMMDLL_ProcessGetInformation ���� PID �б�
* -- szProcName = �������ƣ���Сд�����У���
* -- pdwPID = �ɹ�ʱ�����յ��� PID ��ָ�롣
* -- ����ֵ
*/
_Success_(return)
BOOL VMMDLL_PidGetFromName(_In_ LPSTR szProcName, _Out_ PDWORD pdwPID);

/*
* �г�ϵͳ�еĽ��̱�ʶ����PIDs����
* -- pPIDs = ���پ���ϵͳ�н��������� DWORD ���飬����Ϊ NULL��
* -- pcPIDs = pPIDs ����Ĵ�С���� DWORD ������ʾ��������ʱΪ����Ĵ�С�����ʱΪϵͳ�еĽ���������
* -- return = �ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_PidList(_Out_writes_opt_(*pcPIDs) PDWORD pPIDs, _Inout_ PULONG64 pcPIDs); 

#define VMMDLL_PROCESS_INFORMATION_MAGIC        0xc0ffee663df9301e
#define VMMDLL_PROCESS_INFORMATION_VERSION      5

typedef struct tdVMMDLL_PROCESS_INFORMATION {
    ULONG64 magic;
    WORD wVersion;
    WORD wSize;
    VMMDLL_MEMORYMODEL_TP tpMemoryModel;    // as given by VMMDLL_MEMORYMODEL_* enum
    VMMDLL_SYSTEM_TP tpSystem;              // as given by VMMDLL_SYSTEM_* enum
    BOOL fUserOnly;                         // only user mode pages listed
    DWORD dwPID;
    DWORD dwPPID;
    DWORD dwState;
    CHAR szName[16];
    CHAR szNameLong[64];
    ULONG64 paDTB;
    ULONG64 paDTB_UserOpt;                  // may not exist
    union {
        struct {
            ULONG64 vaEPROCESS;
            ULONG64 vaPEB;
            ULONG64 _Reserved1;
            BOOL fWow64;
            DWORD vaPEB32;                  // WoW64 only
        } win;
    } os;
} VMMDLL_PROCESS_INFORMATION, *PVMMDLL_PROCESS_INFORMATION;

/*
* �ӽ��̱�ʶ����PID���л�ȡ���ֽ�����Ϣ�����Ի�ȡ���̵����ơ�ҳĿ¼��ַ�ͽ���״̬����Ϣ��
* -- dwPID
* -- pProcessInformation = ���Ϊ NULL������ *pcbProcessInformation �и�����С
* -- pcbProcessInformation = pProcessInformation �Ĵ�С�����ֽ�Ϊ��λ������������ʱʹ��
* -- return = �ɹ�/ʧ�ܡ�
*/
_Success_(return)
BOOL VMMDLL_ProcessGetInformation(_In_ DWORD dwPID, _Inout_opt_ PVMMDLL_PROCESS_INFORMATION pProcessInformation, _In_ PSIZE_T pcbProcessInformation);

#define VMMDLL_PROCESS_INFORMATION_OPT_STRING_PATH_KERNEL           1
#define VMMDLL_PROCESS_INFORMATION_OPT_STRING_PATH_USER_IMAGE       2
#define VMMDLL_PROCESS_INFORMATION_OPT_STRING_CMDLINE               3

/*
* ��ȡ���̵��ַ���ֵ���ú��������һ���µ��ַ���������������������ַ����������С��ַ���ʼ���� NULL ��β�����ʧ�ܣ������� NULL��
* ע�⣡�����߸����� VMMDLL_MemFree �ķ���ֵ��
* �������ͷ��ڴ棺VMMDLL_MemFree(return)
* -- dwPID
* -- fOptionString = Ҫ�������ַ���ֵ���� VMMDLL_PROCESS_INFORMATION_OPT_STRING_* ����
* -- ����ֵ - ʧ��: NULL���ɹ�: �ַ��� - ע�⣡�����߱���ʹ�� VMMDLL_MemFree �����ڴ��ͷţ�
*/
LPSTR VMMDLL_ProcessGetInformationString(_In_ DWORD dwPID, _In_ DWORD fOptionString);

typedef struct tdVMMDLL_EAT_ENTRY {
    ULONG64 vaFunction;
    DWORD vaFunctionOffset;
    CHAR szFunction[40];
} VMMDLL_EAT_ENTRY, *PVMMDLL_EAT_ENTRY;

typedef struct tdVMMDLL_IAT_ENTRY {
    ULONG64 vaFunction;
    CHAR szFunction[40];
    CHAR szModule[64];
} VMMDLL_IAT_ENTRY, *PVMMDLL_IAT_ENTRY;

/*
* ��ȡ�й�����Ŀ¼���ڡ�������ַ��͵����ַ��IAT������Ϣ��
* ���������ʱ pData == NULL���򷵻� pData ������Ҫ���е���Ŀ���Ա��ܹ��������ݡ�
* -- dwPID
* -- wszModule
* -- pData
* -- cData
* -- pcData
* -- ����ֵ = �ɹ�/ʧ�ܡ�
*/
_Success_(return) 
BOOL VMMDLL_ProcessGetDirectories(_In_ DWORD dwPID, _In_ LPWSTR wszModule, _Out_writes_(16) PIMAGE_DATA_DIRECTORY pData, _In_ DWORD cData, _Out_ PDWORD pcData);
_Success_(return)
BOOL VMMDLL_ProcessGetSections(_In_ DWORD dwPID, _In_ LPWSTR wszModule, _Out_opt_ PIMAGE_SECTION_HEADER pData, _In_ DWORD cData, _Out_ PDWORD pcData);
_Success_(return)
BOOL VMMDLL_ProcessGetEAT(_In_ DWORD dwPID, _In_ LPWSTR wszModule, _Out_opt_ PVMMDLL_EAT_ENTRY pData, _In_ DWORD cData, _Out_ PDWORD pcData);
_Success_(return)
BOOL VMMDLL_ProcessGetIAT(_In_ DWORD dwPID, _In_ LPWSTR wszModule, _Out_opt_ PVMMDLL_IAT_ENTRY pData, _In_ DWORD cData, _Out_ PDWORD pcData);

/*
* dwPID: Ŀ����̵�PID��Process ID����
* wszModuleName: Ŀ��ģ������ơ�
* szFunctionName: Ŀ�꺯�������ơ�
* ����ֵ: Ŀ�꺯���������ַ�ռ��еĵ�ַ��ʧ��ʱ�����㡣
*/
ULONG64 VMMDLL_ProcessGetProcAddress(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _In_ LPSTR szFunctionName);

/*
* dwPID: Ŀ����̵�PID��Process ID����
* wszModuleName: Ŀ��ģ������ơ�
* ����ֵ: Ŀ��ģ��Ļ���ַ���������ַ�ռ��еĵ�ַ����ʧ��ʱ�����㡣
*/
ULONG64 VMMDLL_ProcessGetModuleBase(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName);



//-----------------------------------------------------------------------------
// WINDOWS SPECIFIC DEBUGGING / SYMBOL FUNCTIONALITY BELOW:
//-----------------------------------------------------------------------------

/*
* szModule: ģ�����ơ�
* szSymbolName: �������ơ�
* pvaSymbolAddress: ���ڴ洢���ŵ������ַ��ָ�롣
* ����ֵ: �ɹ���ʧ�ܡ�����ɹ���pvaSymbolAddress ���������ŵ������ַ�����ʧ�ܣ�pvaSymbolAddress �����ֲ��䡣
*/
_Success_(return)
BOOL VMMDLL_PdbSymbolAddress(_In_ LPSTR szModule, _In_ LPSTR szSymbolName, _Out_ PULONG64 pvaSymbolAddress);

/*
* szModule: ģ�����ơ�
* szTypeName: �������ơ�
* pcbTypeSize: ���ڴ洢���ʹ�С��ָ�롣
* ����ֵ: �ɹ���ʧ�ܡ�����ɹ���pcbTypeSize ���������͵Ĵ�С�����ʧ�ܣ�pcbTypeSize �����ֲ��䡣
*/
_Success_(return)
BOOL VMMDLL_PdbTypeSize(_In_ LPSTR szModule, _In_ LPSTR szTypeName, _Out_ PDWORD pcbTypeSize);

/*
* szModule: ģ�����ơ�
* szTypeName: �������ơ�
* wszTypeChildName: ������������ơ�
* pcbTypeChildOffset: ���ڴ洢��������ƫ������ָ�롣
* ����ֵ: �ɹ���ʧ�ܡ�����ɹ���pcbTypeChildOffset ���������������ƫ���������ʧ�ܣ�pcbTypeChildOffset �����ֲ��䡣
*/
_Success_(return)
BOOL VMMDLL_PdbTypeChildOffset(_In_ LPSTR szModule, _In_ LPSTR szTypeName, _In_ LPWSTR wszTypeChildName, _Out_ PDWORD pcbTypeChildOffset);



//-----------------------------------------------------------------------------
// WINDOWS SPECIFIC REGISTRY FUNCTIONALITY BELOW:
//-----------------------------------------------------------------------------

#define VMMDLL_REGISTRY_HIVE_INFORMATION_MAGIC      0xc0ffee653df8d01e
#define VMMDLL_REGISTRY_HIVE_INFORMATION_VERSION    1

typedef struct td_VMMDLL_REGISTRY_HIVE_INFORMATION {
    ULONG64 magic;
    WORD wVersion;
    WORD wSize;
    BYTE _FutureReserved1[0x14];
    ULONG64 vaCMHIVE;
    ULONG64 vaHBASE_BLOCK;
    DWORD cbLength;
    CHAR szName[136 + 1];
    WCHAR wszNameShort[32 + 1];
    WCHAR wszHiveRootPath[MAX_PATH];
    ULONG64 _FutureReserved2[0x10];
} VMMDLL_REGISTRY_HIVE_INFORMATION, *PVMMDLL_REGISTRY_HIVE_INFORMATION;

/*
* pHives: ���ڽ�������ע��� hive ��Ϣ�Ļ���������СΪ cHives * sizeof(VMMDLL_REGISTRY_HIVE_INFORMATION)��������� NULL����᷵��ע��� hive �������� pcHives��
* cHives: pHives �������������ɵ�ע��� hive ��������
* pcHives: ��� pHives Ϊ NULL�����ڷ���ʱ����ϵͳ���ܹ���ע��� hive ��������� pHives ��Ϊ NULL�����ڷ���ʱ�����Ѷ�ȡ��ע��� hive ������
* ����ֵ: �ɹ���ʧ�ܡ�����ɹ������ض�ȡ��ע��� hive �����������ʧ�ܣ����� 0��
*/
_Success_(return)
BOOL VMMDLL_WinReg_HiveList(_Out_writes_(cHives) PVMMDLL_REGISTRY_HIVE_INFORMATION pHives, _In_ DWORD cHives, _Out_ PDWORD pcHives);

/*
* vaCMHive: ע��� hive �ڴ����ʼ�����ַ��
* ra: ע��� hive �ڴ��������ʼ��ַ��
* pb: ���������ڽ��ն�ȡ�����ݡ�
* cb: Ҫ��ȡ���ֽ�����
* pcbReadOpt: �ڷ���ʱ����ʵ�ʶ�ȡ���ֽ�����
* flags: ��ѡ�ı�־���� VMMDLL_FLAG_* �еı�־��ͬ��
* ����ֵ: �ɹ���ʧ�ܡ�����ɹ������ض�ȡ�����ݵ��ֽ��������ʧ�ܣ����� 0��ע�⣬��ʹ��ȡ���ֽ���Ϊ 0����ȡ����Ҳ���ܱ���Ϊ�ɹ���������֤ pcbReadOpt ������ȷ��ʵ�ʶ�ȡ���ֽ�����
*/
_Success_(return)
BOOL VMMDLL_WinReg_HiveReadEx(_In_ ULONG64 vaCMHive, _In_ DWORD ra, _Out_ PBYTE pb, _In_ DWORD cb, _Out_opt_ PDWORD pcbReadOpt, _In_ ULONG64 flags);

/*
* Write a virtually contigious arbitrary amount of memory to a registry hive.
* NB! Address space does not include regf registry hive file header!
* -- vaCMHive
* -- ra
* -- pb
* -- cb
* -- return = TRUE on success, FALSE on partial or zero write.
*/
_Success_(return)
BOOL VMMDLL_WinReg_HiveWrite(_In_ ULONG64 vaCMHive, _In_ DWORD ra, _In_ PBYTE pb, _In_ DWORD cb);

/*
* wszFullPathKey: ע����������·���������� WINAPI ���� 'RegEnumKeyExW'��
* dwIndex: �Ӽ���������
* lpName: ���ڽ����Ӽ����ƵĻ�������
* lpcchName: ������ʱָ����������С������ʱ�����Ӽ����Ƶĳ��ȡ�
* lpftLastWriteTime: �ڷ���ʱ�����Ӽ�������޸�ʱ�䡣
* ����ֵ: �ɹ���ʧ�ܡ�����ɹ������� ERROR_SUCCESS�����ʧ�ܣ�������Ӧ�Ĵ�����롣
*/
_Success_(return)
BOOL VMMDLL_WinReg_EnumKeyExW(
    _In_ LPWSTR wszFullPathKey,
    _In_ DWORD dwIndex,
    _Out_writes_opt_(*lpcchName) LPWSTR lpName,
    _Inout_ LPDWORD lpcchName,
    _Out_opt_ PFILETIME lpftLastWriteTime
);

/*
* Enumerate registry values given a registry key - similar to WINAPI function
* 'EnumValueW'. Please consult WINAPI function documentation for information.
* May be called in two ways:
* May be called with HKLM base or virtual address of CMHIVE base examples:
*   1) 'HKLM\SOFTWARE\Key\SubKey'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey'              (orphan key)
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey'          (orphan key)
* wszFullPathKey: ע����������·���������� WINAPI ���� 'RegEnumValueW'��
* dwIndex: ֵ��������
* lpValueName: ���ڽ���ֵ���ƵĻ�������
* lpcchValueName: ������ʱָ����������С������ʱ����ֵ���Ƶĳ��ȡ�
* lpType: ���ڽ���ֵ���͵ı���ָ�롣
* lpData: ���ڽ���ֵ���ݵĻ�������
* lpcbData: ������ʱָ����������С������ʱ����ֵ���ݵĴ�С��
* ����ֵ: �ɹ���ʧ�ܡ�����ɹ������� ERROR_SUCCESS�����ʧ�ܣ�������Ӧ�Ĵ�����롣
*/
_Success_(return)
BOOL VMMDLL_WinReg_EnumValueW(
    _In_ LPWSTR wszFullPathKey,
    _In_ DWORD dwIndex,
    _Out_writes_opt_(*lpcchValueName) LPWSTR lpValueName,
    _Inout_ LPDWORD lpcchValueName,
    _Out_opt_ LPDWORD lpType,
    _Out_writes_opt_(*lpcbData) LPBYTE lpData,
    _Inout_opt_ LPDWORD lpcbData
);

/*
* wszFullPathKeyValue: ע����/ֵ������·���������� WINAPI ���� 'RegQueryValueExW'��
* lpType: ���ڽ���ֵ���͵ı���ָ�롣
* lpData: ���ڽ���ֵ���ݵĻ�������
* lpcbData: ������ʱָ����������С������ʱ����ֵ���ݵĴ�С��
* ����ֵ: �ɹ���ʧ�ܡ�����ɹ������� ERROR_SUCCESS�����ʧ�ܣ�������Ӧ�Ĵ�����롣
*/
_Success_(return)
BOOL VMMDLL_WinReg_QueryValueExW(
    _In_ LPWSTR wszFullPathKeyValue,
    _Out_opt_ LPDWORD lpType,
    _Out_writes_opt_(*lpcbData) LPBYTE lpData,
    _When_(lpData == NULL, _Out_opt_) _When_(lpData != NULL, _Inout_opt_) LPDWORD lpcbData
);



//-----------------------------------------------------------------------------
// WINDOWS SPECIFIC NETWORKING FUNCTIONALITY BELOW:
//-----------------------------------------------------------------------------

#define VMMDLL_WIN_TCPIP_MAGIC        0xc0ffee663df93685
#define VMMDLL_WIN_TCPIP_VERSION      1

typedef struct tdVMMDLL_WIN_TCPIP_ENTRY {   // SHARED WITH VMMWINTCPIP
    DWORD dwPID;
    DWORD dwState;
    CHAR szState[12];
    struct {    // address family (IPv4/IPv6)
        BOOL fValid;
        WORD wAF;
    } AF;
    struct {
        BOOL fValid;
        WORD wPort;
        BYTE pbA[16];   // ipv4 = 1st 4 bytes, ipv6 = all bytes
    } Src;
    struct {
        BOOL fValid;
        WORD wPort;
        BYTE pbA[16];   // ipv4 = 1st 4 bytes, ipv6 = all bytes
    } Dst;
    QWORD vaTcpE;
    QWORD qwTime;
    QWORD vaEPROCESS;
    QWORD _Reserved[2];
} VMMDLL_WIN_TCPIP_ENTRY, *PVMMDLL_WIN_TCPIP_ENTRY;

typedef struct tdVMMDLL_WIN_TCPIP {
    QWORD magic;
    DWORD dwVersion;
    DWORD cTcpE;
    VMMDLL_WIN_TCPIP_ENTRY pTcpE[];
} VMMDLL_WIN_TCPIP, *PVMMDLL_WIN_TCPIP;

/*
* ����ֵ��ʧ��ʱΪ NULL���ɹ�ʱΪ PVMMDLL_WIN_TCPIP �ṹ�壬���а����������ע�⣬�����߸����ͷ��ڴ棬ʹ�� VMMDLL_MemFree ���������ͷš�
*/
PVMMDLL_WIN_TCPIP VMMDLL_WinNet_Get();



//-----------------------------------------------------------------------------
// WINDOWS SPECIFIC UTILITY FUNCTIONS BELOW:
//-----------------------------------------------------------------------------

typedef struct tdVMMDLL_WIN_THUNKINFO_IAT {
    BOOL fValid;            // ��ʾ�ṹ���Ƿ������Ч��Ϣ��
    BOOL f32;               // ���ΪTRUE������Ϊ32λ/4�ֽ���Ŀ������Ϊ64λ/8�ֽ���Ŀ��
    ULONG64 vaThunk;        // �����ַ��Import Address Table��'thunk'�ĵ�ַ��
    ULONG64 vaFunction;     // ��������ַ��Import Address Table��'thunk'���ڵ��뺯���ĵ�ַ����Ϊ��ֵ��
    ULONG64 vaNameModule;   // ����ģ�������ַ����ĵ�ַ��
    ULONG64 vaNameFunction; // ���뺯�������ַ����ĵ�ַ��
} VMMDLL_WIN_THUNKINFO_IAT, * PVMMDLL_WIN_THUNKINFO_IAT;

typedef struct tdVMMDLL_WIN_THUNKINFO_EAT {
    BOOL fValid;            // ��ʾ�ṹ���Ƿ������Ч��Ϣ��
    DWORD valueThunk;       // ������ַ��Export Address Table��'thunk'��ֵ��
    ULONG64 vaThunk;        // �����ַ��Import Address Table��'thunk'�ĵ�ַ��
    ULONG64 vaNameFunction; // �������������ַ����ĵ�ַ��
    ULONG64 vaFunction;     // ���������ĵ�ַ��ģ���ַ + value��������
} VMMDLL_WIN_THUNKINFO_EAT, * PVMMDLL_WIN_THUNKINFO_EAT;

/*
* ��ȡ���뺯���ĵ����ַ��Import Address Table��IAT thunk����Ϣ������IAT thunk�������ַ������ڹҹ���hooking�������á�
* -- dwPID������ID
* -- wszModuleName��ģ������
* -- szImportModuleName������ģ������
* -- szImportFunctionName�����뺯������
* -- pThunkIAT��ָ��VMMDLL_WIN_THUNKINFO_IAT�ṹ���ָ�룬���ڽ���IAT thunk����Ϣ
* -- return���ɹ�/ʧ��
*/
_Success_(return)
BOOL VMMDLL_WinGetThunkInfoIAT(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _In_ LPSTR szImportModuleName, _In_ LPSTR szImportFunctionName, _Out_ PVMMDLL_WIN_THUNKINFO_IAT pThunkInfoIAT);

/*
* ��ȡ���������ĵ�����ַ��Export Address Table��EAT thunk����Ϣ������EAT thunk�������ַ������ڹҹ���hooking�������á�
* -- dwPID������ID
* -- wszModuleName��ģ������
* -- pThunkEAT��ָ��VMMDLL_WIN_THUNKINFO_EAT�ṹ���ָ�룬���ڽ���EAT thunk����Ϣ
* -- return���ɹ�/ʧ��
*/
_Success_(return)
BOOL VMMDLL_WinGetThunkInfoEAT(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _In_ LPSTR szExportFunctionName, _Out_ PVMMDLL_WIN_THUNKINFO_EAT pThunkInfoEAT);



//-----------------------------------------------------------------------------
// VMM UTIL FUNCTIONALITY BELOW:
//-----------------------------------------------------------------------------

/*
* ���ɶ���ʮ������ ASCII �ڴ�ת����䵽�������ṩ�� sz �������С�
* -- pb��Ҫת�����ڴ��ַ
* -- cb��Ҫת�����ֽ���
* -- cbInitialOffset��ƫ����������С�ڵ��� 0x1000 ������ 0x10 �ı�����
* -- sz��Ҫ���Ļ����������Ϊ NULL������ pcsz �����з��ػ�������С��
* -- pcsz��
    - ��� sz==NULL�����ػ�������С��������ֹ�� NULL����
    - ��� sz!=NULL��������ʱΪ��������С�������ʱΪ�ַ�������������ֹ�� NULL����
*/
_Success_(return)
BOOL VMMDLL_UtilFillHexAscii(_In_ PBYTE pb, _In_ DWORD cb, _In_ DWORD cbInitialOffset, _Out_opt_ LPSTR sz, _Inout_ PDWORD pcsz);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __VMMDLL_H__ */

#pragma warning(default : 4200)