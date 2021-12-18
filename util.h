#pragma once

#define PI (3.141592653589793f)
#define RELATIVE_ADDR(addr, size) ((PBYTE)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))

#define ReadPointer(base, offset) (*(PVOID *)(((PBYTE)base + offset)))
#define ReadUint64(base, offset) (*(uintptr_t**)(((PBYTE)base + offset)))

#define ReadBool(base, offset) (*(bool *)(((PBYTE)base + offset)))

#define ReadVector2D(base, offset) (*(FVector2D *)(((PBYTE)base + offset)))
#define ReadFVector(base, offset) (*(FVector *)(((PBYTE)base + offset)))

#define ReadInt(base, offset) (*(int *)(((PBYTE)base + offset)))
#define ReadFloat(base, offset) (*(float *)(((PBYTE)base + offset)))

#define ReadDWORD(base, offset) (*(PDWORD)(((PBYTE)base + offset)))
#define ReadBYTE(base, offset) (*(((PBYTE)base + offset)))
