#include "facade/EasyGUI.h"

namespace facade
{

CU_SINGLETON_DEFINITION(EasyGUI);

EasyGUI::EasyGUI()
{
	egui::style_colors_dark(ctx.style);
}

}