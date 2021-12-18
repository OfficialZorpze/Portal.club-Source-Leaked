#pragma once
#define RVA(addr, size) ((uintptr_t)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))
#define M_PI	3.14159265358979323846264338327950288419716939937510

namespace Renderer_Defines {
	float Width;
	float Height;
}



bool PressedKeys[254];

namespace Settings
{
	bool Memory = false;
	float Smoothness = 1;

	bool nocooldown = false;



	inline static const char* SelectableAimTypes[] = { "Memory Aim", "Input Aim"/*, "Mouse Aim", "Silent Aimbot"*/ };
	inline static const char* CurrentAimType = "Memory Aim";

	inline static const char* SelectableAimBones[] = { "Head", "Neck", "Chest", "Torso",  "Pelvis" };
	inline static const char* CurrentAimBone = "Chest";

	inline static const char* SelectableCrosshairs[] = { "Normal Crosshair", "Tilted Crosshair", "Cross Style 1", "Cross Style 2" };
	inline static const char* CurrentCrosshair = "Normal Crosshair";

	bool EnableCrosshair = true;

	bool WaypointTP = false;

	//Aim
	bool EnableAimbot = false;
	static bool Aim = false;
	bool MagicBullets = false;
	//bool SilentAimbot = true;
	bool gay = false;
	bool fortnite1;
	bool fortnite2;
	bool NoClip = false;
	bool UfoESP = false;
	bool PlaneESP = false;
	bool Head = true;
	bool Radar = false;
	bool Neck = false;
	bool Pelvis = false;
	bool toes = false;
	bool fishinghole = true;
	bool debug_info = false;
	bool nobloom = false;
	static bool AimPrediction = false;
	static bool fakePrediction = false;
	static bool MouseAim = 0;
	int aimbone = 66;
	bool bodyaim = false;
	float smoothY = 1;
	float delay = 0.1;
	float speedhacks = 3;
	float trigger_bot_distance_limit = 3;
	float delaytrigger_bot_distance_limit = 3;
	float trigger_spread = 3;
	float spray = 3;
	static bool SilentAim = 0;
	static int  AimbotTypeSelected = 0;
	float FovCircle_Value = 250;
	float fovcircleshape = 0;
	bool RAINBOWRGB = true;
	static float SpeedValue = 10.0f;
	static bool radar = true;
	bool menu = true;
	static bool slowmo = false;
	bool SpinBot = false;
	bool set;
	bool set1;

	bool RadarESP = false;

	bool WaypointESP = true;


	bool ShotgunHead = true;
	bool SniperHead = true;

	//Player ESP
	bool Box = 0;
	bool BotESP = true;
	bool  BoxTypeSelected = 2;
	static bool Skeleton = 1;
	static bool DistanceESP = 1;
	static bool SnapLines = 0;
	static bool SnapLinestop = 0;
	static bool SnapLinesbottom = 0;

	//Loot ESP
	static bool ChestESP = 0;
	bool weakspot = true;
	bool HeliESP = false;
	bool BoardESP = false;
	bool shopping = false;
	bool mech = false;
	bool UFO = true;

	bool Downed = false;
	bool Abducted = true;
	bool botesp = false;
	bool SpeedHack = false;
	static float SpeedAmount = 10.0;




	static bool LLamaESP = 0;
	bool noblooms = false;

	bool SnaplinesVehicles = true;
	bool SnaplinesWeakSpots = true;
	bool outlinedskeleton = false;
	bool trigger_bot = false;
	bool traps = false;
	bool projectile = true;
	bool Gernades = false;
	bool limit_tp_dis = false;
	static bool WeaponESP = 1;
	bool speedhack = false;
	bool bullettp = false;
	static bool LootESP = 0;
	static bool AmmoBoxESP = 0;
	bool RiftEsp = false;
	bool supplydrops = false;
	bool snaplinesvisibleonly = false;
	bool player_tp = false;
	static bool VehiclesESP = 1;
	static bool VehicleSpeed = 0;

	bool stream_proof = false;
	bool ManhattenQC = false;

	//mobility
	bool planeesp = false;
	bool golfcartesp = false;
	bool driftboardesp = false;
	bool boatesp = true;
	bool supplydrop = false;
	bool riftesp = false;
	bool boxfill = 1;
	bool sharkesp = false;
	bool campfire = false;
	bool XPcoin = false;
	bool AirStuck = false;
	bool boattp = false;
	float maxitemtier;
	bool boxs = true;


	//Exploits
	static bool NoSpread = 0;
	bool RefreshEach1s = true;
	static bool InstantRevive = 0;
	static bool AimWhileJumping = 0;
	static bool AirStucks = 0;
	static bool RapidFire = 0;
	static float RapidValue = 1.5f;
	static float RapidFireValue = 1.501f;
	bool sprintspeed = false;
	static bool fov3604 = false;
	static bool FirstCamera = 0;
	static bool RainbowMode = 0;
	static bool Aim1 = false;
	static bool Aim2 = false;
	static bool Aim3 = false;

	//Misc

	bool SkeletonTP = true;
	float BulletTpDistance = 3;
	float MaxESPDistance = 500;
	bool WeaponESPs = false;
	bool WSesp = true;
	bool LootsESP = false;
	static bool crosshairnazi = 0;
	static bool crosshairnormal = 1;
	static bool  ShowFovCircle = 0;
	bool Boatfly = false;
	static bool trigger = 0;
	static bool VisibleCheck = 1;
	static bool godmodeon = 0;
	static bool godmodeoff = 0;
	static bool gravity = 0;
	static bool PlayerFly = 0;
	bool SniperNoClip = false;
	int EspDistance;
	bool IsAiming();
	int AimbotPrediction;
	bool saveconfig;

	//stuff
	float head_rate = 5;
	float neck_rate = 5;
	float chest_rate = 5;
	float pelvis_rate = 5;
	float aimbot_distance = 500;
	float velocity_ajust = 2;
	float aim_shake = 5;
	float shake_speed = 50;
	bool WeakSpot = true;
	bool Random = true;
	int aimkey;
	bool NoAnimation = false;

	//esp options
	bool skeletonsbehindwalls = false;
	bool skeletonsvisible = false;
	bool boxbehindwalls = false;
	bool boxvisible = false;


	bool BindRMB = true;
	bool BindLMB = false;
	bool BindSHIFT = false;
	bool BindCAPS = false;
	bool undpe = false;
	bool invisible = false;

	bool PlayerCollision = false;
	bool FreeCam = false;
	bool FlyOff = false;
	bool UnreleasedEmote = false;


	bool TESTING = false;
	bool PlayerTP = false;
	bool PlayerFlyOff = true;
	bool VehicleFly = false;

	bool BoatSpeed = false;
	bool IsLocalPlayerInVehicle = false;

	bool PlayersWeapon = true;
	bool Bushesp = true;
	float MaxTier = 5;
	bool Botweapon = true;
	bool Nicknames = true;
	bool PlayerAmmo = false;
	bool VehicleTeleport = true;
	bool PlayerReload = false;
	bool Watermark = true;
	bool ProjectileTP = false;
	bool Coeff;
	bool rock = false;
	bool tree = false;


	bool Storm = true;
	bool SHit = true;

}

namespace Draw
{

	std::string string_To_UTF8(const std::string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr(pBuf);
		delete[]pwBuf;
		delete[]pBuf;
		pwBuf = NULL;
		pBuf = NULL;
		return retStr;
	}

	void RegularRGBText(int x, int y, ImColor color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
	}
	void ShadowRGBText(int x, int y, ImColor color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 200)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 200)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
	}
	void OutlinedRBGText(int x, int y, ImColor color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 2), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 30 / 30.0)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 2), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 30 / 30.0)), utf_8_2.c_str());
		//ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
	}
}

namespace FNObjects {
	PVOID GetPlayerName;
}


namespace SettingsColor {
	float FovCircle_float[4] = { 0, 0, 0, 0 };
	ImColor FovCircle = ImColor(0, 0, 0, 0);

	

	float FovCircle_Silent[4] = { 255, 0, 0, 255 };
	ImColor SilentCircle = ImColor(255, 0, 0, 255);


	//Players ESP VISIBLE
	float Box_float[4] = { 255, 0, 0, 255 };
	ImColor Box = ImColor(255, 0, 0, 255);

	float BoxFill_float[4] = { 20, 255, 0, 255 };
	ImColor BoxFill = ImColor(20, 255, 0, 255);

	float Weapon_float[4] = { 255, 255, 255, 255 };
	ImColor Weapon = ImColor(255, 255, 255, 255);

	float Skeleton_float[4] = { 255, 0, 0, 255 };
	ImColor Skeleton = ImColor(255, 0, 0, 255);

	float Nickname_float[4] = { 245, 225, 0, 255 };
	ImColor Nickname = ImColor(245, 225, 0, 255);

	float SkeletonTP_float[4] = { 29, 250, 0, 255 };
	ImColor SkeletonTP = ImColor(29, 250, 0, 255);

	float PlayerReload_float[4] = { 245, 96, 66, 255 };
	ImColor PlayerReload = ImColor(245, 96, 66, 255);

	float Distance_float[4] = { 255, 231, 94, 255 };
	ImColor Distance = ImColor(255, 0, 94, 255);

	float Snaplines_float[4] = { 255, 0, 0, 255 };
	ImColor Snaplines = ImColor(255, 0, 0, 255);

	float Box_notvisible_float[4] = { 245, 245, 245, 255 };
	ImColor Box_notvisible = ImColor(245, 245, 245, 255);

	float BoxFill_notvisible_float[4] = { 255, 0, 0, 255 };
	ImColor BoxFill_notvisible = ImColor(255, 0, 0, 255);

	float Skeleton_notvisible_float[4] = { 255, 255, 255 };
	ImColor Skeleton_notvisible = ImColor(255, 255, 255, 255);

	float SkeletonTP_notvisible_float[4] = { 245, 245, 245 };
	ImColor SkeletonTP_notvisible = ImColor(245, 245, 245, 255);

	float Distance_notvisible_float[4] = { 255, 231, 94, 255 };
	ImColor Distance_notvisible = ImColor(255, 231, 94, 255);

	float Snaplines_notvisible_float[4] = { 255, 255, 255, 255 };
	ImColor Snaplines_notvisible = ImColor(255, 255, 255, 255);


	//Loot ESP

	float ChestESP_float[4] = { 255, 231, 94, 255 };
	ImColor ChestESP = ImColor(255, 231, 94, 255);

	float AmmoBox_float[4] = { 255, 0, 0, 255 };
	ImColor AmmoBox = ImColor(255, 0, 0, 255);

	float LootESP_float[4] = { 0, 0, 255, 255 };
	ImColor LootESP = ImColor(0, 0, 255, 255);

	float LLamaESP_float[4] = { 255, 231, 94, 255 };
	ImColor LLamaESP = ImColor(255, 231, 94, 255);

	float VehicleESP_float[4] = { 255, 231, 94, 255 };
	ImColor VehicleESP = ImColor(255, 231, 94, 255);
}




void RectFilled(int x, int y, int w, int h, ImColor color)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
}
void ShadowText(int posx, int posy, ImColor clr, const char* text)
{
	ImGui::GetOverlayDrawList()->AddText(ImVec2(posx + 1, posy + 2), ImColor(0, 0, 0, 200), text);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(posx + 1, posy + 2), ImColor(0, 0, 0, 200), text);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(posx, posy), ImColor(clr), text);
}
void Rect(int x, int y, int w, int h, ImColor color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0, thickness);
}

namespace PS
{
	void CL_ToggleButton(const char* v, bool* option, float x, float y, float x2)
	{
		ImGui::SetCursorPos({ x + x2, y });

		ImVec2 pos = ImGui::GetWindowPos();
		float height = 20;
		float width = 40;

		if (ImGui::Button(v, ImVec2{ width, height }))
			*option = !*option;

		Rect(pos.x + x + x2, pos.y + y, 40, 20, ImColor(50, 50, 50), 2);
		RectFilled(pos.x + x + x2, pos.y + y, 40, 20, ImColor(100, 100, 100, 255));
		ShadowText(pos.x + x - 5, pos.y + y, ImColor(255, 187, 0, 200), v);

		if (*option)
		{
			RectFilled(pos.x + x + x2, pos.y + y, 20, 20, ImColor(0, 200, 0, 255));
		}
		else
		{
			RectFilled(pos.x + x + 20 + x2, pos.y + y, 20, 20, ImColor(200, 0, 0, 255));
		}
	}
}