#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <iostream>
#include "Defines.h"
#include "SDK.h"
#include "detours.h"

class UClass {
public:
	BYTE _padding_0[0x40];
	UClass* SuperClass;
};

class UObject {
public:
	PVOID VTableObject;
	DWORD ObjectFlags;
	DWORD InternalIndex;
	UClass* Class;
	BYTE _padding_0[0x8];
	UObject* Outer;

	inline BOOLEAN IsA(PVOID parentClass) {
		for (auto super = this->Class; super; super = super->SuperClass) {
			if (super == parentClass) {
				return TRUE;
			}
		}

		return FALSE;
	}
};

class FUObjectItem {
public:
	UObject* Object;
	DWORD Flags;
	DWORD ClusterIndex;
	DWORD SerialNumber;
	DWORD SerialNumber2;
};

class TUObjectArray {
public:
	FUObjectItem* Objects[9];
};

class GObjects {
public:
	TUObjectArray* ObjectArray;
	BYTE _padding_0[0xC];
	DWORD ObjectCount;
};

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

typedef struct {
	float X, Y, Z;
} FVector;

typedef struct {
	float X, Y;
} FVector2D;

typedef struct {
	float Pitch;
	float Yaw;
	float Roll;
} FRotator;

typedef struct {
	FVector Location;
	FRotator Rotation;
	float FOV;
	float OrthoWidth;
	float OrthoNearClipPlane;
	float OrthoFarClipPlane;
	float AspectRatio;
} FMinimalViewInfo;

typedef struct {
	float M[4][4];
} FMatrix;

typedef struct {
	FVector ViewOrigin;
	char _padding_0[4];
	FMatrix ViewRotationMatrix;
	FMatrix ProjectionMatrix;
} FSceneViewProjectionData;

typedef struct {
	FVector Origin;
	FVector BoxExtent;
	float SphereRadius;
} FBoxSphereBounds;
template<typename T>
static T* FindObject(const std::string& name)
{
	for (int i = 0; i < GetGlobalObjects().Num(); ++i)
	{
		auto object = GetGlobalObjects().GetByIndex(i);

		if (object == nullptr)
		{
			continue;
		}

		if (object->GetFullName() == name)
		{
			return static_cast<T*>(object);
		}
	}
	return nullptr;
}

static UClass* FindClass(const std::string& name)
{
	return FindObject<UClass>(name);
}
class UField : public UObject
{
public:
	class UField* Next;                                                     // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY

	static UClass* StaticClass()
	{
		static UClass* ptr = NULL;
		if (!ptr)
			ptr = FindClass(("Class CoreUObject.Field"));

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
			ptr = FindClass(("Class CoreUObject.Struct"));

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
struct FHitResult {
	char bBlockingHit : 1;
	char pad_5E[0x2];
};
struct FVector
{
	float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              Z;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

	inline FVector()
		: X(0), Y(0), Z(0)
	{ }

	inline FVector(float x, float y, float z)
		: X(x),
		Y(y),
		Z(z)
	{ }

	__forceinline FVector operator-(const FVector& V) {
		return FVector(X - V.X, Y - V.Y, Z - V.Z);
	}

	__forceinline FVector operator+(const FVector& V) {
		return FVector(X + V.X, Y + V.Y, Z + V.Z);
	}

	__forceinline FVector operator*(float Scale) const {
		return FVector(X * Scale, Y * Scale, Z * Scale);
	}

	__forceinline FVector operator/(float Scale) const {
		const float RScale = 1.f / Scale;
		return FVector(X * RScale, Y * RScale, Z * RScale);
	}

	__forceinline FVector operator+(float A) const {
		return FVector(X + A, Y + A, Z + A);
	}

	__forceinline FVector operator-(float A) const {
		return FVector(X - A, Y - A, Z - A);
	}

	__forceinline FVector operator*(const FVector& V) const {
		return FVector(X * V.X, Y * V.Y, Z * V.Z);
	}

	__forceinline FVector operator/(const FVector& V) const {
		return FVector(X / V.X, Y / V.Y, Z / V.Z);
	}

	__forceinline float operator|(const FVector& V) const {
		return X * V.X + Y * V.Y + Z * V.Z;
	}

	__forceinline float operator^(const FVector& V) const {
		return X * V.Y - Y * V.X - Z * V.Z;
	}

	__forceinline FVector& operator+=(const FVector& v) {
		(*this);
		(v);
		X += v.X;
		Y += v.Y;
		Z += v.Z;
		return *this;
	}

	__forceinline FVector& operator-=(const FVector& v) {
		(*this);
		(v);
		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;
		return *this;
	}

	__forceinline FVector& operator*=(const FVector& v) {
		(*this);
		(v);
		X *= v.X;
		Y *= v.Y;
		Z *= v.Z;
		return *this;
	}

	__forceinline FVector& operator/=(const FVector& v) {
		(*this);
		(v);
		X /= v.X;
		Y /= v.Y;
		Z /= v.Z;
		return *this;
	}

	__forceinline bool operator==(const FVector& src) const {
		(src);
		(*this);
		return (src.X == X) && (src.Y == Y) && (src.Z == Z);
	}

	__forceinline bool operator!=(const FVector& src) const {
		(src);
		(*this);
		return (src.X != X) || (src.Y != Y) || (src.Z != Z);
	}

	__forceinline float Size() const {
		return sqrt(X * X + Y * Y + Z * Z);
	}

	__forceinline float Size2D() const {
		return sqrt(X * X + Y * Y);
	}

	__forceinline float SizeSquared() const {
		return X * X + Y * Y + Z * Z;
	}

	__forceinline float SizeSquared2D() const {
		return X * X + Y * Y;
	}

	__forceinline float Dot(const FVector& vOther) const {
		const FVector& a = *this;

		return (a.X * vOther.X + a.Y * vOther.Y + a.Z * vOther.Z);
	}

	__forceinline float DistanceFrom(const FVector& Other) const {
		const FVector& a = *this;
		float dx = (a.X - Other.X);
		float dy = (a.Y - Other.Y);
		float dz = (a.Z - Other.Z);

		return (sqrt((dx * dx) + (dy * dy) + (dz * dz)));
	}

	__forceinline FVector Normalize() {
		FVector vector;
		float length = this->Size();

		if (length != 0) {
			vector.X = X / length;
			vector.Y = Y / length;
			vector.Z = Z / length;
		}
		else
			vector.X = vector.Y = 0.0f;
		vector.Z = 1.0f;

		return vector;
	}

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

namespace teleport
{
	UFunction* FindFunction(const char* memes)
	{
		return FindObject<UFunction>(memes);
	}
	bool K2_SetActorLocation(UObject* a, const struct FVector& NewLocation, bool bSweep, bool bTeleport, struct FHitResult* SweepHitResult)
	{
		static UFunction* fn = nullptr;
		if (!fn) fn = FindFunction(("Function Engine.Actor.K2_SetActorLocation"));

		// Function Engine.Actor.K2_SetActorLocation
		struct AActor_K2_SetActorLocation_Params
		{
			struct FVector									   NewLocation;
			bool                                               bSweep;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
			struct FHitResult								   SweepHitResult;
			bool                                               bTeleport;                                                // (Parm, ZeroConstructor, IsPlainOldData)
			bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
		};
		AActor_K2_SetActorLocation_Params params;

		params.NewLocation = NewLocation;
		params.bSweep = bSweep;
		params.bTeleport = bTeleport;

		auto flags = fn->FunctionFlags;

		ProcessEvent(a, fn, &params);

		fn->FunctionFlags = flags;

		if (SweepHitResult != nullptr)
			*SweepHitResult = params.SweepHitResult;

		return params.ReturnValue;
	}

	auto name = SDK::Classes::UObject::GetObjectName(CurrentActor);
	uintptr_t Uworld;
	bool Readtard = false;

	ImDrawList* Renderer = ImGui::GetOverlayDrawList();
	Vector3 LocalRelativeLocation;

	void TeleportToMarker()
	{
		for (int li = 0; li < read<DWORD>(Uworld + 0x140 + sizeof(PVOID)); ++li) {
			DWORD_PTR levels = read<uintptr_t>(Uworld + 0x140);
			if (!levels) break;

			DWORD_PTR level = read<uintptr_t>(levels + li * sizeof(PVOID));

			if (!level) std::cout << "Failed LEVEL\n";

			auto lc = read<DWORD>(level + StaticOffsets::AActors + sizeof(PVOID));

			DWORD_PTR AActorsLEVE = read<uintptr_t>(level + StaticOffsets::AActors);

			for (int ai = 0; ai < lc; ++ai)
				{
						UObject* Acotr = read<UObject*>(AActorsLEVE + ai * sizeof(PVOID));
						//Get Name
						if (strstr(name, ("AthenaPlayerMarker")) != std::string::npos)
						{
							auto Rootcomp = read<uintptr_t>(CurrentActor + 0x130);
							auto Pos = read<Vector3>(Rootcomp + 0x11C);
							FHitResult xxxx;
							K2_SetActorLocation(Acotr, Pos, false, true, &xxxx);
							}
						}
					}
				
			
				
	