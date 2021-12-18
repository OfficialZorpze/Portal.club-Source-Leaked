#pragma once

namespace overlay
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
	// Drawings for custom menus + Toggle Button's / Checkboxes ETC
	void RegularRGBText(int x, int y, ImColor color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImColor(color), utf_8_2.c_str());
	}
	void Test(int x, int y, ImColor color, const char* str)
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
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
	}
	void Rect(int x, int y, int w, int h, ImColor color, int thickness)
	{
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0, thickness);
	}
	void FilledRect(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void OutlinedString(ImGuiWindow& windowshit, std::string str, ImVec2 loc, ImU32 colr, bool centered = false)
	{
		ImVec2 size = { 0,0 };
		float minx = 0;
		float miny = 0;
		if (centered)
		{
			size = ImGui::GetFont()->CalcTextSizeA(windowshit.DrawList->_Data->FontSize, 0x7FFFF, 0, str.c_str());
			minx = size.x / 2.f;
			miny = size.y / 2.f;
		}

		windowshit.DrawList->AddText(ImVec2((loc.x - 1) - minx, (loc.y - 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
		windowshit.DrawList->AddText(ImVec2((loc.x + 1) - minx, (loc.y + 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
		windowshit.DrawList->AddText(ImVec2((loc.x + 1) - minx, (loc.y - 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
		windowshit.DrawList->AddText(ImVec2((loc.x - 1) - minx, (loc.y + 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
		windowshit.DrawList->AddText(ImVec2(loc.x - minx, loc.y - miny), colr, str.c_str());
	}
}

// Watermark and Speed

void DrawWaterMark(ImGuiWindow& windowshit, std::string str, ImVec2 loc, ImU32 colr, bool centered = false)
{
	ImVec2 size = { 0,0 };
	float minx = 0;
	float miny = 0;
	if (centered)
	{
		size = ImGui::GetFont()->CalcTextSizeA(windowshit.DrawList->_Data->FontSize, 0x7FFFF, 0, str.c_str());
		minx = size.x / 2.f;
		miny = size.y / 2.f;
	}

	windowshit.DrawList->AddText(ImVec2((loc.x - 1) - minx, (loc.y - 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
	windowshit.DrawList->AddText(ImVec2((loc.x + 1) - minx, (loc.y + 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
	windowshit.DrawList->AddText(ImVec2((loc.x + 1) - minx, (loc.y - 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
	windowshit.DrawList->AddText(ImVec2((loc.x - 1) - minx, (loc.y + 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
	windowshit.DrawList->AddText(ImVec2(loc.x - minx, loc.y - miny), colr, str.c_str());
}


bool LootESP(ImDrawList* Renderer, const char* name, uintptr_t CurrentActor, Vector3 LocalRelativeLocation) {

	if (SDK::Utils::CheckItemInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

		uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
		Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
		float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 20.f;

		std::string null = xorstr("");

		if (strstr(name, xorstr("FortPickupAthena")) && Settings::LootESP) {


		//if (ItemDist < Settings::MaxESPDistance) {
				auto item = read<uintptr_t>(CurrentActor + StaticOffsets::PrimaryPickupItemEntry + StaticOffsets::ItemDefinition);
				if (!item) return false;

				ImU32 ItemColor;
				auto itemName = reinterpret_cast<SDK::Structs::FText*>(read<uintptr_t>(item + StaticOffsets::DisplayName));
				if (!itemName || !itemName->c_str()) return false;
				auto isAmmo = strstr(itemName->c_str(), xorstr("Ammo: "));

				CHAR text[0xFF] = { 0 };
				uintptr_t CurGay = read<uintptr_t>(CurrentActor + StaticOffsets::CurrentWeapon);
				uintptr_t CurRootGay = read<uintptr_t>(CurGay + StaticOffsets::WeaponData);
				//uintptr_t bWTier = read<uintptr_t>(CurRootGay + StaticOffsets::Tier);
				wcstombs(text, itemName->c_wstr() + (isAmmo ? 6 : 0), sizeof(text));

				if (ReadBYTE(item, StaticOffsets::Tier) == 0)
						{
							ItemColor = ImGui::GetColorU32({ 255, 255, 255, 255 });
						}
						else if (ReadBYTE(item, StaticOffsets::Tier) == 1)
						{
							ItemColor = ImGui::GetColorU32({ 255, 255, 255, 255 }); //common
						}
						else if (ReadBYTE(item, StaticOffsets::Tier) == 2)
						{
							ItemColor = ImGui::GetColorU32({ 0, 255, 0, 255 }); //uncommon
						}
						else if (ReadBYTE(item, StaticOffsets::Tier) == 3)
						{
							ItemColor = ImGui::GetColorU32({ 0, 94, 255, 255 }); //rare
						}
						else if (ReadBYTE(item, StaticOffsets::Tier) == 4)
						{
							ItemColor = ImGui::GetColorU32({ 149, 0, 255, 255 }); //epic
						}
						else if (ReadBYTE(item, StaticOffsets::Tier) == 5)
						{
							ItemColor = ImGui::GetColorU32({ 255, 145, 0, 255 }); //legendary
						}
						else if (ReadBYTE(item, StaticOffsets::Tier) == 6)
						{
							ItemColor = ImGui::GetColorU32({ 255, 255, 255, 255 });
						}
						else if (ReadBYTE(item, StaticOffsets::Tier) == 7)
						{
							ItemColor = ImGui::GetColorU32({ 255, 255, 255, 255 });
						}
						else if (ReadBYTE(item, StaticOffsets::Tier) == 8)
						{
							ItemColor = ImGui::GetColorU32({ 255, 255, 255, 255 });
						}
						else if (ReadBYTE(item, StaticOffsets::Tier) == 9)
						{
							ItemColor = ImGui::GetColorU32({ 255, 255, 255, 255 });
						}
						else if (ReadBYTE(item, StaticOffsets::Tier) == 10)
						{
							ItemColor = ImGui::GetColorU32({ 255, 255, 255, 255 });
						}

				Vector3 LootPosition;

				SDK::Classes::AController::WorldToScreen(ItemPosition, &LootPosition);


				std::string Text = null + text + xorstr(" \n") + std::to_string((int)ItemDist) + xorstr("m");

				ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

				overlay::ShadowRGBText(LootPosition.x - TextSize.x / 2, LootPosition.y - TextSize.y / 2, ItemColor, Text.c_str());
				//Renderer->AddText(ImVec2(LootPosition.x - TextSize.x / 2, LootPosition.y - TextSize.y / 2), ItemColor, Text.c_str());
			}
		}

	}