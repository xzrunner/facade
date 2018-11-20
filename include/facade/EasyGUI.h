#pragma once

#include <cu/cu_macro.h>
#include <easygui/Context.h>
#include <easygui/EntityFactory.h>
#include <easygui/CompStorage.h>

namespace facade
{

class EasyGUI
{
public:
	egui::Context       ctx;
	egui::EntityFactory factory;
	egui::CompStorage   storage;

public:
	

private:
	CU_SINGLETON_DECLARATION(EasyGUI)

}; // EasyGUI

}