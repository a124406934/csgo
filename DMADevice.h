#pragma once


#define PAGE_SIZE 0x1000 // 页大小
#define BELOW_PAGE_SIZE (PAGE_SIZE - 1) // 页内偏移的掩码
#define PAGE_ADDRESS(x) (x & 0xfffffffffffff000) // 获取给定地址所在页的起始地址
#define PAGE_OFFSET(x) (x & BELOW_PAGE_SIZE) // 获取给定地址相对于所在页的偏移量

#define PAGE_SHIFT 12 // 页移位数
#define PAGE_ALIGN(Va) ((QWORD)((ULONG_PTR)(Va) & ~(PAGE_SIZE - 1))) // 将地址按页对齐
#define ADDRESS_AND_SIZE_TO_SPAN_PAGES(Va,Size) ((ULONG)((((ULONG_PTR)(Va) & (PAGE_SIZE -1)) + (Size) + (PAGE_SIZE - 1)) >> PAGE_SHIFT)) // 计算给定地址范围所跨越的页数

typedef struct _SYSTEM_HANDLE
{
	ULONG ProcessId; // 进程 ID
	BYTE ObjectTypeNumber; // 对象类型编号
	BYTE Flags; // 标志
	USHORT Handle; // 句柄
	PVOID Object; // 对象指针
	ACCESS_MASK GrantedAccess; // 句柄的访问权限
} SYSTEM_HANDLE, * PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION2
{
	ULONG HandleCount; // 句柄数量
	SYSTEM_HANDLE Handles[1]; // 句柄数组
} SYSTEM_HANDLE_INFORMATION2, * PSYSTEM_HANDLE_INFORMATION2;


// 命名空间是一个容器，表示命名空间中的所有内容都属于命名空间的名称
// 要使用命名空间中的任何内容，只需使用命名空间名称后面的变量或函数！
namespace DMADevice
{
	// extern 只是声明我们有一个同名的变量，但实际定义在其他地方！
	// 在声明 extern 时，确保在其他地方进行了定义
	extern bool bConnected; // 连接状态
	extern DWORD dwAttachedProcessId; // 附加的进程 ID
	extern ULONG64 dwUnityPlayerBaseAddress; // Unity Player 的基地址

	// 声明函数，以便可以在整个代码中使用
	// 通过添加参数，可以避免在函数内部读取“复选框”的状态，并且等号表示这是默认值，如果不使用参数，则使用默认值！因此，如果调用 Connect();，这两个参数将是 Connect(false, false);
	extern bool Connect(bool bMMapFile = false, bool bStealth = false, bool bRemoveOtherDMAConnections = false); // 连接 DMA 设备

	extern void Disconnect(); // 断开连接

	// 创建一个函数，用于附加到进程，以便在找不到进程或者想要为其他项目更改进程时返回错误？不需要这个函数，但是无妨！
	extern bool AttachToProcessId(LPSTR szProcessName = REMOTE_PROCESS); // 附加到指定进程

	extern bool CheckForModuleInsideProcessId(DWORD dwProcessId, LPCWSTR wszModuleName = L"vmm.dll"); // 检查指定进程中是否存在指定的模块
	extern bool CloseHandleInProcess(DWORD dwProcessId, HANDLE hHandle); // 关闭指定进程中的句柄
	extern bool CloseDMAConnections(DWORD dwProcessId); // 关闭指定进程的 DMA 连接
	extern bool DisconnectOtherConnections(); // 断开其他连接

	template<typename U, typename P>DWORD MemRead(U lpAddress, P lpOutput, size_t uiSize, bool bFullReadRequired = true)
	{
		if (!dwAttachedProcessId || !bConnected || !lpAddress)
			return 0;

		DWORD dwBytesRead = 0;
		BOOL bRetn = (VMMDLL_MemReadEx(dwAttachedProcessId, (ULONG64)lpAddress, (PBYTE)lpOutput, uiSize, &dwBytesRead, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_NOCACHEPUT | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO) && dwBytesRead != 0);
		if (!bRetn || (bFullReadRequired && dwBytesRead != uiSize))
			return dwBytesRead;

		return dwBytesRead;
	}
	template<typename Var, typename U>Var MemReadPtr(U lpAddress)
	{
		Var lpPtr = 0;
		if (MemRead(lpAddress, &lpPtr, sizeof(Var)))
			return lpPtr;
		return 0;
	}
	template<typename U, typename P>BOOL MemWrite(U lpAddress, P lpInput, size_t uiSize)
	{
		if (!dwAttachedProcessId || !bConnected || !lpAddress)
			return FALSE;

		return VMMDLL_MemWrite(dwAttachedProcessId, (ULONG64)lpAddress, (PBYTE)lpInput, uiSize);
	}
	template<typename U, typename P>BOOL MemWritePtr(U lpAddress, P lpInput)
	{
		if (!dwAttachedProcessId || !bConnected || !lpAddress)
			return FALSE;

		return VMMDLL_MemWrite(dwAttachedProcessId, (ULONG64)lpAddress, (PBYTE)&lpInput, sizeof(P));
	}

	bool MemReadUnityString(CUnityString* lpStringPtr, CUnityString* us); // 读取 Unity 字符串
}
