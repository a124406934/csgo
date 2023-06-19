#pragma once


#define PAGE_SIZE 0x1000 // ҳ��С
#define BELOW_PAGE_SIZE (PAGE_SIZE - 1) // ҳ��ƫ�Ƶ�����
#define PAGE_ADDRESS(x) (x & 0xfffffffffffff000) // ��ȡ������ַ����ҳ����ʼ��ַ
#define PAGE_OFFSET(x) (x & BELOW_PAGE_SIZE) // ��ȡ������ַ���������ҳ��ƫ����

#define PAGE_SHIFT 12 // ҳ��λ��
#define PAGE_ALIGN(Va) ((QWORD)((ULONG_PTR)(Va) & ~(PAGE_SIZE - 1))) // ����ַ��ҳ����
#define ADDRESS_AND_SIZE_TO_SPAN_PAGES(Va,Size) ((ULONG)((((ULONG_PTR)(Va) & (PAGE_SIZE -1)) + (Size) + (PAGE_SIZE - 1)) >> PAGE_SHIFT)) // ���������ַ��Χ����Խ��ҳ��

typedef struct _SYSTEM_HANDLE
{
	ULONG ProcessId; // ���� ID
	BYTE ObjectTypeNumber; // �������ͱ��
	BYTE Flags; // ��־
	USHORT Handle; // ���
	PVOID Object; // ����ָ��
	ACCESS_MASK GrantedAccess; // ����ķ���Ȩ��
} SYSTEM_HANDLE, * PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION2
{
	ULONG HandleCount; // �������
	SYSTEM_HANDLE Handles[1]; // �������
} SYSTEM_HANDLE_INFORMATION2, * PSYSTEM_HANDLE_INFORMATION2;


// �����ռ���һ����������ʾ�����ռ��е��������ݶ����������ռ������
// Ҫʹ�������ռ��е��κ����ݣ�ֻ��ʹ�������ռ����ƺ���ı���������
namespace DMADevice
{
	// extern ֻ������������һ��ͬ���ı�������ʵ�ʶ����������ط���
	// ������ extern ʱ��ȷ���������ط������˶���
	extern bool bConnected; // ����״̬
	extern DWORD dwAttachedProcessId; // ���ӵĽ��� ID
	extern ULONG64 dwUnityPlayerBaseAddress; // Unity Player �Ļ���ַ

	// �����������Ա����������������ʹ��
	// ͨ����Ӳ��������Ա����ں����ڲ���ȡ����ѡ�򡱵�״̬�����ҵȺű�ʾ����Ĭ��ֵ�������ʹ�ò�������ʹ��Ĭ��ֵ����ˣ�������� Connect();���������������� Connect(false, false);
	extern bool Connect(bool bMMapFile = false, bool bStealth = false, bool bRemoveOtherDMAConnections = false); // ���� DMA �豸

	extern void Disconnect(); // �Ͽ�����

	// ����һ�����������ڸ��ӵ����̣��Ա����Ҳ������̻�����ҪΪ������Ŀ���Ľ���ʱ���ش��󣿲���Ҫ��������������޷���
	extern bool AttachToProcessId(LPSTR szProcessName = REMOTE_PROCESS); // ���ӵ�ָ������

	extern bool CheckForModuleInsideProcessId(DWORD dwProcessId, LPCWSTR wszModuleName = L"vmm.dll"); // ���ָ���������Ƿ����ָ����ģ��
	extern bool CloseHandleInProcess(DWORD dwProcessId, HANDLE hHandle); // �ر�ָ�������еľ��
	extern bool CloseDMAConnections(DWORD dwProcessId); // �ر�ָ�����̵� DMA ����
	extern bool DisconnectOtherConnections(); // �Ͽ���������

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

	bool MemReadUnityString(CUnityString* lpStringPtr, CUnityString* us); // ��ȡ Unity �ַ���
}
