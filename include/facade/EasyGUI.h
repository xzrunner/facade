#pragma once

#include <cu/cu_macro.h>
#include <easygui/GuiState.h>
#include <easygui/EntityFactory.h>
#include <easygui/CompStorage.h>
#include <easygui/RenderStyle.h>

namespace facade
{

class EasyGUI
{
public:
	egui::GuiState      state;
	egui::EntityFactory factory;
	egui::CompStorage   storage;
	egui::RenderStyle   style;

	CU_SINGLETON_DECLARATION(EasyGUI)

}; // EasyGUI

}