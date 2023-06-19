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
* 使用命令行参数初始化 VMM.DLL。有关参数的更详细信息，请参阅 MemProcFS 和 LeechCore 的 GitHub Wiki。注意：LeechCore 的初始化参数对该函数也有效。重要的参数如下：
*    -printf = 显示 printf 样式的输出。
*    -v -vv -vvv = 额外的详细程度级别。
*    -device = 设备的格式，参见 leechcore.h 或 GitHub 文档以获取更多信息。一些可用的值包括：<file>、fpga、usb3380、hvsavedstate、totalmeltdown、pmem
*    -remote = 远程 LeechCore 实例，参见 leechcore.h 或 GitHub 文档以获取更多信息。
*    -norefresh = 禁用后台刷新（即使支持的内存是易失性内存）。
*    -memmap = 指定由文件给出的物理内存映射或指定为 'auto'。示例：-memmap c:\\temp\\my_custom_memory_map.txt 或 -memmap auto
*    -pagefile[0-9] = 除物理内存外使用的页面文件。通常，pagefile.sys 的索引为 0，swapfile.sys 的索引为 1。页面文件在不断变化，请勿在内存转储和页面文件之间的时间差超过几分钟。示例：'pagefile0 swapfile.sys'
*    -symbolserverdisable = 禁用符号服务器，直到用户更改。此参数将覆盖注册表设置。
*    -waitinitialize = 等待初始化完成后再返回。通常情况下，某些初始化操作是异步进行的，并且在初始化调用完成时可能尚未完成。这包括虚拟内存压缩、注册表等。示例：'-waitinitialize'
*    -userinteract = 允许 vmm.dll 在控制台上询问用户信息，例如，但不限于，leechcore 设备选项。默认情况下，用户交互是禁用的。
*    -forensic = 在启动后立即对物理内存进行取证扫描（如果可能）。允许的参数值范围为 0-4。注意！取证模式不适用于实时内存。1 = 取证模式，使用内存中的 SQLite 数据库。2 = 取证模式，使用临时 SQLite 数据库，退出时删除。3 = 取证模式，使用临时 SQLite 数据库，退出时保留。4 = 取证模式，使用静态命名的 SQLite 数据库（vmm.sqlite3）。示例：-forensic 4
*
* -- argc
* -- argv
* -- ppLcErrorInfo = 可选指针，用于接收一个由 LC_CONFIG_ERRORINFO 结构分配的扩展错误信息的函数内存。在发生失败时，接收到的任何内存应由调用者通过

调用 LcMemFree() 进行释放。
* -- 返回值 = 成功/失败

请注意：上述文本中的注释部分是对代码的解释，下面是相应的翻译。

//-------------------------------------------------------------------------
// 初始化功能如下：
// 选择一种初始化 VMM / MemProcFS 的方式。
//-------------------------------------------------------------------------

/*
* 使用命令行参数初始化 VMM.DLL。有关参数的更详细信息，请参阅 MemProcFS 和 LeechCore 的 GitHub Wiki。注意：LeechCore 的初始化参数对该函数也有效。重要的参数如下：
*    -printf = 显示 printf 样式的输出。
*    -v -vv -vvv = 额外的详细程度级别。
*    -device = 设备的格式，参见 leechcore.h 或 GitHub 文档以获取更多信息。一些可用的值包括：<file>、fpga、usb3380、hvsavedstate、totalmeltdown、pmem
*    -remote = 远程 LeechCore 实例，参见 leechcore.h 或 GitHub 文档以获取更多信息。
*    -norefresh = 禁用后台刷新（即使支持的内存是易失性内存）。
*    -memmap = 指定由文件给出的物理内存映射或指定为 'auto'。示例：-memmap c:\\temp\\my_custom_memory_map.txt 或 -memmap auto
*    -pagefile[0-9] = 除物理内存外使用的页面文件。通常，pagefile.sys 的索引为 0，swapfile.sys 的索引为 1。页面文件在不断变化，请勿在内存转储和页面文件之间的时间差超过几分钟。示例：'pagefile0 swapfile.sys'
*    -symbolserverdisable = 禁用符号服务器，直到用户更改。此参数将覆盖注册表设置。
*    -waitinitialize = 等待初始化完成后再返回。通常情况下，某些初始化操作是异步进行的，并且在初始化调用完成时可能尚未完成。这包括虚拟内存压缩、注册表等。示例：'-waitinitialize'
*    -userinteract = 允许 vmm.dll 在控制台上询问用户信息，例如，但不限于，leechcore 设备选项。默认情况下，用户交互是禁用的。
*    -forensic = 在启动后立即对物理内存进行取证扫描（如果可能）。允许的参数值范围为 0-4。注意！取证模式不适用于实时内存。1 = 取证模式，使用内存中的 SQLite 数据库。2 = 取证模式，使用临时 SQLite 数据库，退出时删除。3 = 取证模式，使用临时 SQLite 数据库，退出时保留。4 = 取证模式，使用静态命名的 SQLite 数据库（vmm.sqlite3）。示例：-forensic 4
*
* -- argc
* -- argv
* -- ppLcErrorInfo

 = 可选指针，用于接收一个由 LC_CONFIG_ERRORINFO 结构分配的扩展错误信息的函数内存。在发生失败时，接收到的任何内存应由调用者通过调用 LcMemFree() 进行释放。
* -- 返回值 = 成功/失败

*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_Initialize(_In_ DWORD argc, _In_ LPSTR argv[]);

EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_InitializeEx(_In_ DWORD argc, _In_ LPSTR argv[], _Out_opt_ PPLC_CONFIG_ERRORINFO ppLcErrorInfo);

/*
* 关闭已初始化的 VMM.DLL 实例并清理所有已分配的资源，包括插件、链接的 PCILeech.DLL 和其他内存资源。
* -- 返回值 = 成功/失败。
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
// 配置设置如下：
// 配置 MemProcFS 或底层的内存获取设备。
//-----------------------------------------------------------------------------

/*
* 与函数 VMMDLL_ConfigGet 和 VMMDLL_ConfigSet 一起使用的选项。
* 选项在此头文件中使用 VMMDLL_OPT_* 进行定义，或在 leechcore.h 中使用 LC_OPT_* 进行定义。
* 有关更详细的信息，请查看各个设备类型的源代码。
*/
#define VMMDLL_OPT_CORE_PRINTF_ENABLE                   0x4000000100000000  // RW - 启用 printf 样式输出
#define VMMDLL_OPT_CORE_VERBOSE                         0x4000000200000000  // RW - 额外详细输出
#define VMMDLL_OPT_CORE_VERBOSE_EXTRA                   0x4000000300000000  // RW - 额外详细输出（扩展）
#define VMMDLL_OPT_CORE_VERBOSE_EXTRA_TLP               0x4000000400000000  // RW - 额外详细输出（TLP）
#define VMMDLL_OPT_CORE_MAX_NATIVE_ADDRESS              0x4000000800000000  // R - 最大本机地址

#define VMMDLL_OPT_CORE_SYSTEM                          0x2000000100000000  // R - 系统配置
#define VMMDLL_OPT_CORE_MEMORYMODEL                     0x2000000200000000  // R - 内存模型

#define VMMDLL_OPT_CONFIG_IS_REFRESH_ENABLED            0x2000000300000000  // R - 刷新是否启用（1/0）
#define VMMDLL_OPT_CONFIG_TICK_PERIOD                   0x2000000400000000  // RW - 基本的时钟周期（毫秒）
#define VMMDLL_OPT_CONFIG_READCACHE_TICKS               0x2000000500000000  // RW - 内存缓存有效期（以时钟周期计算）
#define VMMDLL_OPT_CONFIG_TLBCACHE_TICKS                0x2000000600000000  // RW - 页面表（TLB）缓存有效期（以时钟周期计算）
#define VMMDLL_OPT_CONFIG_PROCCACHE_TICKS_PARTIAL        0x2000000700000000  // RW - 进程刷新（部分）周期（以时钟周期计算）
#define VMMDLL_OPT_CONFIG_PROCCACHE_TICKS_TOTAL          0x2000000800000000  // RW - 进程刷新（完整）周期（以时钟周期计算）
#define VMMDLL_OPT_CONFIG_VMM_VERSION_MAJOR             0x2000000900000000  // R - VMM.DLL 版本号（主版本）
#define VMMDLL_OPT_CONFIG_VMM_VERSION_MINOR             0x2000000A00000000  // R - VMM.DLL 版本号（次版本）
#define VMMDLL_OPT_CONFIG_VMM_VERSION_REVISION          0x2000000B00000000  // R - VMM.DLL 版本号（修订版本）
#define VMMDLL_OPT_CONFIG_STATISTICS_FUNCTIONCALL       0x2000000C00000000  // RW - 启用函数调用统计（.status/statistics_fncall 文件）
#define VMMDLL_OPT_CONFIG_IS_PAGING_ENABLED             0x2000000D00000000  // RW - 是否启用分页（1/0）

#define VMMDLL_OPT_WIN_VERSION_MAJOR                    0x2000010100000000  // R - Windows 版本号（主版本）
#define VMMDLL_OPT_WIN_VERSION_MINOR                    0x2000010200000000  // R - Windows 版本号（次版本）
#define VMMDLL_OPT_WIN_VERSION_BUILD                    0x2000010300000000  // R - Windows 版本号（构建号）
#define VMMDLL_OPT_WIN_SYSTEM_UNIQUE_ID                 0x2000010400000000  // R - Windows

系统唯一标识符

#define VMMDLL_OPT_FORENSIC_MODE                        0x2000020100000000  // RW - 启用/检索取证模式类型[0-4]。

#define VMMDLL_OPT_REFRESH_ALL                          0x2001ffff00000000  // W - 刷新所有缓存
#define VMMDLL_OPT_REFRESH_FREQ_MEM                     0x2001000200000000  // W - 刷新内存缓存（不包括TLB）[部分 33%/调用]
#define VMMDLL_OPT_REFRESH_FREQ_TLB                     0x2001000400000000  // W - 刷新页面表（TLB）缓存[部分 33%/调用]
#define VMMDLL_OPT_REFRESH_FREQ_FAST                    0x2001040000000000  // W - 快速刷新频率 - 包括部分进程刷新
#define VMMDLL_OPT_REFRESH_FREQ_MEDIUM                  0x2001000100000000  // W - 中等刷新频率 - 包括完整进程刷新
#define VMMDLL_OPT_REFRESH_FREQ_SLOW                    0x2001001000000000  // W - 慢速刷新频率
#define VMMDLL_OPT_REFRESH_PROCESS                      0x2001000100000000  // W - 已弃用：刷新进程列表
#define VMMDLL_OPT_REFRESH_READ                         0x2001000200000000  // W - 已弃用：刷新物理读取缓存
#define VMMDLL_OPT_REFRESH_TLB                          0x2001000400000000  // W - 已弃用：刷新页面表（TLB）缓存
#define VMMDLL_OPT_REFRESH_PAGING                       0x2001000800000000  // W - 已弃用：刷新虚拟内存的“paging”缓存
#define VMMDLL_OPT_REFRESH_REGISTRY                     0x2001001000000000  // W - 已弃用：
#define VMMDLL_OPT_REFRESH_USER                         0x2001002000000000  // W - 已弃用：
#define VMMDLL_OPT_REFRESH_PHYSMEMMAP                   0x2001004000000000  // W - 已弃用：
#define VMMDLL_OPT_REFRESH_PFN                          0x2001008000000000  // W - 已弃用：
#define VMMDLL_OPT_REFRESH_OBJ                          0x2001010000000000  // W - 已弃用：
#define VMMDLL_OPT_REFRESH_NET                          0x2001020000000000  // W - 已弃用：

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
* 获取设备特定选项的值。请参阅 VMMDLL_OPT_* 的定义以了解有效的选项值。
* 请注意，不同设备类型之间的选项值可能存在重叠，并具有不同的含义。
* -- fOption：选项标志
* -- pqwValue：指向 ULONG64 的指针，用于接收选项值。
* -- 返回值：成功/失败。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_ConfigGet(_In_ ULONG64 fOption, _Out_ PULONG64 pqwValue);

/*
* 设置设备特定选项的值。请参阅 VMMDLL_OPT_* 的定义以了解有效的选项值。
* 请注意，不同设备类型之间的选项值可能存在重叠，并具有不同的含义。
* -- fOption：选项标志
* -- qwValue：选项值
* -- 返回值：成功/失败。
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
// VFS - 虚拟文件系统功能如下：
// 注意！使用VFS功能需要先调用VMMDLL_InitializePlugins()来初始化插件。
// 这是MemProcFS的核心。通过使用下面的功能，可以对虚拟文件系统（VFS）进行实现和分析。
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
* 用于回调VMM_VFS_FILELIST结构的辅助函数。
*/
EXPORTED_FUNCTION
VOID VMMDLL_VfsList_AddFile(_In_ HANDLE pFileList, _In_ LPSTR uszName, _In_ ULONG64 cb, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo);
VOID VMMDLL_VfsList_AddFileW(_In_ HANDLE pFileList, _In_ LPWSTR wszName, _In_ ULONG64 cb, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo);
EXPORTED_FUNCTION
VOID VMMDLL_VfsList_AddDirectory(_In_ HANDLE pFileList, _In_ LPSTR uszName, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo);
VOID VMMDLL_VfsList_AddDirectoryW(_In_ HANDLE pFileList, _In_ LPWSTR wszName, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo);
EXPORTED_FUNCTION BOOL VMMDLL_VfsList_IsHandleValid(_In_ HANDLE pFileList);

/*
* 在MemProcFS中列出文件目录。通过调用pFileList参数中提供的函数来列出目录和文件。
* 如果需要获取单个文件的信息，则需要列出其目录中的所有文件。
* -- [uw]szPath：目录路径
* -- pFileList：回调函数的参数结构体
* -- 返回值
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_VfsListU(_In_ LPSTR  uszPath, _Inout_ PVMMDLL_VFS_FILELIST2 pFileList);
_Success_(return) BOOL VMMDLL_VfsListW(_In_ LPWSTR wszPath, _Inout_ PVMMDLL_VFS_FILELIST2 pFileList);

/*
* 在MemProcFS中读取文件的选定部分。
* -- [uw]szFileName：文件名
* -- pb：存储读取数据的缓冲区
* -- cb：要读取的字节数
* -- pcbRead：指向一个变量，用于接收实际读取的字节数
* -- cbOffset：文件中的偏移量，从该位置开始读取数据
* -- 返回值
*
*/
EXPORTED_FUNCTION
NTSTATUS VMMDLL_VfsReadU(_In_ LPSTR  uszFileName, _Out_writes_to_(cb, *pcbRead) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset);
NTSTATUS VMMDLL_VfsReadW(_In_ LPWSTR wszFileName, _Out_writes_to_(cb, *pcbRead) PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset);

/*
* 在MemProcFS中将选定的部分写入文件。
* -- [uw]szFileName：文件名
* -- pb：要写入的数据的缓冲区
* -- cb：要写入的字节数
* -- pcbWrite：指向一个变量，用于接收实际写入的字节数
* -- cbOffset：文件中的偏移量，从该位置开始写入数据
* -- 返回值
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
// 插件管理功能如下:
// 用于初始化和使用MemProcFS插件功能的函数和结构。
// 插件管理器通过调用函数VMM_VfsInitializePlugins来启动。每个内置插件和外部插件（DLL名称匹配m_*.dll）都会调用其InitializeVmmPlugin函数。
// 插件/模块可以决定调用pfnPluginManager_Register来注册以不同名称形式的插件一次或多次。
// 下面是插件DLL中的注册函数示例:
// 'VOID InitializeVmmPlugin(_In_ PVMM_PLUGIN_REGINFO pRegInfo)'

/*
* 初始化所有可能的插件，包括内置和外部插件，将其映射到MemProcFS。请注意，默认情况下不会加载插件-必须通过调用此函数显式加载插件。它们将在vmm dll的通用关闭时卸载。
* -- 返回值
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
    // 可选的取证插件功能，用于对各种数据进行取证（更全面的）分析。这些函数是可选的。就插件本身而言，
    //函数在单线程模式下调用，但“摄取”函数在插件之间以并行多线程方式调用。函数的调用顺序为：pfnInitialize()、pfnIngest*()、pfnTimeline()、pfnLogJSON()、pfnFinalize()
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
// VMM 核心功能如下：
// VMM 核心功能包括读取（和写入）虚拟和物理内存。注意！写入操作只在目标支持的情况下有效，即不包括内存转储文件...
// 若要读取物理内存，请将 dwPID 指定为 (DWORD)-1
//-----------------------------------------------------------------------------

#define VMMDLL_PID_PROCESS_WITH_KERNELMEMORY        0x80000000      // Combine with dwPID to enable process kernel memory (NB! use with extreme care).

// 用于在调用 VMM_MemReadEx() 时禁止默认的读取缓存，这将导致始终从目标系统读取数据。
// 仍然使用缓存的页表（用于虚拟地址到物理地址的转换）。
#define VMMDLL_FLAG_NOCACHE                         0x0001  // 不使用数据缓存（强制从内存采集设备读取）
#define VMMDLL_FLAG_ZEROPAD_ON_FAIL                 0x0002  // 失败的物理内存读取进行零填充，并在物理内存范围内报告成功。
#define VMMDLL_FLAG_FORCECACHE_READ                 0x0008  // 强制使用缓存 - 对非缓存页失败 - 仅对读取有效，与 VMM_FLAG_NOCACHE/VMM_FLAG_ZEROPAD_ON_FAIL 不兼容。
#define VMMDLL_FLAG_NOPAGING                        0x0010  // 不尝试从分页文件/压缩的分页内存中检索内存（即使可能）
#define VMMDLL_FLAG_NOPAGING_IO                     0x0020  // 如果读取将导致额外的 I/O，则不尝试从分页的内存中检索内存（即使可能）。
#define VMMDLL_FLAG_NOCACHEPUT                      0x0100  // 成功从内存采集设备读取后，不写回数据缓存。
#define VMMDLL_FLAG_CACHE_RECENT_ONLY               0x0200  // 仅从最近的活动缓存区域读取数据。
#define VMMDLL_FLAG_NO_PREDICTIVE_READ              0x0400  // 不执行额外的预测性页面读取（默认对于较小的请求）。

/*
* 在由 ppMEMs 数组中的项目指针指定的各个非连续位置上读取内存。每个工作单元的结果将分别给出。
* 读取的项目数没有上限，但如果超过硬件限制，则不会提供性能提升。每个工作单元的最大大小为一个4k页面（4096字节）。
* 读取不能跨越4k页面边界。读取必须从偶数DWORD（4字节）开始。
* -- dwPID - 目标进程的PID，(DWORD)-1 代表读取物理内存。
* -- ppMEMs = 分散读取头的数组。
* -- cpMEMs = ppMEMs 的数量。
* -- flags = 可选的标志，使用 VMMDLL_FLAG_* 进行指定。
* -- return = 成功读取的项目数。
*/
EXPORTED_FUNCTION
DWORD VMMDLL_MemReadScatter(_In_ DWORD dwPID, _Inout_ PPMEM_SCATTER ppMEMs, _In_ DWORD cpMEMs, _In_ DWORD flags);

/*
* 读取单个 4096 字节的内存页面。
* -- dwPID - 目标进程的PID，(DWORD)-1 代表读取物理内存。
* -- qwA - 内存页面的起始地址。
* -- pbPage - 存储读取结果的缓冲区。
* -- return - 成功/失败（取决于是否成功读取所有请求的字节）。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_MemReadPage(_In_ DWORD dwPID, _In_ ULONG64 qwA, _Inout_bytecount_(4096) PBYTE pbPage);

/*
* 读取一段连续的任意数量的内存。
* -- dwPID - 目标进程的PID，(DWORD)-1 代表读取物理内存。
* -- qwA - 内存的起始地址。
* -- pb - 存储读取结果的缓冲区。
* -- cb - 要读取的字节数。
* -- return - 成功/失败（取决于是否成功读取了所有请求的字节）。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_MemRead(_In_ DWORD dwPID, _In_ ULONG64 qwA, _Out_writes_(cb) PBYTE pb, _In_ DWORD cb);

/*
* 读取一段连续的内存，并将读取的字节数存储在 pcbRead 参数中。
* -- dwPID - 目标进程的PID，(DWORD)-1 代表读取物理内存。
* -- qwA - 内存的起始地址。
* -- pb - 存储读取结果的缓冲区。
* -- cb - 要读取的字节数。
* -- pcbRead - 读取的字节数将存储在此参数中。
* -- flags - 标志，使用 VMMDLL_FLAG_* 中的标志位。
* -- return - 成功/失败。注意！即使读取的字节数为0，读取操作仍可能报告为成功，建议验证 pcbRead 参数。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_MemReadEx(_In_ DWORD dwPID, _In_ ULONG64 qwA, _Out_writes_(cb) PBYTE pb, _In_ DWORD cb, _Out_opt_ PDWORD pcbReadOpt, _In_ ULONG64 flags);

/*
* 预取一组地址（在pA数组中指定）到内存缓存中。此函数用于批量将较大的已知读取操作预取到本地缓存中，然后从缓存中进行多次较小的读取操作。此函数用于提高性能。
* -- dwPID = 目标进程的PID，(DWORD)-1 代表物理内存。
* -- pPrefetchAddresses = 要预取到缓存的地址数组。
* -- cPrefetchAddresses = 地址数量。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_MemPrefetchPages(_In_ DWORD dwPID, _In_reads_(cPrefetchAddresses) PULONG64 pPrefetchAddresses, _In_ DWORD cPrefetchAddresses);

/*
* 在连续的任意大小的内存中写入数据。请注意，某些虚拟内存，如可执行文件的页面（例如DLL），可能在不同的进程之间共享。例如，在一个进程中对kernel3
  2.dll的写操作可能会影响整个系统中的所有进程中的kernel32。堆和栈以及其他内存通常是可以安全写入的。在写入内存时，请小心处理！
* -- dwPID = 目标进程的PID，(DWORD)-1 代表物理内存。
* -- qwA = 要写入的内存地址。
* -- pb = 要写入的数据缓冲区。
* -- cb = 要写入的数据字节数。
* -- return = 写入成功返回TRUE，部分或零字节写入返回FALSE。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_MemWrite(_In_ DWORD dwPID, _In_ ULONG64 qwA, _In_reads_(cb) PBYTE pb, _In_ DWORD cb);

/*
* 通过遍历指定进程的页表将虚拟地址转换为物理地址。
* -- dwPID = 目标进程的PID。
* -- qwVA = 要转换的虚拟地址。
* -- pqwPA = 接收物理地址的指针。
* -- return = 转换成功返回TRUE，失败返回FALSE。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_MemVirt2Phys(_In_ DWORD dwPID, _In_ ULONG64 qwVA, _Out_ PULONG64 pqwPA);



//-----------------------------------------------------------------------------
// VMM进程映射功能如下：
// 用于检索与进程相关的项目集合，例如页表映射（PTE）、虚拟地址描述符映射（VAD）、已加载模块、堆和线程。
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
* 根据进程的硬件页表（PTE）检索内存映射条目。如果将pPteMap设置为NULL，则将返回所需的字节数存储在参数pcbPteMap中。
* 返回的条目按VMMDLL_MAP_PTEENTRY.vaBase排序。
* -- dwPID
* -- pPteMap = 最小字节长度*pcbPteMap的缓冲区，或为NULL。
* -- pcbPteMap = 指向pPteMap缓冲区字节数的指针。
* -- fIdentifyModules = 是否尝试识别模块（较慢）
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetPteU(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbPteMap) PVMMDLL_MAP_PTE pPteMap, _Inout_ PDWORD pcbPteMap, _In_ BOOL fIdentifyModules);
_Success_(return) BOOL VMMDLL_Map_GetPteW(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbPteMap) PVMMDLL_MAP_PTE pPteMap, _Inout_ PDWORD pcbPteMap, _In_ BOOL fIdentifyModules);

/*
* 根据进程的虚拟地址描述符（VAD）检索内存映射条目。如果将pVadMap设置为NULL，则将返回所需的字节数存储在参数pcbVadMap中。
* 返回的条目按VMMDLL_MAP_VADENTRY.vaStart排序。
* -- dwPID
* -- pVadMap = 最小字节长度*pcbVadMap的缓冲区，或为NULL。
* -- pcbVadMap = 指向pVadMap缓冲区字节数的指针。
* -- fIdentifyModules = 是否尝试识别模块（较慢）
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetVadU(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbVadMap) PVMMDLL_MAP_VAD pVadMap, _Inout_ PDWORD pcbVadMap, _In_ BOOL fIdentifyModules);
_Success_(return) BOOL VMMDLL_Map_GetVadW(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbVadMap) PVMMDLL_MAP_VAD pVadMap, _Inout_ PDWORD pcbVadMap, _In_ BOOL fIdentifyModules);

/*
* 检索有关内存映射的一部分选择子集的扩展内存映射信息。如果将pVadExMap设置为NULL，则将返回所需的字节数存储在参数pcbVadExMap中。
* -- dwPID
* -- pVadExMap = 最小字节长度*pcbVadExMap的缓冲区，或为NULL。
* -- pcbVadExMap = 指向pVadExMap缓冲区字节数的指针。
* -- oPage = 从进程开始的页面偏移量（以页面计）。
* -- cPage = 从oPages基础开始处理的页面数。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_Map_GetVadEx(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbVadExMap) PVMMDLL_MAP_VADEX pVadExMap, _Inout_ PDWORD pcbVadExMap, _In_ DWORD oPage, _In_ DWORD cPage);

/*
* 检索指定进程的模块（.dll文件）。如果将pModuleMap设置为NULL，则将返回所需的字节数存储在参数pcbModuleMap中。
* -- dwPID
* -- pModuleMap = 最小字节长度*pcbModuleMap的缓冲区，或为NULL。
* -- pcbModuleMap = 指向pModuleMap缓冲区字节数的指针。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetModuleU(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbModuleMap) PVMMDLL_MAP_MODULE pModuleMap, _Inout_ PDWORD pcbModuleMap);
_Success_(return) BOOL VMMDLL_Map_GetModuleW(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbModuleMap) PVMMDLL_MAP_MODULE pModuleMap, _Inout_ PDWORD pcbModuleMap);

/*
* 检索给定进程和模块名称的模块（.dll）条目。
* 如果将pModuleMapEntry设置为NULL，则将返回所需的字节数存储在参数pcbModuleMapEntry中。
* 如果将pcbModuleMapEntry设置为NULL，则假定pModuleMapEntry的大小为sizeof(VMMDLL_MAP_MODULEENTRY)，并且将复制没有名称的数据。
* -- dwPID
* -- [uw]szModuleName
* -- pModuleMapEntry = 最小字节长度*pcbModuleMapEntry的缓冲区，或为NULL。
* -- pcbModuleMapEntry = 指向pModuleMapEntry缓冲区字节数的指针，或为NULL。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetModuleFromNameU(_In_ DWORD dwPID, _In_ LPSTR  uszModuleName, _Out_writes_bytes_opt_(*pcbModuleMapEntry) PVMMDLL_MAP_MODULEENTRY pModuleMapEntry, _Inout_opt_ PDWORD pcbModuleMapEntry);
_Success_(return) BOOL VMMDLL_Map_GetModuleFromNameW(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _Out_writes_bytes_opt_(*pcbModuleMapEntry) PVMMDLL_MAP_MODULEENTRY pModuleMapEntry, _Inout_opt_ PDWORD pcbModuleMapEntry);

/*
* 检索指定进程的已卸载的模块（.dll/.sys）。如果将pUnloadedModuleMap设置为NULL，则将返回所需的字节数存储在参数pcbUnloadedModuleMap中。
* -- dwPID
* -- pUnloadedModuleMap = 最小字节长度*pcbUnloadedModuleMap的缓冲区，或为NULL。
* -- pcbUnloadedModuleMap = 指向pUnloadedModuleMap缓冲区字节数的指针。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetUnloadedModuleU(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbUnloadedModuleMap) PVMMDLL_MAP_UNLOADEDMODULE pUnloadedModuleMap, _Inout_ PDWORD pcbUnloadedModuleMap);
_Success_(return) BOOL VMMDLL_Map_GetUnloadedModuleW(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbUnloadedModuleMap) PVMMDLL_MAP_UNLOADEDMODULE pUnloadedModuleMap, _Inout_ PDWORD pcbUnloadedModuleMap);

/*
* 从导出地址表（EAT）中检索模块导出的函数。如果将pEatMap设置为NULL，则将返回所需的字节数存储在参数pcbEatMap中。
* -- dwPID
* -- [uw]szModuleName
* -- pEatMap = 最小字节长度*pcbEatMap的缓冲区，或为NULL。
* -- pcbEatMap = 指向pEatMap缓冲区字节数的指针。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetEATU(_In_ DWORD dwPID, _In_ LPSTR  uszModuleName, _Out_writes_bytes_opt_(*pcbEatMap) PVMMDLL_MAP_EAT pEatMap, _Inout_ PDWORD pcbEatMap);
_Success_(return) BOOL VMMDLL_Map_GetEATW(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _Out_writes_bytes_opt_(*pcbEatMap) PVMMDLL_MAP_EAT pEatMap, _Inout_ PDWORD pcbEatMap);

/*
* 从导入地址表（IAT）中检索模块导入的函数。如果将pIatMap设置为NULL，则将返回所需的字节数存储在参数pcbIatMap中。
* -- dwPID
* -- [uw]szModuleName
* -- pIatMap = 最小字节长度*pcbIatMap的缓冲区，或为NULL。
* -- pcbIatMap = 指向pIatMap缓冲区字节数的指针。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetIATU(_In_ DWORD dwPID, _In_ LPSTR  uszModuleName, _Out_writes_bytes_opt_(*pcbIatMap) PVMMDLL_MAP_IAT pIatMap, _Inout_ PDWORD pcbIatMap);
_Success_(return) BOOL VMMDLL_Map_GetIATW(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _Out_writes_bytes_opt_(*pcbIatMap) PVMMDLL_MAP_IAT pIatMap, _Inout_ PDWORD pcbIatMap);

/*
* 检索指定进程的堆。如果将pHeapMap设置为NULL，则将返回所需的字节数存储在参数pcbHeapMap中。
* -- dwPID
* -- pHeapMap = 最小字节长度*pcbHeapMap的缓冲区，或为NULL。
* -- pcbHeapMap = 指向pHeapMap缓冲区字节数的指针。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetHeap(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbHeapMap) PVMMDLL_MAP_HEAP pHeapMap, _Inout_ PDWORD pcbHeapMap);

/*
* 检索指定进程的线程。如果将pThreadMap设置为NULL，则将返回所需的字节数存储在参数pcbThreadMap中。
* 返回的条目按VMMDLL_MAP_THREADENTRY.dwTID排序。
* -- dwPID
* -- pThreadMap = 最小字节长度*pcbThreadMap的缓冲区，或为NULL。
* -- pcbThreadMap = 指向pThreadMap缓冲区字节数的指针。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetThread(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbThreadMap) PVMMDLL_MAP_THREAD pThreadMap, _Inout_ PDWORD pcbThreadMap);

/*
* 检索指定进程的句柄。如果将pHandleMap设置为NULL，则将返回所需的字节数存储在参数pcbHandleMap中。
* 返回的条目按VMMDLL_MAP_HANDLEENTRY.dwHandle排序。
* -- dwPID
* -- pHandleMap = 最小字节长度*pcbHandleMap的缓冲区，或为NULL。
* -- pcbHandleMap = 指向pHandleMap缓冲区字节数的指针。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetHandleU(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbHandleMap) PVMMDLL_MAP_HANDLE pHandleMap, _Inout_ PDWORD pcbHandleMap);
_Success_(return) BOOL VMMDLL_Map_GetHandleW(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbHandleMap) PVMMDLL_MAP_HANDLE pHandleMap, _Inout_ PDWORD pcbHandleMap);

/*
* 检索Windows枚举的物理内存映射中的物理内存范围。
* -- pPhysMemMap = 最小字节长度*pcbPhysMemMap的缓冲区，或为NULL。
* -- pcbPhysMemMap = 指向pPhysMemMap缓冲区字节数的指针。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetPhysMem(_Out_writes_bytes_opt_(*pcbPhysMemMap) PVMMDLL_MAP_PHYSMEM pPhysMemMap, _Inout_ PDWORD pcbPhysMemMap);

/*
* 检索网络连接映射 - 包括活动网络连接、监听套接字和其他网络功能。
* -- pNetMap = 最小字节长度*pcbNetMap的缓冲区，或为NULL。
* -- pcbNetMap = 指向pNetMap缓冲区字节数的指针。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetNetU(_Out_writes_bytes_opt_(*pcbNetMap) PVMMDLL_MAP_NET pNetMap, _Inout_ PDWORD pcbNetMap);
_Success_(return) BOOL VMMDLL_Map_GetNetW(_Out_writes_bytes_opt_(*pcbNetMap) PVMMDLL_MAP_NET pNetMap, _Inout_ PDWORD pcbNetMap);

/*
* 检索在目标系统中检测到的非知名用户。
* 注意！系统中可能存在更多的用户，但只能检测到已加载注册表文件的用户 - 这是具有活动进程的用户的正常行为。
* -- pUserMap = 最小字节长度*pcbUserMap的缓冲区，或为NULL。
* -- pcbUserMap = 指向pUserMap缓冲区字节数的指针。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_Map_GetUsersU(_Out_writes_bytes_opt_(*pcbUserMap) PVMMDLL_MAP_USER pUserMap, _Inout_ PDWORD pcbUserMap);
_Success_(return) BOOL VMMDLL_Map_GetUsersW(_Out_writes_bytes_opt_(*pcbUserMap) PVMMDLL_MAP_USER pUserMap, _Inout_ PDWORD pcbUserMap);

/*
* Retrieve the services currently known by the service control manager (SCM).
* 检索在目标系统中检测到的服务信息。
* -- pServiceMap = 最小字节长度*pcbServiceMap的缓冲区，或为NULL。
* -- pcbServiceMap = 指向pServiceMap缓冲区字节数的指针。
* -- return = 成功/失败。
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
* 检索散列的页框号（PFN）的信息。PFN按照它们在pPfns集合中存储的顺序返回。
* -- pPfns
* -- cPfns
* -- pPfnMap = 最小字节长度*pcbPfnMap的缓冲区，或为NULL。
* -- pcbPfnMap = 指向pPfnMap缓冲区字节数的指针。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_Map_GetPfn(_In_ DWORD pPfns[], _In_ DWORD cPfns, _Out_writes_bytes_opt_(*pcbPfnMap) PVMMDLL_MAP_PFN pPfnMap, _Inout_ PDWORD pcbPfnMap);



//-----------------------------------------------------------------------------
// VMM PROCESS FUNCTIONALITY BELOW:
// Functionality below is mostly relating to Windows processes.
//-----------------------------------------------------------------------------

/*
* 根据进程名检索活动进程。请注意，如果存在多个具有相同名称的进程，只会返回一个进程。如果需要解析具有相同名称的所有进程，
  请通过调用VMMDLL_PidList和VMMDLL_ProcessGetInformation迭代PID列表。
* -- szProcName = 进程名（不区分大小写）。
* -- pdwPID = 在成功时接收PID的指针。
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_PidGetFromName(_In_ LPSTR szProcName, _Out_ PDWORD pdwPID);

/*
* 列出系统中的PID。
* -- pPIDs = 至少具有系统中PID数量的DWORD数组，或为NULL。
* -- pcPIDs = pPIDs数组的大小（DWORD数）（输入时），系统中的PID数量（输出时）。
* -- return = 成功/失败。
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
* 从PID中检索各种进程信息。可以检索进程的名称、页目录基址和进程状态等信息。
* -- dwPID
* -- pProcessInformation = 如果为NULL，则大小由*pcbProcessInfo给出
* -- pcbProcessInformation = pProcessInfo的大小（以字节为单位）（输入和输出）
* -- return = 成功/失败。
*/
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_ProcessGetInformation(_In_ DWORD dwPID, _Inout_opt_ PVMMDLL_PROCESS_INFORMATION pProcessInformation, _In_ PSIZE_T pcbProcessInformation);

#define VMMDLL_PROCESS_INFORMATION_OPT_STRING_PATH_KERNEL           1
#define VMMDLL_PROCESS_INFORMATION_OPT_STRING_PATH_USER_IMAGE       2
#define VMMDLL_PROCESS_INFORMATION_OPT_STRING_CMDLINE               3

/*
/*
* 从进程中检索字符串值。该函数分配一个新的字符串缓冲区，并在其中返回所请求的字符串。
* 字符串始终以NULL结尾。如果失败，则返回NULL。
* 注意！调用者负责VMMDLL_MemFree的返回值！
* 调用者释放: VMMDLL_MemFree(return)
* -- dwPID
* -- fOptionString = 要检索的字符串值，由VMMDLL_PROCESS_INFORMATION_OPT_STRING_*指定
* -- return - 失败: NULL，成功: 字符串 - 注意！必须由调用者使用VMMDLL_MemFree进行释放！
*/
*/
EXPORTED_FUNCTION
LPSTR VMMDLL_ProcessGetInformationString(_In_ DWORD dwPID, _In_ DWORD fOptionString);

/*
* 检索有关数据目录、节、导出地址表和导入地址表（IAT）的信息。
* 如果pData == NULL，在进入时必须返回pData数组中需要容纳数据的条目数。
* -- dwPID
* -- [uw]szModule
* -- pData
* -- cData
* -- pcData
* -- return = 成功/失败。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_ProcessGetDirectoriesU(_In_ DWORD dwPID, _In_ LPSTR  uszModule, _Out_writes_(16) PIMAGE_DATA_DIRECTORY pData, _In_ DWORD cData, _Out_ PDWORD pcData);
_Success_(return) BOOL VMMDLL_ProcessGetDirectoriesW(_In_ DWORD dwPID, _In_ LPWSTR wszModule, _Out_writes_(16) PIMAGE_DATA_DIRECTORY pData, _In_ DWORD cData, _Out_ PDWORD pcData);
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_ProcessGetSectionsU(_In_ DWORD dwPID, _In_ LPSTR  uszModule, _Out_opt_ PIMAGE_SECTION_HEADER pData, _In_ DWORD cData, _Out_ PDWORD pcData);
_Success_(return) BOOL VMMDLL_ProcessGetSectionsW(_In_ DWORD dwPID, _In_ LPWSTR wszModule, _Out_opt_ PIMAGE_SECTION_HEADER pData, _In_ DWORD cData, _Out_ PDWORD pcData);

/*
/*
* 检索进程/模块中给定函数的虚拟地址。
* -- dwPID
* -- [uw]szModuleName
* -- szFunctionName
* -- return = 函数的虚拟地址，失败时返回零。
*/
*/
EXPORTED_FUNCTION
ULONG64 VMMDLL_ProcessGetProcAddressU(_In_ DWORD dwPID, _In_ LPSTR  uszModuleName, _In_ LPSTR szFunctionName);
ULONG64 VMMDLL_ProcessGetProcAddressW(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _In_ LPSTR szFunctionName);

/*
/*
* 检索给定模块的基址。
* -- dwPID
* -- [uw]szModuleName
* -- return = 模块的虚拟基址，失败时返回零。
*/
*/
EXPORTED_FUNCTION
ULONG64 VMMDLL_ProcessGetModuleBaseU(_In_ DWORD dwPID, _In_ LPSTR  uszModuleName);
ULONG64 VMMDLL_ProcessGetModuleBaseW(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName);



//-----------------------------------------------------------------------------
// WINDOWS SPECIFIC DEBUGGING / SYMBOL FUNCTIONALITY BELOW:
//-----------------------------------------------------------------------------

/*
* 加载一个 .pdb 符号文件，并在成功时返回关联的模块名称。
* -- dwPID
* -- vaModuleBase
* -- szModuleName = 用于接收成功时的模块名称的缓冲区。
* -- 返回值
*/
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_PdbLoad(_In_ DWORD dwPID, _In_ ULONG64 vaModuleBase, _Out_writes_(MAX_PATH) LPSTR szModuleName);

/*
* 根据模块名称和符号名称检索符号的虚拟地址。
* 注意！不是所有模块都可能存在 - 最初只有模块 "nt" 是可用的。
* 注意！如果多个模块具有相同的名称，则将使用第一个添加的模块。
* -- szModule: 模块名称。
* -- cbSymbolAddressOrOffset: 符号的虚拟地址或符号偏移量。
* -- szSymbolName: 接收成功时符号名称的缓冲区。
* -- pdwSymbolDisplacement: 从符号起始位置的位移。
* -- 返回值：

*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_PdbSymbolName(_In_ LPSTR szModule, _In_ QWORD cbSymbolAddressOrOffset, _Out_writes_(MAX_PATH) LPSTR szSymbolName, _Out_opt_ PDWORD pdwSymbolDisplacement);

/*
* 根据模块名称和符号名称获取符号的虚拟地址。
* 注意！并非所有模块都可能存在 - 最初只有模块 "nt" 可用。
* 注意！如果多个模块具有相同的名称，将使用最先添加的模块。
* -- szModule：模块名称。
* -- szSymbolName：符号名称。
* -- pvaSymbolAddress：用于接收符号虚拟地址的指针。
* -- 返回值：成功或失败。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_PdbSymbolAddress(_In_ LPSTR szModule, _In_ LPSTR szSymbolName, _Out_ PULONG64 pvaSymbolAddress);

/*
* 根据模块名称和类型名称获取类型的大小。
* 注意！并非所有模块都可能存在 - 最初只有模块 "nt" 可用。
* 注意！如果多个模块具有相同的名称，将使用最先添加的模块。
* -- szModule：模块名称。
* -- szTypeName：类型名称。
* -- pcbTypeSize：用于接收类型大小的指针。
* -- 返回值：成功或失败。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_PdbTypeSize(_In_ LPSTR szModule, _In_ LPSTR szTypeName, _Out_ PDWORD pcbTypeSize);

/*
* 定位类型子项的偏移量 - 通常是结构体中的子项。
* 注意！并非所有模块都可能存在 - 最初只有模块 "nt" 可用。
* 注意！如果多个模块具有相同的名称，将使用最先添加的模块。
* -- szModule：模块名称。
* -- uszTypeName：类型名称。
* -- uszTypeChildName：类型子项名称。
* -- pcbTypeChildOffset：用于接收类型子项偏移量的指针。
* -- 返回值：成功或失败。
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
* 检索目标系统中注册表Hive的信息。
* -- pHives：用于接收所有Hive信息的缓冲区，大小为 cHives * sizeof(VMMDLL_REGISTRY_HIVE_INFORMATION)。如果为NULL，则在 pcHives 中返回Hive的数量。
* -- cHives：缓冲区 pHives 的大小（以Hive数量为单位）。
* -- pcHives：如果 pHives == NULL，则为总Hive数量；如果 pHives 不为NULL，则为读取的Hive数量。
* -- 返回值：成功或失败。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_WinReg_HiveList(_Out_writes_(cHives) PVMMDLL_REGISTRY_HIVE_INFORMATION pHives, _In_ DWORD cHives, _Out_ PDWORD pcHives);

/*
* 读取连续的任意数量的注册表Hive内存，并报告读取的字节数。
* 注意！地址空间不包括regf注册表Hive文件头！
* -- vaCMHive：Hive内存的虚拟地址。
* -- ra：读取的起始偏移量。
* -- pb：用于接收读取数据的缓冲区。
* -- cb：要读取的字节数。
* -- pcbReadOpt：可选参数，用于返回实际读取的字节数。
* -- flags：标志位，与VMMDLL_FLAG_*中的标志位相同。
* -- 返回值：成功或失败。注意！即使读取了0字节，读取操作可能报告为成功 - 建议验证pcbReadOpt参数。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_WinReg_HiveReadEx(_In_ ULONG64 vaCMHive, _In_ DWORD ra, _Out_ PBYTE pb, _In_ DWORD cb, _Out_opt_ PDWORD pcbReadOpt, _In_ ULONG64 flags);

/*
* 将虚拟连续的任意数量的内存写入注册表Hive。
* 注意！地址空间不包括regf注册表Hive文件头！
* -- vaCMHive：Hive内存的虚拟地址。
* -- ra：写入的起始偏移量。
* -- pb：要写入的数据缓冲区。
* -- cb：要写入的字节数。
* -- 返回值：成功返回TRUE，部分或零写入返回FALSE。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_WinReg_HiveWrite(_In_ ULONG64 vaCMHive, _In_ DWORD ra, _In_ PBYTE pb, _In_ DWORD cb);

/*
* 枚举注册表子键 - 类似于WinAPI函数 'RegEnumKeyExW'。
* 请参考WinAPI函数文档以获取更多信息。
* 可以使用HKLM基址或CMHIVE基址的虚拟地址作为示例：
*   1) 'HKLM\SOFTWARE\Key\SubKey'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey'              （孤立键）
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey'          （孤立键）
* -- uszFullPathKey：完整的键路径。
* -- dwIndex：子键的索引，范围为0到N（-1表示键本身）。
* -- lpName：接收子键名的缓冲区。
* -- lpcchName：接收子键名长度的指针。
* -- lpftLastWriteTime：接收子键的最后写入时间。
* -- 返回值：成功返回TRUE，失败返回FALSE。
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
* 枚举给定注册表键的注册表值 - 类似于WinAPI函数 'EnumValueW'。
* 请参考WinAPI函数文档以获取更多信息。
* 可以通过两种方式调用：
* 可以使用HKLM基址或CMHIVE基址的虚拟地址作为示例：
*   1) 'HKLM\SOFTWARE\Key\SubKey'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey'              （孤立键）
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey'          （孤立键）
* -- uszFullPathKey：完整的键路径。
* -- dwIndex：值的索引。
* -- lpValueName：接收值名的缓冲区。
* -- lpcchValueName：接收值名长度的指针。
* -- lpType：接收值类型的指针。
* -- lpData：接收值数据的缓冲区。
* -- lpcbData：接收值数据长度的指针。
* -- 返回值：成功返回TRUE，失败返回FALSE。
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
* 查询给定注册表键/值路径的注册表值 - 类似于WinAPI函数 'RegQueryValueEx'。
* 请参考WinAPI函数文档以获取更多信息。
* 可以通过两种方式调用：
* 可以使用HKLM基址或CMHIVE基址的虚拟地址作为示例：
*   1) 'HKLM\SOFTWARE\Key\SubKey\Value'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey\'             （孤立键和默认值）
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey\Value'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey\Value'    （孤立键值）
* -- uszFullPathKeyValue：完整的键/值路径。
* -- lpType：接收值类型的指针。
* -- lpData：接收值数据的缓冲区。
* -- lpcbData：接收值数据长度的指针。
* -- 返回值：成功返回TRUE，失败返回FALSE。
*/
EXPORTED_FUNCTION _Success_(return)
BOOL VMMDLL_WinReg_QueryValueExU(
    _In_ LPSTR uszFullPathKeyValue,
    _Out_opt_ LPDWORD lpType,
    _Out_writes_opt_(*lpcbData) LPBYTE lpData,
    _When_(lpData == NULL, _Out_opt_) _When_(lpData != NULL, _Inout_opt_) LPDWORD lpcbData
);

/*
* 枚举注册表子键 - 类似于WinAPI函数 'RegEnumKeyExW'。
* 请参考WinAPI函数文档以获取更多信息。
* 可以通过HKLM基址或CMHIVE基址的虚拟地址来调用示例：
*   1) 'HKLM\SOFTWARE\Key\SubKey'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey'              （孤立键）
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey'          （孤立键）
* -- wszFullPathKey：完整的键路径。
* -- dwIndex：子键的索引，范围为0到N（-1表示键本身）。
* -- lpName：接收子键名的缓冲区。
* -- lpcchName：接收子键名长度的指针。
* -- lpftLastWriteTime：接收子键最后修改时间的指针。
* -- 返回值：成功返回TRUE，失败返回FALSE。
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
* 枚举注册表值 - 类似于WinAPI函数 'EnumValueW'。
* 请参考WinAPI函数文档以获取更多信息。
* 可以通过HKLM基址或CMHIVE基址的虚拟地址来调用示例：
*   1) 'HKLM\SOFTWARE\Key\SubKey'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey'              （孤立键）
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey'          （孤立键）
* -- wszFullPathKey：完整的键路径。
* -- dwIndex：值的索引。
* -- lpValueName：接收值名的缓冲区。
* -- lpcchValueName：接收值名长度的指针。
* -- lpType：接收值类型的指针。
* -- lpData：接收值数据的缓冲区。
* -- lpcbData：接收值数据长度的指针。
* -- 返回值：成功返回TRUE，失败返回FALSE。
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
* 查询注册表值 - 类似于WinAPI函数 'RegQueryValueEx'。
* 请参考WinAPI函数文档以获取更多信息。
* 可以通过HKLM基址或CMHIVE基址的虚拟地址来调用示例：
*   1) 'HKLM\SOFTWARE\Key\SubKey\Value'
*   2) 'HKLM\ORPHAN\SAM\Key\SubKey\'             （孤立键和默认值）
*   3) '0x<vaCMHIVE>\ROOT\Key\SubKey\Value'
*   4) '0x<vaCMHIVE>\ORPHAN\Key\SubKey\Value'    （孤立键值）
* -- wszFullPathKeyValue：完整的键/值路径。
* -- lpType：接收值类型的指针。
* -- lpData：接收值数据的缓冲区。
* -- lpcbData：接收值数据长度的指针。
* -- 返回值：成功返回TRUE，失败返回FALSE。
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
    BOOL f32;               // 如果为TRUE，表示函数是32位/4字节入口；否则为64位/8字节入口。
    ULONG64 vaThunk;        // 导入地址表 'thunk' 的地址。
    ULONG64 vaFunction;     // 如果导入地址表 'thunk' == 导入函数的地址，则为该值。
    ULONG64 vaNameModule;   // 导入模块名称字符串的地址。
    ULONG64 vaNameFunction; // 导入函数名称字符串的地址。
} VMMDLL_WIN_THUNKINFO_IAT, * PVMMDLL_WIN_THUNKINFO_IAT;

/*
* 检索导入函数的导入地址表（IAT）thunk的信息，包括IAT thunk的虚拟地址，对于钩子（hooking）很有用。
* 参数：
   - `dwPID`：进程的ID。
   - `[uw]szModuleName`：模块的名称。
   - `szImportModuleName`：导入模块的名称。
   - `szImportFunctionName`：导入函数的名称。
   - `pThunkIAT`：用于存储导入地址表（IAT）thunk信息的结构体指针。
* 返回值：成功或失败的指示。
*/
EXPORTED_FUNCTION
_Success_(return) BOOL VMMDLL_WinGetThunkInfoIATU(_In_ DWORD dwPID, _In_ LPSTR  uszModuleName, _In_ LPSTR szImportModuleName, _In_ LPSTR szImportFunctionName, _Out_ PVMMDLL_WIN_THUNKINFO_IAT pThunkInfoIAT);
_Success_(return) BOOL VMMDLL_WinGetThunkInfoIATW(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _In_ LPSTR szImportModuleName, _In_ LPSTR szImportFunctionName, _Out_ PVMMDLL_WIN_THUNKINFO_IAT pThunkInfoIAT);



//-----------------------------------------------------------------------------
// VMM UTIL FUNCTIONALITY BELOW:
//-----------------------------------------------------------------------------

/*
* 将可读的十六进制 ASCII 内存转储填充到调用方提供的 sz 缓冲区中。
* 参数：
   - `pb`：要转储的内存起始地址。
   - `cb`：要转储的内存大小（以字节为单位）。
   - `cbInitialOffset`：起始偏移量，必须小于等于 0x1000，且是 0x10 的倍数。
   - `sz`：用于填充的缓冲区，如果为 NULL，则在 pcsz 参数中返回缓冲区大小。
   - `pcsz`：
      - 如果 sz==NULL：退出时为缓冲区的大小（包括终止的 NULL）。
      - 如果 sz!=NULL：输入时为缓冲区的大小，退出时为字符的数量（不包括终止的 NULL）。
* 返回值：成功或失败的指示。
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
