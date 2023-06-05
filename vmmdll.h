
// vmmdll.h：用于包含在使用vmm.dll的项目中的头文件，无论是作为独立项目还是作为vmm.dll的本地插件。
//
// (c) Ulf Frisk, 2018-2019
// 作者：Ulf Frisk, pcileech@frizk.net
//
// 头文件版本：3.0

#include <windows.h>
#include "leechcore.h"

#pragma warning(disable : 4200)

#ifndef __VMMDLL_H__
#define __VMMDLL_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//-----------------------------------------------------------------------------
         // 下面是初始化功能：
        // 选择一种初始化VMM / 内存进程文件系统的方式。
//-----------------------------------------------------------------------------


/*

使用命令行参数初始化 VMM.DLL。有关参数的更详细信息，请参阅 MemProcFS 和 LeechCore 的 GitHub Wiki。
注意！LeechCore 的初始化参数对该函数也是有效的。
重要参数包括：
-printf = 显示 printf 样式的输出。
-v -vv -vvv = 额外的详细程度级别。
-device = 设备，格式如 LeechCore 所示 - 请参阅 leechcore.h 或 GitHub 文档以获取额外信息。一些可能的值包括：<file>、fpga、usb3380、hvsavedstate、totalmeltdown、pmem
-remote = 远程的 LeechCore 实例 - 请参阅 leechcore.h 或 GitHub 文档以获取额外信息。
-norefresh = 禁用后台刷新（即使后备内存是非易失性内存）。
-symbolserverdisable = 禁用符号服务器，直到用户更改为止。此参数将优先于注册表设置。
-pagefile[0-9] = 除物理内存之外要使用的页面文件。通常 pagefile.sys 的索引为 0，swapfile.sys 的索引为 1。页面文件在不断变化中 - 如果内存转储和页面文件之间的时间差超过几分钟，请不要使用。
*-waitinitialize = 在返回之前等待初始化完成。通常情况下，某些初始化是异步进行的，当初始化调用完成时可能尚未完成。这包括虚拟内存压缩、注册表等。
* -- argc
* -- argv
* -- 返回值 = 成功/失败
*/

_Success_(return)
BOOL VMMDLL_Initialize(_In_ DWORD argc, _In_ LPSTR argv[]);

/*
关闭已初始化的VMM.DLL实例并清理所有分配的资源，包括插件、链接的PCILeech.DLL和其他内存资源。

返回值：成功/失败。
*/
_Success_(return)
BOOL VMMDLL_Close();

/*
执行对所有内部缓存的强制刷新，包括：

进程列表
内存缓存
页表缓存
警告：此函数可能需要一些时间来执行！

dwReserved：保留供将来使用，必须为零。
返回值：成功/失败。
*/
_Success_(return)
BOOL VMMDLL_Refresh(_In_ DWORD dwReserved);

/*
释放由VMMDLL分配的内存。

pvMem：要释放的内存指针。
*/
VOID VMMDLL_MemFree(_Frees_ptr_opt_ PVOID pvMem);


//-----------------------------------------------------------------------------
/// 下面是配置设置：
// 配置内存进程文件系统或底层的内存获取设备。
//-----------------------------------------------------------------------------

/*
// 与函数一起使用的选项：VMMDLL_ConfigGet 和 VMMDLL_ConfigSet
// 选项可以使用此头文件中的 VMMDLL_OPT_* 或 leechcore.h 中的 LEECHCORE_OPT_* 进行定义
// 有关更详细的信息，请查阅各个设备类型的源代码。
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
设置设备特定的选项值。请参阅 VMMDLL_OPT_* 定义以了解有效的选项值信息。请注意，不同设备类型之间的选项值可能重叠，具有不同的含义。
-- fOption
-- pqwValue = 指向 ULONG64 的指针，用于接收选项值。
-- 返回值 = 成功/失败。
*/
_Success_(return)
BOOL VMMDLL_ConfigGet(_In_ ULONG64 fOption, _Out_ PULONG64 pqwValue);

/*
设置设备特定的选项值。请参阅 VMMDLL_OPT_* 定义以了解有效的选项值信息。请注意，不同设备类型之间的选项值可能重叠，具有不同的含义。
-- fOption
-- qwValue
-- 返回值 = 成功/失败。
*/
_Success_(return)
BOOL VMMDLL_ConfigSet(_In_ ULONG64 fOption, _In_ ULONG64 qwValue);



//-----------------------------------------------------------------------------
// VFS - 虚拟文件系统功能如下：
// 这是内存进程文件系统的核心。通过使用下面的功能，可以对文件系统进行所有的实现和分析。
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
* 用于回调到 VMM_VFS_FILELIST 结构的辅助内联函数。
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
* 在内存进程文件系统中列出一个目录中的文件。通过回调函数将目录和文件列在提供的 pFileList 参数中。如果需要获取单个文件的信息，必须列出其所在目录中的所有文件。
* -- wcsPath：目录的路径
* -- pFileList：用于接收文件列表的回调函数指针
* -- 返回值：成功/失败
*/
_Success_(return)
BOOL VMMDLL_VfsList(_In_ LPCWSTR wcsPath, _Inout_ PVMMDLL_VFS_FILELIST pFileList);

/*
* 从内存进程文件系统中读取文件的指定部分。
* -- wcsFileName：文件的路径
* -- pb：用于接收读取数据的缓冲区指针
* -- cb：要读取的数据大小
* -- pcbRead：实际读取的数据大小（输出参数）
* -- cbOffset：读取的起始偏移量
* -- 返回值：成功/失败
*
*/
NTSTATUS VMMDLL_VfsRead(_In_ LPCWSTR wcsFileName, _Out_ LPVOID pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset);

/*
* 向内存进程文件系统中的文件写入指定部分数据。
* -- wcsFileName：文件的路径
* -- pb：要写入的数据的缓冲区指针
* -- cb：要写入的数据大小
* -- pcbWrite：实际写入的数据大小（输出参数）
* -- cbOffset：写入的起始偏移量
* -- 返回值：成功/失败
*/
NTSTATUS VMMDLL_VfsWrite(_In_ LPCWSTR wcsFileName, _In_ LPVOID pb, _In_ DWORD cb, _Out_ PDWORD pcbWrite, _In_ ULONG64 cbOffset);

/*
* 用于对不同底层数据表示进行读取和写入的内存进程文件系统的实用函数。
*/
NTSTATUS VMMDLL_UtilVfsReadFile_FromPBYTE(_In_ PBYTE pbFile, _In_ ULONG64 cbFile, _Out_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset);
NTSTATUS VMMDLL_UtilVfsReadFile_FromQWORD(_In_ ULONG64 qwValue, _Out_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset, _In_ BOOL fPrefix);
NTSTATUS VMMDLL_UtilVfsReadFile_FromDWORD(_In_ DWORD dwValue, _Out_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset, _In_ BOOL fPrefix);
NTSTATUS VMMDLL_UtilVfsReadFile_FromBOOL(_In_ BOOL fValue, _Out_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbRead, _In_ ULONG64 cbOffset);
NTSTATUS VMMDLL_UtilVfsWriteFile_BOOL(_Inout_ PBOOL pfTarget, _In_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbWrite, _In_ ULONG64 cbOffset);
NTSTATUS VMMDLL_UtilVfsWriteFile_DWORD(_Inout_ PDWORD pdwTarget, _In_ PBYTE pb, _In_ DWORD cb, _Out_ PDWORD pcbWrite, _In_ ULONG64 cbOffset, _In_ DWORD dwMinAllow);


//-----------------------------------------------------------------------------
// 插件管理功能如下：
// 用于初始化和使用内存进程文件系统插件功能的函数和结构。通过调用函数VMM_VfsInitializePlugins来启动插件管理器。每个内置插件和外部插件，
// 其DLL名称与m_*.dll匹配的插件都将调用其InitializeVmmPlugin函数。插件/模块可以决定调用pfnPluginManager_Register来注册以不同名称形式的插件一次或多次。
// 下面是插件DLL中的注册函数示例：
// 'VOID InitializeVmmPlugin(_In_ PVMM_PLUGIN_REGINFO pRegInfo)'
//-----------------------------------------------------------------------------

/*
* 初始化所有可能的插件，包括内置插件和外部插件，这些插件映射到内存进程文件系统。请注意，默认情况下不会加载插件 - 
   必须通过调用此函数显式加载插件。它们将在 VMM DLL 的关闭时被卸载。
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
// VMM 核心功能如读取（和写入）虚拟内存和物理内存。注意！写入只在目标受支持的情况下才有效，即不是内存转储文件...
// 若要读取物理内存，请将 dwPID 指定为 (DWORD)-1

// 用于在调用 VMM_MemReadEx() 时禁止默认读取缓存的标志，这将导致读取总是从目标系统获取。
// 仍然使用缓存的页表（用于虚拟地址到物理地址的转换）。
#define VMMDLL_FLAG_NOCACHE                        0x0001  // 不使用数据缓存（强制从内存获取设备读取）
#define VMMDLL_FLAG_ZEROPAD_ON_FAIL                0x0002  // 失败的物理内存读取进行零填充，并且如果读取在物理内存范围内，报告成功。
#define VMMDLL_FLAG_FORCECACHE_READ                0x0008  // 强制使用缓存 - 对非缓存页面进行失败处理 - 仅适用于读取，与 VMMDLL_FLAG_NOCACHE/VMMDLL_FLAG_ZEROPAD_ON_FAIL 不兼容。
#define VMMDLL_FLAG_NOPAGING                       0x0010  // 不尝试从分页文件/压缩中检索分页出的内存（即使可能）
#define VMMDLL_FLAG_NOPAGING_IO                    0x0020  // 不尝试从分页出的内存中检索内存，如果读取将产生额外的 I/O（即使可能）。

/*
* 在由 ppMEMs 数组中的项目指针指定的各个非连续位置读取内存。每个单元的结果将逐个给出。没有读取项目数量的上限，
   但如果超过硬件限制，将不会提供性能提升。每个单元的最大大小为4KB页面（4096字节）。
* -- dwPID - 目标进程的 PID，(DWORD)-1 用于读取物理内存。
* -- ppMEMs = 分散读取头的数组。
* -- cpMEMs = ppMEMs 的数量。
* -- flags = 可选的标志，如 VMMDLL_FLAG_* 所指定。
* -- return = 成功读取的项目数量。
*/
DWORD VMMDLL_MemReadScatter(_In_ DWORD dwPID, _Inout_ PPMEM_IO_SCATTER_HEADER ppMEMs, _In_ DWORD cpMEMs, _In_ DWORD flags);

/*
* 读取一个4096字节的内存页面。
* -- dwPID - 目标进程的PID，(DWORD)-1 用于读取物理内存。
* -- qwA
* -- pbPage
* -- return = 成功/失败（取决于是否读取了所有请求的字节）。
*/
_Success_(return)
BOOL VMMDLL_MemReadPage(_In_ DWORD dwPID, _In_ ULONG64 qwA, _Inout_bytecount_(4096) PBYTE pbPage);

/*
这个函数用于读取指定地址开始的连续内存数据。

- `dwPID` 是目标进程的PID，如果将其设置为(DWORD)-1，则表示读取物理内存而非特定进程的虚拟内存。
- `qwA` 是要读取的起始地址。
- `pb` 是用于接收读取数据的缓冲区的指针。
- `cb` 表示要读取的字节数。

函数返回一个布尔值，表示读取操作是否成功。如果所有请求的字节都成功读取，则返回成功；否则返回失败。
*/
_Success_(return)
BOOL VMMDLL_MemRead(_In_ DWORD dwPID, _In_ ULONG64 qwA, _Out_writes_(cb) PBYTE pb, _In_ DWORD cb);

/*
这个函数用于从指定地址开始连续读取一段内存数据，并将实际读取的字节数通过 pcbRead 参数返回。
dwPID 是目标进程的PID，如果将其设置为(DWORD)-1，则表示读取物理内存而非特定进程的虚拟内存。
qwA 是要读取的起始地址。
pb 是用于接收读取数据的缓冲区的指针。
cb 表示要读取的字节数。
pcbRead 是一个指针，用于接收实际读取的字节数。
flags 是一组标志，可以使用 VMMDLL_FLAG_* 中定义的标志进行设置。
函数返回一个布尔值，表示读取操作是否成功。请注意，即使读取了0个字节，读取操作可能会报告成功，因此建议使用 pcbRead 参数来验证实际读取的字节数。
*/
_Success_(return)
BOOL VMMDLL_MemReadEx(_In_ DWORD dwPID, _In_ ULONG64 qwA, _Out_writes_(cb) PBYTE pb, _In_ DWORD cb, _Out_opt_ PDWORD pcbReadOpt, _In_ ULONG64 flags);

/*
* 预取一组地址（在 pA 数组中指定）到内存缓存中。该函数用于将较大的已知读取批量预取到本地缓存中，然后从缓存中进行多次较小的读取。
该函数的存在是为了提高性能。
* -- dwPID = 目标进程的PID，(DWORD)-1 用于物理内存。
* -- pPrefetchAddresses = 要预取到缓存中的地址数组。
* -- cPrefetchAddresses = 地址数量。
*/
_Success_(return)
BOOL VMMDLL_MemPrefetchPages(_In_ DWORD dwPID, _In_reads_(cPrefetchAddresses) PULONG64 pPrefetchAddresses, _In_ DWORD cPrefetchAddresses);

/*
* 写入连续的任意数量的内存。请注意，某些虚拟内存（例如可执行文件的页面，如DLL）可能在不同进程之间共享。作为示例，对一个进程中的
kernel32.dll的写入可能会影响整个系统中所有进程中的kernel32。堆、栈和其他内存通常是可以安全写入的。请在写入内存时小心操作！

参数：
- dwPID: 目标进程的PID，使用(DWORD)-1来表示读取物理内存。
- qwA: 写入的起始地址。
- pb: 要写入的数据缓冲区。
- cb: 要写入的字节数。
- 返回值: 成功时返回TRUE，部分或零写入时返回FALSE。
*/
_Success_(return)
BOOL VMMDLL_MemWrite(_In_ DWORD dwPID, _In_ ULONG64 qwA, _In_reads_(cb) PBYTE pb, _In_ DWORD cb);

/*
* 通过遍历指定进程的页表将虚拟地址转换为物理地址。
* -- dwPID: 目标进程的PID。
* -- qwVA: 要转换的虚拟地址。
* -- pqwPA: 接收转换后的物理地址的指针。
* -- 返回值: 成功时返回TRUE，失败时返回FALSE。
*/
_Success_(return)
BOOL VMMDLL_MemVirt2Phys(_In_ DWORD dwPID, _In_ ULONG64 qwVA, _Out_ PULONG64 pqwPA);



//-----------------------------------------------------------------------------
// VMM 进程映射功能如下：
// 用于检索与进程相关的项目集合的功能，例如页表映射（PTE），虚拟地址描述符映射（VAD），加载的模块，堆和线程。
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
    QWORD vaBase;//模块基地址
    QWORD vaEntry;//模块入口点
    DWORD cbImageSize;//图像大小
    BOOL  fWoW64;//是否为WoW64进程
    LPWSTR wszText;//模块文本
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
* 根据进程的硬件页表（PTE）检索内存映射条目。如果将 pPteMap 设置为 NULL，则将返回所需的字节数在参数 pcbPteMap 中。
* 返回的条目按照 VMMDLL_MAP_PTEENTRY.vaBase 进行排序。
* -- dwPID
* -- pPteMap = 最小字节长度为 *pcbPteMap 的缓冲区，或者为 NULL。
* -- pcbPteMap = 指向 pPteMap 缓冲区的字节数的指针。
* -- fIdentifyModules = 尝试识别模块（= 较慢）
* -- return = 成功/失败。
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetPte(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbPteMap) PVMMDLL_MAP_PTE pPteMap, _Inout_ PDWORD pcbPteMap, _In_ BOOL fIdentifyModules);

/*
* 根据进程的虚拟地址描述符（VAD）检索内存映射条目。如果将 pVadMap 设置为 NULL，则将返回所需的字节数在参数 pcbVadMap 中。
* 返回的条目按照 VMMDLL_MAP_VADENTRY.vaStart 进行排序。
* -- dwPID
* -- pVadMap = 最小字节长度为 *pcbVadMap 的缓冲区，或者为 NULL。
* -- pcbVadMap = 指向 pVadMap 缓冲区的字节数的指针。
* -- fIdentifyModules = 尝试识别模块（= 较慢）
* -- return = 成功/失败。
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetVad(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbVadMap) PVMMDLL_MAP_VAD pVadMap, _Inout_ PDWORD pcbVadMap, _In_ BOOL fIdentifyModules);

/*
* 检索指定进程的模块（.dll）。如果将 pModuleMap 设置为 NULL，则将返回所需的字节数在参数 pcbModuleMap 中。
* -- dwPID
* -- pModuleMap = 最小字节长度为 *pcbModuleMap 的缓冲区，或者为 NULL。
* -- pcbModuleMap = 指向 pModuleMap 缓冲区的字节数的指针。
* -- return = 成功/失败。
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetModule(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbModuleMap) PVMMDLL_MAP_MODULE pModuleMap, _Inout_ PDWORD pcbModuleMap);

/*
* 根据进程和模块名称检索模块映射条目（.exe / .dll）。
* 注意！此函数不会设置 PVMMDLL_MAP_MODULEENTRY->wszText。
* 如果需要模块名称，请使用 VMMDLL_ProcessMap_GetModule()。
* -- dwPID
* -- szModuleName
* -- pModuleMapEntry
* -- return = 成功/失败。
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetModuleFromName(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _Out_ PVMMDLL_MAP_MODULEENTRY pModuleMapEntry);

/*
* 检索指定进程的堆信息。如果 pHeapMap 设置为 NULL，则将在参数 pcbHeapMap 中返回所需的字节数。
* -- dwPID
* -- pHeapMap = 至少长度为 *pcbHeapMap 的缓冲区，如果为 NULL，则返回所需的字节数。
* -- pcbHeapMap = 指向 pHeapMap 缓冲区的字节数的指针。
* -- return = 成功/失败。
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetHeap(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbHeapMap) PVMMDLL_MAP_HEAP pHeapMap, _Inout_ PDWORD pcbHeapMap);

/*
* 检索指定进程的线程信息。如果 pThreadMap 设置为 NULL，则将在参数 pcbThreadMap 中返回所需的字节数。
* 返回的条目按 VMMDLL_MAP_THREADENTRY.dwTID 排序。
* -- dwPID
* -- pThreadMap = 至少长度为 *pcbThreadMap 的缓冲区，如果为 NULL，则返回所需的字节数。
* -- pcbThreadMap = 指向 pThreadMap 缓冲区的字节数的指针。
* -- return = 成功/失败。
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetThread(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbThreadMap) PVMMDLL_MAP_THREAD pThreadMap, _Inout_ PDWORD pcbThreadMap);

/*
* 检索指定进程的句柄信息。如果 pHandleMap 设置为 NULL，则将在参数 pcbHandleMap 中返回所需的字节数。
* 返回的条目按 VMMDLL_MAP_HANDLEENTRY.dwHandle 排序。
* -- dwPID
* -- pHandleMap = 至少长度为 *pcbHandleMap 的缓冲区，如果为 NULL，则返回所需的字节数。
* -- pcbHandleMap = 指向 pHandleMap 缓冲区的字节数的指针。
* -- return = 成功/失败。
*/
_Success_(return)
BOOL VMMDLL_ProcessMap_GetHandle(_In_ DWORD dwPID, _Out_writes_bytes_opt_(*pcbHandleMap) PVMMDLL_MAP_HANDLE pHandleMap, _Inout_ PDWORD pcbHandleMap);



//-----------------------------------------------------------------------------
/*
* 根据进程名称检索活动进程。请注意，如果存在多个具有相同名称的进程，只会返回一个进程。
* 如果需要解析具有相同名称的所有进程，请通过调用 VMMDLL_PidList 并结合 VMMDLL_ProcessGetInformation 迭代 PID 列表。
* -- szProcName = 进程名称（大小写不敏感）。
* -- pdwPID = 成功时将接收到的 PID 的指针。
* -- 返回值
*/
_Success_(return)
BOOL VMMDLL_PidGetFromName(_In_ LPSTR szProcName, _Out_ PDWORD pdwPID);

/*
* 列出系统中的进程标识符（PIDs）。
* -- pPIDs = 至少具有系统中进程数量的 DWORD 数组，或者为 NULL。
* -- pcPIDs = pPIDs 数组的大小（以 DWORD 数量表示），输入时为数组的大小，输出时为系统中的进程数量。
* -- return = 成功/失败。
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
* 从进程标识符（PID）中获取各种进程信息。可以获取进程的名称、页目录基址和进程状态等信息。
* -- dwPID
* -- pProcessInformation = 如果为 NULL，则在 *pcbProcessInformation 中给出大小
* -- pcbProcessInformation = pProcessInformation 的大小（以字节为单位），输入和输出时使用
* -- return = 成功/失败。
*/
_Success_(return)
BOOL VMMDLL_ProcessGetInformation(_In_ DWORD dwPID, _Inout_opt_ PVMMDLL_PROCESS_INFORMATION pProcessInformation, _In_ PSIZE_T pcbProcessInformation);

#define VMMDLL_PROCESS_INFORMATION_OPT_STRING_PATH_KERNEL           1
#define VMMDLL_PROCESS_INFORMATION_OPT_STRING_PATH_USER_IMAGE       2
#define VMMDLL_PROCESS_INFORMATION_OPT_STRING_CMDLINE               3

/*
* 获取进程的字符串值。该函数会分配一个新的字符串缓冲区，并将请求的字符串放入其中。字符串始终以 NULL 结尾。如果失败，将返回 NULL。
* 注意！调用者负责处理 VMMDLL_MemFree 的返回值！
* 调用者释放内存：VMMDLL_MemFree(return)
* -- dwPID
* -- fOptionString = 要检索的字符串值，由 VMMDLL_PROCESS_INFORMATION_OPT_STRING_* 给出
* -- 返回值 - 失败: NULL，成功: 字符串 - 注意！调用者必须使用 VMMDLL_MemFree 进行内存释放！
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
* 获取有关数据目录、节、导出地址表和导入地址表（IAT）的信息。
* 如果在输入时 pData == NULL，则返回 pData 数组需要具有的条目数以便能够容纳数据。
* -- dwPID
* -- wszModule
* -- pData
* -- cData
* -- pcData
* -- 返回值 = 成功/失败。
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
* dwPID: 目标进程的PID（Process ID）。
* wszModuleName: 目标模块的名称。
* szFunctionName: 目标函数的名称。
* 返回值: 目标函数在虚拟地址空间中的地址，失败时返回零。
*/
ULONG64 VMMDLL_ProcessGetProcAddress(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _In_ LPSTR szFunctionName);

/*
* dwPID: 目标进程的PID（Process ID）。
* wszModuleName: 目标模块的名称。
* 返回值: 目标模块的基地址（在虚拟地址空间中的地址），失败时返回零。
*/
ULONG64 VMMDLL_ProcessGetModuleBase(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName);



//-----------------------------------------------------------------------------
// WINDOWS SPECIFIC DEBUGGING / SYMBOL FUNCTIONALITY BELOW:
//-----------------------------------------------------------------------------

/*
* szModule: 模块名称。
* szSymbolName: 符号名称。
* pvaSymbolAddress: 用于存储符号的虚拟地址的指针。
* 返回值: 成功或失败。如果成功，pvaSymbolAddress 将包含符号的虚拟地址；如果失败，pvaSymbolAddress 将保持不变。
*/
_Success_(return)
BOOL VMMDLL_PdbSymbolAddress(_In_ LPSTR szModule, _In_ LPSTR szSymbolName, _Out_ PULONG64 pvaSymbolAddress);

/*
* szModule: 模块名称。
* szTypeName: 类型名称。
* pcbTypeSize: 用于存储类型大小的指针。
* 返回值: 成功或失败。如果成功，pcbTypeSize 将包含类型的大小；如果失败，pcbTypeSize 将保持不变。
*/
_Success_(return)
BOOL VMMDLL_PdbTypeSize(_In_ LPSTR szModule, _In_ LPSTR szTypeName, _Out_ PDWORD pcbTypeSize);

/*
* szModule: 模块名称。
* szTypeName: 类型名称。
* wszTypeChildName: 类型子项的名称。
* pcbTypeChildOffset: 用于存储类型子项偏移量的指针。
* 返回值: 成功或失败。如果成功，pcbTypeChildOffset 将包含类型子项的偏移量；如果失败，pcbTypeChildOffset 将保持不变。
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
* pHives: 用于接收所有注册表 hive 信息的缓冲区，大小为 cHives * sizeof(VMMDLL_REGISTRY_HIVE_INFORMATION)。如果传入 NULL，则会返回注册表 hive 的数量到 pcHives。
* cHives: pHives 缓冲区可以容纳的注册表 hive 的数量。
* pcHives: 如果 pHives 为 NULL，则在返回时包含系统中总共的注册表 hive 数量；如果 pHives 不为 NULL，则在返回时包含已读取的注册表 hive 数量。
* 返回值: 成功或失败。如果成功，返回读取的注册表 hive 的数量；如果失败，返回 0。
*/
_Success_(return)
BOOL VMMDLL_WinReg_HiveList(_Out_writes_(cHives) PVMMDLL_REGISTRY_HIVE_INFORMATION pHives, _In_ DWORD cHives, _Out_ PDWORD pcHives);

/*
* vaCMHive: 注册表 hive 内存的起始虚拟地址。
* ra: 注册表 hive 内存的物理起始地址。
* pb: 缓冲区用于接收读取的数据。
* cb: 要读取的字节数。
* pcbReadOpt: 在返回时包含实际读取的字节数。
* flags: 可选的标志，与 VMMDLL_FLAG_* 中的标志相同。
* 返回值: 成功或失败。如果成功，返回读取的数据的字节数；如果失败，返回 0。注意，即使读取的字节数为 0，读取操作也可能报告为成功，建议验证 pcbReadOpt 参数来确认实际读取的字节数。
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
* wszFullPathKey: 注册表键的完整路径，类似于 WINAPI 函数 'RegEnumKeyExW'。
* dwIndex: 子键的索引。
* lpName: 用于接收子键名称的缓冲区。
* lpcchName: 在输入时指定缓冲区大小，返回时包含子键名称的长度。
* lpftLastWriteTime: 在返回时包含子键的最后修改时间。
* 返回值: 成功或失败。如果成功，返回 ERROR_SUCCESS；如果失败，返回相应的错误代码。
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
* wszFullPathKey: 注册表键的完整路径，类似于 WINAPI 函数 'RegEnumValueW'。
* dwIndex: 值的索引。
* lpValueName: 用于接收值名称的缓冲区。
* lpcchValueName: 在输入时指定缓冲区大小，返回时包含值名称的长度。
* lpType: 用于接收值类型的变量指针。
* lpData: 用于接收值数据的缓冲区。
* lpcbData: 在输入时指定缓冲区大小，返回时包含值数据的大小。
* 返回值: 成功或失败。如果成功，返回 ERROR_SUCCESS；如果失败，返回相应的错误代码。
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
* wszFullPathKeyValue: 注册表键/值的完整路径，类似于 WINAPI 函数 'RegQueryValueExW'。
* lpType: 用于接收值类型的变量指针。
* lpData: 用于接收值数据的缓冲区。
* lpcbData: 在输入时指定缓冲区大小，返回时包含值数据的大小。
* 返回值: 成功或失败。如果成功，返回 ERROR_SUCCESS；如果失败，返回相应的错误代码。
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
* 返回值：失败时为 NULL，成功时为 PVMMDLL_WIN_TCPIP 结构体，其中包含结果。请注意，调用者负责释放内存，使用 VMMDLL_MemFree 函数进行释放。
*/
PVMMDLL_WIN_TCPIP VMMDLL_WinNet_Get();



//-----------------------------------------------------------------------------
// WINDOWS SPECIFIC UTILITY FUNCTIONS BELOW:
//-----------------------------------------------------------------------------

typedef struct tdVMMDLL_WIN_THUNKINFO_IAT {
    BOOL fValid;            // 表示结构体是否包含有效信息。
    BOOL f32;               // 如果为TRUE，则函数为32位/4字节条目；否则为64位/8字节条目。
    ULONG64 vaThunk;        // 导入地址表（Import Address Table）'thunk'的地址。
    ULONG64 vaFunction;     // 如果导入地址表（Import Address Table）'thunk'等于导入函数的地址，则为该值。
    ULONG64 vaNameModule;   // 导入模块名称字符串的地址。
    ULONG64 vaNameFunction; // 导入函数名称字符串的地址。
} VMMDLL_WIN_THUNKINFO_IAT, * PVMMDLL_WIN_THUNKINFO_IAT;

typedef struct tdVMMDLL_WIN_THUNKINFO_EAT {
    BOOL fValid;            // 表示结构体是否包含有效信息。
    DWORD valueThunk;       // 导出地址表（Export Address Table）'thunk'的值。
    ULONG64 vaThunk;        // 导入地址表（Import Address Table）'thunk'的地址。
    ULONG64 vaNameFunction; // 导出函数名称字符串的地址。
    ULONG64 vaFunction;     // 导出函数的地址（模块基址 + value参数）。
} VMMDLL_WIN_THUNKINFO_EAT, * PVMMDLL_WIN_THUNKINFO_EAT;

/*
* 获取导入函数的导入地址表（Import Address Table）IAT thunk的信息。包括IAT thunk的虚拟地址，这对于挂钩（hooking）很有用。
* -- dwPID：进程ID
* -- wszModuleName：模块名称
* -- szImportModuleName：导入模块名称
* -- szImportFunctionName：导入函数名称
* -- pThunkIAT：指向VMMDLL_WIN_THUNKINFO_IAT结构体的指针，用于接收IAT thunk的信息
* -- return：成功/失败
*/
_Success_(return)
BOOL VMMDLL_WinGetThunkInfoIAT(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _In_ LPSTR szImportModuleName, _In_ LPSTR szImportFunctionName, _Out_ PVMMDLL_WIN_THUNKINFO_IAT pThunkInfoIAT);

/*
* 获取导出函数的导出地址表（Export Address Table）EAT thunk的信息。包括EAT thunk的虚拟地址，这对于挂钩（hooking）很有用。
* -- dwPID：进程ID
* -- wszModuleName：模块名称
* -- pThunkEAT：指向VMMDLL_WIN_THUNKINFO_EAT结构体的指针，用于接收EAT thunk的信息
* -- return：成功/失败
*/
_Success_(return)
BOOL VMMDLL_WinGetThunkInfoEAT(_In_ DWORD dwPID, _In_ LPWSTR wszModuleName, _In_ LPSTR szExportFunctionName, _Out_ PVMMDLL_WIN_THUNKINFO_EAT pThunkInfoEAT);



//-----------------------------------------------------------------------------
// VMM UTIL FUNCTIONALITY BELOW:
//-----------------------------------------------------------------------------

/*
* 将可读的十六进制 ASCII 内存转储填充到调用者提供的 sz 缓冲区中。
* -- pb：要转储的内存地址
* -- cb：要转储的字节数
* -- cbInitialOffset：偏移量，必须小于等于 0x1000 并且是 0x10 的倍数。
* -- sz：要填充的缓冲区，如果为 NULL，则在 pcsz 参数中返回缓冲区大小。
* -- pcsz：
    - 如果 sz==NULL：返回缓冲区大小（包括终止的 NULL）。
    - 如果 sz!=NULL：在输入时为缓冲区大小，在输出时为字符数（不包括终止的 NULL）。
*/
_Success_(return)
BOOL VMMDLL_UtilFillHexAscii(_In_ PBYTE pb, _In_ DWORD cb, _In_ DWORD cbInitialOffset, _Out_opt_ LPSTR sz, _Inout_ PDWORD pcsz);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __VMMDLL_H__ */

#pragma warning(default : 4200)