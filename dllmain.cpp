#include "SDK.h"
#include "util.h"
#include "Loot.h"
#include "detours.h"
#include "font.h"
#include "SDK.h"
#include "Defines.h"
#include "Offsets.h"
#include "xorstring.h"
#include "xor.h"


typedef int (WINAPI* LPFN_MBA)(DWORD);
static LPFN_MBA NtGetAsyncKeyState;



bool ShowMenu = true;

int Menu_AimBoneInt = 0;

static const char* AimBone_TypeItems[]{
	"   Head",
	"   Neck",
	"   Pelvis",
	"   Bottom"
};


static const char* ESP_Box_TypeItems[]{
	"   Box",
	"   Cornered",
	"   3D Box"
};

void FilledRect(int x, int y, int w, int h, ImColor color)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
}



std::string GetById(int id);
class UObject;

class FUObjectItem
{
public:
	UObject* Object;
	int32_t Flags;
	int32_t ClusterIndex;
	int32_t SerialNumber;
	char padding[0x4];

	enum class ObjectFlags : int32_t
	{
		None = 0,
		Native = 1 << 25,
		Async = 1 << 26,
		AsyncLoading = 1 << 27,
		Unreachable = 1 << 28,
		PendingKill = 1 << 29,
		RootSet = 1 << 30,
		NoStrongReference = 1 << 31
	};

	inline bool IsUnreachable() const
	{
		return !!(Flags & static_cast<std::underlying_type_t<ObjectFlags>>(ObjectFlags::Unreachable));
	}
	inline bool IsPendingKill() const
	{
		return !!(Flags & static_cast<std::underlying_type_t<ObjectFlags>>(ObjectFlags::PendingKill));
	}
};

class PreFUObjectItem
{
public:
	FUObjectItem* FUObject[10];
};

class TUObjectArray
{
public:
	inline void NumChunks(int* start, int* end) const
	{
		int cStart = 0, cEnd = 0;

		if (!cEnd)
		{
			//find where chunks start
			while (1)
			{
				if (Objects->FUObject[cStart] == 0)
				{
					cStart++;
				}
				else
				{
					break;
				}
			}

			cEnd = cStart;
			//find where chunks end
			while (1)
			{
				if (Objects->FUObject[cEnd] == 0)
				{
					break;
				}
				else
				{
					cEnd++;
				}
			}
		}

		*start = cStart;
		*end = cEnd;
	}

	inline UObject* GetByIndex(int32_t index) const
	{
		int cStart = 0, cEnd = 0;
		int chunkIndex = 0, chunkSize = 0xFFFF, chunkPos;
		FUObjectItem* Object;

		NumChunks(&cStart, &cEnd);

		chunkIndex = index / chunkSize;
		//this is so it stays in the previous chunk when the sizes are the same
		if (chunkSize * chunkIndex != 0 &&
			chunkSize * chunkIndex == index)
		{
			chunkIndex--;
		}

		chunkPos = cStart + chunkIndex;
		if (chunkPos < cEnd)
		{
			Object = Objects->FUObject[chunkPos] + (index - chunkSize * chunkIndex);
			if (!Object) { return NULL; }

			return Object->Object;
		}

		return nullptr;
	}

	inline FUObjectItem* GetItemByIndex(int32_t index) const
	{
		int cStart = 0, cEnd = 0;
		int chunkIndex = 0, chunkSize = 0xFFFF, chunkPos;
		FUObjectItem* Object;

		NumChunks(&cStart, &cEnd);

		chunkIndex = index / chunkSize;
		//this is so it stays in the previous chunk when the sizes are the same
		if (chunkSize * chunkIndex != 0 &&
			chunkSize * chunkIndex == index)
		{
			chunkIndex--;
		}

		chunkPos = cStart + chunkIndex;
		if (chunkPos < cEnd)
		{
			Object = Objects->FUObject[chunkPos] + (index - chunkSize * chunkIndex);
			if (!Object) { return NULL; }

			return Object;
		}

		return nullptr;
	}

	inline int32_t Num() const
	{
		return NumElements;
	}

private:
	PreFUObjectItem* Objects;
	char padding[8];
	int32_t MaxElements;
	int32_t NumElements;
};

class FUObjectArray
{
public:
	//int32_t ObjFirstGCIndex;
	//int32_t ObjLastNonGCIndex;
	//int32_t MaxObjectsNotConsideredByGC;
	//int32_t OpenForDisregardForGC;
	TUObjectArray ObjObjects;
};

class UObject
{
public:
	void** Vtable;                                                   // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	int32_t                                            ObjectFlags;                                              // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	int32_t                                            InternalIndex;                                            // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	class UClass* Class;                                                    // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY

	class UObject* Outer;                                                    // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY

	static inline TUObjectArray GetGlobalObjects()
	{
		static FUObjectArray* GObjects = NULL;

		if (!GObjects)
			GObjects = (FUObjectArray*)((DWORD64)GetModuleHandleW(NULL) + 0x9A39CC0);

		return GObjects->ObjObjects;
	}

	std::string GetName() const;

	std::string GetFullName() const;

	template<typename T>
	static T* FindObject(const std::string& name)
	{
		return nullptr;
	}

	static UClass* FindClass(const std::string& name)
	{
		return FindObject<UClass>(name);
	}

	template<typename T>
	static T* GetObjectCasted(std::size_t index)
	{
		return static_cast<T*>(GetGlobalObjects().GetByIndex(index));
	}


	static UClass* StaticClass()
	{
		static UClass* ptr = NULL;
		if (!ptr)
			ptr = UObject::FindClass(xorstr("Class CoreUObject.Object"));

		return ptr;
	}

};

typedef struct {
	float X, Y, Z;
} FVector;

class FText {
private:
	char _padding_[0x28];
	PWCHAR Name;
	DWORD Length;

public:
	inline PWCHAR c_str() {
		return Name;
	}
};

class UField : public UObject
{
public:
	class UField* Next;                                                     // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY

	static UClass* StaticClass()
	{
		static UClass* ptr = NULL;
		if (!ptr)
			ptr = UObject::FindClass(xorstr("Class CoreUObject.Field"));

		return ptr;
	}

};



class UStruct : public UField
{
public:
	char                                               pad_0030[0x10];                                           // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	class UStruct* SuperField;                                               // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	class UField* Children;                                                 // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	void* ChildProperties;                                          // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	int32_t                                            PropertySize;                                             // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	int32_t                                            MinAlignment;                                             // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	char                                               pad_0060[0x50];                                           // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY

	static UClass* StaticClass()
	{
		static UClass* ptr = NULL;
		if (!ptr)
			ptr = UObject::FindClass(xorstr("Class CoreUObject.Struct"));

		return ptr;
	}

};



class UFunction : public UStruct
{
public:
	int32_t FunctionFlags; //0x0088
	int16_t RepOffset; //0x008C
	int8_t NumParms; //0x008E
	char pad_0x008F[0x1]; //0x008F
	int16_t ParmsSize; //0x0090
	int16_t ReturnValueOffset; //0x0092
	int16_t RPCId; //0x0094
	int16_t RPCResponseId; //0x0096
	class UProperty* FirstPropertyToInit; //0x0098
	UFunction* EventGraphFunction; //0x00A0
	int32_t EventGraphCallOffset; //0x00A8
	char pad_0x00AC[0x4]; //0x00AC
	void* Func; //0x00B0
};

void GetVFunction()
{

}

void ProcessEvent(UObject* obj, class UFunction* function, void* parms)
{
	if (!function)
		return;
	auto func = GetVFunction;


}

template<class T>
struct TArray {
	friend struct FString;

public:
	inline TArray() {
		Data = nullptr;
		Count = Max = 0;
	};

	inline INT Num() const {
		return Count;
	};

	inline T& operator[](INT i) {
		return Data[i];
	};

	inline BOOLEAN IsValidIndex(INT i) {
		return i < Num();
	}

private:
	T* Data;
	INT Count;
	INT Max;
};

struct FString : private TArray<WCHAR> {
	FString() {
		Data = nullptr;
		Max = Count = 0;
	}

	FString(LPCWSTR other) {
		Max = Count = static_cast<INT>(wcslen(other));

		if (Count) {
			Data = const_cast<PWCHAR>(other);
		}
	};

	inline BOOLEAN IsValid() {
		return Data != nullptr;
	}

	inline PWCHAR c_str() {
		return Data;
	}
};

struct APlayerController_FOV_Params
{
	float                                              NewFOV;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
};

void fov(UObject* _this, float NewFOV)
{
	static UFunction* fn = NULL;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.PlayerController.FOV");

	APlayerController_FOV_Params params;
	params.NewFOV = NewFOV;

	auto flags = fn->FunctionFlags;
	fn->FunctionFlags |= 0x400;

	ProcessEvent(_this, fn, &params);

	fn->FunctionFlags = flags;
}




#pragma comment(lib, "detours.lib")



// Dont forget to call this at the start of the cheat or it wont work!






// Watermark and Speed




// Dont forget to call this at the start of the cheat or it wont work!

void gaybow(ImGuiWindow& window) {
	
	if (Settings::SilentAim) {
		
	}
	//DrawWaterMark(window, "Second text", ImVec2(50, 120), FpsColor, false);


}

void RadarRange(float* x, float* y, float range)
{
	if (fabs((*x)) > range || fabs((*y)) > range)
	{
		if ((*y) > (*x))
		{
			if ((*y) > -(*x))
			{
				(*x) = range * (*x) / (*y);
				(*y) = range;
			}
			else
			{
				(*y) = -range * (*y) / (*x);
				(*x) = -range;
			}
		}
		else
		{
			if ((*y) > -(*x))
			{
				(*y) = range * (*y) / (*x);
				(*x) = range;
			}
			else
			{
				(*x) = -range * (*x) / (*y);
				(*y) = -range;
			}
		}
	}
}









void CalcRadarPoint(SDK::Structs::FVector vOrigin, int& screenx, int& screeny)
{
	
	auto fYaw = 0;
	float dx = vOrigin.X;
	float dy = vOrigin.Y;

	float fsin_yaw = sinf(fYaw);
	float fminus_cos_yaw = -cosf(fYaw);

	float x = dy * fminus_cos_yaw + dx * fsin_yaw;
	x = -x;
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;

	float range = (float)15.f;

	RadarRange(&x, &y, range);

	ImVec2 DrawPos = ImGui::GetCursorScreenPos();
	ImVec2 DrawSize = ImGui::GetContentRegionAvail();

	int rad_x = (int)DrawPos.x;
	int rad_y = (int)DrawPos.y;

	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;

	int x_max = (int)r_siz_x + rad_x - 5;
	int y_max = (int)r_siz_y + rad_y - 5;

	screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
	screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));

	if (screenx > x_max)
		screenx = x_max;

	if (screenx < rad_x)
		screenx = rad_x;

	if (screeny > y_max)
		screeny = y_max;

	if (screeny < rad_y)
		screeny = rad_y;
}

std::string UTF8(const std::string& str)
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

void ShadowRGBTexts(int x, int y, ImColor color, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 200)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 200)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
}



SDK::Structs::FVector* GetPawnRootLocations(uintptr_t pawn) {
	auto root = 0;
	if (!root) {
		return nullptr;
	}

}

void renderRadar(uintptr_t CurrentActor, ImColor PlayerPointColor) {

	auto player = CurrentActor;

	int screenx = 0;
	int screeny = 0;

	SDK::Structs::FVector pos = *GetPawnRootLocations(CurrentActor);

	CalcRadarPoint(pos, screenx, screeny);

	ImDrawList* Draw = ImGui::GetOverlayDrawList();

	SDK::Structs::FVector viewPoint = { 0 };
	Draw->AddRectFilled(ImVec2((float)screenx, (float)screeny), ImVec2((float)screenx + 5, (float)screeny + 5), ImColor(PlayerPointColor));
}

bool firstS = false;
uintptr_t PlayerCameraManager;

void RadarLoop(uintptr_t CurrentActor, ImColor PlayerPointColor)
{
	ImGuiWindowFlags TargetFlags;
	if (Settings::menu)
		TargetFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	else
		TargetFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	float radarWidth = 200;
	float PosDx = 1200;
	float PosDy = 60;

	if (!firstS) {
		ImGui::SetNextWindowPos(ImVec2{ 1200, 60 }, ImGuiCond_Once);
		firstS = true;
	}

	auto* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.ChildRounding = 0.0f;
	style.FrameRounding = 0.0f;


	if (ImGui::Begin("black", 0, ImVec2(250, 255), -1.f, TargetFlags)) {

		ImDrawList* Draw = ImGui::GetOverlayDrawList();
		ImVec2 DrawPos = ImGui::GetCursorScreenPos();
		ImVec2 DrawSize = ImGui::GetContentRegionAvail();

		ImVec2 midRadar = ImVec2(DrawPos.x + (DrawSize.x / 2), DrawPos.y + (DrawSize.y / 2));
		ImGui::GetWindowDrawList()->AddLine(ImVec2(midRadar.x - DrawSize.x / 2.f, midRadar.y), ImVec2(midRadar.x + DrawSize.x / 2.f, midRadar.y), ImColor(95, 95, 95));
		ImGui::GetWindowDrawList()->AddLine(ImVec2(midRadar.x, midRadar.y - DrawSize.y / 2.f), ImVec2(midRadar.x, midRadar.y + DrawSize.y / 2.f), ImColor(95, 95, 95));

		//if (valid_pointer(PlayerController) && valid_pointer(PlayerCameraManager);
	}
	ImGui::End();
}




PVOID TargetPawn = nullptr;

namespace HookFunctions {
	bool Init(bool NoSpread, bool CalcShot);
	bool NoSpreadInitialized = false;
	bool CalcShotInitialized = false;
}


ID3D11Device* device = nullptr;
ID3D11DeviceContext* immediateContext = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;



WNDPROC oWndProc = NULL;


typedef HRESULT(*present_fn)(IDXGISwapChain*, UINT, UINT);
inline present_fn present_original{ };

typedef HRESULT(*resize_fn)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
inline resize_fn resize_original{ };



//HRESULT(*present_original)(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) = nullptr;
//HRESULT(*resize_original)(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) = nullptr;



extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Draw2DBoundingBox(Vector3 StartBoxLoc, float flWidth, float Height, ImColor color)
{
	StartBoxLoc.x = StartBoxLoc.x - (flWidth / 2);
	ImDrawList* Renderer = ImGui::GetOverlayDrawList();
	Renderer->AddLine(ImVec2(StartBoxLoc.x, StartBoxLoc.y), ImVec2(StartBoxLoc.x + flWidth, StartBoxLoc.y), color, 1); //bottom
	Renderer->AddLine(ImVec2(StartBoxLoc.x, StartBoxLoc.y), ImVec2(StartBoxLoc.x, StartBoxLoc.y + Height), color, 1); //left
	Renderer->AddLine(ImVec2(StartBoxLoc.x + flWidth, StartBoxLoc.y), ImVec2(StartBoxLoc.x + flWidth, StartBoxLoc.y + Height), color, 1); //right
	Renderer->AddLine(ImVec2(StartBoxLoc.x, StartBoxLoc.y + Height), ImVec2(StartBoxLoc.x + flWidth, StartBoxLoc.y + Height), color, 1); //up
}

void DrawCorneredBox(int X, int Y, int W, int H, ImColor color, int thickness) {
	float lineW = (W / 3);
	float lineH = (H / 3);
	ImDrawList* Renderer = ImGui::GetOverlayDrawList();
	Renderer->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), color, thickness);

	Renderer->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), color, thickness);

	Renderer->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), color, thickness);

	Renderer->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), color, thickness);

	Renderer->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), color, thickness);

	Renderer->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), color, thickness);

	Renderer->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), color, thickness);

	Renderer->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), color, thickness);

}



auto GetSyscallIndex(std::string ModuleName, std::string SyscallFunctionName, void* Function) -> bool
{
	auto ModuleBaseAddress = LI_FN(GetModuleHandleA)(ModuleName.c_str());
	if (!ModuleBaseAddress)
		ModuleBaseAddress = LI_FN(LoadLibraryA)(ModuleName.c_str());
	if (!ModuleBaseAddress)
		return false;

	auto GetFunctionAddress = LI_FN(GetProcAddress)(ModuleBaseAddress, SyscallFunctionName.c_str());
	if (!GetFunctionAddress)
		return false;

	auto SyscallIndex = *(DWORD*)((PBYTE)GetFunctionAddress + 4);

	*(DWORD*)((PBYTE)Function + 4) = SyscallIndex;

	return true;
}

VOID ToMatrixWithScale(float* in, float out[4][4]) {
	auto* rotation = &in[0];
	auto* translation = &in[4];
	auto* scale = &in[8];

	out[3][0] = translation[0];
	out[3][1] = translation[1];
	out[3][2] = translation[2];

	auto x2 = rotation[0] + rotation[0];
	auto y2 = rotation[1] + rotation[1];
	auto z2 = rotation[2] + rotation[2];

	auto xx2 = rotation[0] * x2;
	auto yy2 = rotation[1] * y2;
	auto zz2 = rotation[2] * z2;
	out[0][0] = (1.0f - (yy2 + zz2)) * scale[0];
	out[1][1] = (1.0f - (xx2 + zz2)) * scale[1];
	out[2][2] = (1.0f - (xx2 + yy2)) * scale[2];

	auto yz2 = rotation[1] * z2;
	auto wx2 = rotation[3] * x2;
	out[2][1] = (yz2 - wx2) * scale[2];
	out[1][2] = (yz2 + wx2) * scale[1];

	auto xy2 = rotation[0] * y2;
	auto wz2 = rotation[3] * z2;
	out[1][0] = (xy2 - wz2) * scale[1];
	out[0][1] = (xy2 + wz2) * scale[0];

	auto xz2 = rotation[0] * z2;
	auto wy2 = rotation[3] * y2;
	out[2][0] = (xz2 + wy2) * scale[2];
	out[0][2] = (xz2 - wy2) * scale[0];

	out[0][3] = 0.0f;
	out[1][3] = 0.0f;
	out[2][3] = 0.0f;
	out[3][3] = 1.0f;
}

VOID MultiplyMatrices(float a[4][4], float b[4][4], float out[4][4]) {
	for (auto r = 0; r < 4; ++r) {
		for (auto c = 0; c < 4; ++c) {
			auto sum = 0.0f;

			for (auto i = 0; i < 4; ++i) {
				sum += a[r][i] * b[i][c];
			}

			out[r][c] = sum;
		}
	}
}

VOID GetBoneLocation(float compMatrix[4][4], PVOID bones, DWORD index, float out[3]) {
	float boneMatrix[4][4];
	ToMatrixWithScale((float*)((PBYTE)bones + (index * 0x30)), boneMatrix);

	float result[4][4];
	MultiplyMatrices(boneMatrix, compMatrix, result);

	out[0] = result[3][0];
	out[1] = result[3][1];
	out[2] = result[3][2];
}

extern "C"
{
	NTSTATUS _NtUserSendInput(UINT a1, LPINPUT Input, int Size);
};

VOID mouse_event_(DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, ULONG_PTR dwExtraInfo)
{
	static bool doneonce;
	if (!doneonce)
	{
		if (!GetSyscallIndex(xorstr("win32u.dll"), xorstr("NtUserSendInput"), _NtUserSendInput))
			return;
		doneonce = true;
	}

	INPUT Input[3] = { 0 };
	Input[0].type = INPUT_MOUSE;
	Input[0].mi.dx = dx;
	Input[0].mi.dy = dy;
	Input[0].mi.mouseData = dwData;
	Input[0].mi.dwFlags = dwFlags;
	Input[0].mi.time = 0;
	Input[0].mi.dwExtraInfo = dwExtraInfo;

	_NtUserSendInput((UINT)1, (LPINPUT)&Input, (INT)sizeof(INPUT));
}


Vector3 HandGun, head2, neck, pelvis, chest, leftShoulder, rightShoulder, leftElbow, rightElbow, leftHand, rightHand, leftLeg, rightLeg, leftThigh, rightThigh, leftFoot, rightFoot, leftFeet, rightFeet, leftFeetFinger, rightFeetFinger, Toes;

bool GetAllBones(uintptr_t CurrentActor) {
	Vector3 chesti, chestatright;

	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 98, &head2);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 66, &neck);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 2, &pelvis);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 37, &chesti);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 8, &chestatright);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 93, &leftShoulder);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 9, &rightShoulder);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 94, &leftElbow);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 10, &rightElbow);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 100, &leftHand);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 99, &rightHand);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 76, &leftLeg);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 69, &rightLeg);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 82, &leftThigh);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 75, &rightThigh);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 78, &leftFoot);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 71, &rightFoot);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 85, &leftFeet);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 84, &rightFeet);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 81, &leftFeetFinger);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 74, &rightFeetFinger);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 0, &Toes);


	SDK::Classes::AController::WorldToScreen(head2, &head2);
	SDK::Classes::AController::WorldToScreen(neck, &neck);
	SDK::Classes::AController::WorldToScreen(pelvis, &pelvis);
	SDK::Classes::AController::WorldToScreen(chesti, &chesti);
	SDK::Classes::AController::WorldToScreen(chestatright, &chestatright);
	SDK::Classes::AController::WorldToScreen(leftShoulder, &leftShoulder);
	SDK::Classes::AController::WorldToScreen(rightShoulder, &rightShoulder);
	SDK::Classes::AController::WorldToScreen(leftElbow, &leftElbow);
	SDK::Classes::AController::WorldToScreen(rightElbow, &rightElbow);
	SDK::Classes::AController::WorldToScreen(leftHand, &leftHand);
	SDK::Classes::AController::WorldToScreen(rightHand, &rightHand);
	SDK::Classes::AController::WorldToScreen(leftLeg, &leftLeg);
	SDK::Classes::AController::WorldToScreen(rightLeg, &rightLeg);
	SDK::Classes::AController::WorldToScreen(leftThigh, &leftThigh);
	SDK::Classes::AController::WorldToScreen(rightThigh, &rightThigh);
	SDK::Classes::AController::WorldToScreen(leftFoot, &leftFoot);
	SDK::Classes::AController::WorldToScreen(rightFoot, &rightFoot);
	SDK::Classes::AController::WorldToScreen(leftFeet, &leftFeet);
	SDK::Classes::AController::WorldToScreen(rightFeet, &rightFeet);
	SDK::Classes::AController::WorldToScreen(leftFeetFinger, &leftFeetFinger);
	SDK::Classes::AController::WorldToScreen(rightFeetFinger, &rightFeetFinger);

	chest.x = chesti.x + ((chestatright.x - chesti.x) / 2);
	chest.y = chesti.y;

	return true;
}



bool EntitiyLoop()
{


	ImDrawList* Renderer = ImGui::GetOverlayDrawList();


	if (Settings::crosshairnazi)
	{
		ImVec2 top = ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2 + 10);
		ImVec2 right = ImVec2(Renderer_Defines::Width / 2 + 10, Renderer_Defines::Height / 2);
		ImVec2 bottom = ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2 - 10);
		ImVec2 left = ImVec2(Renderer_Defines::Width / 2 - 10, Renderer_Defines::Height / 2);

		Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2 - 10), ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2 + 10), ImColor(255, 0, 0, 255), 1.5);
		Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2 - 10, Renderer_Defines::Height / 2), ImVec2(Renderer_Defines::Width / 2 + 11, Renderer_Defines::Height / 2), ImColor(255, 0, 0, 255), 1.5);

		Renderer->AddLine(top, ImVec2(top.x + 9, top.y), ImColor(255, 0, 0, 255), 1.5);
		Renderer->AddLine(right, ImVec2(right.x, right.y - 9), ImColor(255, 0, 0, 255), 1.5);
		Renderer->AddLine(bottom, ImVec2(bottom.x - 9, bottom.y), ImColor(255, 0, 0, 255), 1.5);
		Renderer->AddLine(left, ImVec2(left.x, left.y + 9), ImColor(255, 0, 0, 255), 1.5);
	}

	if (Settings::crosshairnormal)
	{

		Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2 - 11, Renderer_Defines::Height / 2), ImVec2(Renderer_Defines::Width / 2 + 1, Renderer_Defines::Height / 2), ImColor(255, 0, 0, 255), 1.0);
		Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2 + 12, Renderer_Defines::Height / 2), ImVec2(Renderer_Defines::Width / 2 + 1, Renderer_Defines::Height / 2), ImColor(255, 0, 0, 255), 1.0);
		Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2 - 11), ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2), ImColor(255, 0, 0, 255), 1.0);
		Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2 + 12), ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2), ImColor(255, 0, 0, 255), 1.0);
	}

	if (Settings::fov3604)
		Renderer->AddCircle(ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2), Settings::FovCircle_Value, SettingsColor::FovCircle, 124);






	




	try
	{
		float FOVmax = 9999.f;

		float closestDistance = FLT_MAX;
		PVOID closestPawn = NULL;
		bool closestPawnVisible = true;


		uintptr_t MyTeamIndex = 0, EnemyTeamIndex = 0;
		uintptr_t GWorld = read<uintptr_t>(UWorld); if (!GWorld) return false;

		uintptr_t Gameinstance = read<uint64_t>(GWorld + StaticOffsets::OwningGameInstance); if (!Gameinstance) return false;

		uintptr_t LocalPlayers = read<uint64_t>(Gameinstance + StaticOffsets::LocalPlayers); if (!LocalPlayers) return false;

		uintptr_t LocalPlayer = read<uint64_t>(LocalPlayers); if (!LocalPlayer) return false;

		PlayerController = read<uint64_t>(LocalPlayer + StaticOffsets::PlayerController); if (!PlayerController) return false;

		uintptr_t PlayerCameraManager = read<uint64_t>(PlayerController + StaticOffsets::PlayerCameraManager); if (!PlayerCameraManager) return false;

		FOVAngle = SDK::Classes::APlayerCameraManager::GetFOVAngle(PlayerCameraManager);
		SDK::Classes::APlayerCameraManager::GetPlayerViewPoint(PlayerCameraManager, &CamLoc, &CamRot);

		LocalPawn = read<uint64_t>(PlayerController + StaticOffsets::AcknowledgedPawn);

		uintptr_t Ulevel = read<uintptr_t>(GWorld + StaticOffsets::PersistentLevel); if (!Ulevel) return false;

		uintptr_t AActors = read<uintptr_t>(Ulevel + StaticOffsets::AActors); if (!AActors) return false;

		uintptr_t ActorCount = read<int>(Ulevel + StaticOffsets::ActorCount); if (!ActorCount) return false;


		uintptr_t LocalRootComponent;
		Vector3 LocalRelativeLocation;

		if (valid_pointer(LocalPawn)) {
			LocalRootComponent = read<uintptr_t>(LocalPawn + 0x130);
			LocalRelativeLocation = read<Vector3>(LocalRootComponent + 0x11C);
		}

		for (int i = 0; i < ActorCount; i++) {

			auto CurrentActor = read<uintptr_t>(AActors + i * sizeof(uintptr_t));
			
			auto name = SDK::Classes::UObject::GetObjectName(CurrentActor);
			//MessageBoxA(0, (LPSTR)name, "1111111taped", 0);

			bool IsVisible = false;

			if (valid_pointer(LocalPawn))
			{


				if (Settings::supplydrop && strstr(name, xorstr("AthenaSupplyDrop_C")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) //strstr(name, xorstr("Valet_Taxi")) || strstr(name, xorstr("Valet_BigRig")) || strstr(name, xorstr("Valet_BasicTr")) || strstr(name, xorstr("Valet_SportsC")) || strstr(name, xorstr("Valet_BasicC")))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("Supply Drop [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						Renderer->AddText(ImVec2(VehiclePosition.x, VehiclePosition.y), ImColor(52, 235, 67, 255), Text.c_str());
					}

				}

				if (Settings::WeaponESP && strstr(name, xorstr("FortPickupAthena")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {


					auto item = ReadPointer(CurrentActor, StaticOffsets::PrimaryPickupItemEntry + StaticOffsets::ItemDefinition);
					if (!item) continue;

					auto itemName = reinterpret_cast<FText*>(ReadPointer(item, StaticOffsets::DisplayName));
					if (!itemName || !itemName->c_str()) continue;

					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr(" ");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						Renderer->AddText(ImVec2(VehiclePosition.x, VehiclePosition.y), ImColor(0, 255, 17, 255), Text.c_str());
					}
				}

				if (Settings::traps && strstr(name, xorstr("BuildingTrap")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {


					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("[") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						Renderer->AddText(ImVec2(VehiclePosition.x, VehiclePosition.y), ImColor(255, 0, 0, 255), Text.c_str());
					}
				}

				if (Settings::riftesp && strstr(name, xorstr("FortAthenaRiftPortal")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {


					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr(" [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						Renderer->AddText(ImVec2(VehiclePosition.x, VehiclePosition.y), ImColor(42, 120, 245, 255), Text.c_str());
					}
				}

				if (Settings::WaypointESP && strstr(name, xorstr("AthenaPlayerMarker")))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {

						Vector3 VehiclePosition;

						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("WP \n\n [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, ImColor(255, 0, 255, 255), Text.c_str());
					}

				}
				//float distance = LocalRelativeLocation.Distance(HeadNotW2SForDistance) / 100.f;
				auto dsit = distance;

				if (Settings::botesp && strstr(name, xorstr("PlayerPawn_Athena_Phoebe_C")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 300.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 BotPosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &BotPosition);

						std::string null = xorstr("");
						std::string Text = null + xorstr("Bot/Ai");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


						ShadowRGBTexts(head2.x - TextSize.x - 5, head2.y - TextSize.y - 30, SettingsColor::Box, Text.c_str());
					}

				}

				if (Settings::Skeleton && strstr(name, xorstr("B_Prj_Bullet_Sniper")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

				uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
				Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
				float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 300.f;

					if (ItemDist < Settings::MaxESPDistance) {
					Vector3 BotPosition;
					SDK::Classes::AController::WorldToScreen(ItemPosition, &BotPosition);

						std::string null = xorstr("");
						std::string Text = null + xorstr("Projectile");

					ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


					Renderer->AddText(ImVec2(BotPosition.x - TextSize.x / 2, BotPosition.y - TextSize.y / 2), SettingsColor::AmmoBox, Text.c_str());

					}

			}

				if (Settings::Skeleton && strstr(name, xorstr("OnRep_FireStart")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 300.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 BotPosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &BotPosition);

						std::string null = xorstr("");
						std::string Text = null + xorstr("Projectile");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


						Renderer->AddText(ImVec2(BotPosition.x - TextSize.x / 2, BotPosition.y - TextSize.y / 2), SettingsColor::AmmoBox, Text.c_str());

					}

				}






				if (Settings::SnaplinesWeakSpots && strstr(name, xorstr("WeakSpot_C")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 300.f;



					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 BotPosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &BotPosition);

						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Snaplines;
						}
						else {
							col = SettingsColor::Snaplines_notvisible;
						}

						Renderer->AddLine(ImVec2(BotPosition.x, BotPosition.y), ImVec2(BotPosition.x, BotPosition.y), col, 0.3);
					}
				}

				if (Settings::Random && strstr(name, xorstr("BP_MangPlayerPawn")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 300.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 BotPosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &BotPosition);




						Renderer - (ImVec2(head2.x / 2, head2.y), SettingsColor::AmmoBox);

					}

				}


				if (Settings::Downed && strstr(name, xorstr("TeleportationDrone_C")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 300.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 BotPosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &BotPosition);

						std::string null = xorstr("");
						std::string Text = null + xorstr("Man Downed!");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


						Renderer->AddText(ImVec2(pelvis.x - TextSize.x / 2, pelvis.y - TextSize.y / 2), SettingsColor::SilentCircle, Text.c_str());

					}

				}

				if (Settings::fishinghole && strstr(name, xorstr("BGA_Athena_FlopperSpawn_C")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 BotPosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &BotPosition);

						std::string null = xorstr("");
						std::string Text = null + xorstr("");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


						Renderer->AddText(ImVec2(BotPosition.x - TextSize.x / 2, BotPosition.y - TextSize.y / 2), SettingsColor::AmmoBox, Text.c_str());

					}

				}


				if (Settings::WSesp && strstr(name, xorstr("WeakSpot_C")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 BotPosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &BotPosition);


						std::string null = xorstr("");
						std::string Text = null + xorstr("WS");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


						ShadowRGBTexts(BotPosition.x - TextSize.x / 2, BotPosition.y - TextSize.y / 2, SettingsColor::AmmoBox, Text.c_str());

						Vector3 HeadPos, Headbox, bottom;
						if (!Settings::fov3604) {
							auto dx = Headbox.x - (Renderer_Defines::Width / 2);
							auto dy = Headbox.y - (Renderer_Defines::Height / 2);
							auto dist = SpoofRuntime::sqrtf_(dx * dx + dy * dy);

							if (dist < Settings::FovCircle_Value && dist < closestDistance) {
								closestDistance = dist;
								closestPawn = (PVOID)CurrentActor;
							}
						}
						else {
							closestPawn = (PVOID)CurrentActor;
						}


					}
					else if (Settings::SilentAim and !IsVisible) {
						closestPawn = nullptr;
					}

					else if (Settings::MagicBullets and !IsVisible) {
						closestPawn = nullptr;
					}
				}













				if (Settings::ChestESP && strstr(name, xorstr("Tiered_Chest")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{

					if (NtGetAsyncKeyState(VK_SHIFT)) {

						uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
						Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
						float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

						if (ItemDist < Settings::MaxESPDistance) {
							Vector3 BotPosition;
							SDK::Classes::AController::WorldToScreen(ItemPosition, &BotPosition);

							std::string null = xorstr("");
							std::string Text = null + xorstr("") + std::to_string((int)ItemDist) + xorstr("m]");

							ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


							Renderer->AddText(ImVec2(BotPosition.x - TextSize.x / 2, BotPosition.y - TextSize.y / 2), SettingsColor::ChestESP, Text.c_str());

						}

					}
				}

				if (Settings::AmmoBoxESP && strstr(name, xorstr("Tiered_Ammo")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{

					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 BotPosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &BotPosition);

						std::string null = xorstr("");
						std::string Text = null + xorstr("Ammo[") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


						Renderer->AddText(ImVec2(BotPosition.x - TextSize.x / 2, BotPosition.y - TextSize.y / 2), SettingsColor::AmmoBox, Text.c_str());

					}

				}

			

				if (Settings::BoardESP && strstr(name, xorstr("JackalVehicle_Athena_C")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("DriftBoard [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, ImColor(255, 255, 0, 255), Text.c_str());
					}

				}

				if (Settings::HeliESP && strstr(name, xorstr("HoagieVehicle_C")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("Helicopter [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, ImColor(255, 0, 255, 255), Text.c_str());
					}

				}

				if (Settings::VehiclesESP && strstr(name, xorstr("Valet_BasicC")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("Sedan Car [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, ImColor(255, 255, 0, 255), Text.c_str());
					}

				}

				if (Settings::VehiclesESP && strstr(name, xorstr("Valet_SportsC")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("Lambo Car [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, ImColor(255, 255, 0, 255), Text.c_str());
					}

				}


				if (Settings::VehiclesESP && strstr(name, xorstr("Valet_Taxi")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("Taxi Car [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, ImColor(255, 255, 0, 255), Text.c_str());
					}

				}

				if (Settings::VehiclesESP && strstr(name, xorstr("Valet_BigRig")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("Big Truck [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, ImColor(255, 255, 0, 255), Text.c_str());
					}

				}

				if (Settings::VehiclesESP && strstr(name, xorstr("Valet_BasicTr")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("Truck [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, ImColor(0, 255, 255, 255), Text.c_str());
					}

				}

				if (Settings::boatesp && strstr(name, xorstr("MeatballVehicle_L")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("Boat [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, ImColor(0, 255, 255, 255), Text.c_str());
					}

				}

				if (Settings::PlaneESP && strstr(name, xorstr("FerretVehicle_C")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {

						Vector3 VehiclePosition;

						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("Plane [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, ImColor(255, 0, 255, 255), Text.c_str());
					}

				}






				if (Settings::UfoESP && strstr(name, xorstr("Nevada_Vehicle_V2_C")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {

						Vector3 VehiclePosition;

						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("UFO [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, ImColor(255, 0, 255, 255), Text.c_str());
					}

				}

				if (Settings::Bushesp && strstr(name, xorstr("Bush")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {

						Vector3 VehiclePosition;

						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("Bush");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, ImColor(255, 255, 255, 255), Text.c_str());
					}

				}

				if (Settings::riftesp && strstr(name, xorstr("FortAthenaRiftPortal")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr(" [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(VehiclePosition.x, VehiclePosition.y, SettingsColor::Nickname, Text.c_str());
					}

				}


				else if (Settings::LLamaESP && strstr(name, xorstr("AthenaSupplyDrop_Llama")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 LLamaPosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &LLamaPosition);

						std::string null = xorstr("");
						std::string Text = null + xorstr("LLama [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						
						ShadowRGBTexts(LLamaPosition.x, LLamaPosition.y, SettingsColor::LLamaESP, Text.c_str());
					}
				}

				else if (Settings::Storm && strstr(name, xorstr("BP_SafeZoneStorm"))) {
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 LLamaPosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &LLamaPosition);

						std::string null = xorstr("");
						std::string Text = null + xorstr("Safe-Zone \n   [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


						ShadowRGBTexts(LLamaPosition.x, LLamaPosition.y, SettingsColor::LLamaESP, Text.c_str());
					}
				}

				else if (Settings::LLamaESP && strstr(name, xorstr("Tiered_Chest")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 LLamaPosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &LLamaPosition);

						std::string null = xorstr("");
						std::string Text = null + xorstr("Chest \n") + std::to_string((int)ItemDist) + xorstr("");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(LLamaPosition.x, LLamaPosition.y, SettingsColor::Nickname, Text.c_str());
					}
				}

				else if (strstr(name, xorstr("PlayerPawn"))) {
					Vector3 HeadPos, Headbox, bottom;

					SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 98, &HeadPos);
					SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 0, &bottom);

					SDK::Classes::AController::WorldToScreen(Vector3(HeadPos.x, HeadPos.y, HeadPos.z + 20), &Headbox);
					SDK::Classes::AController::WorldToScreen(bottom, &bottom);

					if (Headbox.x == 0 && Headbox.y == 0) continue;
					if (bottom.x == 0 && bottom.y == 0) continue;






					uintptr_t MyState = read<uintptr_t>(LocalPawn + StaticOffsets::PlayerState);
					if (!MyState) continue;

					MyTeamIndex = read<uintptr_t>(MyState + StaticOffsets::TeamIndex);
					if (!MyTeamIndex) continue;

					uintptr_t EnemyState = read<uintptr_t>(CurrentActor + StaticOffsets::PlayerState);
					if (!EnemyState) continue;

					EnemyTeamIndex = read<uintptr_t>(EnemyState + StaticOffsets::TeamIndex);
					if (!EnemyTeamIndex) continue;

					if (CurrentActor == LocalPawn) continue;

					Vector3 viewPoint;

					if (Settings::VisibleCheck) {
						IsVisible = SDK::Classes::APlayerCameraManager::LineOfSightTo((PVOID)PlayerController, (PVOID)CurrentActor, &viewPoint);
					}

					if (Settings::botesp && strstr(name, xorstr("PlayerPawn_Athena_Phoebe_C")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

						uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
						Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
						float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 300.f;

						if (ItemDist < Settings::MaxESPDistance) {
							Vector3 BotPosition;
							SDK::Classes::AController::WorldToScreen(ItemPosition, &BotPosition);

							std::string null = xorstr("");
							std::string Text = null + xorstr("Bot/Ai");

							ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


							ShadowRGBTexts(head2.x - TextSize.x - 5, head2.y - TextSize.y - 30, SettingsColor::Box, Text.c_str());

						}

					}

						

					

					if (Settings::SnapLines) {
						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Snaplines;
						}
						else {
							col = SettingsColor::Snaplines_notvisible;
						}
						Vector3 LocalPelvis;
						SDK::Classes::USkeletalMeshComponent::GetBoneLocation(LocalPawn, 2, &LocalPelvis);
						SDK::Classes::AController::WorldToScreen(LocalPelvis, &LocalPelvis);

						Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2), ImVec2(bottom.x, bottom.y), col, 1.f);
					}

					if (Settings::SnapLinesbottom) {
						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Snaplines;
						}
						else {
							col = SettingsColor::Snaplines_notvisible;
						}
						Vector3 LocalPelvis;
						SDK::Classes::USkeletalMeshComponent::GetBoneLocation(LocalPawn, 2, &LocalPelvis);
						SDK::Classes::AController::WorldToScreen(LocalPelvis, &LocalPelvis);

						Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 1), ImVec2(bottom.x, bottom.y), col, 1.f);
					}

					if (Settings::SnapLinestop) {
						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Snaplines;
						}
						else {
							col = SettingsColor::Snaplines_notvisible;
						}
						Vector3 LocalPelvis;
						SDK::Classes::USkeletalMeshComponent::GetBoneLocation(LocalPawn, 2, &LocalPelvis);
						SDK::Classes::AController::WorldToScreen(LocalPelvis, &LocalPelvis);

						Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 13), ImVec2(bottom.x, bottom.y), col, 1.f);
					}

					

					/*if (Settings::DistanceESP && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {
						Vector3 HeadNotW2SForDistance;
						SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 98, &HeadNotW2SForDistance);
						float distance = LocalRelativeLocation.Distance(HeadNotW2SForDistance) / 100.f;

						std::string null = "";
						std::string finalstring = null + xorstr(" [") + std::to_string((int)distance) + xorstr("m]");

						ImVec2 DistanceTextSize = ImGui::CalcTextSize(finalstring.c_str());

						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Box;
						}
						else {
							col = SettingsColor::Box_notvisible;
						}

						Renderer->AddText(ImVec2(bottom.x - DistanceTextSize.x / 2, bottom.y + DistanceTextSize.y / 2), col, finalstring.c_str());

					}*/

				

					if (Settings::NoSpread) {
						if (!HookFunctions::NoSpreadInitialized) {
							HookFunctions::Init(true, false);
						}
					}

					if (Settings::AimWhileJumping) {
						*(bool*)(LocalPawn + StaticOffsets::bADSWhileNotOnGround) = true;
					}
					else {
						*(bool*)(LocalPawn + StaticOffsets::bADSWhileNotOnGround) = false;
					}

					if (Settings::godmodeon) {
						write<float>(LocalPawn + 0x58E, 0.05f); //bAllowBuildingActorTeleport::bIsDBNO 0x57E
						Settings::godmodeon = false;
					}

					if (Settings::godmodeoff) {
						write<float>(LocalPawn + 0x58E, 1.00f); //bAllowBuildingActorTeleport::bIsDBNO 0x57E
						Settings::godmodeoff = false;
					}

					if (Settings::PlayerFly)
					{
						if (GetAsyncKeyState(VK_SHIFT))
						{
							write<float>(LocalPawn + 0x13FD, 0.05f); //bIsSkydiving 
						}
						else {
							write<float>(LocalPawn + 0x13FD, 1.00f); //bIsSkydiving 
						}
					}

					if (Settings::SHit && (GetAsyncKeyState(VK_SHIFT)))
					{
						write<float>(LocalPawn + 0xD51, 0.05f); //bIsSkydiving 
					}
					else {
						write<float>(LocalPawn + 0xD51, 1.00f); //bIsSkydiving 
					}

					if (Settings::UnreleasedEmote) {
						write<float>(LocalPawn + 0x6C0, 0.05f); //AnimationData
						if (GetAsyncKeyState(VK_CAPITAL))
						{
							write<float>(LocalPawn + 0x6C0, 1.00f); //AnimationData
						}
						//CrouchFloat 0x518
					}

#include "SDK.h"


					
					bool wasClicked = false;

					

					if (Settings::TESTING)
					{
						//write<float>(LocalPawn + 0x142F, 0.05f); //bIsSkydivingFromLaunchPad
					}

					if (Settings::VehicleFly) {
						write<float>(LocalPawn + 0x1D8, 0.05f); //bDisableCollision
						if (GetAsyncKeyState(VK_SHIFT))
						{
							write<float>(LocalPawn + 0x1D8, 1.00f); //bIsSkydivingFromLaunchPad
						}
					}

					bool InstantRevive_AlreadyDisabled = false;

					if (Settings::InstantRevive) {
						*(float*)(PBYTE)(LocalPawn + 0x3580) = 0.101f;
						InstantRevive_AlreadyDisabled = false;
					}
					else {
						if (!InstantRevive_AlreadyDisabled) {
							*(float*)(PBYTE)(LocalPawn + 0x3580) = 10.0f;
							InstantRevive_AlreadyDisabled = true;
						}
					}

					/*if (Settings::nocooldown) {
						uintptr_t CurrentWeapon = uintptr_t(LocalPawn + 0x5E8); //CurrentWeapon Offset
						if (CurrentWeapon) {
							if (GetAsyncKeyState(VK_RBUTTON))
							{

								write<float>(CurrentWeapon + 0x818, 0.05f); //bMovingEmote
								//wasClicked = true;
							}
							else
							{
								write<float>(LocalPawn + 0x818, 1.00f); //bAllowBuildingActorTeleport::bIsDBNO 0x57E
							}
						}
					}*/

				

					if (Settings::botesp && strstr(name, xorstr("PlayerPawn_Athena_Phoebe_C")) && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

						uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
						Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
						float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 300.f;

						if (ItemDist < Settings::MaxESPDistance) {
							Vector3 BotPosition;
							SDK::Classes::AController::WorldToScreen(ItemPosition, &BotPosition);

							std::string null = xorstr("");
							std::string Text = null + xorstr("Bot/Ai");

							ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

							ShadowRGBTexts(head2.x - TextSize.x - 5, head2.y - TextSize.y - 30, SettingsColor::Box, Text.c_str());

						}

					}

					

					

					

					
					

					

					if (Settings::RadarESP)
					{
						RadarLoop(CurrentActor, LocalRootComponent);
					}


					if (Settings::AirStuck)
					{
						if (GetAsyncKeyState(VK_CONTROL))
						{
							//*reinterpret_cast<float*>(reinterpret_cast<PBYTE>(LocalPawn) + SDK::Classes::StaticOffsets::CostumTimeDilation) = 0.01;
							(*(FLOAT*)(((PBYTE)LocalPawn + StaticOffsets::CostumTimeDilation))) = 0.09;
							wasClicked = true;
						}
						else
						{
							//*reinterpret_cast<float*>(reinterpret_cast<PBYTE>(LocalPawn) + SDK::Classes::StaticOffsets::CostumTimeDilation) = 1;
							(*(FLOAT*)(((PBYTE)LocalPawn + StaticOffsets::CostumTimeDilation))) = 1;
							wasClicked = false;
						}
					}
					else {
						if (wasClicked) {
							//*reinterpret_cast<float*>(reinterpret_cast<PBYTE>(LocalPawn) + SDK::Classes::StaticOffsets::CostumTimeDilation) = 1;
							(*(FLOAT*)(((PBYTE)LocalPawn + StaticOffsets::CostumTimeDilation))) = 1;
							wasClicked = false;
						}
					}

					bool InsideVehicle = false;
					float closestDistanceForVehicle = FLT_MAX;
					PVOID closestPawnForVehicle = NULL;

					float closestVehicle = FLT_MAX;
					PVOID closestPawnVehicle = NULL;

				

					

					

					

					if (Settings::SilentAim) {
						if (!HookFunctions::CalcShotInitialized) {
							HookFunctions::Init(false, true);
						}

						/*if (IsAiming)
						{
							if (SDK::Utils::CheckIfInFOV(CurrentActor, FOVmax)) {

								if (Settings::Memory)
								{
									if (Settings::VisibleCheck and IsVisible) {

										Vector3 NewAngle = SDK::Utils::GetRotation(CurrentActor);

										if (NewAngle.x == 0 && NewAngle.y == 0) continue;

										if (Settings::Smoothness > 0)
											NewAngle = SDK::Utils::SmoothAngles(SDK::Utils::CamRot, NewAngle);

										NewAngle.z = 0;

										SDK::Classes::AController::ValidateClientSetRotation(NewAngle, false);
										SDK::Classes::AController::ClientSetRotation(NewAngle, false);
									}
								}
								else if (!Settings::VisibleCheck) {

									if (Settings::Memory)
									{
										Vector3 NewAngle = SDK::Utils::GetRotation(CurrentActor);

										if (NewAngle.x == 0 && NewAngle.y == 0) continue;

										if (Settings::Smoothness > 0)
											NewAngle = SDK::Utils::SmoothAngles(SDK::Utils::CamRot, NewAngle);

										NewAngle.z = 0;

										SDK::Classes::AController::ValidateClientSetRotation(NewAngle, false);
										SDK::Classes::AController::ClientSetRotation(NewAngle, false);
									}
								}
							}
						}*/

						




						Vector3 closestPawnviewPoint;
						closestPawnVisible = SDK::Classes::APlayerCameraManager::LineOfSightTo((PVOID)PlayerController, (PVOID)closestPawn, &closestPawnviewPoint);

						if (!Settings::fov3604) {
							auto dx = Headbox.x - (Renderer_Defines::Width / 2);
							auto dy = Headbox.y - (Renderer_Defines::Height / 2);
							auto dist = SpoofRuntime::sqrtf_(dx * dx + dy * dy);

							if (dist < Settings::FovCircle_Value && dist < closestDistance) {
								closestDistance = dist;
								closestPawn = (PVOID)CurrentActor;
							}
						}
						else {
							closestPawn = (PVOID)CurrentActor;
						}


					}
					else if (Settings::SilentAim and !IsVisible) {
						closestPawn = nullptr;
					}
				}
				}



					

					








			if(strstr(name, xorstr("BP_PlayerPawn")) || strstr(name, xorstr("PlayerPawn")) || strstr(name, xorstr("PlayerPawn_Athena_Phoebe_C")))
			{

				if (Settings::PlayersWeapon && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 WeaponPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					Vector3 GunPosition;


					auto actorCurrentWeapon = ReadPointer(CurrentActor, StaticOffsets::CurrentWeapon);
					if (!actorCurrentWeapon) continue;

					auto actorItemDef = ReadPointer(actorCurrentWeapon, StaticOffsets::WeaponData);
					if (!actorItemDef) continue;

					auto actorItemDisplayName = reinterpret_cast<FText*>(ReadPointer(actorItemDef, StaticOffsets::DisplayName));
					if (!actorItemDisplayName || !actorItemDisplayName->c_str()) continue;


					if (SDK::Classes::AController::WorldToScreen(WeaponPosition, &GunPosition))
					{
						CHAR text[0xFF] = { 0 };
						wcstombs(text, actorItemDisplayName->c_str(), sizeof(text));

						std::string null = xorstr("");
						std::string Text = null + text + xorstr(" ");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						ShadowRGBTexts(pelvis.x - TextSize.x, pelvis.y - TextSize.y, SettingsColor::PlayerReload, Text.c_str());
						//Renderer->AddText(ImVec2(pelvis.x - TextSize.x, pelvis.y - TextSize.y), SettingsColor::PlayerReload, Text.c_str());

					}
				}

				if (SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

					Vector3 HeadPos, Headbox, bottom;

					SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 98, &HeadPos);
					SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 0, &bottom);

					SDK::Classes::AController::WorldToScreen(Vector3(HeadPos.x, HeadPos.y, HeadPos.z + 20), &Headbox);
					SDK::Classes::AController::WorldToScreen(bottom, &bottom);


					if (Settings::BoxTypeSelected == 0 or Settings::BoxTypeSelected == 1 or Settings::Skeleton or Settings::SkeletonTP) {
						GetAllBones(CurrentActor);
					}

					//int MostRightBone, MostLeftBone;
					int array[20] = { head2.x, neck.x, pelvis.x, chest.x, leftShoulder.x, rightShoulder.x, leftElbow.x, rightElbow.x, leftHand.x, rightHand.x, leftLeg.x, rightLeg.x, leftThigh.x, rightThigh.x, leftFoot.x, rightFoot.x, leftFeet.x, rightFeet.x, leftFeetFinger.x, rightFeetFinger.x };
					int MostRightBone = array[0];
					int MostLeftBone = array[0];

					for (int mostrighti = 0; mostrighti < 20; mostrighti++)
					{
						if (array[mostrighti] > MostRightBone)
							MostRightBone = array[mostrighti];
					}

					for (int mostlefti = 0; mostlefti < 20; mostlefti++)
					{
						if (array[mostlefti] < MostLeftBone)
							MostLeftBone = array[mostlefti];
					}



					float ActorHeight = (Headbox.y - bottom.y);
					if (ActorHeight < 0) ActorHeight = ActorHeight * (-1.f);

					int ActorWidth = MostRightBone - MostLeftBone;

					


					if (Settings::Skeleton)
					{

						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Skeleton;
						}
						else {
							col = SettingsColor::Skeleton_notvisible;
						}


						Renderer->AddLine(ImVec2(head2.x, head2.y), ImVec2(neck.x, neck.y), col, 1.4f);
						Renderer->AddLine(ImVec2(neck.x, neck.y), ImVec2(chest.x, chest.y), col, 1.4f);
						Renderer->AddLine(ImVec2(chest.x, chest.y), ImVec2(pelvis.x, pelvis.y), col, 1.4f);
						Renderer->AddLine(ImVec2(chest.x, chest.y), ImVec2(leftShoulder.x, leftShoulder.y), col, 1.4f);
						Renderer->AddLine(ImVec2(chest.x, chest.y), ImVec2(rightShoulder.x, rightShoulder.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftShoulder.x, leftShoulder.y), ImVec2(leftElbow.x, leftElbow.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightShoulder.x, rightShoulder.y), ImVec2(rightElbow.x, rightElbow.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftElbow.x, leftElbow.y), ImVec2(leftHand.x, leftHand.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightElbow.x, rightElbow.y), ImVec2(rightHand.x, rightHand.y), col, 1.4f);
						Renderer->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(leftLeg.x, leftLeg.y), col, 1.4f);
						Renderer->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(rightLeg.x, rightLeg.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftLeg.x, leftLeg.y), ImVec2(leftThigh.x, leftThigh.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightLeg.x, rightLeg.y), ImVec2(rightThigh.x, rightThigh.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftThigh.x, leftThigh.y), ImVec2(leftFoot.x, leftFoot.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightThigh.x, rightThigh.y), ImVec2(rightFoot.x, rightFoot.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftFoot.x, leftFoot.y), ImVec2(leftFeet.x, leftFeet.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightFoot.x, rightFoot.y), ImVec2(rightFeet.x, rightFeet.y), col, 1.4f);
					Renderer->AddLine(ImVec2(leftFeet.x, leftFeet.y), ImVec2(leftFeetFinger.x, leftFeetFinger.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightFeet.x, rightFeet.y), ImVec2(rightFeetFinger.x, rightFeetFinger.y), col, 1.4f);






					}



					if (Settings::skeletonsvisible)
					{

						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Skeleton;
						}
						else {

						}


						Renderer->AddLine(ImVec2(head2.x, head2.y), ImVec2(neck.x, neck.y), col, 1.4f);
						Renderer->AddLine(ImVec2(neck.x, neck.y), ImVec2(chest.x, chest.y), col, 1.4f);
						Renderer->AddLine(ImVec2(chest.x, chest.y), ImVec2(pelvis.x, pelvis.y), col, 1.4f);
						Renderer->AddLine(ImVec2(chest.x, chest.y), ImVec2(leftShoulder.x, leftShoulder.y), col, 1.4f);
						Renderer->AddLine(ImVec2(chest.x, chest.y), ImVec2(rightShoulder.x, rightShoulder.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftShoulder.x, leftShoulder.y), ImVec2(leftElbow.x, leftElbow.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightShoulder.x, rightShoulder.y), ImVec2(rightElbow.x, rightElbow.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftElbow.x, leftElbow.y), ImVec2(leftHand.x, leftHand.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightElbow.x, rightElbow.y), ImVec2(rightHand.x, rightHand.y), col, 1.4f);
						Renderer->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(leftLeg.x, leftLeg.y), col, 1.4f);
						Renderer->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(rightLeg.x, rightLeg.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftLeg.x, leftLeg.y), ImVec2(leftThigh.x, leftThigh.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightLeg.x, rightLeg.y), ImVec2(rightThigh.x, rightThigh.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftThigh.x, leftThigh.y), ImVec2(leftFoot.x, leftFoot.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightThigh.x, rightThigh.y), ImVec2(rightFoot.x, rightFoot.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftFoot.x, leftFoot.y), ImVec2(leftFeet.x, leftFeet.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightFoot.x, rightFoot.y), ImVec2(rightFeet.x, rightFeet.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftFeet.x, leftFeet.y), ImVec2(leftFeetFinger.x, leftFeetFinger.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightFeet.x, rightFeet.y), ImVec2(rightFeetFinger.x, rightFeetFinger.y), col, 1.4f);




					}

					if (Settings::skeletonsbehindwalls)
					{

						ImColor col;
						if (IsVisible) {

						}
						else {
							col = SettingsColor::Skeleton_notvisible;
						}


						Renderer->AddLine(ImVec2(head2.x, head2.y), ImVec2(neck.x, neck.y), col, 1.4f);
						Renderer->AddLine(ImVec2(neck.x, neck.y), ImVec2(chest.x, chest.y), col, 1.4f);
						Renderer->AddLine(ImVec2(chest.x, chest.y), ImVec2(pelvis.x, pelvis.y), col, 1.4f);
						Renderer->AddLine(ImVec2(chest.x, chest.y), ImVec2(leftShoulder.x, leftShoulder.y), col, 1.4f);
						Renderer->AddLine(ImVec2(chest.x, chest.y), ImVec2(rightShoulder.x, rightShoulder.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftShoulder.x, leftShoulder.y), ImVec2(leftElbow.x, leftElbow.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightShoulder.x, rightShoulder.y), ImVec2(rightElbow.x, rightElbow.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftElbow.x, leftElbow.y), ImVec2(leftHand.x, leftHand.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightElbow.x, rightElbow.y), ImVec2(rightHand.x, rightHand.y), col, 1.4f);
						Renderer->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(leftLeg.x, leftLeg.y), col, 1.4f);
						Renderer->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(rightLeg.x, rightLeg.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftLeg.x, leftLeg.y), ImVec2(leftThigh.x, leftThigh.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightLeg.x, rightLeg.y), ImVec2(rightThigh.x, rightThigh.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftThigh.x, leftThigh.y), ImVec2(leftFoot.x, leftFoot.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightThigh.x, rightThigh.y), ImVec2(rightFoot.x, rightFoot.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftFoot.x, leftFoot.y), ImVec2(leftFeet.x, leftFeet.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightFoot.x, rightFoot.y), ImVec2(rightFeet.x, rightFeet.y), col, 1.4f);
						Renderer->AddLine(ImVec2(leftFeet.x, leftFeet.y), ImVec2(leftFeetFinger.x, leftFeetFinger.y), col, 1.4f);
						Renderer->AddLine(ImVec2(rightFeet.x, rightFeet.y), ImVec2(rightFeetFinger.x, rightFeetFinger.y), col, 1.4f);




					}



					if (Settings::Box and Settings::BoxTypeSelected == 2) {

						Vector3 BottomNoW2S;
						Vector3 HeadNoW2S;

						SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 98, &HeadNoW2S);
						SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 0, &BottomNoW2S);


						Vector3 bottom1;
						Vector3 bottom2;
						Vector3 bottom3;
						Vector3 bottom4;

						SDK::Classes::AController::WorldToScreen(Vector3(BottomNoW2S.x + 30, BottomNoW2S.y - 30, BottomNoW2S.z), &bottom1);
						SDK::Classes::AController::WorldToScreen(Vector3(BottomNoW2S.x - 30, BottomNoW2S.y - 30, BottomNoW2S.z), &bottom2);
						SDK::Classes::AController::WorldToScreen(Vector3(BottomNoW2S.x - 30, BottomNoW2S.y + 30, BottomNoW2S.z), &bottom3);
						SDK::Classes::AController::WorldToScreen(Vector3(BottomNoW2S.x + 30, BottomNoW2S.y + 30, BottomNoW2S.z), &bottom4);



						Vector3 top1;
						Vector3 top2;
						Vector3 top3;
						Vector3 top4;

						SDK::Classes::AController::WorldToScreen(Vector3(HeadNoW2S.x + 30, HeadNoW2S.y - 30, HeadNoW2S.z), &top1);
						SDK::Classes::AController::WorldToScreen(Vector3(HeadNoW2S.x - 30, HeadNoW2S.y - 30, HeadNoW2S.z), &top2);
						SDK::Classes::AController::WorldToScreen(Vector3(HeadNoW2S.x - 30, HeadNoW2S.y + 30, HeadNoW2S.z), &top3);
						SDK::Classes::AController::WorldToScreen(Vector3(HeadNoW2S.x + 30, HeadNoW2S.y + 30, HeadNoW2S.z), &top4);


						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Box;
						}
						else {
							col = SettingsColor::Box_notvisible;
						}

						Renderer->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), col, 1.f);
						Renderer->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), col, 1.f);
						Renderer->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), col, 1.f);
						Renderer->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), col, 1.f);


						Renderer->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), col, 1.f);
						Renderer->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), col, 1.f);
						Renderer->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), col, 1.f);
						Renderer->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), col, 1.f);


						Renderer->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), col, 1.f);
						Renderer->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), col, 1.f);
						Renderer->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), col, 1.f);
						Renderer->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), col, 1.f);




					}


					float BoxHeight = (Headbox.y - bottom.y);
					if (BoxHeight < 0) BoxHeight = BoxHeight * (5.f);

					int Width = MostRightBone - MostLeftBone;


					if (Settings::Box) {


						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Box;
						}
						else {
							col = SettingsColor::Box_notvisible;
						}


						Draw2DBoundingBox(Headbox, ActorWidth, ActorHeight, col);


					}

					if (Settings::boxfill)
					{
						FilledRect(Headbox.x - (ActorWidth / 2), Headbox.y, ActorWidth, ActorHeight, ImColor(0, 0, 0, 50));
					}


					if (Settings::boxs) {



						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Box;
						}
						else {
							col = SettingsColor::Box_notvisible;
						}


						DrawCorneredBox(Headbox.x - (ActorWidth / 2), Headbox.y, ActorWidth, ActorHeight, col, 2.2);
						//DrawCorneredBox(Headbox.x - (ActorWidth / 2), Headbox.y, ActorWidth, ActorHeight, col, 1.8f);
					}

					if (Settings::boxvisible and Settings::BoxTypeSelected == 1) {



						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Box;
						}
						else {

						}

						DrawCorneredBox(Headbox.x - (ActorWidth / 2), Headbox.y, ActorWidth, ActorHeight, col, 1.8f);
					}

					if (Settings::boxbehindwalls and Settings::BoxTypeSelected == 1) {



						ImColor col;
						if (IsVisible) {

						}
						else {
							col = SettingsColor::Box_notvisible;
						}

						DrawCorneredBox(Headbox.x - (ActorWidth / 2), Headbox.y, ActorWidth, ActorHeight, col, 1.8f);
					}
				}


			}
		}


		if (Settings::MagicBullets || Settings::SilentAim) {
			if (closestPawn) {
				TargetPawn = closestPawn;
			}
			else {
				TargetPawn = nullptr;
			}
		}
		else {
			TargetPawn = nullptr;
		}


		if (!LocalPawn) return false;


		int AtLeastOneBool = 0;
		if (Settings::ChestESP) AtLeastOneBool++; if (Settings::AmmoBoxESP) AtLeastOneBool++; if (Settings::LootESP) AtLeastOneBool++;

		if (AtLeastOneBool == 0) return false;


		for (auto Itemlevel_i = 0UL; Itemlevel_i < read<DWORD>(GWorld + (StaticOffsets::Levels + sizeof(PVOID))); ++Itemlevel_i) {
			uintptr_t ItemLevels = read<uintptr_t>(GWorld + StaticOffsets::Levels);
			if (!ItemLevels) return false;

			uintptr_t ItemLevel = read<uintptr_t>(ItemLevels + (Itemlevel_i * sizeof(uintptr_t)));
			if (!ItemLevel) return false;

			for (int i = 0; i < read<DWORD>(ItemLevel + (StaticOffsets::AActors + sizeof(PVOID))); ++i) {


				uintptr_t ItemsPawns = read<uintptr_t>(ItemLevel + StaticOffsets::AActors);
				if (!ItemsPawns) return false;

				uintptr_t CurrentItemPawn = read<uintptr_t>(ItemsPawns + (i * sizeof(uintptr_t)));

				auto CurrentItemPawnName = SDK::Classes::UObject::GetObjectName(CurrentItemPawn);

				if (LocalPawn) {
					//Loot ESP
					LootESP(Renderer, CurrentItemPawnName, CurrentItemPawn, LocalRelativeLocation);
				}



			}
		}



	}
	catch (...) {}


}



void ColorAndStyle() {
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(4.5f, 2.5f);
	style->FrameRounding = 2.5f;
	style->ItemSpacing = ImVec2(12, 8);

	style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style->IndentSpacing = 25.0f;


	//Tabs
	style->ItemInnerSpacing = ImVec2(18, 6);
	//style->TabRounding = 0.0f;

	style->ScrollbarSize = 0.0f;
	style->ScrollbarRounding = 0.0f;

	//Sliders
	style->GrabMinSize = 6.0f;
	style->GrabRounding = 0.0f;


	style->Colors[ImGuiCol_Text] = ImVec4(255.0f, 0.0f, 0.0f, 255.0f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
	style->Colors[ImGuiCol_FrameBg] = ImColor(37, 38, 51, 255);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(42, 43, 56, 255);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(37, 38, 51, 255);


	//style->Colors[ImGuiCol_Tab] = ImColor(29, 28, 37, 255);
	//style->Colors[ImGuiCol_TabActive] = ImColor(79, 79, 81, 255);
	//style->Colors[ImGuiCol_TabHovered] = ImColor(62, 62, 66, 255);

	style->Colors[ImGuiCol_TitleBg] = ImColor(35, 35, 35, 255);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(35, 35, 35, 255);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(35, 35, 35, 255);

	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImColor(0, 242, 36, 255);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImColor(35, 35, 35, 255);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(60, 60, 60, 255);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(35, 35, 35, 255);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
}


namespace ImGui
{
	IMGUI_API bool Tab(unsigned int index, const char* label, int* selected, float width = 46, float height = 17)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImColor color = ImColor(27, 26, 35, 255)/*style.Colors[ImGuiCol_Button]*/;
		ImColor colorActive = ImColor(79, 79, 81, 255); /*style.Colors[ImGuiCol_ButtonActive]*/;
		ImColor colorHover = ImColor(62, 62, 66, 255)/*style.Colors[ImGuiCol_ButtonHovered]*/;


		if (index > 0)
			ImGui::SameLine();

		if (index == *selected)
		{
			style.Colors[ImGuiCol_Button] = colorActive;
			style.Colors[ImGuiCol_ButtonActive] = colorActive;
			style.Colors[ImGuiCol_ButtonHovered] = colorActive;
		}
		else
		{
			style.Colors[ImGuiCol_Button] = color;
			style.Colors[ImGuiCol_ButtonActive] = colorActive;
			style.Colors[ImGuiCol_ButtonHovered] = colorHover;
		}

		if (ImGui::Button(label, ImVec2(width, height)))
			*selected = index;

		style.Colors[ImGuiCol_Button] = color;
		style.Colors[ImGuiCol_ButtonActive] = colorActive;
		style.Colors[ImGuiCol_ButtonHovered] = colorHover;

		return *selected == index;
	}
}

/*
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//if (msg == WM_QUIT && ShowMenu) {
	//	ExitProcess(0);
	//}

	if (ShowMenu) {
		//ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return TRUE;
	}

	return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}
*/


void ProcessVehicle(uintptr_t pawn)
{
	auto rc = read<uintptr_t>(pawn + StaticOffsets::RootComponent);

	if (!rc)
		return;

	auto loc = read<FVector>(rc + StaticOffsets::RootComponent);

	/*
	if (GetDistanceMeters(loc) > 10)
		return;*/

	auto veh_mesh = *(uintptr_t*)(pawn + 0x6669);

	if (veh_mesh)
	{
		float coeff = (Settings::Coeff * Settings::Coeff);

		bool bKp = false;

		/*if (GetAsyncKeyState(0x57))
		{
			FVector vel;
			auto yaw = GCameraCache->Rotation.Yaw;
			float theta = 2.f * M_PI * (yaw / 360.f);

			vel.X = (coeff * cos(theta));
			vel.Y = (coeff * sin(theta));
			vel.Z = 0.f;

			SetAllPhysicsLinearVelocity(veh_mesh, vel, true);
			bKp = true;
		}
		if (GetAsyncKeyState(0x53))
		{
			FVector vel;
			auto yaw = GCameraCache->Rotation.Yaw;
			float theta = 2.f * M_PI * (yaw / 360.f);

			vel.X = -(coeff * cos(theta));
			vel.Y = -(coeff * sin(theta));

			SetAllPhysicsLinearVelocity(veh_mesh, vel, true); //{ -80.f, 0.f, 0.f }
			bKp = true;
		}
		if (GetAsyncKeyState(0x41)) // A
		{
			FVector vel;
			auto yaw = GCameraCache->Rotation.Yaw;
			float theta = 2.f * M_PI * (yaw / 360.f);

			vel.X = (coeff * sin(theta));
			vel.Y = -(coeff * cos(theta));

			SetAllPhysicsLinearVelocity(veh_mesh, vel, true); //{ -80.f, 0.f, 0.f }
			bKp = true;
		}
		if (GetAsyncKeyState(0x44)) // D
		{
			FVector vel;
			auto yaw = GCameraCache->Rotation.Yaw;
			float theta = 2.f * M_PI * (yaw / 360.f);

			vel.X = -(coeff * sin(theta));
			vel.Y = (coeff * cos(theta));

			SetAllPhysicsLinearVelocity(veh_mesh, vel, true); //{ -80.f, 0.f, 0.f }
			bKp = true;
		}

		if (GetAsyncKeyState(VK_SPACE))
		{
			SetAllPhysicsLinearVelocity(veh_mesh, { 0.f, 0.f, coeff / 2 }, true);
			bKp = true;
		}

		if (GetAsyncKeyState(VK_SHIFT))
		{
			SetAllPhysicsLinearVelocity(veh_mesh, { 0.f, 0.f, -coeff / 2 }, true);
			bKp = true;
		}*/
	}


}

ImGuiWindow& CreateScene() {
	ImGui_ImplDX11_NewFrame();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin(xorstr("##mainscenee"), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);

	auto& io = ImGui::GetIO();
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);

	return *ImGui::GetCurrentWindow();
}

VOID MenuAndDestroy(ImGuiWindow& window) {
	window.DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);

	if (Settings::menu) {
		ColorAndStyle();
		ImGui::SetNextWindowSize({ 650, 475 });
		ImGuiStyle* style = &ImGui::GetStyle();
		static int maintabs = 0;
		static int esptabs = 0;
		if (ImGui::Begin(xorstr("Portal Aim Rage Beta (Menu Usable In Game)"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize))



			ImGui::SetCursorPos({ 0.f,375.f });
		ImGui::BeginChild("Portal", { 0.f, 0.f }, true, ImGuiWindowFlags_NoScrollbar);
		if (ImGui::Button("Aimbot", { 135.f, 67.f }))
		{
			maintabs = 0;
		}

		ImGui::SetCursorPos({ 175.f,15.f });
		if (ImGui::Button("Player ESP", { 135.f, 67.f }))
		{
			maintabs = 1;
		}

		ImGui::SetCursorPos({ 335.f,15.f });
		if (ImGui::Button("World ESP", { 135.f, 67.f }))
		{
			maintabs = 2;
		}

		ImGui::SetCursorPos({ 495.f,15.f });
		if (ImGui::Button("Exploits", { 135.f, 67.f }))
		{
			maintabs = 3;
		}



		ImGui::EndChild();







		if (maintabs == 0)
		{
			ImGui::SetCursorPos({ 154.f,59.f - 30.f });
			ImGui::Text(" ");
			ImGui::Checkbox("Memory Aimbot", &Settings::Memory);
			if (Settings::Memory)
			{
				//Settings::SilentAim = false;
			}

			ImGui::Checkbox("Silent Aimbot", &Settings::SilentAim);

			if (Settings::SilentAim)
			{
				//Settings::Memory = false;
			}
			ImGui::Checkbox("Check Visibility", &Settings::VisibleCheck);
			ImGui::Text(" ");
			ImGui::Checkbox("Head Aim", &Settings::Head);
			if (Settings::Head)
			{
				Settings::aimbone = 98;
				Settings::Neck = false;
				Settings::Pelvis = false;
			}
			ImGui::Checkbox("Neck Aim", &Settings::Neck);
			if (Settings::Neck)
			{
				Settings::aimbone = 66;
				Settings::Head = false;
				Settings::Pelvis = false;
			}
			ImGui::Checkbox("Body Aim", &Settings::Pelvis);
			if (Settings::Pelvis)
			{
				Settings::aimbone = 5;
				Settings::Neck = false;
				Settings::Head = false;
			}
			ImGui::Text(" ");

			ImGui::SliderFloat(xorstr("Smoothing"), &Settings::Smoothness, 1, 15);
			ImGui::Text(" ");
			ImGui::SliderFloat(xorstr("FOV Circle Value"), &Settings::FovCircle_Value, 0, 1100);
			ImGui::Text(" ");
			ImGui::Checkbox(_xor("Crosshair").c_str(), &Settings::crosshairnormal);
			ImGui::Text(" ");



			ImGui::Text(" ");
		}

		if (maintabs == 1)
		{
			ImGui::SetCursorPos({ 154.f,59.f - 30.f });
			ImGui::Text(" ");
			ImGui::Checkbox(_xor("Player's Weapon").c_str(), &Settings::PlayersWeapon);
			ImGui::Checkbox(_xor("Player's Ammo").c_str(), &Settings::PlayerAmmo);
			ImGui::Checkbox("Skeleton ESP", &Settings::Skeleton);
			ImGui::Checkbox("2D Box ESP", &Settings::Box);
			ImGui::Checkbox("Cornered Box ESP", &Settings::boxs);
			ImGui::Checkbox("Fill Box ESP", &Settings::boxfill);
			ImGui::Checkbox("Snaplines ESP Bottom", &Settings::SnapLinesbottom);
			ImGui::Checkbox("Snaplines ESP Middle", &Settings::SnapLines);
			ImGui::Checkbox("Snaplines ESP Top", &Settings::SnapLinestop);
			ImGui::Checkbox("Distance ESP", &Settings::DistanceESP);
			ImGui::Checkbox("Radar ESP", &Settings::Radar);
		}
		if (maintabs == 2)
		{
			ImGui::SetCursorPos({ 154.f,59.f - 30.f });
			ImGui::Text(" ");
			ImGui::Checkbox(_xor("Weapon ESP").c_str(), &Settings::LootESP);
			ImGui::Checkbox(_xor("Bot ESP").c_str(), &Settings::botesp);
			ImGui::Checkbox(_xor("Weakspot ESP").c_str(), &Settings::WSesp);
			ImGui::Checkbox(_xor("Supply Drop ESP").c_str(), &Settings::supplydrop);
			ImGui::Checkbox(_xor("Helicopter ESP").c_str(), &Settings::HeliESP);
			ImGui::Checkbox(_xor("Plane ESP").c_str(), &Settings::PlaneESP);
			ImGui::Checkbox(_xor("Drift Board ESP").c_str(), &Settings::BoardESP);
			ImGui::Checkbox(_xor("Vehicle ESP").c_str(), &Settings::VehiclesESP);
			ImGui::Checkbox(_xor("Boat ESP").c_str(), &Settings::boatesp);
		}

		if (maintabs == 3)
		{
			ImGui::SetCursorPos({ 154.f,59.f - 30.f });
			ImGui::Text(" ");
			ImGui::Checkbox(_xor("Disable Weapon Spread").c_str(), &Settings::NoSpread);
			ImGui::Checkbox(_xor("Rapid Fire (Works fine with burts and snipers)").c_str(), &Settings::RapidFire);
			ImGui::Checkbox(_xor("Aim While Jumping").c_str(), &Settings::AimWhileJumping);
			ImGui::Checkbox(_xor("Air Stuck [CTRL]").c_str(), &Settings::AirStuck);
			ImGui::Checkbox(_xor("Bullet Teleport [Vehicles] / Pickaxe TP [11m]").c_str(), &Settings::MagicBullets);
			ImGui::Checkbox(xorstr("Waypoint Teleport [ENTER]"), &Settings::WaypointTP);
			ImGui::Checkbox(xorstr("Player Collision while aimboting [Working Fine]"), &Settings::PlayerCollision);
			ImGui::Checkbox(xorstr("Unreleased Emote [CAPS > DISABLE]"), &Settings::UnreleasedEmote);
			ImGui::Checkbox(xorstr("Player Fly (may cause player to freeze)"), &Settings::PlayerFly);
			ImGui::Checkbox(xorstr("No Weapon Cooldown"), &Settings::nocooldown);
			ImGui::Checkbox(xorstr("Instant Revive"), &Settings::InstantRevive);
			if (Settings::PlayerFly)
			{
				Settings::PlayerCollision = false;
				//write<float>(LocalPawn + 0x541, 1.00f); //bAllowBuildingActorTeleport::bIsDBNO 0x57E
			}
			if (Settings::PlayerCollision)
			{
				Settings::PlayerFly = false;
				//write<float>(LocalPawn + 0x1430, 1.00f); //bAllowBuildingActorTeleport::bIsDBNO 0x57E
			}
			if (Settings::MagicBullets)
			{
				Settings::SilentAim = true;
				Settings::SilentAim = true;
				Settings::SilentAim = true;
			}


			if (ImGui::IsItemHovered())

			if (Settings::MagicBullets) {
				Settings::SilentAim = true;
				Settings::SilentAim = true;
			}
			ImGui::BeginGroup();
			ImGui::EndGroup();

		}
		ImGui::End();
	}
	ImGui::Render();
}













#include "font.h"

HRESULT present_hooked(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags)
{



	static float width = 0;
	static float height = 0;
	static HWND hWnd = 0;
	if (!device)
	{

		//MessageBoxA(NULL, "IM INTO THE PRESENT HOOOOKED!!", "", MB_OK);

		swapChain->GetDevice(__uuidof(device), reinterpret_cast<PVOID*>(&device));
		device->GetImmediateContext(&immediateContext);
		//MessageBoxA(NULL, "Before RenderTargetDefine and release", "", MB_OK);
		ID3D11Texture2D* renderTarget = nullptr;
		swapChain->GetBuffer(0, __uuidof(renderTarget), reinterpret_cast<PVOID*>(&renderTarget));
		device->CreateRenderTargetView(renderTarget, nullptr, &renderTargetView);
		renderTarget->Release();
		//MessageBoxA(NULL, "Before backBuffer", "", MB_OK);
		ID3D11Texture2D* backBuffer = 0;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (PVOID*)&backBuffer);
		D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };
		backBuffer->GetDesc(&backBufferDesc);
		//MessageBoxA(NULL, "Before FindWindow", "", MB_OK);
		HWND hWnd = LI_FN(FindWindowA)(xorstr("UnrealWindow"), xorstr("Fortnite  "));
		//if (!width) {
			//oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));
			//oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&WndProc)); // Hook
		//}



		width = (float)backBufferDesc.Width;
		height = (float)backBufferDesc.Height;
		backBuffer->Release();

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromMemoryTTF(Fontss::DefaultFont, 16.0f, 16.0f);
		io.Fonts->AddFontFromMemoryTTF(Fontss::SecondaryFont, 12.0f, 12.0f);
		io.Fonts->AddFontFromMemoryTTF(Fontss::WeaponFont, 20.0f, 20.0f);
		//ImGui::GetIO().Fonts->AddFontFromFileTTF(xorstr("C:\\Windows\\Fonts\\Verdana.ttf"), 17.0f);


		ImGui_ImplDX11_Init(hWnd, device, immediateContext);
		ImGui_ImplDX11_CreateDeviceObjects();

	}
	immediateContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
	//MessageBoxA(NULL, "Before CreateScene", "", MB_OK);
	auto& window = CreateScene();

	if (ShowMenu) {
		ImGuiIO& io = ImGui::GetIO();

		POINT p;
		SpoofCall(GetCursorPos, &p);
		io.MousePos.x = p.x;
		io.MousePos.y = p.y;


		//MessageBoxA(NULL, "Before NtGetAsyncKeyState", "", MB_OK);
		if (NtGetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].y = io.MousePos.y;
		}
		else {
			io.MouseDown[0] = false;
		}
	}



	//MessageBoxA(NULL, "Before EntityLoop", "", MB_OK);
	EntitiyLoop();
	//MessageBoxA(NULL, "Before second NtGetAsyncKeyState", "", MB_OK);
	if (NtGetAsyncKeyState(VK_INSERT) & 1)
	{
		Settings::menu = !Settings::menu;
		ImGui::GetIO().MouseDrawCursor = Settings::menu, ImColor(236, 3, 252, 255);
	}

	//MessageBoxA(NULL, "Before MenuAndDestroy", "", MB_OK);
	MenuAndDestroy(window);
	//MessageBoxA(NULL, "Before TheReturn", "", MB_OK);
	return SpoofCall(present_original, swapChain, syncInterval, flags);
}



HRESULT resize_hooked(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
	ImGui_ImplDX11_Shutdown();
	renderTargetView->Release();
	immediateContext->Release();
	device->Release();
	device = nullptr;

	return SpoofCall(resize_original, swapChain, bufferCount, width, height, newFormat, swapChainFlags);
}




PVOID SpreadCaller = nullptr;
BOOL(*Spread)(PVOID a1, float* a2, float* a3);
BOOL SpreadHook(PVOID a1, float* a2, float* a3)
{
	if (Settings::NoSpread && _ReturnAddress() == SpreadCaller) {
		return 0;
	}

	return SpoofCall(Spread, a1, a2, a3);
}

float* (*CalculateShot)(PVOID, PVOID, PVOID) = nullptr;

float* CalculateShotHook(PVOID arg0, PVOID arg1, PVOID arg2) {
	auto ret = CalculateShot(arg0, arg1, arg2);

	

	if (ret && Settings::MagicBullets || Settings::SilentAim && TargetPawn && LocalPawn)
	{

		Vector3 headvec3;
		SDK::Classes::USkeletalMeshComponent::GetBoneLocation((uintptr_t)TargetPawn, 8, &headvec3);
		SDK::Structs::FVector head = { headvec3.x, headvec3.y , headvec3.z };

		uintptr_t RootComp = read<uintptr_t>(LocalPawn + StaticOffsets::RootComponent);
		Vector3 RootCompLocationvec3 = read<Vector3>(RootComp + StaticOffsets::RelativeLocation);
		SDK::Structs::FVector RootCompLocation = { RootCompLocationvec3.x, RootCompLocationvec3.y , RootCompLocationvec3.z };
		SDK::Structs::FVector* RootCompLocation_check = &RootCompLocation;
		if (!RootCompLocation_check) return ret;
		auto root = RootCompLocation;

		auto dx = head.X - root.X;
		auto dy = head.Y - root.Y;
		auto dz = head.Z - root.Z;

		if (Settings::MagicBullets) {
			ret[4] = head.X;
			ret[5] = head.Y;
			ret[6] = head.Z;
			head.Z -= 16.0f;
			root.Z += 45.0f;

			auto y = SpoofRuntime::atan2f_(head.Y - root.Y, head.X - root.X);

			root.X += SpoofRuntime::cosf_(y + 1.5708f) * 32.0f;
			root.Y += SpoofRuntime::sinf_(y + 1.5708f) * 32.0f;

			auto length = SpoofRuntime::sqrtf_(SpoofRuntime::powf_(head.X - root.X, 2) + SpoofRuntime::powf_(head.Y - root.Y, 2));
			auto x = -SpoofRuntime::atan2f_(head.Z - root.Z, length);
			y = SpoofRuntime::atan2f_(head.Y - root.Y, head.X - root.X);

			x /= 2.0f;
			y /= 2.0f;

			ret[0] = -(SpoofRuntime::sinf_(x) * SpoofRuntime::sinf_(y));
			ret[1] = SpoofRuntime::sinf_(x) * SpoofRuntime::cosf_(y);
			ret[2] = SpoofRuntime::cosf_(x) * SpoofRuntime::sinf_(y);
			ret[3] = SpoofRuntime::cosf_(x) * SpoofRuntime::cosf_(y);
		}

		if (Settings::SilentAim) {
			if (dx * dx + dy * dy + dz * dz < 125000.0f) {
				ret[4] = head.X;
				ret[5] = head.Y;
				ret[6] = head.Z;
			}
			else {
				head.Z -= 16.0f;
				root.Z += 45.0f;

				auto y = SpoofRuntime::atan2f_(head.Y - root.Y, head.X - root.X);

				root.X += SpoofRuntime::cosf_(y + 1.5708f) * 32.0f;
				root.Y += SpoofRuntime::sinf_(y + 1.5708f) * 32.0f;

				auto length = SpoofRuntime::sqrtf_(SpoofRuntime::powf_(head.X - root.X, 2) + SpoofRuntime::powf_(head.Y - root.Y, 2));
				auto x = -SpoofRuntime::atan2f_(head.Z - root.Z, length);
				y = SpoofRuntime::atan2f_(head.Y - root.Y, head.X - root.X);

				x /= 2.0f;
				y /= 2.0f;

				ret[0] = -(SpoofRuntime::sinf_(x) * SpoofRuntime::sinf_(y));
				ret[1] = SpoofRuntime::sinf_(x) * SpoofRuntime::cosf_(y);
				ret[2] = SpoofRuntime::cosf_(x) * SpoofRuntime::sinf_(y);
				ret[3] = SpoofRuntime::cosf_(x) * SpoofRuntime::cosf_(y);
			}
		}

	}


	return ret;
}


bool HookFunctions::Init(bool NoSpread, bool CalcShot) {
	if (!NoSpreadInitialized) {
		if (NoSpread) {
			auto SpreadAddr = MemoryHelper::PatternScan(xorstr("E8 ? ? ? ? 48 8D 4B 28 E8 ? ? ? ? 48 8B C8"));
			SpreadAddr = RVA(SpreadAddr, 5);
			HookHelper::InsertHook(SpreadAddr, (uintptr_t)SpreadHook, (uintptr_t)&Spread);
			SpreadCaller = (PVOID)(MemoryHelper::PatternScan(xorstr("0F 57 D2 48 8D 4C 24 ? 41 0F 28 CC E8 ? ? ? ? 48 8B 4D B0 0F 28 F0 48 85 C9")));
			NoSpreadInitialized = true;
		}
	}
	if (!CalcShotInitialized) {
		if (CalcShot) {
			auto CalcShotAddr = MemoryHelper::PatternScan(xorstr("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 44 0F 29 A0 ? ? ? ? 44 0F 29 A8 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B F1"));
			HookHelper::InsertHook(CalcShotAddr, (uintptr_t)CalculateShotHook, (uintptr_t)&CalculateShot);
			CalcShotInitialized = true;
		}
	}
	return true;
}



bool InitializeHack()
{
	Renderer_Defines::Width = LI_FN(GetSystemMetrics)(SM_CXSCREEN);
	Renderer_Defines::Height = LI_FN(GetSystemMetrics)(SM_CYSCREEN);
	UWorld = MemoryHelper::PatternScan("48 89 05 ? ? ? ? 48 8B 4B 78");
	UWorld = RVA(UWorld, 7);

	FreeFn = MemoryHelper::PatternScan(xorstr("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0"));
	ProjectWorldToScreen = MemoryHelper::PatternScan(xorstr("E8 ? ? ? ? 48 8B 5C 24 ? 41 88 07 48 83 C4 30"));
	ProjectWorldToScreen = RVA(ProjectWorldToScreen, 5);


	LineOfS = MemoryHelper::PatternScan(xorstr("48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 45 8A E9"));
	LineOfS = RVA(LineOfS, 5);

	GetNameByIndex = MemoryHelper::PatternScan(xorstr("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 8B F2 4C 8B F1 E8 ? ? ? ? 45 8B 06 33 ED 41 0F B7 16 41 C1 E8 10 89 54 24 24 44 89 44 24 ? 48 8B 4C 24 ? 48 C1 E9 20 8D 3C 09 4A 03 7C C0 ? 0F B7 17 C1 EA 06 41 39 6E 04"));
	BoneMatrix = MemoryHelper::PatternScan(xorstr("E8 ? ? ? ? 48 8B 47 30 F3 0F 10 45"));
	BoneMatrix = RVA(BoneMatrix, 5);



	NtGetAsyncKeyState = (LPFN_MBA)LI_FN(GetProcAddress)(LI_FN(GetModuleHandleA)(xorstr("win32u.dll")), xorstr("NtUserGetAsyncKeyState"));


	auto level = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC sd;
	{
		ZeroMemory(&sd, sizeof sd);
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = LI_FN(FindWindowA)(xorstr("UnrealWindow"), xorstr("Fortnite  "));
		sd.SampleDesc.Count = 1;
		sd.Windowed = TRUE;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	IDXGISwapChain* swap_chain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;

	LI_FN(D3D11CreateDeviceAndSwapChain)(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &level, 1, D3D11_SDK_VERSION, &sd, &swap_chain, &device, nullptr, &context);

	auto* swap_chainvtable = reinterpret_cast<uintptr_t*>(swap_chain);
	swap_chainvtable = reinterpret_cast<uintptr_t*>(swap_chainvtable[0]);

	DWORD old_protect;
	present_original = reinterpret_cast<present_fn>(reinterpret_cast<DWORD_PTR*>(swap_chainvtable[8]));
	LI_FN(VirtualProtect)(swap_chainvtable, 0x1000, PAGE_EXECUTE_READWRITE, &old_protect);
	swap_chainvtable[8] = reinterpret_cast<DWORD_PTR>(present_hooked);
	LI_FN(VirtualProtect)(swap_chainvtable, 0x1000, old_protect, &old_protect);

	DWORD old_protect_resize;
	resize_original = reinterpret_cast<resize_fn>(reinterpret_cast<DWORD_PTR*>(swap_chainvtable[13]));
	LI_FN(VirtualProtect)(swap_chainvtable, 0x1000, PAGE_EXECUTE_READWRITE, &old_protect_resize);
	swap_chainvtable[13] = reinterpret_cast<DWORD_PTR>(resize_hooked);
	LI_FN(VirtualProtect)(swap_chainvtable, 0x1000, old_protect_resize, &old_protect_resize);
	
	// Call this here or somewhere else!
	return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		InitializeHack();
	}
	return TRUE;
}
