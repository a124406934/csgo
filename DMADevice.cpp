#include "globals.h"

// 将变量设置为我们想要的，以便编译器在执行之前可以设置该值
bool DMADevice::bConnected = false;
DWORD DMADevice::dwAttachedProcessId = NULL;
ULONG64 DMADevice::dwUnityPlayerBaseAddress = NULL;


bool DMADevice::Connect(bool bMMapFile, bool bStealth, bool bRemoveOtherDMAConnections)
{
	//检查是否已连接到DMA设备？
	if (bConnected)
		return true;

	//在这里声明返回值，这样我们可以返回任何我们想要的状态，如果发生了错误！
	bool bReturnStatus = true;

	//默认参数计数为3，即["", "-device", "fpga"]
	unsigned int iArgumentCount = 3;

	//设置参数！
	LPSTR args[7] = { "", "-device", "fpga", "-max", 0, "-memmap", "mmap.txt" };

	//首先，让我们移除所有其他连接？
	if (bRemoveOtherDMAConnections)
	{
		//检查状态，看是否已关闭DMA连接
		if (DisconnectOtherConnections() == false)
		{
			MessageBoxA(0, "无法关闭其他DMA连接。您确定有一个连接已打开吗？", 0, 0);

			//现在我不确定是否应该返回false表示不连接，因为如果他们没有打开连接会怎样？
			return false;
		}
	}

	// 查看是否要使用映射文件
if (bMMapFile)
{
    // 设置为false，因为只有在通过了所有检查后才会设置为true，这样更容易跟踪！
    bReturnStatus = false;

    // 创建一个用于存储映射内存大小的缓冲区
    char szMapMemory[64] = { 0 };

    // 使用CreateFile创建或打开一个现有文件，具体取决于dwCreationDisposition标志
    HANDLE hFile = CreateFileA("mmap.txt", FILE_GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    // 检查是否有该文件的句柄
    if (hFile != INVALID_HANDLE_VALUE)
    {
        // 获取文件大小
        DWORD dwFileSize = GetFileSize(hFile, NULL);

        // 检查文件大小是否大于0
        if (dwFileSize > 0)
        {
            // 创建一个文件缓冲区，以便将文件存储到内存中，这可以是BYTE或char类型，但BYTE类型更好，因为它允许范围为-256到256，而char类型只允许范围为0到255
            // 我们在后面加1是因为我们想要文件缓冲区 + 一个空字符终止符（虽然我们不需要它，因为我们知道文件大小，但是内存可能是危险的，如果搞砸了！）
            BYTE* lpFileBuffer = new BYTE[dwFileSize + 1];

            // 检查系统是否有足够的内存存储此文件大小，一般来说，所有系统至少应具有10MB的内存，但是我们在处理内存时总是要进行检查！
            if (lpFileBuffer != NULL)
            {
                // 创建一个变量来存储从文件中读取的字节数/字符数
                DWORD dwNumberBytesRead = 0;
                if (ReadFile(hFile, lpFileBuffer, dwFileSize, &dwNumberBytesRead, NULL))
                {
                    // strrchr读取整个缓冲区（即文件缓冲区）并查找文件中的最后一个"needle（即-）"，然后返回该字符串及其后面的内容！
                    char* szLastMemoryLine = strrchr((char*)lpFileBuffer, '-');
                    if (szLastMemoryLine != NULL)
                    {
                        // szLastMemoryLine现在应该包含- "- MEMORY_ADDRESS"

                        // 将减号和空格更改为0和x，现在应该是0xMEMORY_ADDRESS，我们现在可以使用它了？或者应该能够使用！
                        szLastMemoryLine[0] = '0';
                        szLastMemoryLine[1] = 'x';

                        // 现在将最后一个内存映射复制，以便我们可以使用它
                        strcpy_s(szMapMemory, szLastMemoryLine);

                        // 将返回状态设置为true，因为我们将第一个参数设置为true，表示我们要使用映射文件，而且我们有所有需要的内容？
                        bReturnStatus = true;

                        // 现在更改参数计数，因为我们使用了更多的参数！
                        iArgumentCount = 7;
                    }
                }
                // 现在我来花点时间解释一下这部分，因为有些人总是对delete []、delete和free感到困惑
                // delete [] 用于删除已分配的数组，因此任何使用NEW [SIZE]创建的内存都必须作为数组进行删除！
                // delete 用于删除使用NEW创建的内存，不应该使用delete []删除使用NEW创建的内存，因为那段内存不是数组，只是内存指针
                // 因此，任何时候您创建内存或创建内存（即"new VARIABLE_TYPE[]"），应该使用"delete []"，但是如果您使用的是"new VARABLE_TYPE"，应该使用delete
                delete[] lpFileBuffer;
            }
        }

        // 必须关闭已打开的文件句柄，以便系统可以接管该文件！
        CloseHandle(hFile);
    }
    else
    {
        // 无法获取文件句柄，因此检查文件是否存在！
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            // 文件未找到！
            MessageBoxA(0, "mmap.txt未找到，请确保mmap.txt有效且具有有效的内存映射", 0, 0);

            // 返回false，以便我们不会使用错误的参数连接到dma设备，并给出错误的印象！
            return false;
        }
    }

    // 现在将参数设置为找到的内存映射文件？
    args[4] = { szMapMemory };
}


// 检查返回状态是否为false，如果为false，则表示无法连接，可能存在问题。
if (bReturnStatus == false)
{
	// 提示用户可能的解决方案，比如以管理员身份运行程序，确保文件有效等。
	MessageBoxA(0, "mmap.txt未找到，请确保mmap.txt有效且具有有效的内存映射", 0, 0);

	// 返回false，表示连接失败。
	return false;
}

// 现在进行连接。
if (!VMMDLL_Initialize(iArgumentCount, args))
{
	// 给出连接失败的提示，比如提醒用户检查连接电缆的长度是否合适。
	MessageBoxA(0, "无法连接到DMA设备，请确保连接电缆长度适当且能够连接到DMA设备", 0, 0);

	// 返回false，表示连接失败。
	return false;
}

// 是否要强制清除"主中止标志"，即实现隐蔽操作，确保没有标志被设置？
if (bStealth)
{
	// 创建变量来保存结果
	ULONG64 qwID = 0, qwVersionMajor = 0, qwVersionMinor = 0;

	// 向设备请求获取ID、主要版本号和次要版本号的结果
	if (!VMMDLL_ConfigGet(LC_OPT_FPGA_FPGA_ID, &qwID) ||
		!VMMDLL_ConfigGet(LC_OPT_FPGA_VERSION_MAJOR, &qwVersionMajor) ||
		!VMMDLL_ConfigGet(LC_OPT_FPGA_VERSION_MINOR, &qwVersionMinor))
	{
		// 显示错误提示，例如提醒用户检查连接是否正常。
		MessageBoxA(0, "无法获取FPGA版本，请确认连接正常", 0, 0);

		// 因为已经建立连接，现在需要关闭连接。
		VMMDLL_Close();

		// 返回false，表示连接失败。
		return false;
	}

	// 检查FPGA的版本是否支持隐蔽功能
	if (!((qwVersionMajor >= 4) && ((qwVersionMajor >= 5) || (qwVersionMinor >= 7))))
	{
		// 显示错误提示，例如提醒用户禁用隐蔽选项或更新FPGA板至最新版本。
		MessageBoxA(0, "您的FPGA版本不支持隐蔽功能，请禁用或更新FPGA板至最新版本", 0, 0);

		// 因为已经建立连接，现在需要关闭连接。
		VMMDLL_Close();

		// 返回false，表示连接失败。
		return false;
	}

	// 创建变量以存储配置数据
	LC_CONFIG LcConfig = {};

	// 设置leech core配置版本
	LcConfig.dwVersion = LC_CONFIG_VERSION;

	// 设置设备名称，设备名称已存在，因为我们已经建立了连接，如果您想严格一些，可以使用FPGA或其他设备名称。
	strcpy_s(LcConfig.szDevice, "existing");

	// 创建独立的leech core实例
	HANDLE hLeechCore = LcCreate(&LcConfig);

	// 检查LcCreate的返回值，如果返回NULL表示创建失败，否则返回一个句柄（或称为"内存"）指向创建的leech core。
	if (hLeechCore == NULL)
	{
		// 显示错误提示，例如提醒用户重试或禁用隐蔽选项。
		MessageBoxA(0, "无法创建或打开现有设备的leech core，请重试，如果问题仍然存在，请禁用隐蔽选项", 0, 0);

		// 因为已经建立连接，现在需要关闭连接。
		VMMDLL_Close();

		// 返回false，表示连接失败。
		return false;
	}

	// 请不要问我这是什么或者这些代码是做什么的，这是ufrisk希望您发送到配置中以重置"pci config space flags"的东西。
	BYTE bBytes[4] = { 0x10, 0x00, 0x10, 0x00 };

	// 现在我们向设备发送命令，要求设备执行清除PCI config空间标志的命令，具体如何工作我不清楚，我只知道这样可以，并且ufrisk在他的代码中展示了这个操作。如果想要更多关于这个如何工作的信息，可以向ufrisk提问。
	// https://github.com/ufrisk/MemProcFS/blob/master/vmm_example/vmmdll_example.c#L184 是他的代码示例。
	LcCommand(hLeechCore, LC_CMD_FPGA_CFGREGPCIE_MARKWR | 0x002, 4, bBytes, NULL, NULL);

	// 关闭leech core的句柄，这不会关闭连接，因为连接有一个计数器，记录连接的次数，所以我们建立了2次连接，但只关闭1次，因此连接会继续保持打开状态。
	LcClose(hLeechCore);
}

// 将返回状态复制到bConnected变量，以指示连接是否完成。
bConnected = bReturnStatus;

// 一切完成，我们已经建立了连接并设置了所需的一切 :)
return bReturnStatus;

}
bool DMADevice::AttachToProcessId(LPSTR szProcessName)
{
	// 如果没有建立连接，我们尝试查找进程是不好的事情，这是错误的！尽管这种情况不应该发生，但检查一下总是好的。
	if (bConnected == false)
		return false;

	// 通过进程名称获取进程ID，有些游戏使用两个进程ID或者像PUBG这样运行两个相同名称的exe，所以需要进行过滤。
	if (VMMDLL_PidGetFromName(szProcessName, &dwAttachedProcessId) == FALSE)
		return false;

	// 获取UnityPlayer.dll的基址，如果进程是Unity游戏，它应该始终被找到。
	dwUnityPlayerBaseAddress = VMMDLL_ProcessGetModuleBase(dwAttachedProcessId, L"UnityPlayer.dll");

	// 检查是否找到了UnityPlayer.dll。
	if (dwUnityPlayerBaseAddress == 0)
	{
		// 将dwAttachedProcessId设置为NULL，因为我们不再希望附加到进程上。
		dwAttachedProcessId = NULL;
		return false;
	}

	// 如果dwAttachedProcessId和dwUnityPlayerBaseAddress都不为NULL，返回true；否则返回false。
	return (dwAttachedProcessId != NULL && dwUnityPlayerBaseAddress != NULL);
}

void DMADevice::Disconnect()
{
	// 没有连接了！
	bConnected = false;

	// 重置附加的进程ID
	dwAttachedProcessId = NULL;

	// 告诉DMA关闭设备
	VMMDLL_Close();
}

bool DMADevice::CheckForModuleInsideProcessId(DWORD dwProcessId, LPCWSTR wszModuleName)
{
	// 创建一个进程的模块快照列表
	HANDLE hProcessList = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);

	// 如果无法获取列表
	if (hProcessList == INVALID_HANDLE_VALUE)
		return false;

	// 创建一个变量来存储结果
	MODULEENTRY32 me32 = {};

	// 告诉Windows结构的大小
	me32.dwSize = sizeof(MODULEENTRY32);

	// 获取列表中的第一个模块
	if (!Module32First(hProcessList, &me32))
	{
		// 总是清理系统句柄
		CloseHandle(hProcessList);
		return false;
	}

	// 比较模块的名称与我们想要的模块名
	do
	{
		// 比较模块名称与指定的模块名（不区分大小写）
		if (_wcsicmp(me32.szModule, wszModuleName) == 0)
		{
			// 总是清理系统句柄
			CloseHandle(hProcessList);

			// 成功！找到匹配的模块
			return true;
		}

		// 获取列表中的下一个模块（如果有的话）
	} while (Module32Next(hProcessList, &me32));

	// 总是清理系统句柄
	CloseHandle(hProcessList);

	// 没有找到匹配的模块名
	return false;
}

using fnFreeCall = DWORD_PTR(__fastcall*)(...);
template<typename ...Params>NTSTATUS NtRoutine(const char* szName, Params &&... params)
{
	HMODULE hNtDll = GetModuleHandleA("ntdll.dll");
	if (hNtDll == 0)
		return STATUS_NOT_FOUND;

	auto fn = (fnFreeCall)GetProcAddress(hNtDll, szName);
	if (fn == 0)
		return STATUS_NOT_FOUND;

	return (NTSTATUS)fn(std::forward<Params>(params) ...);
}
//simple API calls to ntdll for some reason ntdll is different on MOST version of windows so we use a simple API to just grab the "function" and forward the arguments to it really simple nothing much to explain
#define NtQuerySystemInformation(...) NtRoutine("NtQuerySystemInformation", __VA_ARGS__)
#define NtDuplicateObject(...) NtRoutine("NtDuplicateObject", __VA_ARGS__)
#define NtQueryObject(...) NtRoutine("NtQueryObject", __VA_ARGS__)

bool DMADevice::CloseHandleInProcess(DWORD dwProcessId, HANDLE hHandle)
{
	// 打开需要访问的进程，使用全部访问权限，包括创建线程、内存读写和查询进程等等
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

	// 如果无法打开该进程，可能是因为权限不足或该进程是系统进程等等
	if (hProcess == NULL)
		return false;

	// 获取 CloseHandle 函数的地址，通过在系统模块中查找，不用担心在本地（当前进程）找不到该函数
	LPTHREAD_START_ROUTINE Address_CloseHandle = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary(L"Kernel32.dll"), "CloseHandle");

	// 如果无法找到 CloseHandle 函数，可能是因为运行环境有问题（比如在 macOS 上运行？）
	if (Address_CloseHandle == NULL)
	{
		// 清理系统句柄
		CloseHandle(hProcess);
		return false;
	}

	// 在目标进程中创建一个远程线程，用于执行代码。由于这个进程没有反作弊保护，我们可以直接使用普通的 CreateThread 函数。
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, Address_CloseHandle, hHandle, 0, 0);

	// 如果无法在目标进程中创建线程，可能是权限不足或其他问题
	if (hRemoteThread == NULL)
	{
		// 清理系统句柄
		CloseHandle(hProcess);
		return false;
	}

	// 等待线程执行完毕，使用无限等待时间
	WaitForSingleObject(hRemoteThread, INFINITE);

	// 清理系统句柄
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);

	// 成功！
	return true;
}

bool DMADevice::CloseDMAConnections(DWORD dwProcessId)
{
	// 使用2MB作为默认大小，如果需要更多可以调整！
	DWORD dwSize = 0x2000;

	// 如果需要更多，使用该变量存储大小！（大家总是需要更多！）
	DWORD dwRequired = 0;

	// 分配一些内存，因为我们不能使用变量，虽然可以使用变量，但是它可能会很大，比如50MB，那么可执行文件大小就要增加50MB，所以我们创建一些内存，这很简单、快速！
	SYSTEM_HANDLE_INFORMATION2* lpSysHandleInformation = (SYSTEM_HANDLE_INFORMATION2*)malloc(dwSize);

	// 等一下，我们无法获得足够的内存？失败了吗？
	if (lpSysHandleInformation == NULL)
		return false;

	// 获取系统上的所有句柄对象
	NTSTATUS ntStatus = NtQuerySystemInformation(16/*SystemHandleInformation*/, lpSysHandleInformation, dwSize, &dwRequired);

	// 失败了？需要处理吗？
	if (!NT_SUCCESS(ntStatus))
	{
		// 如果是因为缓冲区大小不够，那么我们扩大缓冲区大小，并重新尝试
		if (ntStatus != STATUS_INFO_LENGTH_MISMATCH || !dwRequired)
		{
			// 清理内存！
			free(lpSysHandleInformation);
			return false;
		}

		// 大小不够，清理旧的内存
		free(lpSysHandleInformation);

		// 增大大小，并留出额外的空间！
		dwSize = dwRequired + 1024;

		// 重新分配更大的内存空间
		lpSysHandleInformation = (SYSTEM_HANDLE_INFORMATION2*)malloc(dwSize);

		// 总是检查内存是否分配成功
		if (lpSysHandleInformation == 0)
		{
			return false;
		}

		// 使用更大的大小再次调用API，这次应该不会再失败了！
		ntStatus = NtQuerySystemInformation(16/*SystemHandleInformation*/, lpSysHandleInformation, dwSize, &dwRequired);

		// 再次失败了？这次直接返回失败！
		if (!NT_SUCCESS(ntStatus))
		{
			// 清理内存！
			free(lpSysHandleInformation);
			return false;
		}
	}

	// 为了避免重复创建，我们在这里创建内存用于获取类型信息
	POBJECT_TYPE_INFORMATION lpObjectTypeInfo = (POBJECT_TYPE_INFORMATION)malloc(0x5000);
	if (lpObjectTypeInfo == 0)
	{
		// 清理内存！
		free(lpSysHandleInformation);
		// 内存不足，返回失败
		return false;
	}

	// 现在对所有系统句柄进行循环，并进行一些检查！
	for (int i = 0; i < lpSysHandleInformation->HandleCount; i++)
	{
		// 从索引i获取句柄
		SYSTEM_HANDLE handle = lpSysHandleInformation->Handles[i];

		// 检查对象类型、进程ID是否大于4并且进程ID是否与给定的进程ID相同！
		if (handle.ObjectTypeNumber != 10 && handle.ProcessId > 4 && handle.ProcessId == dwProcessId)
		{
			// 打开拥有句柄的进程，以便我们可以复制句柄
			HANDLE hProcessHandle = OpenProcess(PROCESS_DUP_HANDLE, FALSE, handle.ProcessId);

			// 无法打开进程，继续并希望能找到我们要找的内容！
			if (hProcessHandle == 0)
				continue;

			// 用于保存句柄数据的变量
			HANDLE hDupHandle = INVALID_HANDLE_VALUE;

			// 复制对象，即复制句柄，句柄是基于进程的，所以除非你请求并复制它，否则无法使用另一个进程的句柄！
			if (!NT_SUCCESS(NtDuplicateObject(hProcessHandle, handle.Handle, GetCurrentProcess(), &hDupHandle, 0, 0, DUPLICATE_SAME_ACCESS)))
			{
				// 总是关闭和清理句柄！
				CloseHandle(hProcessHandle);
				continue;
			}

			// 清空重用的缓冲区，以确保没有旧的数据！
			ZeroMemory(lpObjectTypeInfo, 0x5000);

			// 只设置为null，我们不需要也不想使用它，但是某些参数是必需的！
			dwRequired = 0;

			// 现在查询句柄对象
			if (!NT_SUCCESS(NtQueryObject(hDupHandle, ObjectTypeInformation, lpObjectTypeInfo, 0x5000, &dwRequired)))
			{
				// 总是关闭和清理句柄！
				CloseHandle(hProcessHandle);
				CloseHandle(hDupHandle);
				continue;
			}

			// 检查对象类型是否是文件，因为打开设备、文件夹或与文件系统相关的任何操作都是文件类型！
			if (wcscmp(lpObjectTypeInfo->TypeName.Buffer, L"File") != 0)
			{
				// 总是关闭和清理句柄！
				CloseHandle(hProcessHandle);
				CloseHandle(hDupHandle);
				continue;
			}

			// 清空重用的缓冲区，以确保没有旧的数据！
			ZeroMemory(lpObjectTypeInfo, 0x5000);

			// 将对象类型信息转换为名称信息
			POBJECT_NAME_INFORMATION lpObjectNameInfo = (POBJECT_NAME_INFORMATION)lpObjectTypeInfo;

			// 再次查询对象信息，但这次我们想要的是名称而不是类型名称
			if (!NT_SUCCESS(NtQueryObject(hDupHandle, ObjectNameInformation, lpObjectTypeInfo, 0x5000, &dwRequired)))
			{
				// 总是关闭和清理句柄！
				CloseHandle(hProcessHandle);
				CloseHandle(hDupHandle);
				continue;
			}

			// 现在我们将对象名称信息与设备的DMA设备连接进行比较，格式为Device\\000X，其中X是设备的ID，有时还有WDeviceW000，不知道为什么，但是Windows将\\转换为W，可能是API的bug？
			if (wcsncmp(lpObjectNameInfo->Name.Buffer, L"\\Device\\000", wcslen(L"\\Device\\000")) == 0 ||
				wcsncmp(lpObjectNameInfo->Name.Buffer, L"WDeviceW000", wcslen(L"WDeviceW000")) == 0)
			{
				// 备份我们找到的内容
				DWORD dwDMAProcessId = (DWORD)handle.ProcessId;
				HANDLE hDeviceHandle = (HANDLE)handle.Handle;

				// 清理一切，因为肯定不应该有其他连接，只有一个...？

				// 总是清理内存！
				free(lpSysHandleInformation);
				free(lpObjectTypeInfo);

				// 总是关闭和清理句柄！
				CloseHandle(hProcessHandle);
				CloseHandle(hDupHandle);

				// 现在处理"断开连接请求"或"强制断开连接"
				return CloseHandleInProcess(dwDMAProcessId, hDeviceHandle);
			}

			// 总是关闭和清理句柄！
			CloseHandle(hProcessHandle);
			CloseHandle(hDupHandle);
		}
	}
	// 总是清理内存！
	free(lpObjectTypeInfo);
	free(lpSysHandleInformation);
	return false;
}

bool DMADevice::DisconnectOtherConnections()
{
	// 创建一个进程列表句柄，用于获取当前系统上的所有进程
	HANDLE hProcessList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hProcessList == INVALID_HANDLE_VALUE)
		return false;

	// 创建一个用于存储当前进程的结构体
	PROCESSENTRY32 pe32 = {};
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// 获取列表中的第一个进程
	if (!Process32First(hProcessList, &pe32))
	{
		CloseHandle(hProcessList);
		return false;
	}

	// 保存当前进程的进程ID
	DWORD dwCurrentProcessId = GetCurrentProcessId();

	do {
		// 首先检查进程ID是否大于5，任何小于5的进程都是系统进程，我们不应该干扰系统进程！
		// 然后比较进程ID确保它不是我们自己的进程！
		// 然后检查进程中是否存在所需的模块，即vmm.dll
		if (pe32.th32ProcessID > 5 && pe32.th32ProcessID != dwCurrentProcessId && CheckForModuleInsideProcessId(pe32.th32ProcessID))
		{
			// 使用完毕后始终关闭句柄，使系统保持清洁！
			CloseHandle(hProcessList);

			// 关闭DMA连接并返回状态
			return CloseDMAConnections(pe32.th32ProcessID);
		}
		// 获取列表中的下一个进程
	} while (Process32Next(hProcessList, &pe32));

	// 使用完毕后始终关闭句柄，使系统保持清洁！
	CloseHandle(hProcessList);

	// 没有找到具有DMA连接的进程（至少我们能找到的），所以我们只能说我们关闭了它们。如果存在打开的连接，DMA将无法连接！
	return true;
}


bool DMADevice::MemReadUnityString(CUnityString* lpStringPtr, CUnityString* us)
{
	if (!dwAttachedProcessId || !bConnected || !lpStringPtr)
		return false;

	// 因为Unity字符串可能非常长或非常短，所以使用"内存读取散开"，据ufrisk说更快？
	DWORD dwNumPages = ADDRESS_AND_SIZE_TO_SPAN_PAGES((QWORD)lpStringPtr, sizeof(CUnityString));

	// 获取页面地址，页面地址通常与0x1000对齐，大多数系统都是这样设置的，但有些系统可能会更改，不过他们不应该这么做
	QWORD qwPageAddress = PAGE_ALIGN((QWORD)lpStringPtr);

	// 获取偏移量，偏移量是地址的最后3个十六进制数字，例如0x1337的偏移量将是0x0337
	DWORD dwPageOffset = PAGE_OFFSET((QWORD)lpStringPtr);

	// 创建3个散开内存请求
	PPMEM_SCATTER ppMEMs;

	// 创建内存并初始化散开内存
	if (!LcAllocScatter1(3, &ppMEMs))
		return false;

	// 每个散开内存地址都需要与0x1000 + (i * 0x1000)对齐
	for (int i = 0; i < dwNumPages; i++)
		ppMEMs[i]->qwA = (qwPageAddress + (i << PAGE_SHIFT));

	// 将请求发送给设备
	DWORD dwProcessedCount = VMMDLL_MemReadScatter(dwAttachedProcessId, ppMEMs, dwNumPages, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_NOCACHEPUT | VMMDLL_FLAG_ZEROPAD_ON_FAIL);

	// 检查我们处理了多少页
	if (dwProcessedCount > 0)
	{
		// 获取当前缓冲区大小
		DWORD cb = min(sizeof(CUnityString), PAGE_SIZE - dwPageOffset);

		// 将缓冲区转换为字节，因为字节大小为1，指针大小为8
		LPBYTE lpBuffer = (LPBYTE)us;

		// 循环处理的请求，即我们获得的页面
		for (int i = 0; i < dwProcessedCount; i++)
		{
			// 检查此请求是否已完成或有效
			if (!ppMEMs[i]->f)
				continue;

			// 将缓冲区复制到我们的输出缓冲区
			memcpy(lpBuffer, (ppMEMs[i]->pb + dwPageOffset), cb);

			// 移动缓冲区指针
			lpBuffer += cb;

			// 将当前缓冲区大小更改为页面大小
			cb = PAGE_SIZE;

			// 检查是否在页面大小以下
			if (((dwPageOffset + sizeof(CUnityString)) & 0xfff))
			{
				// 将当前缓冲区大小设置为剩余的长度
				cb = ((dwPageOffset + sizeof(CUnityString)) & 0xfff);
			}
			// 重置页面偏移量，因为我们现在从页面的开始位置开始读取
			dwPageOffset = 0;
		}
		// 清理所需的内存和散开内存
		LcMemFree(ppMEMs);

		// 设置为null，虽然不需要，但还是设置一下！
		ppMEMs = 0;

		// 检查缓冲区的有效性，字符串大小应与缓冲区中的内容完全相同，没有多余的字符也没有缺少的字符！
		return (wcslen(us->szString) == us->dwStringSize);
	}
	// 清理所需的内存和散开内存
	LcMemFree(ppMEMs);

	// 设置为null，虽然不需要，但还是设置一下！
	ppMEMs = 0;

	// 没有达到我们需要的目标？
	return false;
}
