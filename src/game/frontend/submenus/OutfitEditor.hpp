#pragma once
#include "core/frontend/manager/Category.hpp"

namespace YimMenu
{
    int GetMaxDrawable(int slot);
    int GetMaxTexture(int slot, int drawable);
    int GetMaxPropDrawable(int slot);
    int GetMaxPropTexture(int slot, int drawable);
    void GetOutfitSlot(int slot, int& drawable, int& texture);
    void GetPropSlot(int slot, int& drawable, int& texture);
    void SetOutfitSlot(int slot, int drawable, int texture);
    void SetPropSlot(int slot, int drawable, int texture);
    
    std::shared_ptr<Category> CreateOutfitsMenu();
}
