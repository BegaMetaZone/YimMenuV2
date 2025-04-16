#include "OutfitEditor.hpp"
#include "core/frontend/manager/UIManager.hpp"
#include "game/backend/Self.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/gta/Natives.hpp"
#include "imgui.h"
#include <random>
#include <format>

namespace YimMenu
{
    int GetMaxDrawable(int slot)
    {
        auto ped = Self::GetPed();
        return PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(static_cast<int>(ped.GetHandle()), slot);
    }

    int GetMaxTexture(int slot, int drawable)
    {
        auto ped = Self::GetPed();
        return PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(static_cast<int>(ped.GetHandle()), slot, drawable);
    }

    int GetMaxPropDrawable(int slot)
    {
        auto ped = Self::GetPed();
        return PED::GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(static_cast<int>(ped.GetHandle()), slot);
    }

    int GetMaxPropTexture(int slot, int drawable)
    {
        auto ped = Self::GetPed();
        return PED::GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(static_cast<int>(ped.GetHandle()), slot, drawable);
    }

    void GetOutfitSlot(int slot, int& drawable, int& texture)
    {
        auto ped = Self::GetPed();
        drawable = PED::GET_PED_DRAWABLE_VARIATION(static_cast<int>(ped.GetHandle()), slot);
        texture = PED::GET_PED_TEXTURE_VARIATION(static_cast<int>(ped.GetHandle()), slot);
    }

    void GetPropSlot(int slot, int& drawable, int& texture)
    {
        auto ped = Self::GetPed();
        drawable = PED::GET_PED_PROP_INDEX(static_cast<int>(ped.GetHandle()), slot, 0);
        if (drawable == -1)
        {
            drawable = 0;
            texture = 0;
            return;
        }
        texture = PED::GET_PED_PROP_TEXTURE_INDEX(static_cast<int>(ped.GetHandle()), slot);
    }

    void SetOutfitSlot(int slot, int drawable, int texture)
    {
        auto ped = Self::GetPed();
        if (!ENTITY::DOES_ENTITY_EXIST(static_cast<int>(ped.GetHandle())))
            return;

        int maxDrawable = GetMaxDrawable(slot);
        int maxTexture = GetMaxTexture(slot, drawable);

        if (drawable < 0 || drawable >= maxDrawable || texture < 0 || texture >= maxTexture)
            return;

        PED::SET_PED_COMPONENT_VARIATION(static_cast<int>(ped.GetHandle()), slot, drawable, texture, 0);
    }

    void SetPropSlot(int slot, int drawable, int texture)
    {
        auto ped = Self::GetPed();
        if (!ENTITY::DOES_ENTITY_EXIST(static_cast<int>(ped.GetHandle())))
            return;

        int maxDrawable = GetMaxPropDrawable(slot);
        int maxTexture = GetMaxPropTexture(slot, drawable);

        if (drawable < 0 || drawable >= maxDrawable || texture < 0 || texture >= maxTexture)
            return;

        PED::SET_PED_PROP_INDEX(static_cast<int>(ped.GetHandle()), slot, drawable, texture, true, 0);
    }

    // Helper function for underlined text
    static void TextUnderlined(const char* text)
    {
        ImGui::Text("%s", text);
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        min.y = max.y;
        ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Text]));
    }

    std::shared_ptr<Category> CreateOutfitsMenu()
    {
        auto category = std::make_shared<Category>("Outfit Editor");

        category->AddItem(std::make_shared<ImGuiItem>([] {
            auto ped = Self::GetPed();
            if (!ENTITY::DOES_ENTITY_EXIST(static_cast<int>(ped.GetHandle())))
            {
                ImGui::TextDisabled("Error: Player not found");
                return;
            }

            if (ImGui::Button("Randomize Outfit"))
            {
                std::random_device rd;
                std::mt19937 gen(rd());

                // Randomize components
                for (int i = 0; i < 12; ++i)
                {
                    int maxDrawable = GetMaxDrawable(i);
                    if (maxDrawable > 0)
                    {
                        int drawable = std::uniform_int_distribution<>(0, maxDrawable - 1)(gen);
                        int maxTexture = GetMaxTexture(i, drawable);
                        int texture = maxTexture > 0 ? std::uniform_int_distribution<>(0, maxTexture - 1)(gen) : 0;
                        SetOutfitSlot(i, drawable, texture);
                    }
                }

                // Randomize props
                for (int i : {0, 1, 2, 6, 7})
                {
                    int maxDrawable = GetMaxPropDrawable(i);
                    if (maxDrawable > 0)
                    {
                        int drawable = std::uniform_int_distribution<>(0, maxDrawable - 1)(gen);
                        int maxTexture = GetMaxPropTexture(i, drawable);
                        int texture = maxTexture > 0 ? std::uniform_int_distribution<>(0, maxTexture - 1)(gen) : 0;
                        SetPropSlot(i, drawable, texture);
                    }
                }
            }

            // Two-column layout
            const float windowWidth = ImGui::GetContentRegionAvail().x;
            const float columnWidth = windowWidth / 2.0f - 5.0f;

            ImGui::BeginGroup(); // Components column
            TextUnderlined("Components");

            // Components (left column)
            const struct {
                const char* name;
                int slot;
            } componentSlots[] = {
                {"Top", 11},
                {"Undershirt", 8},
                {"Legs", 4},
                {"Feet", 6},
                {"Accessories", 7},
                {"Bags", 5},
                {"Mask", 1},
                {"Gloves", 3},
                {"Decals", 10},
                {"Armor", 9}
            };

            for (const auto& component : componentSlots)
            {
                int drawable, texture;
                GetOutfitSlot(component.slot, drawable, texture);

                const int maxDrawable = GetMaxDrawable(component.slot);
                if (maxDrawable <= 0) continue;

                ImGui::Text("%s", component.name);
                ImGui::SameLine(columnWidth / 2);

                ImGui::PushItemWidth(columnWidth / 2 - 5);
                if (ImGui::InputInt(std::format("##{}Drawable", component.name).c_str(), &drawable, 0, 0))
                {
                    drawable = std::clamp(drawable, 0, maxDrawable - 1);
                    SetOutfitSlot(component.slot, drawable, texture);
                }
                ImGui::SameLine();

                int maxTexture = GetMaxTexture(component.slot, drawable);
                if (ImGui::InputInt(std::format("##{}Texture", component.name).c_str(), &texture, 0, 0))
                {
                    texture = std::clamp(texture, 0, maxTexture - 1);
                    SetOutfitSlot(component.slot, drawable, texture);
                }
                ImGui::PopItemWidth();
            }
            ImGui::EndGroup();

            ImGui::SameLine();
            ImGui::GetWindowDrawList()->AddLine(
                ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y),
                ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y),
                ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Separator]));
            ImGui::SameLine();

            ImGui::BeginGroup(); // Props column
            TextUnderlined("Props");

            // Props (right column)
            const struct {
                const char* name;
                int slot;
            } propSlots[] = {
                {"Hats", 0},
                {"Glasses", 1},
                {"Ears", 2},
                {"Watches", 6}
            };

            for (const auto& prop : propSlots)
            {
                int drawable, texture;
                GetPropSlot(prop.slot, drawable, texture);

                const int maxDrawable = GetMaxPropDrawable(prop.slot);
                if (maxDrawable <= 0) continue;

                ImGui::Text("%s", prop.name);
                ImGui::SameLine(columnWidth / 2);

                ImGui::PushItemWidth(columnWidth / 2 - 5);
                if (ImGui::InputInt(std::format("##{}Drawable", prop.name).c_str(), &drawable, 0, 0))
                {
                    drawable = std::clamp(drawable, 0, maxDrawable - 1);
                    SetPropSlot(prop.slot, drawable, texture);
                }
                ImGui::SameLine();

                int maxTexture = GetMaxPropTexture(prop.slot, drawable);
                if (ImGui::InputInt(std::format("##{}Texture", prop.name).c_str(), &texture, 0, 0))
                {
                    texture = std::clamp(texture, 0, maxTexture - 1);
                    SetPropSlot(prop.slot, drawable, texture);
                }
                ImGui::PopItemWidth();
            }
            ImGui::EndGroup();
        }));

        return category;
    }
}
