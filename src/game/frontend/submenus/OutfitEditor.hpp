#pragma once
#include <memory>
#include "core/frontend/manager/Category.hpp"

namespace YimMenu::Submenus
{
	class OutfitEditor
	{
	public:
		static std::shared_ptr<YimMenu::Category> CreateCategory();

		static void SetOutfitSlot(int slot, int drawable, int texture);
		static void SetPropSlot(int slot, int drawable, int texture);
		static void GetOutfitSlot(int slot, int& drawable, int& texture);
		static void GetPropSlot(int slot, int& drawable, int& texture);
		static int GetMaxDrawable(int slot);
		static int GetMaxTexture(int slot, int drawable);
		static int GetMaxPropDrawable(int slot);
		static int GetMaxPropTexture(int slot, int drawable);
	};
}
