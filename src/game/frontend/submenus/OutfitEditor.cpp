#include "OutfitEditor.hpp"

#include "core/frontend/manager/UIManager.hpp"
#include "game/backend/Self.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/gta/Natives.hpp"
#include "imgui.h"

namespace YimMenu::Submenus
{
	std::shared_ptr<YimMenu::Category> OutfitEditor::CreateCategory()
	{
		auto category = std::make_shared<YimMenu::Category>("Edit Outfits and Props");

		category->AddItem(std::make_shared<ImGuiItem>([] {
			auto ped = Self::GetPed();
			if (!ENTITY::DOES_ENTITY_EXIST(static_cast<int>(ped.GetHandle())))
			{
				ImGui::TextDisabled("Error: Player not found");
				return;
			}

			// Outfit Slots
			static const char* outfitSlotNames[] = {"Head", "Masks", "Hair", "Torso", "Legs", "Bags/Parachutes", "Shoes", "Accessories", "Undershirts", "Body Armor", "Decals", "Tops 2"};

			for (int i = 0; i < 12; ++i)
			{
				int drawable, texture;
				OutfitEditor::GetOutfitSlot(i, drawable, texture);

				const int maxDrawable = OutfitEditor::GetMaxDrawable(i);
				if (maxDrawable <= 0)
				{
					ImGui::TextDisabled("%s (Slot %d): No options available", outfitSlotNames[i], i);
					continue;
				}

				int maxTexture = OutfitEditor::GetMaxTexture(i, drawable);

				ImGui::Text("%s (Slot %d)", outfitSlotNames[i], i);
				if (ImGui::SliderInt(std::format("{} Drawable##{}", outfitSlotNames[i], i).c_str(), &drawable, 0, maxDrawable - 1))
				{
					drawable = std::clamp(drawable, 0, maxDrawable - 1);
					OutfitEditor::SetOutfitSlot(i, drawable, texture);
					maxTexture = OutfitEditor::GetMaxTexture(i, drawable);
				}
				if (maxTexture > 0 && ImGui::SliderInt(std::format("{} Texture##{}", outfitSlotNames[i], i).c_str(), &texture, 0, maxTexture - 1))
				{
					texture = std::clamp(texture, 0, maxTexture - 1);
					OutfitEditor::SetOutfitSlot(i, drawable, texture);
				}
			}

			// Prop Slots
			static const char* propNames[] = {"Hat", "Glasses", "Earrings", nullptr, nullptr, nullptr, "Watches", "Bracelets"};

			for (int i : {0, 1, 2, 6, 7})
			{
				int drawable, texture;
				OutfitEditor::GetPropSlot(i, drawable, texture);

				const int maxDrawable = OutfitEditor::GetMaxPropDrawable(i);
				if (maxDrawable <= 0)
				{
					ImGui::TextDisabled("%s (Slot %d): No options available", propNames[i], i);
					continue;
				}

				int maxTexture = OutfitEditor::GetMaxPropTexture(i, drawable);

				if (propNames[i]) // Skip nullptr entries
				{
					ImGui::Text("%s (Slot %d)", propNames[i], i);
					if (ImGui::SliderInt(std::format("{} Drawable##{}", propNames[i], i).c_str(), &drawable, 0, maxDrawable - 1))
					{
						drawable = std::clamp(drawable, 0, maxDrawable - 1);
						OutfitEditor::SetPropSlot(i, drawable, texture);
						maxTexture = OutfitEditor::GetMaxPropTexture(i, drawable);
					}
					if (maxTexture > 0 && ImGui::SliderInt(std::format("{} Texture##{}", propNames[i], i).c_str(), &texture, 0, maxTexture - 1))
					{
						texture = std::clamp(texture, 0, maxTexture - 1);
						OutfitEditor::SetPropSlot(i, drawable, texture);
					}
				}
			}
		}));

		return category;
	}

	void OutfitEditor::SetOutfitSlot(int slot, int drawable, int texture)
	{
		auto ped = Self::GetPed();
		if (!ENTITY::DOES_ENTITY_EXIST(static_cast<int>(ped.GetHandle())))
		{
			return; // Invalid ped handle
		}

		int maxDrawable = GetMaxDrawable(slot);
		int maxTexture  = GetMaxTexture(slot, drawable);

		if (drawable < 0 || drawable >= maxDrawable || texture < 0 || texture >= maxTexture)
		{
			return; // Invalid drawable or texture
		}

		PED::SET_PED_COMPONENT_VARIATION(static_cast<int>(ped.GetHandle()), slot, drawable, texture, 0);
	}

	void OutfitEditor::SetPropSlot(int slot, int drawable, int texture)
	{
		auto ped = Self::GetPed();
		if (!ENTITY::DOES_ENTITY_EXIST(static_cast<int>(ped.GetHandle())))
		{
			return; // Invalid ped handle
		}

		int maxDrawable = GetMaxPropDrawable(slot);
		int maxTexture  = GetMaxPropTexture(slot, drawable);

		if (drawable < 0 || drawable >= maxDrawable || texture < 0 || texture >= maxTexture)
		{
			return; // Invalid drawable or texture
		}

		PED::SET_PED_PROP_INDEX(static_cast<int>(ped.GetHandle()), slot, drawable, texture, true, 0);
	}

	void OutfitEditor::GetOutfitSlot(int slot, int& drawable, int& texture)
	{
		auto ped = Self::GetPed();
		drawable = PED::GET_PED_DRAWABLE_VARIATION(static_cast<int>(ped.GetHandle()), slot);
		texture  = PED::GET_PED_TEXTURE_VARIATION(static_cast<int>(ped.GetHandle()), slot);
	}

	void OutfitEditor::GetPropSlot(int slot, int& drawable, int& texture)
	{
		auto ped = Self::GetPed();
		drawable = PED::GET_PED_PROP_INDEX(static_cast<int>(ped.GetHandle()), slot, 0);
		if (drawable == -1)
		{
			drawable = 0;
			texture  = 0;
			return;
		}
		texture = PED::GET_PED_PROP_TEXTURE_INDEX(static_cast<int>(ped.GetHandle()), slot);
	}

	int OutfitEditor::GetMaxDrawable(int slot)
	{
		auto ped = Self::GetPed();
		return PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(static_cast<int>(ped.GetHandle()), slot);
	}

	int OutfitEditor::GetMaxTexture(int slot, int drawable)
	{
		auto ped = Self::GetPed();
		return PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(static_cast<int>(ped.GetHandle()), slot, drawable);
	}

	int OutfitEditor::GetMaxPropDrawable(int slot)
	{
		auto ped = Self::GetPed();
		return PED::GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(static_cast<int>(ped.GetHandle()), slot);
	}

	int OutfitEditor::GetMaxPropTexture(int slot, int drawable)
	{
		auto ped = Self::GetPed();
		return PED::GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(static_cast<int>(ped.GetHandle()), slot, drawable);
	}
}
