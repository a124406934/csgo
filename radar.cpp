// ����һЩ����������
#define _CRT_SECURE_NO_WARNINGS 1
#define _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS 1
#pragma warning(disable : 4996)

#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <vmmdll.h>
#include "Utils.h"


#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib, "libprotobuf.lib")
//#include "utils/protobuf/protobuf.pb.h"

#include "utils/process/process.hpp"
#include "utils/json.hpp"

using namespace std;
//�������
LPSTR process_name = (LPSTR)"csgo.exe"; //ָ��Ҫ���ҵĽ��̵�����
LPWSTR clientModuleName = (LPWSTR)L"client.dll"; //ָ��Ҫ���ҵĿͻ���ģ������ơ�
VMMDLL_MAP_MODULEENTRY g_clientModule; //�洢�ҵ��Ŀͻ���ģ�����Ϣ��

LPWSTR engineModuleName = (LPWSTR)L"engine.dll"; //ָ��Ҫ���ҵ�����ģ������ơ�
VMMDLL_MAP_MODULEENTRY g_engineModule; //�洢�ҵ�������ģ�����Ϣ��

DWORD G_PID;//�������PID

int Shutdown();//��������ж��VMM

BOOL VMMDLL_ProcessMap_GetHandle(DWORD dwPID, PVMMDLL_MAP_HANDLE pHandleMap, PDWORD pcbHandleMap);

enum weapons : int {
	DEAGLE = 1,
	ELITE = 2,
	FIVESEVEN = 3,
	GLOCK = 4,
	AK47 = 7,
	AUG = 8,
	AWP = 9,
	FAMAS = 10,
	G3SG1 = 11,
	GALIL = 13,
	M249 = 14,
	M4A4 = 16,
	MAC10 = 17,
	P90 = 19,
	UMP45 = 24,
	XM1014 = 25,
	BIZON = 26,
	MAG7 = 27,
	NEGEV = 28,
	SAWEDOFF = 29,
	TEC9 = 30,
	ZEUS = 31,
	P2000 = 32,
	MP7 = 33,
	MP9 = 34,
	NOVA = 35,
	P250 = 36,
	SCAR20 = 38,
	SG553 = 39,
	SSG08 = 40,
	KNIFE_T = 42,
	FLASHBANG = 43,
	HEGRENADE = 44,
	SMOKE = 45,
	MOLOTOV = 46,
	DECOY = 47,
	FIREBOMB = 48,
	C4 = 49,
	MUSICKIT = 58,
	KNIFE_CT = 59,
	M4A1S = 60,
	USPS = 61,
	TRADEUPCONTRACT = 62,
	CZ75A = 63,
	REVOLVER = 64,
	KNIFE_BAYONET = 500,
	KNIFE_FLIP = 505,
	KNIFE_GUT = 506,
	KNIFE_KARAMBIT = 507,
	KNIFE_M9_BAYONET = 508,
	KNIFE_HUNTSMAN = 509,
	KNIFE_FALCHION = 512,
	KNIFE_BOWIE = 514,
	KNIFE_BUTTERFLY = 515,
	KNIFE_SHADOW_DAGGERS = 516,
	LAST
};

vector<const char*> weapon_names = {
	"none", // 0
	"DEAGLE", // 1
	"DUALS", // 2
	"FIVESEVEN", // 3
	"GLOCK", // 4
	"none",
	"none",
	"AK47", // 7
	"AUG", // 8
	"AWP", // 9
	"FAMAS", // 10
	"G3SG1", // 11
	"none",
	"GALIL", // 13
	"M249", // 14
	"none",
	"M4A4", // 16
	"MAC10", // 17
	"none",
	"P90", // 19
	"none",
	"none",
	"none",
	"none",
	"UMP45", // 24
	"XM1014", // 25
	"BIZON", // 26
	"MAG7", // 27
	"NEGEV", // 28
	"SAWEDOFF", // 29
	"TEC9", // 30
	"ZEUS", // 31
	"P2000", // 32
	"MP7", // 33
	"MP9", // 34
	"NOVA", // 35
	"P250", // 36
	"none",
	"SCAR20", // 38
	"SG553", // 39
	"SSG08", // 40
	"none", // 41
	"KNIFE", // 42 KNIFE_T
	"FLASHBANG", // 43
	"HEGRANADE", // 44
	"SMOKE", // 45
	"MOLOTOV", // 46
	"DECOY", // 47
	"FIREBOMB", // 48
	"C4", // 49
	"none",
	"none",
	"none",
	"none",
	"none",
	"none",
	"none",
	"none",
	"none", // 58 MUSICKIT
	"KNIFE", // 59 KNIFE_CT
	"M4A1S", // 60
	"USPS", // 61
	"none", // 62 TRADEUPCONTRACT
	"CZ75A", // 63
	"REVOLVER" // 64
};

//--------- �����Ƕ����˹ܵ��������ص��������������------------------

HANDLE pipe = INVALID_HANDLE_VALUE;// �ܵ����
char pipe_input[1023];  // �ܵ����뻺����
char pipe_output[1023]; // �ܵ����������

//--------- �����Ƕ����˹ܵ��������ص��������������------------------




auto connect() -> bool {
	pipe = nullptr;                                 // ��ʼ���ܵ����Ϊ��ָ��
	memset(pipe_input, 0, sizeof(pipe_input));       // ��չܵ����뻺����
	memset(pipe_output, 0, sizeof(pipe_output));     // ��չܵ����������

	// ���������ܵ�
	pipe = CreateNamedPipeA(
		"\\\\.\\pipe\\23d339ddef636cb0a5b9d0be60a289bc4ae87cc62cfd12b8f322e6310c1eea66",  // �����ܵ�������
		PIPE_ACCESS_OUTBOUND,                              // �ܵ�����Ȩ��Ϊֻд
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,  // �ܵ�����Ϊ��Ϣ���ݡ���ȡģʽΪ��Ϣģʽ���ȴ�ģʽΪ����
		PIPE_UNLIMITED_INSTANCES,                          // �ܵ���������ʵ����
		sizeof(pipe_output), sizeof(pipe_input),           // �ܵ���������������뻺�����Ĵ�С
		NULL,                                             // Ĭ�ϵĳ�ʱʱ��
		nullptr                                           // Ĭ�ϵİ�ȫ����
	);

	if (pipe == INVALID_HANDLE_VALUE) {
		cout << "pipe INVALID_HANDLE_VALUE = " << GetLastError() << endl;
	}

	auto pipe_connect = ConnectNamedPipe(pipe, nullptr);  // ���������ܵ�

	if (!pipe_connect)
	{
		cout << "pipe_connect false = " << GetLastError() << endl;
	}

	return true;
}


int main()
{

	// ----------------------��ʼ��VMM.dll----------------------------------
	LPSTR args[] = { (LPSTR)"", (LPSTR)"-device", (LPSTR)"FPGA" };
	if (!VMMDLL_Initialize(3, args))
	{
		cout << "[-] ��ʼ��vmm.dllʧ��!" << endl;
		return -1;
	}
	cout << "[+] vmm.dll �ɹ���ʼ��!" << endl;

	// ----------------------��ȡ����PID----------------------------------

	cout << "[+] �ȴ���Ϸ����..." << endl;
	cout << "[+] ****************************************************************" << endl;

		if (!VMMDLL_PidGetFromName(process_name, &G_PID))//������ȡ����PID
		return false;

	cout << "[+] " << process_name << "��������...    " << "����PID:" << G_PID << endl;
	cout << "[+] ****************************************************************" << endl;




	// ----------------------��ȡģ����----------------------------------


	//DWORD dwRequiredBytes = 0;
	//VMMDLL_ProcessMap_GetHandle(process_id, NULL, &dwRequiredBytes);
	//PVMMDLL_MAP_HANDLE pHandleMap = (PVMMDLL_MAP_HANDLE)malloc(dwRequiredBytes);
	//VMMDLL_ProcessMap_GetHandle(process_id, pHandleMap, &dwRequiredBytes);


	// ȡclientģ����Ϣ
	if (!VMMDLL_ProcessMap_GetModuleFromName(G_PID, clientModuleName, &g_clientModule))//������ģ����ȡģ��ָ��
		return false;

	int clientMokuaiMingChangDu = WideCharToMultiByte(CP_UTF8, 0, clientModuleName, -1, NULL, 0, NULL, NULL);
	std::string clientMokuaiMing(clientMokuaiMingChangDu, '\0');
	WideCharToMultiByte(CP_UTF8, 0, clientModuleName, -1, &clientMokuaiMing[0], clientMokuaiMingChangDu, NULL, NULL);
	cout << "    ģ����:" << clientMokuaiMing.c_str() << "     ģ���ַ:" << hex << g_clientModule.vaBase << endl;
	// ȡengineģ����Ϣ
	if (!VMMDLL_ProcessMap_GetModuleFromName(G_PID, engineModuleName, &g_engineModule))//������ģ����ȡģ��ָ��
		return false;

	int engineMokuaiMingChangDu = WideCharToMultiByte(CP_UTF8, 0, engineModuleName, -1, NULL, 0, NULL, NULL);
	std::string engineMokuaiMing(engineMokuaiMingChangDu, '\0');
	WideCharToMultiByte(CP_UTF8, 0, engineModuleName, -1, &engineMokuaiMing[0], engineMokuaiMingChangDu, NULL, NULL);
	cout << "    ģ����:" << engineMokuaiMing.c_str() << "     ģ���ַ:" << hex << g_engineModule.vaBase << endl;

		// ת������ģ��	//�ҵ� client.dll �� engine.dll ģ�����Ϣ,����һ����Ϣ��	
		printf("[*] ת��ģ�� %s\n", clientMokuaiMing.c_str());
		ModuleDump client = Utils::DumpModule(G_PID, g_clientModule);
		//process->client = process->get_module_base_address("client.dll");

		printf("[*] ת��ģ�� %s\n", engineMokuaiMing.c_str());
		//process->engine = process->get_module_base_address("engine.dll");
		ModuleDump engine = Utils::DumpModule(G_PID, g_engineModule);
		cout << "[+] ****************************************************************" << endl;

		//ȫ��ָ��
		auto global_pointer = Utils::FindSignature(G_PID, client, "A1 ? ? ? ? F3 0F 10 40 10", 1, 0, false);
		VMMDLL_MemRead(G_PID, static_cast<ULONG64>(global_pointer), reinterpret_cast<PBYTE>(&global_pointer), sizeof(DWORD_PTR));
		//std::cout << "��ȡȫ��ָ�� = " << std::hex << global_pointer << std::endl;
		printf("[*] ��ȡȫ��ָ�� %llx\n", static_cast<uintptr_t>(global_pointer));


		// ������ҵ�ַ OK
		auto local_player = Utils::FindSignature(G_PID, client, "8D 34 85 ? ? ? ? 89 15 ? ? ? ? 8B 41 08 8B 48 04 83 F9 FF", 3, 4);
		std::cout << "��ȡ������� = " << std::hex << local_player << std::endl;

		// �ͻ���״̬��ַ OK
		auto client_state = Utils::FindSignature(G_PID, engine, "A1 ? ? ? ? 33 D2 6A 00 6A 00 33 C9 89 B0", 1, 0, false);
		std::cout << "��ȡ�ͻ���״̬ = " << std::hex << client_state << std::endl;

		// �ͻ��˱����������
		auto client_state_local_player = Utils::FindSignature(G_PID, engine, "8B 80 ? ? ? ? 40 C3", 2, 0);
		std::cout << "�ͻ��˱���������� = " << std::hex << client_state_local_player << std::endl;

		//��ȡ�ͻ���״̬�����Ϣ
		auto client_state_player_info = Utils::FindSignature(G_PID, engine, "8B 89 ? ? ? ? 85 C9 0F 84 ? ? ? ? 8B 01", 2, 0, false);
		std::cout << "�ͻ��������Ϣָ�� = " << std::hex << client_state_player_info << std::endl;

		// ��ͼ����ָ��
		auto client_state_map = Utils::FindSignature(G_PID, engine, "05 ? ? ? ? C3 CC CC CC CC CC CC CC A1", 1, 0, false);
		std::cout << "��ͼ����ָ�� = " << std::hex << client_state_map << std::endl;

		// ��ͼĿ¼ָ��
		auto client_state_map_directory = Utils::FindSignature(G_PID, engine, "B8 ? ? ? ? C3 05 ? ? ? ? C3", 7, 0, false);
		std::cout << "��ͼĿ¼ָ�� = " << std::hex << client_state_map_directory << std::endl;

		// ��ȡ��ϷĿ¼·��
		auto game_dir = Utils::FindSignature(G_PID, engine, "68 ? ? ? ? 8D 85 ? ? ? ? 50 68 ? ? ? ? 68", 1, 0, false);
		const char* path[MAX_PATH];
		VMMDLL_MemRead(G_PID, static_cast<ULONG64>(game_dir), reinterpret_cast<PBYTE>(&path), sizeof(path));
		std::cout << "��ȡ��ϷĿ¼·�� = " << reinterpret_cast<const char*>(path) << std::endl;


		// ��ȡʵ���б�ָ��
		auto entity_list = Utils::FindSignature(G_PID, client, "BB ? ? ? ? 83 FF 01 0F 8C ? ? ? ? 3B F8", 1, 0, false);
		std::cout << "��ȡʵ���б�ָ�� = " << std::hex << entity_list << std::endl;

		// ��ȡ�״��ַ
		auto radar_base = Utils::FindSignature(G_PID, client, "A1 ? ? ? ? 8B 0C B0 8B 01 FF 50 ? 46 3B 35 ? ? ? ? 7C EA 8B 0D", 1, 0, false);
		std::cout << "��ȡ�״��ַ = " << std::hex << radar_base << std::endl;

		// ��ȡ����״ָ̬��
		auto dormant = Utils::FindSignature(G_PID, client, "8A 81 ? ? ? ? C3 32 C0", 2, 8, false);
		std::cout << "��ȡ����״ָ̬�� = " << std::hex << dormant << std::endl;
		std::cout << "--------------------------------------------------------------------------------" << std::endl;

		//-------------------------------------------------------------------------------------------------------------


		





	connect();  // ���Ӻ��������������ڽ�����ĳ�������������ӣ�����ʵ����Ҫ�鿴 connect() �����Ķ���������ġ�

	while (true)
	{
		if (GetAsyncKeyState(VK_END)) {
			break;  // ������� END ����������ѭ��
		}

		//BYTE map[64];


		ULONG64 clientStateAddress = Utils::ReadDWORD(G_PID, g_clientModule, client_state) + client_state_map;
		cout << clientStateAddress << endl;

		const char* map[64];
		BYTE buffer[sizeof(map)];

		if (VMMDLL_MemRead(G_PID, clientStateAddress, buffer, sizeof(buffer)))
		{
			memcpy(map, buffer, sizeof(map));
			cout << map << endl;
		}
		else
		{
			cout << "Failed to read memory." << endl;
		}

			cout << map << endl;
	

	



		//DWORD targetAddress = 0xfbfb029c;
		//PBYTE map[64];





		nlohmann::json j;

		//j["global"]["directory"] = reinterpret_cast<const char*>(path);
		//j["global"]["map"] = reinterpret_cast<const char*>(map);

		//const auto max_clients = process->read<int>(global_pointer + 0x18);
		//for (auto i = 0; i <= max_clients; i++)
		//{
		//	const auto _local_player = process->read<DWORD_PTR>(local_player);
		//	const auto entity = process->read<DWORD_PTR>(entity_list + 0x10 * i);

		//	if (!entity)
		//		continue;  // ���ʵ��Ϊ�գ��������һ��ѭ��

		//	const auto team = process->read<int>(entity + 0xF4);
		//	if (_local_player == entity)
		//		j["global"]["team"] = team;  // ����Ǳ�����ң�������ȫ���Ŷ���Ϣ

		//	const auto health = process->read<int>(entity + 0x100);

		//	if (!health)
		//		continue;  // �������ֵΪ�㣬�������һ��ѭ��

		//	const auto lifestate = process->read<BYTE>(entity + 0x25F);

		//	if (lifestate)
		//		continue;  // �������״̬��Ϊ�㣬�������һ��ѭ��

		//	const auto bdormant = process->read<BYTE>(entity + dormant);

		//	if (bdormant)
		//		continue;  // ���ʵ�屻���ߣ��������һ��ѭ��

		//	const auto active_weapon = process->read<DWORD>(entity + 0x2F08);
		//	const auto weapon_entity = process->read<DWORD>(entity_list + ((active_weapon & 0xFFF) - 1) * 0x10);
		//	auto weapon_index = process->read<short>(weapon_entity + 0x2FBA);

		//	if (weapon_index > 64 || weapon_index < 1)
		//		weapon_index = 42;  // �����������������Χ��������Ĭ������Ϊ 42

		//	const auto weapon_name = strcmp(weapon_names[weapon_index], "none") ? weapon_names[weapon_index] : "KNIFE";

		//	auto radar = process->read<DWORD_PTR>(radar_base);
		//	radar = process->read<DWORD_PTR>(radar + 0x78);
		//	const char* name[128];
		//	ReadProcessMemory(process->process, reinterpret_cast<LPCVOID>(radar + 0x174 * (i + 2) + 0x18), &name, sizeof(name), nullptr);

		//	const auto x = process->read<float>(entity + 0x138);
		//	const auto y = process->read<float>(entity + 0x13C);

		//	const auto _i = to_string(i);

		//	j[_i]["health"] = health;

		//	if (_local_player == entity)
		//		j[_i]["local"] = true;  // ����Ǳ�����ң������þֲ� "local" ����Ϊ true
		//	else
		//		j[_i]["local"] = false;  // ������Ǳ�����ң������þֲ� "local" ����Ϊ false

		//	j[_i]["name"] = reinterpret_cast<const char*>(name);
		//	j[_i]["position"] = { x, y };
		//	j[_i]["team"] = team;
		//	j[_i]["weapon"] = weapon_name;
		//}

		/*
		file << setw(4) << j << endl;
		file.flush();
		file.close();
		*/

		DWORD write_buffer_written_bytes;
		const auto pipe_write = WriteFile(pipe, j.dump().c_str(), j.dump().size(), &write_buffer_written_bytes, nullptr);

		if (!pipe_write)
		{
			cout << "д��ܵ�ʧ��... = " << dec << GetLastError() << endl;
			if (GetLastError() == 232) {
				DisconnectNamedPipe(pipe);
				CloseHandle(pipe);
				connect();  // ���д��ܵ�ʧ���Ҵ�����Ϊ 232���Ͽ��������ܵ����رվ������������
			}
		}

		auto pipe_flush = FlushFileBuffers(pipe);

		if (!pipe_flush)
		{
			cout << "�رչܵ�ˢ�� = " << dec << GetLastError() << endl;
		}

		this_thread::sleep_for(chrono::milliseconds(5));  // �߳����� 5 ����

	}

	DisconnectNamedPipe(pipe);  // �Ͽ��������ܵ�
	CloseHandle(pipe);  // �رվ��
}


int Shutdown()
{
	// Close vmm.dll
	if (!VMMDLL_Close())
	{
		cout << "[-] �޷��ر�vmm.dll!" << endl;
		return 0;
	}
	cout << "[+] vmm.dll �ɹ��ر�!" << endl;

	cout << "[*] �����˳�����...." << endl;
	return 0;
}
