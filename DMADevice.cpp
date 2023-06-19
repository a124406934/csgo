#include "globals.h"

// ����������Ϊ������Ҫ�ģ��Ա��������ִ��֮ǰ�������ø�ֵ
bool DMADevice::bConnected = false;
DWORD DMADevice::dwAttachedProcessId = NULL;
ULONG64 DMADevice::dwUnityPlayerBaseAddress = NULL;


bool DMADevice::Connect(bool bMMapFile, bool bStealth, bool bRemoveOtherDMAConnections)
{
	//����Ƿ������ӵ�DMA�豸��
	if (bConnected)
		return true;

	//��������������ֵ���������ǿ��Է����κ�������Ҫ��״̬����������˴���
	bool bReturnStatus = true;

	//Ĭ�ϲ�������Ϊ3����["", "-device", "fpga"]
	unsigned int iArgumentCount = 3;

	//���ò�����
	LPSTR args[7] = { "", "-device", "fpga", "-max", 0, "-memmap", "mmap.txt" };

	//���ȣ��������Ƴ������������ӣ�
	if (bRemoveOtherDMAConnections)
	{
		//���״̬�����Ƿ��ѹر�DMA����
		if (DisconnectOtherConnections() == false)
		{
			MessageBoxA(0, "�޷��ر�����DMA���ӡ���ȷ����һ�������Ѵ���", 0, 0);

			//�����Ҳ�ȷ���Ƿ�Ӧ�÷���false��ʾ�����ӣ���Ϊ�������û�д����ӻ�������
			return false;
		}
	}

	// �鿴�Ƿ�Ҫʹ��ӳ���ļ�
if (bMMapFile)
{
    // ����Ϊfalse����Ϊֻ����ͨ�������м���Ż�����Ϊtrue�����������׸��٣�
    bReturnStatus = false;

    // ����һ�����ڴ洢ӳ���ڴ��С�Ļ�����
    char szMapMemory[64] = { 0 };

    // ʹ��CreateFile�������һ�������ļ�������ȡ����dwCreationDisposition��־
    HANDLE hFile = CreateFileA("mmap.txt", FILE_GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    // ����Ƿ��и��ļ��ľ��
    if (hFile != INVALID_HANDLE_VALUE)
    {
        // ��ȡ�ļ���С
        DWORD dwFileSize = GetFileSize(hFile, NULL);

        // ����ļ���С�Ƿ����0
        if (dwFileSize > 0)
        {
            // ����һ���ļ����������Ա㽫�ļ��洢���ڴ��У��������BYTE��char���ͣ���BYTE���͸��ã���Ϊ������ΧΪ-256��256����char����ֻ����ΧΪ0��255
            // �����ں����1����Ϊ������Ҫ�ļ������� + һ�����ַ���ֹ������Ȼ���ǲ���Ҫ������Ϊ����֪���ļ���С�������ڴ������Σ�յģ���������ˣ���
            BYTE* lpFileBuffer = new BYTE[dwFileSize + 1];

            // ���ϵͳ�Ƿ����㹻���ڴ�洢���ļ���С��һ����˵������ϵͳ����Ӧ����10MB���ڴ棬���������ڴ����ڴ�ʱ����Ҫ���м�飡
            if (lpFileBuffer != NULL)
            {
                // ����һ���������洢���ļ��ж�ȡ���ֽ���/�ַ���
                DWORD dwNumberBytesRead = 0;
                if (ReadFile(hFile, lpFileBuffer, dwFileSize, &dwNumberBytesRead, NULL))
                {
                    // strrchr��ȡ���������������ļ����������������ļ��е����һ��"needle����-��"��Ȼ�󷵻ظ��ַ��������������ݣ�
                    char* szLastMemoryLine = strrchr((char*)lpFileBuffer, '-');
                    if (szLastMemoryLine != NULL)
                    {
                        // szLastMemoryLine����Ӧ�ð���- "- MEMORY_ADDRESS"

                        // �����źͿո����Ϊ0��x������Ӧ����0xMEMORY_ADDRESS���������ڿ���ʹ�����ˣ�����Ӧ���ܹ�ʹ�ã�
                        szLastMemoryLine[0] = '0';
                        szLastMemoryLine[1] = 'x';

                        // ���ڽ����һ���ڴ�ӳ�临�ƣ��Ա����ǿ���ʹ����
                        strcpy_s(szMapMemory, szLastMemoryLine);

                        // ������״̬����Ϊtrue����Ϊ���ǽ���һ����������Ϊtrue����ʾ����Ҫʹ��ӳ���ļ�������������������Ҫ�����ݣ�
                        bReturnStatus = true;

                        // ���ڸ��Ĳ�����������Ϊ����ʹ���˸���Ĳ�����
                        iArgumentCount = 7;
                    }
                }
                // ������������ʱ�����һ���ⲿ�֣���Ϊ��Щ�����Ƕ�delete []��delete��free�е�����
                // delete [] ����ɾ���ѷ�������飬����κ�ʹ��NEW [SIZE]�������ڴ涼������Ϊ�������ɾ����
                // delete ����ɾ��ʹ��NEW�������ڴ棬��Ӧ��ʹ��delete []ɾ��ʹ��NEW�������ڴ棬��Ϊ�Ƕ��ڴ治�����飬ֻ���ڴ�ָ��
                // ��ˣ��κ�ʱ���������ڴ�򴴽��ڴ棨��"new VARIABLE_TYPE[]"����Ӧ��ʹ��"delete []"�����������ʹ�õ���"new VARABLE_TYPE"��Ӧ��ʹ��delete
                delete[] lpFileBuffer;
            }
        }

        // ����ر��Ѵ򿪵��ļ�������Ա�ϵͳ���Խӹܸ��ļ���
        CloseHandle(hFile);
    }
    else
    {
        // �޷���ȡ�ļ��������˼���ļ��Ƿ���ڣ�
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            // �ļ�δ�ҵ���
            MessageBoxA(0, "mmap.txtδ�ҵ�����ȷ��mmap.txt��Ч�Ҿ�����Ч���ڴ�ӳ��", 0, 0);

            // ����false���Ա����ǲ���ʹ�ô���Ĳ������ӵ�dma�豸�������������ӡ��
            return false;
        }
    }

    // ���ڽ���������Ϊ�ҵ����ڴ�ӳ���ļ���
    args[4] = { szMapMemory };
}


// ��鷵��״̬�Ƿ�Ϊfalse�����Ϊfalse�����ʾ�޷����ӣ����ܴ������⡣
if (bReturnStatus == false)
{
	// ��ʾ�û����ܵĽ�������������Թ���Ա������г���ȷ���ļ���Ч�ȡ�
	MessageBoxA(0, "mmap.txtδ�ҵ�����ȷ��mmap.txt��Ч�Ҿ�����Ч���ڴ�ӳ��", 0, 0);

	// ����false����ʾ����ʧ�ܡ�
	return false;
}

// ���ڽ������ӡ�
if (!VMMDLL_Initialize(iArgumentCount, args))
{
	// ��������ʧ�ܵ���ʾ�����������û�������ӵ��µĳ����Ƿ���ʡ�
	MessageBoxA(0, "�޷����ӵ�DMA�豸����ȷ�����ӵ��³����ʵ����ܹ����ӵ�DMA�豸", 0, 0);

	// ����false����ʾ����ʧ�ܡ�
	return false;
}

// �Ƿ�Ҫǿ�����"����ֹ��־"����ʵ�����β�����ȷ��û�б�־�����ã�
if (bStealth)
{
	// ����������������
	ULONG64 qwID = 0, qwVersionMajor = 0, qwVersionMinor = 0;

	// ���豸�����ȡID����Ҫ�汾�źʹ�Ҫ�汾�ŵĽ��
	if (!VMMDLL_ConfigGet(LC_OPT_FPGA_FPGA_ID, &qwID) ||
		!VMMDLL_ConfigGet(LC_OPT_FPGA_VERSION_MAJOR, &qwVersionMajor) ||
		!VMMDLL_ConfigGet(LC_OPT_FPGA_VERSION_MINOR, &qwVersionMinor))
	{
		// ��ʾ������ʾ�����������û���������Ƿ�������
		MessageBoxA(0, "�޷���ȡFPGA�汾����ȷ����������", 0, 0);

		// ��Ϊ�Ѿ��������ӣ�������Ҫ�ر����ӡ�
		VMMDLL_Close();

		// ����false����ʾ����ʧ�ܡ�
		return false;
	}

	// ���FPGA�İ汾�Ƿ�֧�����ι���
	if (!((qwVersionMajor >= 4) && ((qwVersionMajor >= 5) || (qwVersionMinor >= 7))))
	{
		// ��ʾ������ʾ�����������û���������ѡ������FPGA�������°汾��
		MessageBoxA(0, "����FPGA�汾��֧�����ι��ܣ�����û����FPGA�������°汾", 0, 0);

		// ��Ϊ�Ѿ��������ӣ�������Ҫ�ر����ӡ�
		VMMDLL_Close();

		// ����false����ʾ����ʧ�ܡ�
		return false;
	}

	// ���������Դ洢��������
	LC_CONFIG LcConfig = {};

	// ����leech core���ð汾
	LcConfig.dwVersion = LC_CONFIG_VERSION;

	// �����豸���ƣ��豸�����Ѵ��ڣ���Ϊ�����Ѿ����������ӣ���������ϸ�һЩ������ʹ��FPGA�������豸���ơ�
	strcpy_s(LcConfig.szDevice, "existing");

	// ����������leech coreʵ��
	HANDLE hLeechCore = LcCreate(&LcConfig);

	// ���LcCreate�ķ���ֵ���������NULL��ʾ����ʧ�ܣ����򷵻�һ����������Ϊ"�ڴ�"��ָ�򴴽���leech core��
	if (hLeechCore == NULL)
	{
		// ��ʾ������ʾ�����������û����Ի��������ѡ�
		MessageBoxA(0, "�޷�������������豸��leech core�������ԣ����������Ȼ���ڣ����������ѡ��", 0, 0);

		// ��Ϊ�Ѿ��������ӣ�������Ҫ�ر����ӡ�
		VMMDLL_Close();

		// ����false����ʾ����ʧ�ܡ�
		return false;
	}

	// �벻Ҫ��������ʲô������Щ��������ʲô�ģ�����ufriskϣ�������͵�������������"pci config space flags"�Ķ�����
	BYTE bBytes[4] = { 0x10, 0x00, 0x10, 0x00 };

	// �����������豸�������Ҫ���豸ִ�����PCI config�ռ��־�����������ι����Ҳ��������ֻ֪���������ԣ�����ufrisk�����Ĵ�����չʾ����������������Ҫ������������ι�������Ϣ��������ufrisk���ʡ�
	// https://github.com/ufrisk/MemProcFS/blob/master/vmm_example/vmmdll_example.c#L184 �����Ĵ���ʾ����
	LcCommand(hLeechCore, LC_CMD_FPGA_CFGREGPCIE_MARKWR | 0x002, 4, bBytes, NULL, NULL);

	// �ر�leech core�ľ�����ⲻ��ر����ӣ���Ϊ������һ������������¼���ӵĴ������������ǽ�����2�����ӣ���ֻ�ر�1�Σ�������ӻ�������ִ�״̬��
	LcClose(hLeechCore);
}

// ������״̬���Ƶ�bConnected��������ָʾ�����Ƿ���ɡ�
bConnected = bReturnStatus;

// һ����ɣ������Ѿ����������Ӳ������������һ�� :)
return bReturnStatus;

}
bool DMADevice::AttachToProcessId(LPSTR szProcessName)
{
	// ���û�н������ӣ����ǳ��Բ��ҽ����ǲ��õ����飬���Ǵ���ģ��������������Ӧ�÷����������һ�����Ǻõġ�
	if (bConnected == false)
		return false;

	// ͨ���������ƻ�ȡ����ID����Щ��Ϸʹ����������ID������PUBG��������������ͬ���Ƶ�exe��������Ҫ���й��ˡ�
	if (VMMDLL_PidGetFromName(szProcessName, &dwAttachedProcessId) == FALSE)
		return false;

	// ��ȡUnityPlayer.dll�Ļ�ַ�����������Unity��Ϸ����Ӧ��ʼ�ձ��ҵ���
	dwUnityPlayerBaseAddress = VMMDLL_ProcessGetModuleBase(dwAttachedProcessId, L"UnityPlayer.dll");

	// ����Ƿ��ҵ���UnityPlayer.dll��
	if (dwUnityPlayerBaseAddress == 0)
	{
		// ��dwAttachedProcessId����ΪNULL����Ϊ���ǲ���ϣ�����ӵ������ϡ�
		dwAttachedProcessId = NULL;
		return false;
	}

	// ���dwAttachedProcessId��dwUnityPlayerBaseAddress����ΪNULL������true�����򷵻�false��
	return (dwAttachedProcessId != NULL && dwUnityPlayerBaseAddress != NULL);
}

void DMADevice::Disconnect()
{
	// û�������ˣ�
	bConnected = false;

	// ���ø��ӵĽ���ID
	dwAttachedProcessId = NULL;

	// ����DMA�ر��豸
	VMMDLL_Close();
}

bool DMADevice::CheckForModuleInsideProcessId(DWORD dwProcessId, LPCWSTR wszModuleName)
{
	// ����һ�����̵�ģ������б�
	HANDLE hProcessList = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);

	// ����޷���ȡ�б�
	if (hProcessList == INVALID_HANDLE_VALUE)
		return false;

	// ����һ���������洢���
	MODULEENTRY32 me32 = {};

	// ����Windows�ṹ�Ĵ�С
	me32.dwSize = sizeof(MODULEENTRY32);

	// ��ȡ�б��еĵ�һ��ģ��
	if (!Module32First(hProcessList, &me32))
	{
		// ��������ϵͳ���
		CloseHandle(hProcessList);
		return false;
	}

	// �Ƚ�ģ���������������Ҫ��ģ����
	do
	{
		// �Ƚ�ģ��������ָ����ģ�����������ִ�Сд��
		if (_wcsicmp(me32.szModule, wszModuleName) == 0)
		{
			// ��������ϵͳ���
			CloseHandle(hProcessList);

			// �ɹ����ҵ�ƥ���ģ��
			return true;
		}

		// ��ȡ�б��е���һ��ģ�飨����еĻ���
	} while (Module32Next(hProcessList, &me32));

	// ��������ϵͳ���
	CloseHandle(hProcessList);

	// û���ҵ�ƥ���ģ����
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
	// ����Ҫ���ʵĽ��̣�ʹ��ȫ������Ȩ�ޣ����������̡߳��ڴ��д�Ͳ�ѯ���̵ȵ�
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

	// ����޷��򿪸ý��̣���������ΪȨ�޲����ý�����ϵͳ���̵ȵ�
	if (hProcess == NULL)
		return false;

	// ��ȡ CloseHandle �����ĵ�ַ��ͨ����ϵͳģ���в��ң����õ����ڱ��أ���ǰ���̣��Ҳ����ú���
	LPTHREAD_START_ROUTINE Address_CloseHandle = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary(L"Kernel32.dll"), "CloseHandle");

	// ����޷��ҵ� CloseHandle ��������������Ϊ���л��������⣨������ macOS �����У���
	if (Address_CloseHandle == NULL)
	{
		// ����ϵͳ���
		CloseHandle(hProcess);
		return false;
	}

	// ��Ŀ������д���һ��Զ���̣߳�����ִ�д��롣�����������û�з����ױ��������ǿ���ֱ��ʹ����ͨ�� CreateThread ������
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, Address_CloseHandle, hHandle, 0, 0);

	// ����޷���Ŀ������д����̣߳�������Ȩ�޲������������
	if (hRemoteThread == NULL)
	{
		// ����ϵͳ���
		CloseHandle(hProcess);
		return false;
	}

	// �ȴ��߳�ִ����ϣ�ʹ�����޵ȴ�ʱ��
	WaitForSingleObject(hRemoteThread, INFINITE);

	// ����ϵͳ���
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);

	// �ɹ���
	return true;
}

bool DMADevice::CloseDMAConnections(DWORD dwProcessId)
{
	// ʹ��2MB��ΪĬ�ϴ�С�������Ҫ������Ե�����
	DWORD dwSize = 0x2000;

	// �����Ҫ���࣬ʹ�øñ����洢��С�������������Ҫ���࣡��
	DWORD dwRequired = 0;

	// ����һЩ�ڴ棬��Ϊ���ǲ���ʹ�ñ�������Ȼ����ʹ�ñ��������������ܻ�ܴ󣬱���50MB����ô��ִ���ļ���С��Ҫ����50MB���������Ǵ���һЩ�ڴ棬��ܼ򵥡����٣�
	SYSTEM_HANDLE_INFORMATION2* lpSysHandleInformation = (SYSTEM_HANDLE_INFORMATION2*)malloc(dwSize);

	// ��һ�£������޷�����㹻���ڴ棿ʧ������
	if (lpSysHandleInformation == NULL)
		return false;

	// ��ȡϵͳ�ϵ����о������
	NTSTATUS ntStatus = NtQuerySystemInformation(16/*SystemHandleInformation*/, lpSysHandleInformation, dwSize, &dwRequired);

	// ʧ���ˣ���Ҫ������
	if (!NT_SUCCESS(ntStatus))
	{
		// �������Ϊ��������С��������ô�������󻺳�����С�������³���
		if (ntStatus != STATUS_INFO_LENGTH_MISMATCH || !dwRequired)
		{
			// �����ڴ棡
			free(lpSysHandleInformation);
			return false;
		}

		// ��С����������ɵ��ڴ�
		free(lpSysHandleInformation);

		// �����С������������Ŀռ䣡
		dwSize = dwRequired + 1024;

		// ���·��������ڴ�ռ�
		lpSysHandleInformation = (SYSTEM_HANDLE_INFORMATION2*)malloc(dwSize);

		// ���Ǽ���ڴ��Ƿ����ɹ�
		if (lpSysHandleInformation == 0)
		{
			return false;
		}

		// ʹ�ø���Ĵ�С�ٴε���API�����Ӧ�ò�����ʧ���ˣ�
		ntStatus = NtQuerySystemInformation(16/*SystemHandleInformation*/, lpSysHandleInformation, dwSize, &dwRequired);

		// �ٴ�ʧ���ˣ����ֱ�ӷ���ʧ�ܣ�
		if (!NT_SUCCESS(ntStatus))
		{
			// �����ڴ棡
			free(lpSysHandleInformation);
			return false;
		}
	}

	// Ϊ�˱����ظ����������������ﴴ���ڴ����ڻ�ȡ������Ϣ
	POBJECT_TYPE_INFORMATION lpObjectTypeInfo = (POBJECT_TYPE_INFORMATION)malloc(0x5000);
	if (lpObjectTypeInfo == 0)
	{
		// �����ڴ棡
		free(lpSysHandleInformation);
		// �ڴ治�㣬����ʧ��
		return false;
	}

	// ���ڶ�����ϵͳ�������ѭ����������һЩ��飡
	for (int i = 0; i < lpSysHandleInformation->HandleCount; i++)
	{
		// ������i��ȡ���
		SYSTEM_HANDLE handle = lpSysHandleInformation->Handles[i];

		// ���������͡�����ID�Ƿ����4���ҽ���ID�Ƿ�������Ľ���ID��ͬ��
		if (handle.ObjectTypeNumber != 10 && handle.ProcessId > 4 && handle.ProcessId == dwProcessId)
		{
			// ��ӵ�о���Ľ��̣��Ա����ǿ��Ը��ƾ��
			HANDLE hProcessHandle = OpenProcess(PROCESS_DUP_HANDLE, FALSE, handle.ProcessId);

			// �޷��򿪽��̣�������ϣ�����ҵ�����Ҫ�ҵ����ݣ�
			if (hProcessHandle == 0)
				continue;

			// ���ڱ��������ݵı���
			HANDLE hDupHandle = INVALID_HANDLE_VALUE;

			// ���ƶ��󣬼����ƾ��������ǻ��ڽ��̵ģ����Գ��������󲢸������������޷�ʹ����һ�����̵ľ����
			if (!NT_SUCCESS(NtDuplicateObject(hProcessHandle, handle.Handle, GetCurrentProcess(), &hDupHandle, 0, 0, DUPLICATE_SAME_ACCESS)))
			{
				// ���ǹرպ���������
				CloseHandle(hProcessHandle);
				continue;
			}

			// ������õĻ���������ȷ��û�оɵ����ݣ�
			ZeroMemory(lpObjectTypeInfo, 0x5000);

			// ֻ����Ϊnull�����ǲ���ҪҲ����ʹ����������ĳЩ�����Ǳ���ģ�
			dwRequired = 0;

			// ���ڲ�ѯ�������
			if (!NT_SUCCESS(NtQueryObject(hDupHandle, ObjectTypeInformation, lpObjectTypeInfo, 0x5000, &dwRequired)))
			{
				// ���ǹرպ���������
				CloseHandle(hProcessHandle);
				CloseHandle(hDupHandle);
				continue;
			}

			// �����������Ƿ����ļ�����Ϊ���豸���ļ��л����ļ�ϵͳ��ص��κβ��������ļ����ͣ�
			if (wcscmp(lpObjectTypeInfo->TypeName.Buffer, L"File") != 0)
			{
				// ���ǹرպ���������
				CloseHandle(hProcessHandle);
				CloseHandle(hDupHandle);
				continue;
			}

			// ������õĻ���������ȷ��û�оɵ����ݣ�
			ZeroMemory(lpObjectTypeInfo, 0x5000);

			// ������������Ϣת��Ϊ������Ϣ
			POBJECT_NAME_INFORMATION lpObjectNameInfo = (POBJECT_NAME_INFORMATION)lpObjectTypeInfo;

			// �ٴβ�ѯ������Ϣ�������������Ҫ�������ƶ�������������
			if (!NT_SUCCESS(NtQueryObject(hDupHandle, ObjectNameInformation, lpObjectTypeInfo, 0x5000, &dwRequired)))
			{
				// ���ǹرպ���������
				CloseHandle(hProcessHandle);
				CloseHandle(hDupHandle);
				continue;
			}

			// �������ǽ�����������Ϣ���豸��DMA�豸���ӽ��бȽϣ���ʽΪDevice\\000X������X���豸��ID����ʱ����WDeviceW000����֪��Ϊʲô������Windows��\\ת��ΪW��������API��bug��
			if (wcsncmp(lpObjectNameInfo->Name.Buffer, L"\\Device\\000", wcslen(L"\\Device\\000")) == 0 ||
				wcsncmp(lpObjectNameInfo->Name.Buffer, L"WDeviceW000", wcslen(L"WDeviceW000")) == 0)
			{
				// ���������ҵ�������
				DWORD dwDMAProcessId = (DWORD)handle.ProcessId;
				HANDLE hDeviceHandle = (HANDLE)handle.Handle;

				// ����һ�У���Ϊ�϶���Ӧ�����������ӣ�ֻ��һ��...��

				// ���������ڴ棡
				free(lpSysHandleInformation);
				free(lpObjectTypeInfo);

				// ���ǹرպ���������
				CloseHandle(hProcessHandle);
				CloseHandle(hDupHandle);

				// ���ڴ���"�Ͽ���������"��"ǿ�ƶϿ�����"
				return CloseHandleInProcess(dwDMAProcessId, hDeviceHandle);
			}

			// ���ǹرպ���������
			CloseHandle(hProcessHandle);
			CloseHandle(hDupHandle);
		}
	}
	// ���������ڴ棡
	free(lpObjectTypeInfo);
	free(lpSysHandleInformation);
	return false;
}

bool DMADevice::DisconnectOtherConnections()
{
	// ����һ�������б��������ڻ�ȡ��ǰϵͳ�ϵ����н���
	HANDLE hProcessList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hProcessList == INVALID_HANDLE_VALUE)
		return false;

	// ����һ�����ڴ洢��ǰ���̵Ľṹ��
	PROCESSENTRY32 pe32 = {};
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// ��ȡ�б��еĵ�һ������
	if (!Process32First(hProcessList, &pe32))
	{
		CloseHandle(hProcessList);
		return false;
	}

	// ���浱ǰ���̵Ľ���ID
	DWORD dwCurrentProcessId = GetCurrentProcessId();

	do {
		// ���ȼ�����ID�Ƿ����5���κ�С��5�Ľ��̶���ϵͳ���̣����ǲ�Ӧ�ø���ϵͳ���̣�
		// Ȼ��ȽϽ���IDȷ�������������Լ��Ľ��̣�
		// Ȼ����������Ƿ���������ģ�飬��vmm.dll
		if (pe32.th32ProcessID > 5 && pe32.th32ProcessID != dwCurrentProcessId && CheckForModuleInsideProcessId(pe32.th32ProcessID))
		{
			// ʹ����Ϻ�ʼ�չرվ����ʹϵͳ������࣡
			CloseHandle(hProcessList);

			// �ر�DMA���Ӳ�����״̬
			return CloseDMAConnections(pe32.th32ProcessID);
		}
		// ��ȡ�б��е���һ������
	} while (Process32Next(hProcessList, &pe32));

	// ʹ����Ϻ�ʼ�չرվ����ʹϵͳ������࣡
	CloseHandle(hProcessList);

	// û���ҵ�����DMA���ӵĽ��̣������������ҵ��ģ�����������ֻ��˵���ǹر������ǡ�������ڴ򿪵����ӣ�DMA���޷����ӣ�
	return true;
}


bool DMADevice::MemReadUnityString(CUnityString* lpStringPtr, CUnityString* us)
{
	if (!dwAttachedProcessId || !bConnected || !lpStringPtr)
		return false;

	// ��ΪUnity�ַ������ܷǳ�����ǳ��̣�����ʹ��"�ڴ��ȡɢ��"����ufrisk˵���죿
	DWORD dwNumPages = ADDRESS_AND_SIZE_TO_SPAN_PAGES((QWORD)lpStringPtr, sizeof(CUnityString));

	// ��ȡҳ���ַ��ҳ���ַͨ����0x1000���룬�����ϵͳ�����������õģ�����Щϵͳ���ܻ���ģ��������ǲ�Ӧ����ô��
	QWORD qwPageAddress = PAGE_ALIGN((QWORD)lpStringPtr);

	// ��ȡƫ������ƫ�����ǵ�ַ�����3��ʮ���������֣�����0x1337��ƫ��������0x0337
	DWORD dwPageOffset = PAGE_OFFSET((QWORD)lpStringPtr);

	// ����3��ɢ���ڴ�����
	PPMEM_SCATTER ppMEMs;

	// �����ڴ沢��ʼ��ɢ���ڴ�
	if (!LcAllocScatter1(3, &ppMEMs))
		return false;

	// ÿ��ɢ���ڴ��ַ����Ҫ��0x1000 + (i * 0x1000)����
	for (int i = 0; i < dwNumPages; i++)
		ppMEMs[i]->qwA = (qwPageAddress + (i << PAGE_SHIFT));

	// �������͸��豸
	DWORD dwProcessedCount = VMMDLL_MemReadScatter(dwAttachedProcessId, ppMEMs, dwNumPages, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_NOCACHEPUT | VMMDLL_FLAG_ZEROPAD_ON_FAIL);

	// ������Ǵ����˶���ҳ
	if (dwProcessedCount > 0)
	{
		// ��ȡ��ǰ��������С
		DWORD cb = min(sizeof(CUnityString), PAGE_SIZE - dwPageOffset);

		// ��������ת��Ϊ�ֽڣ���Ϊ�ֽڴ�СΪ1��ָ���СΪ8
		LPBYTE lpBuffer = (LPBYTE)us;

		// ѭ����������󣬼����ǻ�õ�ҳ��
		for (int i = 0; i < dwProcessedCount; i++)
		{
			// ���������Ƿ�����ɻ���Ч
			if (!ppMEMs[i]->f)
				continue;

			// �����������Ƶ����ǵ����������
			memcpy(lpBuffer, (ppMEMs[i]->pb + dwPageOffset), cb);

			// �ƶ�������ָ��
			lpBuffer += cb;

			// ����ǰ��������С����Ϊҳ���С
			cb = PAGE_SIZE;

			// ����Ƿ���ҳ���С����
			if (((dwPageOffset + sizeof(CUnityString)) & 0xfff))
			{
				// ����ǰ��������С����Ϊʣ��ĳ���
				cb = ((dwPageOffset + sizeof(CUnityString)) & 0xfff);
			}
			// ����ҳ��ƫ��������Ϊ�������ڴ�ҳ��Ŀ�ʼλ�ÿ�ʼ��ȡ
			dwPageOffset = 0;
		}
		// ����������ڴ��ɢ���ڴ�
		LcMemFree(ppMEMs);

		// ����Ϊnull����Ȼ����Ҫ������������һ�£�
		ppMEMs = 0;

		// ��黺��������Ч�ԣ��ַ�����СӦ�뻺�����е�������ȫ��ͬ��û�ж�����ַ�Ҳû��ȱ�ٵ��ַ���
		return (wcslen(us->szString) == us->dwStringSize);
	}
	// ����������ڴ��ɢ���ڴ�
	LcMemFree(ppMEMs);

	// ����Ϊnull����Ȼ����Ҫ������������һ�£�
	ppMEMs = 0;

	// û�дﵽ������Ҫ��Ŀ�ꣿ
	return false;
}
