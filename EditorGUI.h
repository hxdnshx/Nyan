#pragma once


#include "MinimalAllocator.hpp"

#define MINIMAL_USE_PROCESSHEAPSTRING
#include "MinimalPath.hpp"

#define MINIMAL_USE_PROCESSHEAPARRAY
#include "MinimalArray.hpp"

#include "../../../Src/nnnEngine/nnn.h"

namespace Nyan
{
	class EditorGUI
	{
		class NNN::GUI::c_GUI_System			*g_gui;
		class NNN::GUI::Controls::c_Button	*g_btnEdit;
	};
}