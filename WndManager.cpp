#include "WndManager.h"
#include "IMAF/fonts.h"

static MS::WndMgr* sp_mgr = nullptr;

MS::WndMgr::WndMgr(const IMAF::AppProperties& props) :
	m_props(props),
	m_app(props),
	m_titlebar_height(40),
	mp_grid(),
	mp_menu(std::make_shared<MS::MenuPanel>(this)),
	mp_win(),
	m_seconds(0),
	m_current_panel(MS::PanelId::Menu),
	m_current_id(0)
{
	m_app.SetUp([this](const IMAF::AppProperties& props, GLFWwindow* window)
		{
			IMAF::Application::SetPrurpleDarkColorTheme();
			IMAF::Titlebar_Properties titlebar_props = props.custom_titlebar_props;
			titlebar_props.height = IMAF::Val(40);
			m_titlebar_height = IMAF::Val(40);
			titlebar_props.AddButton(0, IMAF::ButtonSpec(IMAF::Val(30), IMAF::Val(30), IMAF::Val(5), IMAF::ButtonType::Close));
			titlebar_props.AddButton(0, IMAF::ButtonSpec(IMAF::Val(30), IMAF::Val(30), IMAF::Val(5), IMAF::ButtonType::Maximize));
			titlebar_props.AddButton(0, IMAF::ButtonSpec(IMAF::Val(30), IMAF::Val(30), IMAF::Val(5), IMAF::ButtonType::Minimize), 0.001f, IMAF::GroupAlign::Right, IMAF::Val(5));
			titlebar_props.AddExclusion(IMAF::ExclusionSpec(0, 0, IMAF::Val(75), m_titlebar_height));
			titlebar_props.titlebar_draw_f = MS::WndMgr::_DefCostumTitleBarDraw;
			m_app.SetTitlebarProperties(titlebar_props);
		}
	);

	m_app.AddPanel(mp_menu);
	m_current_id = mp_menu->GetId();
	sp_mgr = this;
}

void MS::WndMgr::SwitchPanel(uint64_t panel_id, PanelId target_panel)
{
	m_current_panel = target_panel;
	switch (target_panel)
	{
	case MS::PanelId::Menu:
		mp_menu = std::make_shared<MS::MenuPanel>(this);
		m_app.AddPanel(mp_menu);
		m_current_id = mp_menu->GetId();
		m_app.RemovePanel(panel_id);
		if (mp_win)
			mp_win.reset();
		if (mp_grid)
			mp_grid.reset();
		break;
	case MS::PanelId::Grid:
		if (!mp_minefield)
			break;
		mp_grid = std::make_shared<MS::GridPanel>(this, mp_minefield);
		m_app.AddPanel(mp_grid);
		m_current_id = mp_grid->GetId();
		m_app.RemovePanel(panel_id);
		if (mp_menu)
			mp_menu.reset();
		if (mp_win)
			mp_win.reset();
		break;
	case MS::PanelId::Win:
		mp_win = std::make_shared<MS::WinPanel>(this, m_seconds);
		m_app.AddPanel(mp_win);
		m_current_id = mp_win->GetId();
		m_app.RemovePanel(panel_id);
		if (mp_grid)
			mp_grid.reset();
		if (mp_menu)
			mp_menu.reset();
		break;
	default:
		break;
	}
}

void MS::WndMgr::SetMinefield(std::shared_ptr<MS::MineField> minefield)
{
	mp_minefield = minefield;
}

void MS::WndMgr::SetTime(long long seconds)
{
	m_seconds = seconds;
}

void MS::WndMgr::SetMineCount(int32_t mines)
{
	m_mines = mines;
}

void MS::WndMgr::Run()
{
	m_app.Run();
}

void MS::WndMgr::DefCostumTitleBarDraw(const IMAF::AppProperties* app_props, GLFWwindow* window)
{
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	int x, y;
	glfwGetWindowPos(window, &x, &y);

	const IMAF::Titlebar_Properties* props = &app_props->custom_titlebar_props;

	ImGui::SetNextWindowSize({ (float)w,(float)props->height });
	ImGui::SetNextWindowPos({ (float)x,(float)y });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, RGB2_IMVEC4(37, 37, 37));
	IMAF::Begin("##CustomTitlebar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking);
	const GLFWcustomtitlebar* p = glfwGetCustomTitlebarProperties(window);

	ImGui::PushStyleColor(ImGuiCol_Button, RGBA2_IMVEC4(37, 37, 37, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 35);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0,0 });

	for (size_t i = 0; i < props->button_groups.size(); i++)
	{
		if (props->button_groups[i].buttons.size() == 0)
			continue;

		float start_pos = 0;

		switch (props->button_groups[i].align)
		{
		case IMAF::GroupAlign::Left:
			start_pos = (float)w * props->button_groups[i].edge_offset;
			break;
		case IMAF::GroupAlign::Right:
			start_pos = w - props->button_groups[i].edge_offset * w - props->button_groups[i].buttons[0].width;
			break;
		case IMAF::GroupAlign::Center:
		{
			int width_sum = 0;

			for (const auto& i : props->button_groups[i].buttons)
			{
				width_sum += i.width;
			}
			width_sum += (props->button_groups[i].buttons.size() - 1) * props->button_groups[i].inner_spacing;
			start_pos = (float)(w - width_sum) / 2.f;
			break;
		}
		}

		for (const auto& btn : props->button_groups[i].buttons)
		{
			ImVec2 button_pos;
			ImVec2 button_size;
			button_pos.x = start_pos;
			button_pos.y = btn.top_offset;
			button_size.x = btn.width;
			button_size.y = btn.height;

			switch (btn.type)
			{
			case IMAF::ButtonType::Close:
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, RGBA2_IMVEC4(195, 65, 65, 255));
				ImGui::SetCursorPos(button_pos);
				ImGui::Button("X", button_size);
				ImGui::PopStyleColor();
				break;
			case IMAF::ButtonType::Minimize:
				ImGui::SetCursorPos(button_pos);
				ImGui::Button("_", button_size);
				break;
			case IMAF::ButtonType::Maximize:
				ImGui::SetCursorPos(button_pos);
				ImGui::Button("O", button_size);
				break;
			default:
				break;
			}

			if (props->button_groups[i].align == IMAF::GroupAlign::Center || props->button_groups[i].align == IMAF::GroupAlign::Left)
				start_pos += btn.width + props->button_groups[i].inner_spacing;
			else if (props->button_groups[i].align == IMAF::GroupAlign::Right)
				start_pos -= btn.width + props->button_groups[i].inner_spacing;
		}
	}

	ImVec2 text_pos;
	text_pos.x = (float)w / 2 - ImGui::CalcTextSize(app_props->name).x / 2;
	text_pos.y = props->height / 2.f - ImGui::CalcTextSize(app_props->name).y / 2;

	ImGui::SetCursorPos(text_pos);
	ImGui::Text(app_props->name);

	ImDrawList* DrawList = ImGui::GetWindowDrawList();
	DrawList->AddLine({ (float)x,(float)(props->height - 2 + y) }, { (float)(w + x),(float)(props->height - 2 + y) }, IM_COL32(73, 73, 73, 255), 3.0f);

#ifdef DRAW_CT_EXCLUSIONS
	for (const auto& i : props->exclusions)
	{
		ImVec2 start_pos = { (float)(i.start_offset * w + x),(float)(i.top_offset + y) };
		ImVec2 end_pos = { (float)(i.start_offset * w + x + i.width),(float)(i.top_offset + i.height + y) };
		DrawList->AddRectFilled(start_pos, end_pos, IM_COL32(0, 175, 0, 100));
	}
#endif // DRAW_CT_EXCLUSIONS

	if (m_current_panel != MS::PanelId::Menu)
	{
		ImGui::SetCursorPos(ImVec2(5, 5));
		if (ImGui::Button("Back", ImVec2(IMAF::Val(75), IMAF::Val(40) - 10)))
		{
			SwitchPanel(m_current_id, MS::PanelId::Menu);
		}
	}

	if (m_current_panel == MS::PanelId::Grid)
	{
		ImVec2 text_size = ImGui::CalcTextSize("Mines:1234567890");
		ImGui::SetCursorPos(ImVec2(10 + IMAF::Val(75), (IMAF::Val(40) - text_size.y) / 2));
		if (m_mines > 0)
			ImGui::Text("Mines: %i", m_mines);
		else
			ImGui::TextColored(RGB2_IMVEC4(125, 190, 90), "Mines: %i", m_mines);
	}

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor();

	IMAF::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

void MS::WndMgr::_DefCostumTitleBarDraw(const IMAF::AppProperties* props, GLFWwindow* window)
{
	if (sp_mgr)
		sp_mgr->DefCostumTitleBarDraw(props, window);
}
