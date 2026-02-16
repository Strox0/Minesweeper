#include <iostream>
#include <Windows.h>
#include "WndManager.h"

#include "Menu.h"
#include "Grid.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	IMAF::AppProperties props;
	props.center_window = true;
	props.custom_titlebar = true;
	props.dpi_aware = true;
	props.gen_ini = false;
	props.name = "Minesweeper";
	props.resizeable = true;
	props.imgui_docking = false;
	props.relative_size = true;
	props.height.relative = 0.45;
	props.width.relative = 0.45;
	props.font_size = 23;
	
	props.custom_titlebar_props.top_border = IMAF::TopBorder::Thin;

	MS::WndMgr wndmgr(props);

	wndmgr.Run();

	return 0;
}