
#include <iostream>
#include <windows.h>
#include <cmath>


double pi = 3.1415926535897932;

DWORD client = (DWORD)GetModuleHandle("client.dll");
DWORD engine = (DWORD)GetModuleHandle("engine.dll");




bool ValidPlayer(DWORD ent, DWORD lp)
{
	if (ent != lp && *(DWORD*)(ent+ 0x100) > 0 && *(DWORD*)(ent + 0xED))
		return true;
	else
		return false;

}

struct vec3
{
	float x, y, z;
};

float GetDistance(vec3 p1, vec3 p2)
{
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}



vec3 GetBonePos(DWORD entaddress, int bone)
{
	DWORD boneptr = *(DWORD*)(entaddress + 0x26A8);
	vec3 bonepos;
	bonepos.x = *(float*)(boneptr + 0x30 * bone + 0x0C);
	bonepos.y = *(float*)(boneptr + 0x30 * bone + 0x1C);
	bonepos.z = *(float*)(boneptr + 0x30 * bone + 0x2C);

	return bonepos;
}

DWORD GetClosestEnemy(DWORD lp, DWORD entitylist)
{
	 float maxDist = 99999;
	 DWORD entNum;
	for (int i = 0; i < 32; i++)
	{
		DWORD entityptr = (entitylist + i * 0x10);
		if (*(DWORD*)entityptr != NULL)
		{
			DWORD entityaddr = *(DWORD*)entityptr;
			if (ValidPlayer(entityaddr,lp))
			{
				DWORD entdistance = GetDistance(*(vec3*)(entityaddr + 0x138), *(vec3*)(lp + 0x138));
				if (entdistance < maxDist)
				{
					maxDist = entdistance;
					entNum = i;
				}
			}
		}
	}


	return 	*(DWORD*)(entitylist + entNum * 0x10);

}


DWORD Thread(HMODULE hModule)
{
	DWORD clientstate = *(DWORD*)(engine + 0x58ADD4);
	DWORD entitylist = client + 0x4D5450C;
	DWORD pLocalplayer = (client + 0xD3FC5C);
	

	vec3 localcamerapos;
	DWORD entityaddr;
	DWORD localplayer;
	vec3 entheadpos;
	vec3 cameraoffset;
	vec3 localplayerpos;


	while (true)

	{
		Sleep(2);

		if(GetAsyncKeyState(VK_MENU)) //alt
		{
			
			if (*(DWORD*)pLocalplayer != NULL) 
			{
	
		
				localplayer = *(DWORD*)pLocalplayer;
				 entityaddr = GetClosestEnemy(localplayer,entitylist);
				entheadpos = GetBonePos(entityaddr, 8);

				localplayerpos = *(vec3*)(localplayer + 0x138);
				 cameraoffset = *(vec3*)(localplayer + 0x108);
				

				localcamerapos.x = localplayerpos.x + cameraoffset.x;
				localcamerapos.y = localplayerpos.y + cameraoffset.y;
				localcamerapos.z = localplayerpos.z + cameraoffset.z;
			
				
				
					

			
				
				float hyp = sqrt(pow(entheadpos.x - localcamerapos.x, 2) + pow(entheadpos.y - localcamerapos.y, 2) + pow(entheadpos.z - localcamerapos.z, 2));

				float opp = entheadpos.z - localcamerapos.z;

				double delta{0};
				double delta2{0};

				delta = -asin(opp / hyp) * (180 / pi); 


				*(float*)(clientstate + 0x4D88) = delta; 

				float adj = entheadpos.y - localcamerapos.y;

				float opp2 = entheadpos.x - localcamerapos.x;

				delta2 = atan2(adj, opp2) * (180 / pi);

				
				*(float*)(clientstate + 0x4D8C) = delta2;

			}
		}

		else if(GetAsyncKeyState(VK_END))
		{
			break;
		}
			

	}

	
	FreeLibraryAndExitThread(hModule, 0);
	return 0;

}












BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Thread, hModule, 0, nullptr);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}