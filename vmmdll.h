// vmmdll.h : header file to include in projects that use vmm.dll / vmm.so
// 
// Windows may access both UTF-8 *U and Wide-Char *W versions of functions
// while Linux may only access UTF-8 versions. Some functionality may also
// be degraded or unavailable on Linux.
//
// (c) Ulf Frisk, 2018-2021
// Author: Ulf Frisk, pcileech@frizk.net
//
// Header Version: 4.0
//

#include "leechcore.h"

#ifndef __VMMDLL_H__
#define __VMMDLL_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32

#include <Windows.h>
#undef EXPORTED_FUNCTION
#define EXPORTED_FUNCTION
typedef unsigned __int64                    QWORD, *PQWORD;

#endif /* _WIN32 */
#ifdef LINUX

#include <inttypes.h>
#include <stdlib.h>
#undef EXPORTED_FUNCTION
#define EXPORTED_FUNCTION                   __attribute__((visibility("default")))
typedef void                                VOID, *PVOID, *HANDLE, **PHANDLE, *HMODULE;
typedef long long unsigned int              QWORD, *PQWORD, ULONG64, *PULONG64;
typedef uint64_t                            SIZE_T, *PSIZE_T, FILETIME, *PFILETIME;
typedef uint32_t                            DWORD, *PDWORD, *LPDWORD, BOOL, *PBOOL, NTSTATUS;
typedef uint16_t                            WORD, *PWORD;
typedef uint8_t                             BYTE, *PBYTE, *LPBYTE, UCHAR;
typedef char                                CHAR, *PCHAR, *LPSTR, *LPCSTR;
typedef uint16_t                            WCHAR, *PWCHAR, *LPWSTR, *LPCWSTR;
#define MAX_PATH                            260
#define _In_
#define _In_z_
#define _In_opt_
#define _In_reads_(x)
#define _In_reads_opt_(x)
#define _Inout_
#define _Inout_bytecount_(x)
#define _Inout_opt_
#define _Inout_updates_opt_(x)
#define _Out_
#define _Out_opt_
#define _Out_writes_(x)
#define _Out_writes_bytes_opt_(x)
#define _Out_writes_opt_(x)
#define _Out_writes_to_(x,y)
#define _When_(x,y)
#define _Frees_ptr_opt_
#define _Post_ptr_invalid_
#define _Check_return_opt_
#define _Printf_format_string_
#define _Success_(x)

#endif /* LINUX */



//-----------------------------------------------------------------------------
// INITIALIZATION FUNCTIONALITY BELOW:
// Choose one way of initializing the VMM / MemProcFS.
//-----------------------------------------------------------------------------

/*
* ʹ�������в�����ʼ�� VMM.DLL���йز����ĸ���ϸ��Ϣ������� MemProcFS �� LeechCore �� GitHub Wiki��ע�⣺LeechCore �ĳ�ʼ�������Ըú���Ҳ��Ч����Ҫ�Ĳ������£�
*    -printf = ��ʾ printf ��ʽ�������
*    -v -vv -vvv = �������ϸ�̶ȼ���
*    -device = �豸�ĸ�ʽ���μ� leechcore.h �� GitHub �ĵ��Ի�ȡ������Ϣ��һЩ���õ�ֵ������<file>��fpga��usb3380��hvsavedstate��totalmeltdown��pmem
*    -remote = Զ�� LeechCore ʵ�����μ� leechcore.h �� GitHub �ĵ��Ի�ȡ������Ϣ��
*    -norefresh = ���ú�̨ˢ�£���ʹ֧�ֵ��ڴ�����ʧ���ڴ棩��
*    -memmap = ָ�����ļ������������ڴ�ӳ���ָ��Ϊ 'auto'��ʾ����-memmap c:\\temp\\my_custom_memory_map.txt �� -memmap auto
*    -pagefile[0-9] = �������ڴ���ʹ�õ�ҳ���ļ���ͨ����pagefile.sys ������Ϊ 0��swapfile.sys ������Ϊ 1��ҳ���ļ��ڲ��ϱ仯���������ڴ�ת����ҳ���ļ�֮���ʱ���������ӡ�ʾ����'pagefile0 swapfile.sys'
*    -symbolserverdisable = ���÷��ŷ�������ֱ���û����ġ��˲���������ע������á�
*    -waitinitialize = �ȴ���ʼ����ɺ��ٷ��ء�ͨ������£�ĳЩ��ʼ���������첽���еģ������ڳ�ʼ���������ʱ������δ��ɡ�����������ڴ�ѹ����ע���ȡ�ʾ����'-waitinitialize'
*    -userinteract = ���� vmm.dll �ڿ���̨��ѯ���û���Ϣ�����磬�������ڣ�leechcore �豸ѡ�Ĭ������£��û������ǽ��õġ�
*    -forensic = �������������������ڴ����ȡ֤ɨ�裨������ܣ�������Ĳ���ֵ��ΧΪ 0-4��ע�⣡ȡ֤ģʽ��������ʵʱ�ڴ档1 = ȡ֤ģʽ��ʹ���ڴ��е� SQLite ���ݿ⡣2 = ȡ֤ģʽ��ʹ����ʱ SQLite ���ݿ⣬�˳�ʱɾ����3 = ȡ֤ģʽ��ʹ����ʱ SQLite ���ݿ⣬�˳�ʱ������4 = ȡ֤ģʽ��ʹ�þ�̬������ SQLite ���ݿ⣨vmm.sqlite3����ʾ����-forensic 4
*
* -- argc
* -- argv
* -- ppLcErrorInfo = ��ѡָ�룬���ڽ���һ���� LC_CONFIG_ERRORINFO �ṹ�������չ������Ϣ�ĺ����ڴ档�ڷ���ʧ��ʱ�����յ����κ��ڴ�Ӧ�ɵ�����ͨ��

���� LcMemFree() �����ͷš�
* -- ����ֵ = �ɹ�/ʧ��

��ע�⣺�����ı��е�ע�Ͳ����ǶԴ���Ľ��ͣ���������Ӧ�ķ��롣

//-------------------------------------------------------------------------
// ��ʼ���������£�
// ѡ��һ�ֳ�ʼ�� VMM / MemProcFS �ķ�ʽ��
//-------------------------------------------------------------------------

/*
* ʹ�������в�����ʼ�� VMM.DLL���йز����ĸ���ϸ��Ϣ������� MemProcFS �� LeechCore �� GitHub Wiki��ע�⣺LeechCore �ĳ�ʼ�������Ըú���Ҳ��Ч����Ҫ�Ĳ������£�
*    -printf = ��ʾ printf ��ʽ�������
*    -v -vv -vvv = �������ϸ�̶ȼ���
*    -device = �豸�ĸ�ʽ���μ� leechcore.h �� GitHub �ĵ��Ի�ȡ������Ϣ��һЩ���õ�ֵ������<file>��fpga��usb3380��hvsavedstate��totalmeltdown��pmem
*    -remote = Զ�� LeechCore ʵ�����μ� leechcore.h �� GitHub �ĵ��Ի�ȡ������Ϣ��
*    -norefresh = ���ú�̨ˢ�£���ʹ֧�ֵ��ڴ�����ʧ���ڴ棩��
*    -memmap = ָ�����ļ������������ڴ�ӳ���ָ��Ϊ 'auto'��ʾ����-memmap c:\\temp\\my_custom_memory_map.txt �� -memmap auto
*    -pagefile[0-9] = �������ڴ���ʹ�õ�ҳ���ļ���ͨ����pagefile.sys ������Ϊ 0��swapfile.sys ������Ϊ 1��ҳ���ļ��ڲ��ϱ仯���������ڴ�ת����ҳ���ļ�֮���ʱ���������ӡ�ʾ����'pagefile0 swapfile.sys'
*    -symbolserverdisable = ���÷��ŷ�������ֱ���û����ġ��˲���������ע������á�
*    -waitinitialize = �ȴ���ʼ����ɺ��ٷ��ء�ͨ������£�ĳЩ��ʼ���������첽���еģ������ڳ�ʼ���������ʱ������δ��ɡ�����������ڴ�ѹ����ע���ȡ�ʾ����'-waitinitialize'
*    -userinteract = ���� vmm.dll �ڿ���̨��ѯ���û���Ϣ�����磬�������ڣ�leechcore �豸ѡ�Ĭ������£��û������ǽ��õġ�
*    -forensic = �������������������ڴ����ȡ֤ɨ�裨������ܣ�������Ĳ���ֵ��ΧΪ 0-4��ע�⣡ȡ֤ģʽ��������ʵʱ�ڴ档1 = ȡ֤ģʽ��ʹ���ڴ��е� SQLite ���ݿ⡣2 = ȡ֤ģʽ��ʹ����ʱ SQLite ���ݿ⣬�˳�ʱɾ����3 = ȡ֤ģʽ��ʹ����ʱ SQLite ���ݿ⣬�˳�ʱ������4 = ȡ֤ģʽ��ʹ�þ�̬������ SQLite ���ݿ⣨vmm.sqlite3����ʾ����-forensic 4
*
* -- argc
* -- argv
* -- ppLcErrorInfo

 = ��ѡָ�룬���ڽ���һ���� LC_CONFIG_ERRORINFO �ṹ�������չ������Ϣ�ĺ����ڴ档�ڷ���ʧ��ʱ�����յ����κ��ڴ�Ӧ�ɵ�����ͨ������ LcMemFree() �����ͷš�
* -- ����ֵ = �ɹ�/ʧ��

*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_Initialize(_In_ DWORD argc, _In_ LPSTR argv[]);

EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_InitializeEx(_In_ DWORD argc, _In_ LPSTR argv[], _Out_opt_ PPLC_CONFIG_ERRORINFO ppLcErrorInfo);

/*
* �ر��ѳ�ʼ���� VMM.DLL ʵ�������������ѷ������Դ��������������ӵ� PCILeech.DLL �������ڴ���Դ��
* -- ����ֵ = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_Close();

/*
* Free memory allocated by the VMMDLL.
* -- pvMem
*/
EXPORTED_FUNCTION
VOID VMMDLL_MemFree(_Frees_ptr_opt_ PVOID pvMem);



//-----------------------------------------------------------------------------
// �����������£�
// ���� MemProcFS ��ײ���ڴ��ȡ�豸��
//-----------------------------------------------------------------------------

/*
* �뺯�� VMMDLL_ConfigGet �� VMMDLL_ConfigSet һ��ʹ�õ�ѡ�
* ѡ���ڴ�ͷ�ļ���ʹ�� VMMDLL_OPT_* ���ж��壬���� leechcore.h ��ʹ�� LC_OPT_* ���ж��塣
* �йظ���ϸ����Ϣ����鿴�����豸���͵�Դ���롣
*/
#define VMMDLL_OPT_CORE_PRINTF_ENABLE                   0x4000000100000000  // RW - ���� printf ��ʽ���
#define VMMDLL_OPT_CORE_VERBOSE                         0x4000000200000000  // RW - ������ϸ���
#define VMMDLL_OPT_CORE_VERBOSE_EXTRA                   0x4000000300000000  // RW - ������ϸ�������չ��
#define VMMDLL_OPT_CORE_VERBOSE_EXTRA_TLP               0x4000000400000000  // RW - ������ϸ�����TLP��
#define VMMDLL_OPT_CORE_MAX_NATIVE_ADDRESS              0x4000000800000000  // R - ��󱾻���ַ

#define VMMDLL_OPT_CORE_SYSTEM                          0x2000000100000000  // R - ϵͳ����
#define VMMDLL_OPT_CORE_MEMORYMODEL                     0x2000000200000000  // R - �ڴ�ģ��

#define VMMDLL_OPT_CONFIG_IS_REFRESH_ENABLED            0x2000000300000000  // R - ˢ���Ƿ����ã�1/0��
#define VMMDLL_OPT_CONFIG_TICK_PERIOD                   0x2000000400000000  // RW - ������ʱ�����ڣ����룩
#define VMMDLL_OPT_CONFIG_READCACHE_TICKS               0x2000000500000000  // RW - �ڴ滺����Ч�ڣ���ʱ�����ڼ��㣩
#define VMMDLL_OPT_CONFIG_TLBCACHE_TICKS                0x2000000600000000  // RW - ҳ���TLB��������Ч�ڣ���ʱ�����ڼ��㣩
#define VMMDLL_OPT_CONFIG_PROCCACHE_TICKS_PARTIAL        0x2000000700000000  // RW - ����ˢ�£����֣����ڣ���ʱ�����ڼ��㣩
#define VMMDLL_OPT_CONFIG_PROCCACHE_TICKS_TOTAL          0x2000000800000000  // RW - ����ˢ�£����������ڣ���ʱ�����ڼ��㣩
#define VMMDLL_OPT_CONFIG_VMM_VERSION_MAJOR             0x2000000900000000  // R - VMM.DLL �汾�ţ����汾��
#define VMMDLL_OPT_CONFIG_VMM_VERSION_MINOR             0x2000000A00000000  // R - VMM.DLL �汾�ţ��ΰ汾��
#define VMMDLL_OPT_CONFIG_VMM_VERSION_REVISION          0x2000000B00000000  // R - VMM.DLL �汾�ţ��޶��汾��
#define VMMDLL_OPT_CONFIG_STATISTICS_FUNCTIONCALL       0x2000000C00000000  // RW - ���ú�������ͳ�ƣ�.status/statistics_fncall �ļ���
#define VMMDLL_OPT_CONFIG_IS_PAGING_ENABLED             0x2000000D00000000  // RW - �Ƿ����÷�ҳ��1/0��

#define VMMDLL_OPT_WIN_VERSION_MAJOR                    0x2000010100000000  // R - Windows �汾�ţ����汾��
#define VMMDLL_OPT_WIN_VERSION_MINOR                    0x2000010200000000  // R - Windows �汾�ţ��ΰ汾��
#define VMMDLL_OPT_WIN_VERSION_BUILD                    0x2000010300000000  // R - Windows �汾�ţ������ţ�
#define VMMDLL_OPT_WIN_SYSTEM_UNIQUE_ID                 0x2000010400000000  // R - Windows

ϵͳΨһ��ʶ��

#define VMMDLL_OPT_FORENSIC_MODE                        0x2000020100000000  // RW - ����/����ȡ֤ģʽ����[0-4]��

#define VMMDLL_OPT_REFRESH_ALL                          0x2001ffff00000000  // W - ˢ�����л���
#define VMMDLL_OPT_REFRESH_FREQ_MEM                     0x2001000200000000  // W - ˢ���ڴ滺�棨������TLB��[���� 33%/����]
#define VMMDLL_OPT_REFRESH_FREQ_TLB                     0x2001000400000000  // W - ˢ��ҳ���TLB������[���� 33%/����]
#define VMMDLL_OPT_REFRESH_FREQ_FAST                    0x2001040000000000  // W - ����ˢ��Ƶ�� - �������ֽ���ˢ��
#define VMMDLL_OPT_REFRESH_FREQ_MEDIUM                  0x2001000100000000  // W - �е�ˢ��Ƶ�� - ������������ˢ��
#define VMMDLL_OPT_REFRESH_FREQ_SLOW                    0x2001001000000000  // W - ����ˢ��Ƶ��
#define VMMDLL_OPT_REFRESH_PROCESS                      0x2001000100000000  // W - �����ã�ˢ�½����б�
#define VMMDLL_OPT_REFRESH_READ                         0x2001000200000000  // W - �����ã�ˢ�������ȡ����
#define VMMDLL_OPT_REFRESH_TLB                          0x2001000400000000  // W - �����ã�ˢ��ҳ���TLB������
#define VMMDLL_OPT_REFRESH_PAGING                       0x2001000800000000  // W - �����ã�ˢ�������ڴ�ġ�paging������
#define VMMDLL_OPT_REFRESH_REGISTRY                     0x2001001000000000  // W - �����ã�
#define VMMDLL_OPT_REFRESH_USER                         0x2001002000000000  // W - �����ã�
#define VMMDLL_OPT_REFRESH_PHYSMEMMAP                   0x2001004000000000  // W - �����ã�
#define VMMDLL_OPT_REFRESH_PFN                          0x2001008000000000  // W - �����ã�
#define VMMDLL_OPT_REFRESH_OBJ                          0x2001010000000000  // W - �����ã�
#define VMMDLL_OPT_REFRESH_NET                          0x2001020000000000  // W - �����ã�

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
* ��ȡ�豸�ض�ѡ���ֵ������� VMMDLL_OPT_* �Ķ������˽���Ч��ѡ��ֵ��
* ��ע�⣬��ͬ�豸����֮���ѡ��ֵ���ܴ����ص��������в�ͬ�ĺ��塣
* -- fOption��ѡ���־
* -- pqwValue��ָ�� ULONG64 ��ָ�룬���ڽ���ѡ��ֵ��
* -- ����ֵ���ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_ConfigGet(_In_ ULONG64 fOption, _Out_ PULONG64 pqwValue);

/*
* �����豸�ض�ѡ���ֵ������� VMMDLL_OPT_* �Ķ������˽���Ч��ѡ��ֵ��
* ��ע�⣬��ͬ�豸����֮���ѡ��ֵ���ܴ����ص��������в�ͬ�ĺ��塣
* -- fOption��ѡ���־
* -- qwValue��ѡ��ֵ
* -- ����ֵ���ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_ConfigSet(_In_ ULONG64 fOption, _In_ ULONG64 qwValue);
*/



//-----------------------------------------------------------------------------
// FORWARD DECLARATIONS:
//-----------------------------------------------------------------------------

typedef struct tdVMMDLL_MAP_PFN *PVMMDLL_MAP_PFN;



//-----------------------------------------------------------------------------
// LINUX SPECIFIC DEFINES:
//-----------------------------------------------------------------------------
#ifdef LINUX

#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct _IMAGE_DATA_DIRECTORY {
    DWORD   VirtualAddress;
    DWORD   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_SECTION_HEADER {
    BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
        DWORD   PhysicalAddress;
        DWORD   VirtualSize;
    } Misc;
    DWORD   VirtualAddress;
    DWORD   SizeOfRawData;
    DWORD   PointerToRawData;
    DWORD   PointerToRelocations;
    DWORD   PointerToLinenumbers;
    WORD    NumberOfRelocations;
    WORD    NumberOfLinenumbers;
    DWORD   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef struct _SERVICE_STATUS {
    DWORD   dwServiceType;
    DWORD   dwCurrentState;
    DWORD   dwControlsAccepted;
    DWORD   dwWin32ExitCode;
    DWORD   dwServiceSpecificExitCode;
    DWORD   dwCheckPoint;
    DWORD   dwWaitHint;
} SERVICE_STATUS, *LPSERVICE_STATUS;
#endif /* LINUX */



//-----------------------------------------------------------------------------
// VFS - �����ļ�ϵͳ�������£�
// ע�⣡ʹ��VFS������Ҫ�ȵ���VMMDLL_InitializePlugins()����ʼ�������
// ����MemProcFS�ĺ��ġ�ͨ��ʹ������Ĺ��ܣ����Զ������ļ�ϵͳ��VFS������ʵ�ֺͷ�����
//-----------------------------------------------------------------------------
#define VMMDLL_STATUS_SUCCESS                       ((NTSTATUS)0x00000000L)
#define VMMDLL_STATUS_UNSUCCESSFUL                  ((NTSTATUS)0xC0000001L)
#define VMMDLL_STATUS_END_OF_FILE                   ((NTSTATUS)0xC0000011L)
#define VMMDLL_STATUS_FILE_INVALID                  ((NTSTATUS)0xC0000098L)
#define VMMDLL_STATUS_FILE_SYSTEM_LIMITATION        ((NTSTATUS)0xC0000427L)

#define VMMDLL_VFS_FILELIST_EXINFO_VERSION          1
#define VMMDLL_VFS_FILELIST_VERSION                 2

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

typedef struct tdVMMDLL_VFS_FILELIST2 {
    DWORD dwVersion;
    VOID(*pfnAddFile)     (_Inout_ HANDLE h, _In_ LPSTR uszName, _In_ ULONG64 cb, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo);
    VOID(*pfnAddDirectory)(_Inout_ HANDLE h, _In_ LPSTR uszName, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo);
    HANDLE h;
} VMMDLL_VFS_FILELIST2, *PVMMDLL_VFS_FILELIST2;

/*
* ���ڻص�VMM_VFS_FILELIST�ṹ�ĸ���������
*/
EXPORTED_FUNCTION
VOID VMMDLL_VfsList_AddFile(_In_ HANDLE pFileList, _In_ LPSTR uszName, _In_ ULONG64 cb, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo);
VOID VMMDLL_VfsList_AddFileW(_In_ HANDLE pFileList, _In_ LPWSTR wszName, _In_ ULONG64 cb, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo);
EXPORTED_FUNCTION
VOID VMMDLL_VfsList_AddDirectory(_In_ HANDLE pFileList, _In_ LPSTR uszName, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo);
VOID VMMDLL_VfsList_AddDirectoryW(_In_ HANDLE pFileList, _In_ LPWSTR wszName, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo);
EXPORTED_FUNCTION BOOL VMMDLL_VfsList_IsHandleValid(_In_ HANDLE pFileList);

/*
* ��MemProcFS���г��ļ�Ŀ¼��ͨ������pFileList�������ṩ�ĺ������г�Ŀ¼���ļ���
* �����Ҫ��ȡ�����ļ�����Ϣ������Ҫ�г���Ŀ¼�е������ļ���
* -- [uw]szPath��Ŀ¼·��
* -- pFileList���ص������Ĳ����ṹ��
* -- ����ֵ
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_VfsListU(_In_ LPSTR  uszPath, _Inout_ PVMMDLL_VFS_FILELIST2 pFileList);
_Success_(return) BOOL VMMDLL_VfsListW(_In_ LPWSTR wszPath, _Inout_ PVMMDLL_VFS_FILELIST2 pFileList);

/*
* ��MemProcFS�ж�ȡ�ļ���ѡ�����֡�
* -- [uw]szFileName���ļ���
* -- pb���洢��ȡ���ݵĻ�����
* -- cb��Ҫ��ȡ���ֽ���
* -- pcbRead��ָ��һ�����������ڽ���ʵ�ʶ�ȡ���ֽ���
* -- cbOffset���ļ��е�ƫ�������Ӹ�λ�ÿ�ʼ��ȡ����
* -- ����ֵ
*
*/
EXPORTED_FUNCTION
NTSTATUS VMMDLL_VfsReadU(_In_ LPSTR  uszFileName, _Out_writes_to_(cb, *pcbRead) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset);
NTSTATUS VMMDLL_VfsReadW(_In_ LPWSTR wszFileName, _Out_writes_to_(cb, *pcbRead) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset);

/*
* ��MemProcFS�н�ѡ���Ĳ���д���ļ���
* -- [uw]szFileName���ļ���
* -- pb��Ҫд������ݵĻ�����
* -- cb��Ҫд����ֽ���
* -- pcbWrite��ָ��һ�����������ڽ���ʵ��д����ֽ���
* -- cbOffset���ļ��е�ƫ�������Ӹ�λ�ÿ�ʼд������
* -- ����ֵ
*/
EXPORTED_FUNCTION
NTSTATUS VMMDLL_VfsWriteU(_In_ LPSTR  uszFileName, _In_reads_(cb) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbWrite, _In_ ULONG64 cbOffset);
NTSTATUS VMMDLL_VfsWriteW(_In_ LPWSTR wszFileName, _In_reads_(cb) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbWrite, _In_ ULONG64 cbOffset);

/*
* Utility functions for MemProcFS read/write towards different underlying data
* representations.
*/
EXPORTED_FUNCTION NTSTATUS VMMDLL_UtilVfsReadFile_FromPBYTE(_In_ PBYTE pbFile, _In_ ULONG64 cbFile, _Out_writes_to_(cb, *pcbRead) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset);
EXPORTED_FUNCTION NTSTATUS VMMDLL_UtilVfsReadFile_FromQWORD(_In_ ULONG64 qwValue, _Out_writes_to_(cb, *pcbRead) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset, _In_ BOOL fPrefix);
EXPORTED_FUNCTION NTSTATUS VMMDLL_UtilVfsReadFile_FromDWORD(_In_ DWORD dwValue, _Out_writes_to_(cb, *pcbRead) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset, _In_ BOOL fPrefix);
EXPORTED_FUNCTION NTSTATUS VMMDLL_UtilVfsReadFile_FromBOOL(_In_ BOOL fValue, _Out_writes_to_(cb, *pcbRead) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset);
EXPORTED_FUNCTION NTSTATUS VMMDLL_UtilVfsWriteFile_BOOL(_Inout_ PBOOL pfTarget, _In_reads_(cb) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbWrite, _In_ ULONG64 cbOffset);
EXPORTED_FUNCTION NTSTATUS VMMDLL_UtilVfsWriteFile_DWORD(_Inout_ PDWORD pdwTarget, _In_reads_(cb) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbWrite, _In_ ULONG64 cbOffset, _In_ DWORD dwMinAllow);



//-----------------------------------------------------------------------------
// �������������:
// ���ڳ�ʼ����ʹ��MemProcFS������ܵĺ����ͽṹ��
// ���������ͨ�����ú���VMM_VfsInitializePlugins��������ÿ�����ò�����ⲿ�����DLL����ƥ��m_*.dll�����������InitializeVmmPlugin������
// ���/ģ����Ծ�������pfnPluginManager_Register��ע���Բ�ͬ������ʽ�Ĳ��һ�λ��Ρ�
// �����ǲ��DLL�е�ע�ắ��ʾ��:
// 'VOID InitializeVmmPlugin(_In_ PVMM_PLUGIN_REGINFO pRegInfo)'

/*
* ��ʼ�����п��ܵĲ�����������ú��ⲿ���������ӳ�䵽MemProcFS����ע�⣬Ĭ������²�����ز��-����ͨ�����ô˺�����ʽ���ز�������ǽ���vmm dll��ͨ�ùر�ʱж�ء�
* -- ����ֵ
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_InitializePlugins();

#define VMMDLL_PLUGIN_CONTEXT_MAGIC                 0xc0ffee663df9301c
#define VMMDLL_PLUGIN_CONTEXT_VERSION               4
#define VMMDLL_PLUGIN_REGINFO_MAGIC                 0xc0ffee663df9301d
#define VMMDLL_PLUGIN_REGINFO_VERSION               13
#define VMMDLL_PLUGIN_FORENSIC_JSONDATA_VERSION     0xc0ee0001

#define VMMDLL_PLUGIN_NOTIFY_VERBOSITYCHANGE        0x01
#define VMMDLL_PLUGIN_NOTIFY_REFRESH_FAST           0x05    // refresh fast event   - at partial process refresh.
#define VMMDLL_PLUGIN_NOTIFY_REFRESH_MEDIUM         0x02    // refresh medium event - at full process refresh.
#define VMMDLL_PLUGIN_NOTIFY_REFRESH_SLOW           0x04    // refresh slow event   - at registry refresh.

#define VMMDLL_PLUGIN_NOTIFY_FORENSIC_INIT          0x01000100
#define VMMDLL_PLUGIN_NOTIFY_FORENSIC_INIT_COMPLETE 0x01000200

typedef HANDLE                                      *PVMMDLL_PLUGIN_INTERNAL_CONTEXT;

typedef struct tdVMMDLL_PLUGIN_CONTEXT {
    ULONG64 magic;
    WORD wVersion;
    WORD wSize;
    DWORD dwPID;
    PVOID pProcess;
    LPSTR uszModule;
    LPSTR uszPath;
    PVOID pvReserved1;
    PVMMDLL_PLUGIN_INTERNAL_CONTEXT ctxM;       // optional internal module context.
} VMMDLL_PLUGIN_CONTEXT, *PVMMDLL_PLUGIN_CONTEXT;

typedef struct tdVMMDLL_PLUGIN_FORENSIC_JSONDATA {
    DWORD dwVersion;        // must equal VMMDLL_PLUGIN_FORENSIC_JSONDATA_VERSION
    BOOL fVerbose;
    LPSTR szjType;          // log type/name (json encoded)
    DWORD i;
    DWORD dwPID;
    QWORD vaObj;
    BOOL fva[2];            // log va even if zero
    QWORD va[2];
    BOOL fNum[2];           // log num even if zero
    QWORD qwNum[2];
    BOOL fHex[2];           // log hex even if zero
    QWORD qwHex[2];
    // str: will be prioritized in order: szu > wsz.
    LPCSTR usz[2];          // str: utf-8 encoded
    LPCWSTR wsz[2];         // str: wide
    BYTE _Reserved[0x4000+256];
} VMMDLL_PLUGIN_FORENSIC_JSONDATA, *PVMMDLL_PLUGIN_FORENSIC_JSONDATA;

typedef struct tdVMMDLL_PLUGIN_FORENSIC_INGEST_PHYSMEM {
    DWORD cMEMs;
    BOOL fValid;
    QWORD paBase;
    PPMEM_SCATTER ppMEMs;
    PVMMDLL_MAP_PFN pPfnMap;
} VMMDLL_PLUGIN_FORENSIC_INGEST_PHYSMEM, *PVMMDLL_PLUGIN_FORENSIC_INGEST_PHYSMEM;

typedef struct tdVMMDLL_PLUGIN_REGINFO {
    ULONG64 magic;
    WORD wVersion;
    WORD wSize;
    VMMDLL_MEMORYMODEL_TP tpMemoryModel;
    VMMDLL_SYSTEM_TP tpSystem;
    HMODULE hDLL;
    BOOL(*pfnPluginManager_Register)(struct tdVMMDLL_PLUGIN_REGINFO *pPluginRegInfo);
    DWORD _Reserved[32];
    // python plugin information - not for general use
    struct {
        BOOL fPythonStandalone;
        DWORD _Reserved;
        HMODULE hReservedDllPython3;
        HMODULE hReservedDllPython3X;
    } python;
    // general plugin registration info to be filled out by the plugin below:
    struct {
        PVMMDLL_PLUGIN_INTERNAL_CONTEXT ctxM;   // optional internal module context [must be cleaned by pfnClose() call].
        CHAR uszPathName[128];
        BOOL fRootModule;
        BOOL fProcessModule;
        BOOL fRootModuleHidden;
        BOOL fProcessModuleHidden;
        CHAR sTimelineNameShort[6];
        CHAR _Reserved[2];
        CHAR uszTimelineFile[32];
        CHAR _Reserved2[32];
    } reg_info;
    // function plugin registration info to be filled out by the plugin below:
    struct {
        BOOL(*pfnList)(_In_ PVMMDLL_PLUGIN_CONTEXT ctxP, _Inout_ PHANDLE pFileList);
        NTSTATUS(*pfnRead)(_In_ PVMMDLL_PLUGIN_CONTEXT ctxP, _Out_writes_to_(cb, *pcbRead) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead,  _In_ ULONG64 cbOffset);
        NTSTATUS(*pfnWrite)(_In_ PVMMDLL_PLUGIN_CONTEXT ctxP, _In_reads_(cb) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbWrite, _In_ ULONG64 cbOffset);
        VOID(*pfnNotify)(_In_ PVMMDLL_PLUGIN_CONTEXT ctxP, _In_ DWORD fEvent, _In_opt_ PVOID pvEvent, _In_opt_ DWORD cbEvent);
        VOID(*pfnClose)(_In_ PVMMDLL_PLUGIN_CONTEXT ctxP);
        BOOL(*pfnVisibleModule)(_In_ PVMMDLL_PLUGIN_CONTEXT ctxP);
        PVOID pvReserved[10];
    } reg_fn;
    // ��ѡ��ȡ֤������ܣ����ڶԸ������ݽ���ȡ֤����ȫ��ģ���������Щ�����ǿ�ѡ�ġ��Ͳ��������ԣ�
    //�����ڵ��߳�ģʽ�µ��ã�������ȡ�������ڲ��֮���Բ��ж��̷߳�ʽ���á������ĵ���˳��Ϊ��pfnInitialize()��pfnIngest*()��pfnTimeline()��pfnLogJSON()��pfnFinalize()
    struct {
        PVOID(*pfnInitialize)(_In_ PVMMDLL_PLUGIN_CONTEXT ctxP);
        VOID(*pfnFinalize)(_In_opt_ PVOID ctxfc);
        VOID(*pfnTimeline)(
            _In_opt_ PVOID ctxfc,
            _In_ HANDLE hTimeline,
            _In_ VOID(*pfnAddEntry)(_In_ HANDLE hTimeline, _In_ QWORD ft, _In_ DWORD dwAction, _In_ DWORD dwPID, _In_ DWORD dwData32, _In_ QWORD qwData64, _In_ LPSTR uszText),
            _In_ VOID(*pfnEntryAddBySql)(_In_ HANDLE hTimeline, _In_ DWORD cEntrySql, _In_ LPSTR *pszEntrySql));
        VOID(*pfnIngestPhysmem)(_In_opt_ PVOID ctxfc, _In_ PVMMDLL_PLUGIN_FORENSIC_INGEST_PHYSMEM pIngestPhysmem);
        VOID(*pfnIngestFinalize)(_In_opt_ PVOID ctxfc);
        PVOID pvReserved[10];
        VOID(*pfnLogJSON)(_In_ PVMMDLL_PLUGIN_CONTEXT ctxP, _In_ VOID(*pfnLogJSON)(_In_ PVMMDLL_PLUGIN_FORENSIC_JSONDATA pData));
    } reg_fnfc;
    // Additional system information - read/only by the plugins.
    struct {
        BOOL f32;
        DWORD dwVersionMajor;
        DWORD dwVersionMinor;
        DWORD dwVersionBuild;
        DWORD _Reserved[32];
    } sysinfo;
} VMMDLL_PLUGIN_REGINFO, *PVMMDLL_PLUGIN_REGINFO;



//-----------------------------------------------------------------------------
// VMM ���Ĺ������£�
// VMM ���Ĺ��ܰ�����ȡ����д�룩����������ڴ档ע�⣡д�����ֻ��Ŀ��֧�ֵ��������Ч�����������ڴ�ת���ļ�...
// ��Ҫ��ȡ�����ڴ棬�뽫 dwPID ָ��Ϊ (DWORD)-1
//-----------------------------------------------------------------------------

#define VMMDLL_PID_PROCESS_WITH_KERNELMEMORY        0x80000000      // Combine with dwPID to enable process kernel memory (NB! use with extreme care).

// �����ڵ��� VMM_MemReadEx() ʱ��ֹĬ�ϵĶ�ȡ���棬�⽫����ʼ�մ�Ŀ��ϵͳ��ȡ���ݡ�
// ��Ȼʹ�û����ҳ�����������ַ�������ַ��ת������
#define VMMDLL_FLAG_NOCACHE                         0x0001  // ��ʹ�����ݻ��棨ǿ�ƴ��ڴ�ɼ��豸��ȡ��
#define VMMDLL_FLAG_ZEROPAD_ON_FAIL                 0x0002  // ʧ�ܵ������ڴ��ȡ��������䣬���������ڴ淶Χ�ڱ���ɹ���
#define VMMDLL_FLAG_FORCECACHE_READ                 0x0008  // ǿ��ʹ�û��� - �Էǻ���ҳʧ�� - ���Զ�ȡ��Ч���� VMM_FLAG_NOCACHE/VMM_FLAG_ZEROPAD_ON_FAIL �����ݡ�
#define VMMDLL_FLAG_NOPAGING                        0x0010  // �����Դӷ�ҳ�ļ�/ѹ���ķ�ҳ�ڴ��м����ڴ棨��ʹ���ܣ�
#define VMMDLL_FLAG_NOPAGING_IO                     0x0020  // �����ȡ�����¶���� I/O���򲻳��Դӷ�ҳ���ڴ��м����ڴ棨��ʹ���ܣ���
#define VMMDLL_FLAG_NOCACHEPUT                      0x0100  // �ɹ����ڴ�ɼ��豸��ȡ�󣬲�д�����ݻ��档
#define VMMDLL_FLAG_CACHE_RECENT_ONLY               0x0200  // ��������Ļ���������ȡ���ݡ�
#define VMMDLL_FLAG_NO_PREDICTIVE_READ              0x0400  // ��ִ�ж����Ԥ����ҳ���ȡ��Ĭ�϶��ڽ�С�����󣩡�

/*
* ���� ppMEMs �����е���Ŀָ��ָ���ĸ���������λ���϶�ȡ�ڴ档ÿ��������Ԫ�Ľ�����ֱ������
* ��ȡ����Ŀ��û�����ޣ����������Ӳ�����ƣ��򲻻��ṩ����������ÿ��������Ԫ������СΪһ��4kҳ�棨4096�ֽڣ���
* ��ȡ���ܿ�Խ4kҳ��߽硣��ȡ�����ż��DWORD��4�ֽڣ���ʼ��
* -- dwPID - Ŀ����̵�PID��(DWORD)-1 �����ȡ�����ڴ档
* -- ppMEMs = ��ɢ��ȡͷ�����顣
* -- cpMEMs = ppMEMs ��������
* -- flags = ��ѡ�ı�־��ʹ�� VMMDLL_FLAG_* ����ָ����
* -- return = �ɹ���ȡ����Ŀ����
*/
EXPORTED_FUNCTION
DWORD VMMDLL_MemReadScatter(_In_ DWORD dwPID, _Inout_ PPMEM_SCATTER ppMEMs, _In_ DWORD cpMEMs, _In_ DWORD flags);

/*
* ��ȡ���� 4096 �ֽڵ��ڴ�ҳ�档
* -- dwPID - Ŀ����̵�PID��(DWORD)-1 �����ȡ�����ڴ档
* -- qwA - �ڴ�ҳ�����ʼ��ַ��
* -- pbPage - �洢��ȡ����Ļ�������
* -- return - �ɹ�/ʧ�ܣ�ȡ�����Ƿ�ɹ���ȡ����������ֽڣ���
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_MemReadPage(_In_ DWORD dwPID, _In_ ULONG64 qwA, _Inout_bytecount_(4096) PBYTE pbPage);

/*
* ��ȡһ�������������������ڴ档
* -- dwPID - Ŀ����̵�PID��(DWORD)-1 �����ȡ�����ڴ档
* -- qwA - �ڴ����ʼ��ַ��
* -- pb - �洢��ȡ����Ļ�������
* -- cb - Ҫ��ȡ���ֽ�����
* -- return - �ɹ�/ʧ�ܣ�ȡ�����Ƿ�ɹ���ȡ������������ֽڣ���
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_MemRead(_In_ DWORD dwPID, _In_ ULONG64 qwA, _Out_writes_(cb) PBYTE pb, _In_ DWORD cb);

/*
* ��ȡһ���������ڴ棬������ȡ���ֽ����洢�� pcbRead �����С�
* -- dwPID - Ŀ����̵�PID��(DWORD)-1 �����ȡ�����ڴ档
* -- qwA - �ڴ����ʼ��ַ��
* -- pb - �洢��ȡ����Ļ�������
* -- cb - Ҫ��ȡ���ֽ�����
* -- pcbRead - ��ȡ���ֽ������洢�ڴ˲����С�
* -- flags - ��־��ʹ�� VMMDLL_FLAG_* �еı�־λ��
* -- return - �ɹ�/ʧ�ܡ�ע�⣡��ʹ��ȡ���ֽ���Ϊ0����ȡ�����Կ��ܱ���Ϊ�ɹ���������֤ pcbRead ������
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_MemReadEx(_In_ DWORD dwPID, _In_ ULONG64 qwA, _Out_writes_(cb) PBYTE pb, _In_ DWORD cb, _Out_opt_ PDWORD pcbReadOpt, _In_ ULONG64 flags);

/*
* Ԥȡһ���ַ����pA������ָ�������ڴ滺���С��˺��������������ϴ����֪��ȡ����Ԥȡ�����ػ����У�Ȼ��ӻ����н��ж�ν�С�Ķ�ȡ�������˺�������������ܡ�
* -- dwPID = Ŀ����̵�PID��(DWORD)-1 ���������ڴ档
* -- pPrefetchAddresses = ҪԤȡ������ĵ�ַ���顣
* -- cPrefetchAddresses = ��ַ������
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_MemPrefetchPages(_In_ DWORD dwPID, _In_reads_(cPrefetchAddresses) PULONG64 pPrefetchAddresses, _In_ DWORD cPrefetchAddresses);

/*
* �������������С���ڴ���д�����ݡ���ע�⣬ĳЩ�����ڴ棬���ִ���ļ���ҳ�棨����DLL���������ڲ�ͬ�Ľ���֮�乲�����磬��һ�������ж�kernel3
  2.dll��д�������ܻ�Ӱ������ϵͳ�е����н����е�kernel32���Ѻ�ջ�Լ������ڴ�ͨ���ǿ��԰�ȫд��ġ���д���ڴ�ʱ����С�Ĵ���
* -- dwPID = Ŀ����̵�PID��(DWORD)-1 ���������ڴ档
* -- qwA = Ҫд����ڴ��ַ��
* -- pb = Ҫд������ݻ�������
* -- cb = Ҫд��������ֽ�����
* -- return = д��ɹ�����TRUE�����ֻ����ֽ�д�뷵��FALSE��
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_MemWrite(_In_ DWORD dwPID, _In_ ULONG64 qwA, _In_reads_(cb) PBYTE pb, _In_ DWORD cb);

/*
* ͨ������ָ�����̵�ҳ�������ַת��Ϊ�����ַ��
* -- dwPID = Ŀ����̵�PID��
* -- qwVA = Ҫת���������ַ��
* -- pqwPA = ���������ַ��ָ�롣
* -- return = ת���ɹ�����TRUE��ʧ�ܷ���FALSE��
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_MemVirt2Phys(_In_ DWORD dwPID, _In_ ULONG64 qwVA, _Out_ PULONG64 pqwPA);



//-----------------------------------------------------------------------------
// VMM����ӳ�书�����£�
// ���ڼ����������ص���Ŀ���ϣ�����ҳ��ӳ�䣨PTE���������ַ������ӳ�䣨VAD�����Ѽ���ģ�顢�Ѻ��̡߳�
//-----------------------------------------------------------------------------

#define VMMDLL_MAP_PTE_VERSION              2
#define VMMDLL_MAP_VAD_VERSION              6
#define VMMDLL_MAP_VADEX_VERSION            3
#define VMMDLL_MAP_MODULE_VERSION           5
#define VMMDLL_MAP_UNLOADEDMODULE_VERSION   2
#define VMMDLL_MAP_EAT_VERSION              2
#define VMMDLL_MAP_IAT_VERSION              2
#define VMMDLL_MAP_HEAP_VERSION             2
#define VMMDLL_MAP_THREAD_VERSION           3
#define VMMDLL_MAP_HANDLE_VERSION           2
#define VMMDLL_MAP_NET_VERSION              3
#define VMMDLL_MAP_PHYSMEM_VERSION          2
#define VMMDLL_MAP_USER_VERSION             2
#define VMMDLL_MAP_SERVICE_VERSION          3

// flags to check for existence in the fPage field of VMMDLL_MAP_PTEENTRY
#define VMMDLL_MEMMAP_FLAG_PAGE_W          0x0000000000000002
#define VMMDLL_MEMMAP_FLAG_PAGE_NS         0x0000000000000004
#define VMMDLL_MEMMAP_FLAG_PAGE_NX         0x8000000000000000
#define VMMDLL_MEMMAP_FLAG_PAGE_MASK       0x8000000000000006

typedef enum tdVMMDLL_PTE_TP {
    VMMDLL_PTE_TP_NA = 0,
    VMMDLL_PTE_TP_HARDWARE = 1,
    VMMDLL_PTE_TP_TRANSITION = 2,
    VMMDLL_PTE_TP_PROTOTYPE = 3,
    VMMDLL_PTE_TP_DEMANDZERO = 4,
    VMMDLL_PTE_TP_COMPRESSED = 5,
    VMMDLL_PTE_TP_PAGEFILE = 6,
} VMMDLL_PTE_TP, *PVMMDLL_PTE_TP;

typedef struct tdVMMDLL_MAP_PTEENTRY {
    QWORD vaBase;
    QWORD cPages;
    QWORD fPage;
    BOOL  fWoW64;
    DWORD _FutureUse1;
    union { LPSTR  uszText; LPWSTR wszText; };              // U/W dependant
    DWORD _Reserved1;
    DWORD cSoftware;    // # software (non active) PTEs in region
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
    union { LPSTR  uszText; LPWSTR wszText; };              // U/W dependant
    DWORD _FutureUse1;
    DWORD _Reserved1;
    QWORD vaFileObject;             // only valid if fFile/fImage _and_ after wszText is initialized
    DWORD cVadExPages;              // number of "valid" VadEx pages in this VAD.
    DWORD cVadExPagesBase;          // number of "valid" VadEx pages in "previous" VADs
    QWORD _Reserved2;
} VMMDLL_MAP_VADENTRY, *PVMMDLL_MAP_VADENTRY;

typedef struct tdVMMDLL_MAP_VADEXENTRY {
    VMMDLL_PTE_TP tp;
    DWORD iPML;
    QWORD va;
    QWORD pa;
    QWORD pte;
    struct {
        DWORD _Reserved1;
        VMMDLL_PTE_TP tp;
        QWORD pa;
        QWORD pte;
    } proto;
    QWORD vaVadBase;
} VMMDLL_MAP_VADEXENTRY, *PVMMDLL_MAP_VADEXENTRY;

typedef enum tdVMMDLL_MODULE_TP {
    VMMDLL_MODULE_TP_NORMAL = 0,
    VMMDLL_MODULE_TP_DATA = 1,
    VMMDLL_MODULE_TP_NOTLINKED = 2,
    VMMDLL_MODULE_TP_INJECTED = 3,
} VMMDLL_MODULE_TP;

typedef struct tdVMMDLL_MAP_MODULEENTRY {
    QWORD vaBase;
    QWORD vaEntry;
    DWORD cbImageSize;
    BOOL  fWoW64;
    union { LPSTR  uszText; LPWSTR wszText; };              // U/W dependant
    DWORD _Reserved3;
    DWORD _Reserved4;
    union { LPSTR  uszFullName; LPWSTR wszFullName; };      // U/W dependant
    VMMDLL_MODULE_TP tp;
    DWORD cbFileSizeRaw;
    DWORD cSection;
    DWORD cEAT;
    DWORD cIAT;
    DWORD _Reserved2;
    QWORD _Reserved1[2];
} VMMDLL_MAP_MODULEENTRY, *PVMMDLL_MAP_MODULEENTRY;

typedef struct tdVMMDLL_MAP_UNLOADEDMODULEENTRY {
    QWORD vaBase;
    DWORD cbImageSize;
    BOOL  fWoW64;
    union { LPSTR  uszText; LPWSTR wszText; };              // U/W dependant
    DWORD _FutureUse1;
    DWORD dwCheckSum;               // user-mode only
    DWORD dwTimeDateStamp;          // user-mode only
    DWORD _Reserved1;
    QWORD ftUnload;                 // kernel-mode only
} VMMDLL_MAP_UNLOADEDMODULEENTRY, *PVMMDLL_MAP_UNLOADEDMODULEENTRY;

typedef struct tdVMMDLL_MAP_EATENTRY {
    QWORD vaFunction;
    DWORD dwOrdinal;
    DWORD oFunctionsArray;          // PIMAGE_EXPORT_DIRECTORY->AddressOfFunctions[oFunctionsArray]
    DWORD oNamesArray;              // PIMAGE_EXPORT_DIRECTORY->AddressOfNames[oNamesArray]
    DWORD _FutureUse1;
    union { LPSTR  uszFunction; LPWSTR wszFunction; };      // U/W dependant
} VMMDLL_MAP_EATENTRY, *PVMMDLL_MAP_EATENTRY;

typedef struct tdVMMDLL_MAP_IATENTRY {
    QWORD vaFunction;
    union { LPSTR  uszFunction; LPWSTR wszFunction; };      // U/W dependant
    DWORD _FutureUse1;
    DWORD _FutureUse2;
    union { LPSTR  uszModule; LPWSTR wszModule; };          // U/W dependant
    struct {
        BOOL f32;
        WORD wHint;
        WORD _Reserved1;
        DWORD rvaFirstThunk;
        DWORD rvaOriginalFirstThunk;
        DWORD rvaNameModule;
        DWORD rvaNameFunction;
    } Thunk;
} VMMDLL_MAP_IATENTRY, *PVMMDLL_MAP_IATENTRY;

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
    QWORD vaStackBaseUser;          // value from _NT_TIB / _TEB
    QWORD vaStackLimitUser;         // value from _NT_TIB / _TEB
    QWORD vaStackBaseKernel;
    QWORD vaStackLimitKernel;
    QWORD vaTrapFrame;
    QWORD vaRIP;                    // RIP register (if user mode)
    QWORD vaRSP;                    // RSP register (if user mode)
    QWORD qwAffinity;
    DWORD dwUserTime;
    DWORD dwKernelTime;
    UCHAR bSuspendCount;
    UCHAR _FutureUse1[3];
    DWORD _FutureUse2[15];
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
    union { LPSTR  uszText; LPWSTR wszText; };              // U/W dependant
    DWORD _FutureUse2;
    DWORD dwPID;
    DWORD dwPoolTag;
    DWORD _FutureUse[5];
    union { LPSTR  uszType; LPWSTR wszType; };              // U/W dependant
} VMMDLL_MAP_HANDLEENTRY, *PVMMDLL_MAP_HANDLEENTRY;

typedef struct tdVMMDLL_MAP_NETENTRY {
    DWORD dwPID;
    DWORD dwState;
    WORD _FutureUse3[3];
    WORD AF;                        // address family (IPv4/IPv6)
    struct {
        BOOL fValid;
        WORD _Reserved;
        WORD port;
        BYTE pbAddr[16];            // ipv4 = 1st 4 bytes, ipv6 = all bytes
        union { LPSTR  uszText; LPWSTR wszText; };          // U/W dependant
    } Src;
    struct {
        BOOL fValid;
        WORD _Reserved;
        WORD port;
        BYTE pbAddr[16];            // ipv4 = 1st 4 bytes, ipv6 = all bytes
        union { LPSTR  uszText; LPWSTR wszText; };          // U/W dependant
    } Dst;
    QWORD vaObj;
    QWORD ftTime;
    DWORD dwPoolTag;
    DWORD _FutureUse4;
    union { LPSTR  uszText; LPWSTR wszText; };              // U/W dependant
    DWORD _FutureUse2[4];
} VMMDLL_MAP_NETENTRY, *PVMMDLL_MAP_NETENTRY;

typedef struct tdVMMDLL_MAP_PHYSMEMENTRY {
    QWORD pa;
    QWORD cb;
} VMMDLL_MAP_PHYSMEMENTRY, *PVMMDLL_MAP_PHYSMEMENTRY;

typedef struct tdVMMDLL_MAP_USERENTRY {
    DWORD _FutureUse1[2];
    union { LPSTR  uszText; LPWSTR wszText; };              // U/W dependant
    ULONG64 vaRegHive;
    union { LPSTR  uszSID; LPWSTR wszSID; };                // U/W dependant
    DWORD _FutureUse2[2];
} VMMDLL_MAP_USERENTRY, *PVMMDLL_MAP_USERENTRY;

typedef struct tdVMMDLL_MAP_SERVICEENTRY {
    QWORD vaObj;
    DWORD dwOrdinal;
    DWORD dwStartType;
    SERVICE_STATUS ServiceStatus;
    union { LPSTR  uszServiceName; LPWSTR wszServiceName; };// U/W dependant
    union { LPSTR  uszDisplayName; LPWSTR wszDisplayName; };// U/W dependant
    union { LPSTR  uszPath; LPWSTR wszPath; };              // U/W dependant
    union { LPSTR  uszUserTp; LPWSTR wszUserTp; };          // U/W dependant
    union { LPSTR  uszUserAcct; LPWSTR wszUserAcct; };      // U/W dependant
    union { LPSTR  uszImagePath; LPWSTR wszImagePath; };    // U/W dependant
    DWORD dwPID;
    DWORD _FutureUse1;
    QWORD _FutureUse2;
} VMMDLL_MAP_SERVICEENTRY, *PVMMDLL_MAP_SERVICEENTRY;

typedef struct tdVMMDLL_MAP_PTE {
    DWORD dwVersion;
    DWORD _Reserved1[5];
    PBYTE pbMultiText;              // NULL or multi-wstr pointed into by VMMDLL_MAP_VADENTRY.wszText
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_PTEENTRY pMap[];     // map entries.
} VMMDLL_MAP_PTE, *PVMMDLL_MAP_PTE;

typedef struct tdVMMDLL_MAP_VAD {
    DWORD dwVersion;
    DWORD _Reserved1[4];
    DWORD cPage;                    // # pages in vad map.
    PBYTE pbMultiText;              // NULL or multi-wstr pointed into by VMMDLL_MAP_VADENTRY.wszText
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_VADENTRY pMap[];     // map entries.
} VMMDLL_MAP_VAD, *PVMMDLL_MAP_VAD;

typedef struct tdVMMDLL_MAP_VADEX {
    DWORD dwVersion;
    DWORD _Reserved1[4];
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_VADEXENTRY pMap[];   // map entries.
} VMMDLL_MAP_VADEX, *PVMMDLL_MAP_VADEX;

typedef struct tdVMMDLL_MAP_MODULE {
    DWORD dwVersion;
    DWORD _Reserved1[5];
    PBYTE pbMultiText;              // multi-wstr pointed into by VMMDLL_MAP_MODULEENTRY.wszText
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_MODULEENTRY pMap[];  // map entries.
} VMMDLL_MAP_MODULE, *PVMMDLL_MAP_MODULE;

typedef struct tdVMMDLL_MAP_UNLOADEDMODULE {
    DWORD dwVersion;
    DWORD _Reserved1[5];
    PBYTE pbMultiText;              // multi-wstr pointed into by VMMDLL_MAP_MODULEENTRY.wszText
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_UNLOADEDMODULEENTRY pMap[];  // map entries.
} VMMDLL_MAP_UNLOADEDMODULE, *PVMMDLL_MAP_UNLOADEDMODULE;

typedef struct tdVMMDLL_MAP_EAT {
    DWORD dwVersion;
    DWORD dwOrdinalBase;
    DWORD cNumberOfNames;
    DWORD cNumberOfFunctions;
    DWORD _Reserved1[4];
    QWORD vaModuleBase;
    QWORD vaAddressOfFunctions;
    QWORD vaAddressOfNames;
    PBYTE pbMultiText;              // multi-str pointed into by VMM_MAP_EATENTRY.wszFunction
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_EATENTRY pMap[];     // map entries.
} VMMDLL_MAP_EAT, *PVMMDLL_MAP_EAT;

typedef struct tdVMMDLL_MAP_IAT {
    DWORD dwVersion;
    DWORD _Reserved1[5];
    QWORD vaModuleBase;
    PBYTE pbMultiText;              // multi-str pointed into by VMM_MAP_EATENTRY.[wszFunction|wszModule]
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_IATENTRY pMap[];     // map entries.
} VMMDLL_MAP_IAT, *PVMMDLL_MAP_IAT;

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
    PBYTE pbMultiText;              // multi-wstr pointed into by VMMDLL_MAP_HANDLEENTRY.wszText
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_HANDLEENTRY pMap[];  // map entries.
} VMMDLL_MAP_HANDLE, *PVMMDLL_MAP_HANDLE;

typedef struct tdVMMDLL_MAP_NET {
    DWORD dwVersion;
    DWORD _Reserved1;
    PBYTE pbMultiText;              // multi-wstr pointed into by VMM_MAP_NETENTRY.wszText
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_NETENTRY pMap[];     // map entries.
} VMMDLL_MAP_NET, *PVMMDLL_MAP_NET;

typedef struct tdVMMDLL_MAP_PHYSMEM {
    DWORD dwVersion;
    DWORD _Reserved1[5];
    DWORD cMap;                     // # map entries.
    DWORD _Reserved2;
    VMMDLL_MAP_PHYSMEMENTRY pMap[]; // map entries.
} VMMDLL_MAP_PHYSMEM, *PVMMDLL_MAP_PHYSMEM;

typedef struct tdVMMDLL_MAP_USER {
    DWORD dwVersion;
    DWORD _Reserved1[5];
    PBYTE pbMultiText;              // multi-wstr pointed into by VMMDLL_MAP_USERENTRY.wszText
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_USERENTRY pMap[];    // map entries.
} VMMDLL_MAP_USER, *PVMMDLL_MAP_USER;

typedef struct tdVMMDLL_MAP_SERVICE {
    DWORD dwVersion;
    DWORD _Reserved1[5];
    PBYTE pbMultiText;              // multi-wstr pointed into by VMMDLL_MAP_SERVICEENTRY.wsz*
    DWORD cbMultiText;
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_SERVICEENTRY pMap[]; // map entries.
} VMMDLL_MAP_SERVICE, *PVMMDLL_MAP_SERVICE;

/*
* ���ݽ��̵�Ӳ��ҳ��PTE�������ڴ�ӳ����Ŀ�������pPteMap����ΪNULL���򽫷���������ֽ����洢�ڲ���pcbPteMap�С�
* ���ص���Ŀ��VMMDLL_MAP_PTEENTRY.vaBase����
* -- dwPID
* -- pPteMap = ��С�ֽڳ���*pcbPteMap�Ļ���������ΪNULL��
* -- pcbPteMap = ָ��pPteMap�������ֽ�����ָ�롣
* -- fIdentifyModules = �Ƿ���ʶ��ģ�飨������
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetPteU(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbPteMap) PVMMDLL_MAP_PTE pPteMap, _Inout_ PDWORD pcbPteMap, _In_ BOOL fIdentifyModules);
_Success_(return) BOOL VMMDLL_Map_GetPteW(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbPteMap) PVMMDLL_MAP_PTE pPteMap, _Inout_ PDWORD pcbPteMap, _In_ BOOL fIdentifyModules);

/*
* ���ݽ��̵������ַ��������VAD�������ڴ�ӳ����Ŀ�������pVadMap����ΪNULL���򽫷���������ֽ����洢�ڲ���pcbVadMap�С�
* ���ص���Ŀ��VMMDLL_MAP_VADENTRY.vaStart����
* -- dwPID
* -- pVadMap = ��С�ֽڳ���*pcbVadMap�Ļ���������ΪNULL��
* -- pcbVadMap = ָ��pVadMap�������ֽ�����ָ�롣
* -- fIdentifyModules = �Ƿ���ʶ��ģ�飨������
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetVadU(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbVadMap) PVMMDLL_MAP_VAD pVadMap, _Inout_ PDWORD pcbVadMap, _In_ BOOL fIdentifyModules);
_Success_(return) BOOL VMMDLL_Map_GetVadW(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbVadMap) PVMMDLL_MAP_VAD pVadMap, _Inout_ PDWORD pcbVadMap, _In_ BOOL fIdentifyModules);

/*
* �����й��ڴ�ӳ���һ����ѡ���Ӽ�����չ�ڴ�ӳ����Ϣ�������pVadExMap����ΪNULL���򽫷���������ֽ����洢�ڲ���pcbVadExMap�С�
* -- dwPID
* -- pVadExMap = ��С�ֽڳ���*pcbVadExMap�Ļ���������ΪNULL��
* -- pcbVadExMap = ָ��pVadExMap�������ֽ�����ָ�롣
* -- oPage = �ӽ��̿�ʼ��ҳ��ƫ��������ҳ��ƣ���
* -- cPage = ��oPages������ʼ�����ҳ������
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_Map_GetVadEx(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbVadExMap) PVMMDLL_MAP_VADEX pVadExMap, _Inout_ PDWORD pcbVadExMap, _In_ DWORD oPage, _In_ DWORD cPage);

/*
* ����ָ�����̵�ģ�飨.dll�ļ����������pModuleMap����ΪNULL���򽫷���������ֽ����洢�ڲ���pcbModuleMap�С�
* -- dwPID
* -- pModuleMap = ��С�ֽڳ���*pcbModuleMap�Ļ���������ΪNULL��
* -- pcbModuleMap = ָ��pModuleMap�������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetModuleU(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbModuleMap) PVMMDLL_MAP_MODULE pModuleMap, _Inout_ PDWORD pcbModuleMap);
_Success_(return) BOOL VMMDLL_Map_GetModuleW(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbModuleMap) PVMMDLL_MAP_MODULE pModuleMap, _Inout_ PDWORD pcbModuleMap);

/*
* �����������̺�ģ�����Ƶ�ģ�飨.dll����Ŀ��
* �����pModuleMapEntry����ΪNULL���򽫷���������ֽ����洢�ڲ���pcbModuleMapEntry�С�
* �����pcbModuleMapEntry����ΪNULL����ٶ�pModuleMapEntry�Ĵ�СΪsizeof(VMMDLL_MAP_MODULEENTRY)�����ҽ�����û�����Ƶ����ݡ�
* -- dwPID
* -- [uw]szModuleName
* -- pModuleMapEntry = ��С�ֽڳ���*pcbModuleMapEntry�Ļ���������ΪNULL��
* -- pcbModuleMapEntry = ָ��pModuleMapEntry�������ֽ�����ָ�룬��ΪNULL��
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetModuleFromNameU(_In_ DWORD dwPID, _In_ LPSTR  uszModuleName, _Out_writes_bytes_opt_(*pcbModuleMapEntry) PVMMDLL_MAP_MODULEENTRY pModuleMapEntry, _Inout_opt_ PDWORD pcbModuleMapEntry);
_Success_(return) BOOL VMMDLL_Map_GetModuleFromNameW(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _Out_writes_bytes_opt_(*pcbModuleMapEntry) PVMMDLL_MAP_MODULEENTRY pModuleMapEntry, _Inout_opt_ PDWORD pcbModuleMapEntry);

/*
* ����ָ�����̵���ж�ص�ģ�飨.dll/.sys���������pUnloadedModuleMap����ΪNULL���򽫷���������ֽ����洢�ڲ���pcbUnloadedModuleMap�С�
* -- dwPID
* -- pUnloadedModuleMap = ��С�ֽڳ���*pcbUnloadedModuleMap�Ļ���������ΪNULL��
* -- pcbUnloadedModuleMap = ָ��pUnloadedModuleMap�������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetUnloadedModuleU(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbUnloadedModuleMap) PVMMDLL_MAP_UNLOADEDMODULE pUnloadedModuleMap, _Inout_ PDWORD pcbUnloadedModuleMap);
_Success_(return) BOOL VMMDLL_Map_GetUnloadedModuleW(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbUnloadedModuleMap) PVMMDLL_MAP_UNLOADEDMODULE pUnloadedModuleMap, _Inout_ PDWORD pcbUnloadedModuleMap);

/*
* �ӵ�����ַ��EAT���м���ģ�鵼���ĺ����������pEatMap����ΪNULL���򽫷���������ֽ����洢�ڲ���pcbEatMap�С�
* -- dwPID
* -- [uw]szModuleName
* -- pEatMap = ��С�ֽڳ���*pcbEatMap�Ļ���������ΪNULL��
* -- pcbEatMap = ָ��pEatMap�������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetEATU(_In_ DWORD dwPID, _In_ LPSTR  uszModuleName, _Out_writes_bytes_opt_(*pcbEatMap) PVMMDLL_MAP_EAT pEatMap, _Inout_ PDWORD pcbEatMap);
_Success_(return) BOOL VMMDLL_Map_GetEATW(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _Out_writes_bytes_opt_(*pcbEatMap) PVMMDLL_MAP_EAT pEatMap, _Inout_ PDWORD pcbEatMap);

/*
* �ӵ����ַ��IAT���м���ģ�鵼��ĺ����������pIatMap����ΪNULL���򽫷���������ֽ����洢�ڲ���pcbIatMap�С�
* -- dwPID
* -- [uw]szModuleName
* -- pIatMap = ��С�ֽڳ���*pcbIatMap�Ļ���������ΪNULL��
* -- pcbIatMap = ָ��pIatMap�������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetIATU(_In_ DWORD dwPID, _In_ LPSTR  uszModuleName, _Out_writes_bytes_opt_(*pcbIatMap) PVMMDLL_MAP_IAT pIatMap, _Inout_ PDWORD pcbIatMap);
_Success_(return) BOOL VMMDLL_Map_GetIATW(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _Out_writes_bytes_opt_(*pcbIatMap) PVMMDLL_MAP_IAT pIatMap, _Inout_ PDWORD pcbIatMap);

/*
* ����ָ�����̵Ķѡ������pHeapMap����ΪNULL���򽫷���������ֽ����洢�ڲ���pcbHeapMap�С�
* -- dwPID
* -- pHeapMap = ��С�ֽڳ���*pcbHeapMap�Ļ���������ΪNULL��
* -- pcbHeapMap = ָ��pHeapMap�������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetHeap(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbHeapMap) PVMMDLL_MAP_HEAP pHeapMap, _Inout_ PDWORD pcbHeapMap);

/*
* ����ָ�����̵��̡߳������pThreadMap����ΪNULL���򽫷���������ֽ����洢�ڲ���pcbThreadMap�С�
* ���ص���Ŀ��VMMDLL_MAP_THREADENTRY.dwTID����
* -- dwPID
* -- pThreadMap = ��С�ֽڳ���*pcbThreadMap�Ļ���������ΪNULL��
* -- pcbThreadMap = ָ��pThreadMap�������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetThread(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbThreadMap) PVMMDLL_MAP_THREAD pThreadMap, _Inout_ PDWORD pcbThreadMap);

/*
* ����ָ�����̵ľ���������pHandleMap����ΪNULL���򽫷���������ֽ����洢�ڲ���pcbHandleMap�С�
* ���ص���Ŀ��VMMDLL_MAP_HANDLEENTRY.dwHandle����
* -- dwPID
* -- pHandleMap = ��С�ֽڳ���*pcbHandleMap�Ļ���������ΪNULL��
* -- pcbHandleMap = ָ��pHandleMap�������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetHandleU(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbHandleMap) PVMMDLL_MAP_HANDLE pHandleMap, _Inout_ PDWORD pcbHandleMap);
_Success_(return) BOOL VMMDLL_Map_GetHandleW(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbHandleMap) PVMMDLL_MAP_HANDLE pHandleMap, _Inout_ PDWORD pcbHandleMap);

/*
* ����Windowsö�ٵ������ڴ�ӳ���е������ڴ淶Χ��
* -- pPhysMemMap = ��С�ֽڳ���*pcbPhysMemMap�Ļ���������ΪNULL��
* -- pcbPhysMemMap = ָ��pPhysMemMap�������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetPhysMem(_Out_writes_bytes_opt_(*pcbPhysMemMap) PVMMDLL_MAP_PHYSMEM pPhysMemMap, _Inout_ PDWORD pcbPhysMemMap);

/*
* ������������ӳ�� - ������������ӡ������׽��ֺ��������繦�ܡ�
* -- pNetMap = ��С�ֽڳ���*pcbNetMap�Ļ���������ΪNULL��
* -- pcbNetMap = ָ��pNetMap�������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetNetU(_Out_writes_bytes_opt_(*pcbNetMap) PVMMDLL_MAP_NET pNetMap, _Inout_ PDWORD pcbNetMap);
_Success_(return) BOOL VMMDLL_Map_GetNetW(_Out_writes_bytes_opt_(*pcbNetMap) PVMMDLL_MAP_NET pNetMap, _Inout_ PDWORD pcbNetMap);

/*
* ������Ŀ��ϵͳ�м�⵽�ķ�֪���û���
* ע�⣡ϵͳ�п��ܴ��ڸ�����û�����ֻ�ܼ�⵽�Ѽ���ע����ļ����û� - ���Ǿ��л���̵��û���������Ϊ��
* -- pUserMap = ��С�ֽڳ���*pcbUserMap�Ļ���������ΪNULL��
* -- pcbUserMap = ָ��pUserMap�������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetUsersU(_Out_writes_bytes_opt_(*pcbUserMap) PVMMDLL_MAP_USER pUserMap, _Inout_ PDWORD pcbUserMap);
_Success_(return) BOOL VMMDLL_Map_GetUsersW(_Out_writes_bytes_opt_(*pcbUserMap) PVMMDLL_MAP_USER pUserMap, _Inout_ PDWORD pcbUserMap);

/*
* Retrieve the services currently known by the service control manager (SCM).
* ������Ŀ��ϵͳ�м�⵽�ķ�����Ϣ��
* -- pServiceMap = ��С�ֽڳ���*pcbServiceMap�Ļ���������ΪNULL��
* -- pcbServiceMap = ָ��pServiceMap�������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetServicesU(_Out_writes_bytes_opt_(*pcbServiceMap) PVMMDLL_MAP_SERVICE pServiceMap, _Inout_ PDWORD pcbServiceMap);
_Success_(return) BOOL VMMDLL_Map_GetServicesW(_Out_writes_bytes_opt_(*pcbServiceMap) PVMMDLL_MAP_SERVICE pServiceMap, _Inout_ PDWORD pcbServiceMap);



//-----------------------------------------------------------------------------
// WINDOWS SPECIFIC PAGE FRAME NUMBER (PFN) FUNCTIONALITY BELOW
//-----------------------------------------------------------------------------

#define VMMDLL_MAP_PFN_VERSION              1

static LPCSTR VMMDLL_PFN_TYPE_TEXT[] = { "Zero", "Free", "Standby", "Modifiy", "ModNoWr", "Bad", "Active", "Transit" };
static LPCSTR VMMDLL_PFN_TYPEEXTENDED_TEXT[] = { "-", "Unused", "ProcPriv", "PageTable", "LargePage", "DriverLock", "Shareable", "File" };

typedef enum tdVMMDLL_MAP_PFN_TYPE {
    VmmDll_PfnTypeZero = 0,
    VmmDll_PfnTypeFree = 1,
    VmmDll_PfnTypeStandby = 2,
    VmmDll_PfnTypeModified = 3,
    VmmDll_PfnTypeModifiedNoWrite = 4,
    VmmDll_PfnTypeBad = 5,
    VmmDll_PfnTypeActive = 6,
    VmmDll_PfnTypeTransition = 7
} VMMDLL_MAP_PFN_TYPE;

typedef enum tdVMMDLL_MAP_PFN_TYPEEXTENDED {
    VmmDll_PfnExType_Unknown = 0,
    VmmDll_PfnExType_Unused = 1,
    VmmDll_PfnExType_ProcessPrivate = 2,
    VmmDll_PfnExType_PageTable = 3,
    VmmDll_PfnExType_LargePage = 4,
    VmmDll_PfnExType_DriverLocked = 5,
    VmmDll_PfnExType_Shareable = 6,
    VmmDll_PfnExType_File = 7,
} VMMDLL_MAP_PFN_TYPEEXTENDED;

typedef struct tdVMMDLL_MAP_PFNENTRY {
    DWORD dwPfn;
    VMMDLL_MAP_PFN_TYPEEXTENDED tpExtended;
    struct {        // Only valid if active non-prototype PFN
        union {
            DWORD dwPid;
            DWORD dwPfnPte[5];  // PFN of paging levels 1-4 (x64)
        };
        QWORD va;               // valid if non-zero
    } AddressInfo;
    QWORD vaPte;
    QWORD OriginalPte;
    union {
        DWORD _u3;
        struct {
            WORD ReferenceCount;
            // MMPFNENTRY
            BYTE PageLocation       : 3;    // Pos 0  - VMMDLL_MAP_PFN_TYPE
            BYTE WriteInProgress    : 1;    // Pos 3
            BYTE Modified           : 1;    // Pos 4
            BYTE ReadInProgress     : 1;    // Pos 5
            BYTE CacheAttribute     : 2;    // Pos 6
            BYTE Priority           : 3;    // Pos 0
            BYTE Rom_OnProtectedStandby : 1;// Pos 3
            BYTE InPageError        : 1;    // Pos 4
            BYTE KernelStack_SystemChargedPage : 1; // Pos 5
            BYTE RemovalRequested   : 1;    // Pos 6
            BYTE ParityError        : 1;    // Pos 7
        };
    };
    union {
        QWORD _u4;
        struct {
            DWORD PteFrame;
            DWORD PteFrameHigh      : 4;    // Pos 32
            DWORD _Reserved         : 21;   // Pos 36
            DWORD PrototypePte      : 1;    // Pos 57
            DWORD PageColor         : 6;    // Pos 58
        };
    };
    DWORD _FutureUse[6];
} VMMDLL_MAP_PFNENTRY, *PVMMDLL_MAP_PFNENTRY;

typedef struct tdVMMDLL_MAP_PFN {
    DWORD dwVersion;
    DWORD _Reserved1[5];
    DWORD cMap;                     // # map entries.
    VMMDLL_MAP_PFNENTRY pMap[];     // map entries.
} VMMDLL_MAP_PFN, *PVMMDLL_MAP_PFN;

/*
* ����ɢ�е�ҳ��ţ�PFN������Ϣ��PFN����������pPfns�����д洢��˳�򷵻ء�
* -- pPfns
* -- cPfns
* -- pPfnMap = ��С�ֽڳ���*pcbPfnMap�Ļ���������ΪNULL��
* -- pcbPfnMap = ָ��pPfnMap�������ֽ�����ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_Map_GetPfn(_In_ DWORD pPfns[], _In_ DWORD cPfns, _Out_writes_bytes_opt_(*pcbPfnMap) PVMMDLL_MAP_PFN pPfnMap, _Inout_ PDWORD pcbPfnMap);



//-----------------------------------------------------------------------------
// VMM PROCESS FUNCTIONALITY BELOW:
// Functionality below is mostly relating to Windows processes.
//-----------------------------------------------------------------------------

/*
* ���ݽ�������������̡���ע�⣬������ڶ��������ͬ���ƵĽ��̣�ֻ�᷵��һ�����̡������Ҫ����������ͬ���Ƶ����н��̣�
  ��ͨ������VMMDLL_PidList��VMMDLL_ProcessGetInformation����PID�б�
* -- szProcName = �������������ִ�Сд����
* -- pdwPID = �ڳɹ�ʱ����PID��ָ�롣
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_PidGetFromName(_In_ LPSTR szProcName, _Out_ PDWORD pdwPID);

/*
* �г�ϵͳ�е�PID��
* -- pPIDs = ���پ���ϵͳ��PID������DWORD���飬��ΪNULL��
* -- pcPIDs = pPIDs����Ĵ�С��DWORD����������ʱ����ϵͳ�е�PID���������ʱ����
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_PidList(_Out_writes_opt_(*pcPIDs) PDWORD pPIDs, _Inout_ PULONG64 pcPIDs); 

#define VMMDLL_PROCESS_INFORMATION_MAGIC        0xc0ffee663df9301e
#define VMMDLL_PROCESS_INFORMATION_VERSION      6

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
    struct {
        ULONG64 vaEPROCESS;
        ULONG64 vaPEB;
        ULONG64 _Reserved1;
        BOOL fWow64;
        DWORD vaPEB32;                  // WoW64 only
        DWORD dwSessionId;
        ULONG64 qwLUID;
        CHAR szSID[MAX_PATH];
    } win;
} VMMDLL_PROCESS_INFORMATION, *PVMMDLL_PROCESS_INFORMATION;

/*
/*
* ��PID�м������ֽ�����Ϣ�����Լ������̵����ơ�ҳĿ¼��ַ�ͽ���״̬����Ϣ��
* -- dwPID
* -- pProcessInformation = ���ΪNULL�����С��*pcbProcessInfo����
* -- pcbProcessInformation = pProcessInfo�Ĵ�С�����ֽ�Ϊ��λ��������������
* -- return = �ɹ�/ʧ�ܡ�
*/
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_ProcessGetInformation(_In_ DWORD dwPID, _Inout_opt_ PVMMDLL_PROCESS_INFORMATION pProcessInformation, _In_ PSIZE_T pcbProcessInformation);

#define VMMDLL_PROCESS_INFORMATION_OPT_STRING_PATH_KERNEL           1
#define VMMDLL_PROCESS_INFORMATION_OPT_STRING_PATH_USER_IMAGE       2
#define VMMDLL_PROCESS_INFORMATION_OPT_STRING_CMDLINE               3

/*
/*
* �ӽ����м����ַ���ֵ���ú�������һ���µ��ַ������������������з�����������ַ�����
* �ַ���ʼ����NULL��β�����ʧ�ܣ��򷵻�NULL��
* ע�⣡�����߸���VMMDLL_MemFree�ķ���ֵ��
* �������ͷ�: VMMDLL_MemFree(return)
* -- dwPID
* -- fOptionString = Ҫ�������ַ���ֵ����VMMDLL_PROCESS_INFORMATION_OPT_STRING_*ָ��
* -- return - ʧ��: NULL���ɹ�: �ַ��� - ע�⣡�����ɵ�����ʹ��VMMDLL_MemFree�����ͷţ�
*/
*/
EXPORTED_FUNCTION
LPSTR VMMDLL_ProcessGetInformationString(_In_ DWORD dwPID, _In_ DWORD fOptionString);

/*
* �����й�����Ŀ¼���ڡ�������ַ��͵����ַ��IAT������Ϣ��
* ���pData == NULL���ڽ���ʱ���뷵��pData��������Ҫ�������ݵ���Ŀ����
* -- dwPID
* -- [uw]szModule
* -- pData
* -- cData
* -- pcData
* -- return = �ɹ�/ʧ�ܡ�
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_ProcessGetDirectoriesU(_In_ DWORD dwPID, _In_ LPSTR  uszModule, _Out_writes_(16) PIMAGE_DATA_DIRECTORY pData, _In_ DWORD cData, _Out_ PDWORD pcData);
_Success_(return) BOOL VMMDLL_ProcessGetDirectoriesW(_In_ DWORD dwPID, _In_ LPWSTR wszModule, _Out_writes_(16) PIMAGE_DATA_DIRECTORY pData, _In_ DWORD cData, _Out_ PDWORD pcData);
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_ProcessGetSectionsU(_In_ DWORD dwPID, _In_ LPSTR  uszModule, _Out_opt_ PIMAGE_SECTION_HEADER pData, _In_ DWORD cData, _Out_ PDWORD pcData);
_Success_(return) BOOL VMMDLL_ProcessGetSectionsW(_In_ DWORD dwPID, _In_ LPWSTR wszModule, _Out_opt_ PIMAGE_SECTION_HEADER pData, _In_ DWORD cData, _Out_ PDWORD pcData);

/*
/*
* ��������/ģ���и��������������ַ��
* -- dwPID
* -- [uw]szModuleName
* -- szFunctionName
* -- return = �����������ַ��ʧ��ʱ�����㡣
*/
*/
EXPORTED_FUNCTION
ULONG64 VMMDLL_ProcessGetProcAddressU(_In_ DWORD dwPID, _In_ LPSTR  uszModuleName, _In_ LPSTR szFunctionName);
ULONG64 VMMDLL_ProcessGetProcAddressW(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _In_ LPSTR szFunctionName);

/*
/*
* ��������ģ��Ļ�ַ��
* -- dwPID
* -- [uw]szModuleName
* -- return = ģ��������ַ��ʧ��ʱ�����㡣
*/
*/
EXPORTED_FUNCTION
ULONG64 VMMDLL_ProcessGetModuleBaseU(_In_ DWORD dwPID, _In_ LPSTR  uszModuleName);
ULONG64 VMMDLL_ProcessGetModuleBaseW(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName);



//-----------------------------------------------------------------------------
// WINDOWS SPECIFIC DEBUGGING / SYMBOL FUNCTIONALITY BELOW:
//-----------------------------------------------------------------------------

/*
* ����һ�� .pdb �����ļ������ڳɹ�ʱ���ع�����ģ�����ơ�
* -- dwPID
* -- vaModuleBase
* -- szModuleName = ���ڽ��ճɹ�ʱ��ģ�����ƵĻ�������
* -- ����ֵ
*/
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_PdbLoad(_In_ DWORD dwPID, _In_ ULONG64 vaModuleBase, _Out_writes_(MAX_PATH) LPSTR szModuleName);

/*
* ����ģ�����ƺͷ������Ƽ������ŵ������ַ��
* ע�⣡��������ģ�鶼���ܴ��� - ���ֻ��ģ�� "nt" �ǿ��õġ�
* ע�⣡������ģ�������ͬ�����ƣ���ʹ�õ�һ����ӵ�ģ�顣
* -- szModule: ģ�����ơ�
* -- cbSymbolAddressOrOffset: ���ŵ������ַ�����ƫ������
* -- szSymbolName: ���ճɹ�ʱ�������ƵĻ�������
* -- pdwSymbolDisplacement: �ӷ�����ʼλ�õ�λ�ơ�
* -- ����ֵ��

*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_PdbSymbolName(_In_ LPSTR szModule, _In_ QWORD cbSymbolAddressOrOffset, _Out_writes_(MAX_PATH) LPSTR szSymbolName, _Out_opt_ PDWORD pdwSymbolDisplacement);

/*
* ����ģ�����ƺͷ������ƻ�ȡ���ŵ������ַ��
* ע�⣡��������ģ�鶼���ܴ��� - ���ֻ��ģ�� "nt" ���á�
* ע�⣡������ģ�������ͬ�����ƣ���ʹ��������ӵ�ģ�顣
* -- szModule��ģ�����ơ�
* -- szSymbolName���������ơ�
* -- pvaSymbolAddress�����ڽ��շ��������ַ��ָ�롣
* -- ����ֵ���ɹ���ʧ�ܡ�
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_PdbSymbolAddress(_In_ LPSTR szModule, _In_ LPSTR szSymbolName, _Out_ PULONG64 pvaSymbolAddress);

/*
* ����ģ�����ƺ��������ƻ�ȡ���͵Ĵ�С��
* ע�⣡��������ģ�鶼���ܴ��� - ���ֻ��ģ�� "nt" ���á�
* ע�⣡������ģ�������ͬ�����ƣ���ʹ��������ӵ�ģ�顣
* -- szModule��ģ�����ơ�
* -- szTypeName���������ơ�
* -- pcbTypeSize�����ڽ������ʹ�С��ָ�롣
* -- ����ֵ���ɹ���ʧ�ܡ�
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_PdbTypeSize(_In_ LPSTR szModule, _In_ LPSTR szTypeName, _Out_ PDWORD pcbTypeSize);

/*
* ��λ���������ƫ���� - ͨ���ǽṹ���е����
* ע�⣡��������ģ�鶼���ܴ��� - ���ֻ��ģ�� "nt" ���á�
* ע�⣡������ģ�������ͬ�����ƣ���ʹ��������ӵ�ģ�顣
* -- szModule��ģ�����ơ�
* -- uszTypeName���������ơ�
* -- uszTypeChildName�������������ơ�
* -- pcbTypeChildOffset�����ڽ�����������ƫ������ָ�롣
* -- ����ֵ���ɹ���ʧ�ܡ�
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_PdbTypeChildOffset(_In_ LPSTR szModule, _In_ LPSTR uszTypeName, _In_ LPSTR uszTypeChildName, _Out_ PDWORD pcbTypeChildOffset);



//-----------------------------------------------------------------------------
// WINDOWS SPECIFIC REGISTRY FUNCTIONALITY BELOW:
//-----------------------------------------------------------------------------

#define VMMDLL_REGISTRY_HIVE_INFORMATION_MAGIC      0xc0ffee653df8d01e
#define VMMDLL_REGISTRY_HIVE_INFORMATION_VERSION    3

typedef struct td_VMMDLL_REGISTRY_HIVE_INFORMATION {
    ULONG64 magic;
    WORD wVersion;
    WORD wSize;
    BYTE _FutureReserved1[0x14];
    ULONG64 vaCMHIVE;
    ULONG64 vaHBASE_BLOCK;
    DWORD cbLength;
    CHAR uszName[128];
    CHAR uszNameShort[32 + 1];
    CHAR uszHiveRootPath[MAX_PATH];
    QWORD _FutureReserved[0x10];
} VMMDLL_REGISTRY_HIVE_INFORMATION, *PVMMDLL_REGISTRY_HIVE_INFORMATION;

/*
* ����Ŀ��ϵͳ��ע���Hive����Ϣ��
* -- pHives�����ڽ�������Hive��Ϣ�Ļ���������СΪ cHives * sizeof(VMMDLL_REGISTRY_HIVE_INFORMATION)�����ΪNULL������ pcHives �з���Hive��������
* -- cHives�������� pHives �Ĵ�С����Hive����Ϊ��λ����
* -- pcHives����� pHives == NULL����Ϊ��Hive��������� pHives ��ΪNULL����Ϊ��ȡ��Hive������
* -- ����ֵ���ɹ���ʧ�ܡ�
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_WinReg_HiveList(_Out_writes_(cHives) PVMMDLL_REGISTRY_HIVE_INFORMATION pHives, _In_ DWORD cHives, _Out_ PDWORD pcHives);

/*
* ��ȡ����������������ע���Hive�ڴ棬�������ȡ���ֽ�����
* ע�⣡��ַ�ռ䲻����regfע���Hive�ļ�ͷ��
* -- vaCMHive��Hive�ڴ�������ַ��
* -- ra����ȡ����ʼƫ������
* -- pb�����ڽ��ն�ȡ���ݵĻ�������
* -- cb��Ҫ��ȡ���ֽ�����
* -- pcbReadOpt����ѡ���������ڷ���ʵ�ʶ�ȡ���ֽ�����
* -- flags����־λ����VMMDLL_FLAG_*�еı�־λ��ͬ��
* -- ����ֵ���ɹ���ʧ�ܡ�ע�⣡��ʹ��ȡ��0�ֽڣ���ȡ�������ܱ���Ϊ�ɹ� - ������֤pcbReadOpt������
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_WinReg_HiveReadEx(_In_ ULONG64 vaCMHive, _In_ DWORD ra, _Out_ PBYTE pb, _In_ DWORD cb, _Out_opt_ PDWORD pcbReadOpt, _In_ ULONG64 flags);

/*
* �����������������������ڴ�д��ע���Hive��
* ע�⣡��ַ�ռ䲻����regfע���Hive�ļ�ͷ��
* -- vaCMHive��Hive�ڴ�������ַ��
* -- ra��д�����ʼƫ������
* -- pb��Ҫд������ݻ�������
* -- cb��Ҫд����ֽ�����
* -- ����ֵ���ɹ�����TRUE�����ֻ���д�뷵��FALSE��
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_WinReg_HiveWrite(_In_ ULONG64 vaCMHive, _In_ DWORD ra, _In_ PBYTE pb, _In_ DWORD cb);

/*
* ö��ע����Ӽ� - ������WinAPI���� 'RegEnumKeyExW'��
* ��ο�WinAPI�����ĵ��Ի�ȡ������Ϣ��
* ����ʹ��HKLM��ַ��CMHIVE��ַ�������ַ��Ϊʾ����
*   1) 'HKLM\SOFTWARE\Key\SubKey'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey'              ����������
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey'          ����������
* -- uszFullPathKey�������ļ�·����
* -- dwIndex���Ӽ�����������ΧΪ0��N��-1��ʾ��������
* -- lpName�������Ӽ����Ļ�������
* -- lpcchName�������Ӽ������ȵ�ָ�롣
* -- lpftLastWriteTime�������Ӽ������д��ʱ�䡣
* -- ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE��
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_WinReg_EnumKeyExU(
    _In_ LPSTR uszFullPathKey,
    _In_ DWORD dwIndex,
    _Out_writes_opt_(*lpcchName) LPSTR lpName,
    _Inout_ LPDWORD lpcchName,
    _Out_opt_ PFILETIME lpftLastWriteTime
);

/*
* ö�ٸ���ע������ע���ֵ - ������WinAPI���� 'EnumValueW'��
* ��ο�WinAPI�����ĵ��Ի�ȡ������Ϣ��
* ����ͨ�����ַ�ʽ���ã�
* ����ʹ��HKLM��ַ��CMHIVE��ַ�������ַ��Ϊʾ����
*   1) 'HKLM\SOFTWARE\Key\SubKey'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey'              ����������
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey'          ����������
* -- uszFullPathKey�������ļ�·����
* -- dwIndex��ֵ��������
* -- lpValueName������ֵ���Ļ�������
* -- lpcchValueName������ֵ�����ȵ�ָ�롣
* -- lpType������ֵ���͵�ָ�롣
* -- lpData������ֵ���ݵĻ�������
* -- lpcbData������ֵ���ݳ��ȵ�ָ�롣
* -- ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE��
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_WinReg_EnumValueU(
    _In_ LPSTR uszFullPathKey,
    _In_ DWORD dwIndex,
    _Out_writes_opt_(*lpcchValueName) LPSTR lpValueName,
    _Inout_ LPDWORD lpcchValueName,
    _Out_opt_ LPDWORD lpType,
    _Out_writes_opt_(*lpcbData) LPBYTE lpData,
    _Inout_opt_ LPDWORD lpcbData
);

/*
* ��ѯ����ע����/ֵ·����ע���ֵ - ������WinAPI���� 'RegQueryValueEx'��
* ��ο�WinAPI�����ĵ��Ի�ȡ������Ϣ��
* ����ͨ�����ַ�ʽ���ã�
* ����ʹ��HKLM��ַ��CMHIVE��ַ�������ַ��Ϊʾ����
*   1) 'HKLM\SOFTWARE\Key\SubKey\Value'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey\'             ����������Ĭ��ֵ��
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey\Value'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey\Value'    ��������ֵ��
* -- uszFullPathKeyValue�������ļ�/ֵ·����
* -- lpType������ֵ���͵�ָ�롣
* -- lpData������ֵ���ݵĻ�������
* -- lpcbData������ֵ���ݳ��ȵ�ָ�롣
* -- ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE��
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_WinReg_QueryValueExU(
    _In_ LPSTR uszFullPathKeyValue,
    _Out_opt_ LPDWORD lpType,
    _Out_writes_opt_(*lpcbData) LPBYTE lpData,
    _When_(lpData == NULL, _Out_opt_) _When_(lpData != NULL, _Inout_opt_) LPDWORD lpcbData
);

/*
* ö��ע����Ӽ� - ������WinAPI���� 'RegEnumKeyExW'��
* ��ο�WinAPI�����ĵ��Ի�ȡ������Ϣ��
* ����ͨ��HKLM��ַ��CMHIVE��ַ�������ַ������ʾ����
*   1) 'HKLM\SOFTWARE\Key\SubKey'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey'              ����������
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey'          ����������
* -- wszFullPathKey�������ļ�·����
* -- dwIndex���Ӽ�����������ΧΪ0��N��-1��ʾ��������
* -- lpName�������Ӽ����Ļ�������
* -- lpcchName�������Ӽ������ȵ�ָ�롣
* -- lpftLastWriteTime�������Ӽ�����޸�ʱ���ָ�롣
* -- ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE��
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
* ö��ע���ֵ - ������WinAPI���� 'EnumValueW'��
* ��ο�WinAPI�����ĵ��Ի�ȡ������Ϣ��
* ����ͨ��HKLM��ַ��CMHIVE��ַ�������ַ������ʾ����
*   1) 'HKLM\SOFTWARE\Key\SubKey'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey'              ����������
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey'          ����������
* -- wszFullPathKey�������ļ�·����
* -- dwIndex��ֵ��������
* -- lpValueName������ֵ���Ļ�������
* -- lpcchValueName������ֵ�����ȵ�ָ�롣
* -- lpType������ֵ���͵�ָ�롣
* -- lpData������ֵ���ݵĻ�������
* -- lpcbData������ֵ���ݳ��ȵ�ָ�롣
* -- ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE��
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
* ��ѯע���ֵ - ������WinAPI���� 'RegQueryValueEx'��
* ��ο�WinAPI�����ĵ��Ի�ȡ������Ϣ��
* ����ͨ��HKLM��ַ��CMHIVE��ַ�������ַ������ʾ����
*   1) 'HKLM\SOFTWARE\Key\SubKey\Value'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey\'             ����������Ĭ��ֵ��
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey\Value'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey\Value'    ��������ֵ��
* -- wszFullPathKeyValue�������ļ�/ֵ·����
* -- lpType������ֵ���͵�ָ�롣
* -- lpData������ֵ���ݵĻ�������
* -- lpcbData������ֵ���ݳ��ȵ�ָ�롣
* -- ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE��
*/
_Success_(return)
BOOL VMMDLL_WinReg_QueryValueExW(
    _In_ LPWSTR wszFullPathKeyValue,
    _Out_opt_ LPDWORD lpType,
    _Out_writes_opt_(*lpcbData) LPBYTE lpData,
    _When_(lpData == NULL, _Out_opt_) _When_(lpData != NULL, _Inout_opt_) LPDWORD lpcbData
);



//-----------------------------------------------------------------------------
// WINDOWS SPECIFIC UTILITY FUNCTIONS BELOW:
//-----------------------------------------------------------------------------

typedef struct tdVMMDLL_WIN_THUNKINFO_IAT {
    BOOL fValid;
    BOOL f32;               // ���ΪTRUE����ʾ������32λ/4�ֽ���ڣ�����Ϊ64λ/8�ֽ���ڡ�
    ULONG64 vaThunk;        // �����ַ�� 'thunk' �ĵ�ַ��
    ULONG64 vaFunction;     // ��������ַ�� 'thunk' == ���뺯���ĵ�ַ����Ϊ��ֵ��
    ULONG64 vaNameModule;   // ����ģ�������ַ����ĵ�ַ��
    ULONG64 vaNameFunction; // ���뺯�������ַ����ĵ�ַ��
} VMMDLL_WIN_THUNKINFO_IAT, * PVMMDLL_WIN_THUNKINFO_IAT;

/*
* �������뺯���ĵ����ַ��IAT��thunk����Ϣ������IAT thunk�������ַ�����ڹ��ӣ�hooking�������á�
* ������
   - `dwPID`�����̵�ID��
   - `[uw]szModuleName`��ģ������ơ�
   - `szImportModuleName`������ģ������ơ�
   - `szImportFunctionName`�����뺯�������ơ�
   - `pThunkIAT`�����ڴ洢�����ַ��IAT��thunk��Ϣ�Ľṹ��ָ�롣
* ����ֵ���ɹ���ʧ�ܵ�ָʾ��
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_WinGetThunkInfoIATU(_In_ DWORD dwPID, _In_ LPSTR  uszModuleName, _In_ LPSTR szImportModuleName, _In_ LPSTR szImportFunctionName, _Out_ PVMMDLL_WIN_THUNKINFO_IAT pThunkInfoIAT);
_Success_(return) BOOL VMMDLL_WinGetThunkInfoIATW(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _In_ LPSTR szImportModuleName, _In_ LPSTR szImportFunctionName, _Out_ PVMMDLL_WIN_THUNKINFO_IAT pThunkInfoIAT);



//-----------------------------------------------------------------------------
// VMM UTIL FUNCTIONALITY BELOW:
//-----------------------------------------------------------------------------

/*
* ���ɶ���ʮ������ ASCII �ڴ�ת����䵽���÷��ṩ�� sz �������С�
* ������
   - `pb`��Ҫת�����ڴ���ʼ��ַ��
   - `cb`��Ҫת�����ڴ��С�����ֽ�Ϊ��λ����
   - `cbInitialOffset`����ʼƫ����������С�ڵ��� 0x1000������ 0x10 �ı�����
   - `sz`���������Ļ����������Ϊ NULL������ pcsz �����з��ػ�������С��
   - `pcsz`��
      - ��� sz==NULL���˳�ʱΪ�������Ĵ�С��������ֹ�� NULL����
      - ��� sz!=NULL������ʱΪ�������Ĵ�С���˳�ʱΪ�ַ�����������������ֹ�� NULL����
* ����ֵ���ɹ���ʧ�ܵ�ָʾ��
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_UtilFillHexAscii(
    _In_reads_opt_(cb) PBYTE pb,
    _In_ DWORD cb,
    _In_ DWORD cbInitialOffset,
    _Out_writes_opt_(*pcsz) LPSTR sz,
    _Inout_ PDWORD pcsz
);



//-----------------------------------------------------------------------------
// DEFAULT (WINDOWS ONLY) COMPATIBILITY FUNCTION DEFINITIONS BELOW:
//-----------------------------------------------------------------------------

#ifdef _WIN32
#define VMMDLL_VfsList                  VMMDLL_VfsListW
#define VMMDLL_VfsRead                  VMMDLL_VfsReadW
#define VMMDLL_VfsWrite                 VMMDLL_VfsWriteW
#define VMMDLL_ProcessGetDirectories    VMMDLL_ProcessGetDirectoriesW
#define VMMDLL_ProcessGetSections       VMMDLL_ProcessGetSectionsW
#define VMMDLL_ProcessGetProcAddress    VMMDLL_ProcessGetProcAddressW
#define VMMDLL_ProcessGetModuleBase     VMMDLL_ProcessGetModuleBaseW
#define VMMDLL_Map_GetPte               VMMDLL_Map_GetPteW
#define VMMDLL_Map_GetVad               VMMDLL_Map_GetVadW
#define VMMDLL_Map_GetModule            VMMDLL_Map_GetModuleW
#define VMMDLL_Map_GetModuleFromName    VMMDLL_Map_GetModuleFromNameW
#define VMMDLL_Map_GetUnloadedModule    VMMDLL_Map_GetUnloadedModuleW
#define VMMDLL_Map_GetEAT               VMMDLL_Map_GetEATW
#define VMMDLL_Map_GetIAT               VMMDLL_Map_GetIATW
#define VMMDLL_Map_GetHandle            VMMDLL_Map_GetHandleW
#define VMMDLL_Map_GetNet               VMMDLL_Map_GetNetW
#define VMMDLL_Map_GetUsers             VMMDLL_Map_GetUsersW
#define VMMDLL_Map_GetServices          VMMDLL_Map_GetServicesW
#define VMMDLL_WinGetThunkInfoIAT       VMMDLL_WinGetThunkInfoIATW
#endif /* _WIN32 */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __VMMDLL_H__ */
