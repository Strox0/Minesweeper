#include "Menu.h"
#include "Save.h"
#include "imgui.h"
#include "IMAF/Application.h"
#include "WndManager.h"

MS::MenuPanel::MenuPanel(MS::IPanelComm* panel_comm) : mp_panel_comm(panel_comm), m_height(16), m_width(16), m_minecount(40)
{
	mp_minefield = MS::GetSavedField();
	if (mp_minefield->width != 0)
		m_show_continue = true;
}

void MS::MenuPanel::UiRender()
{
	auto CenterCursorX = [](float item_width)
		{
			float x = (ImGui::GetWindowContentRegionMax().x + ImGui::GetWindowContentRegionMin().x - item_width) * 0.5f;
			ImGui::SetCursorPosX(x);
		};

	const int titlebar_height = IMAF::Val(40);
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 pos = viewport->Pos;
	pos.y += titlebar_height;
	ImGui::SetNextWindowPos(pos);
	ImVec2 size = viewport->Size;
	size.y -= titlebar_height;
	ImGui::SetNextWindowSize(size);

	IMAF::Begin("Minesweeper Setup", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

	ImGuiStyle& style = ImGui::GetStyle();

	//Continue
	ImVec2 continue_button_size;
	ImVec2 continue_text_size = ImGui::CalcTextSize("Continue?");
	continue_button_size.x = continue_text_size.x * 2;
	continue_button_size.y = continue_text_size.y * 2.5;
	if (m_show_continue)
	{
		ImGui::SetCursorPos(ImVec2(10, 10));
		if (ImGui::Button("Continue?", continue_button_size))
		{
			mp_panel_comm->SetMinefield(mp_minefield);
			mp_panel_comm->SwitchPanel(this->id, MS::PanelId::Grid);
		}
		ImGui::SetCursorPos(ImVec2(20 + continue_button_size.x, 10 + (continue_button_size.y - ImGui::CalcTextSize("SM|").y) / 2));
		ImGui::Text("Size: %d x %d | Mine count: %d", mp_minefield->width, mp_minefield->height, mp_minefield->num_mines);
	}

	
	ImVec2 grid_size_text_size = ImGui::CalcTextSize("Grid Size");
	CenterCursorX(grid_size_text_size.x);
	ImGui::SetCursorPosY(continue_button_size.y + style.WindowPadding.y + style.FramePadding.y + grid_size_text_size.y);
	ImGui::Text("Grid Size");
	const float grid_size_input_box = grid_size_text_size.x * 4.f + style.ItemSpacing.x;

	//Width
	const float width_text_input_size = grid_size_input_box / 2;
	static char width_text_buf[3] = {'1','6',0};
	CenterCursorX(grid_size_input_box);
	ImGui::SetNextItemWidth(width_text_input_size);
	if (ImGui::InputText("##width_input", &width_text_buf[0], 3, ImGuiInputTextFlags_CharsDecimal))
	{
		if (width_text_buf[0] != 0)
		{
			if (!FilterUnwantedDecimals(width_text_buf))
			{
				m_width = std::stoi(width_text_buf);
			}
			else
			{
				if (m_width > 9)
				{
					int tmp = m_width;
					width_text_buf[1] = tmp % 10 + '0';
					tmp /= 10;
					width_text_buf[0] = tmp + '0';
				}
				else
				{
					width_text_buf[0] = m_width + '0';
				}
			}
		}
	}

	//Height
	static char height_text_buf[3] = { '1','6',0};
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width_text_input_size);
	if (ImGui::InputText("##height_input", &height_text_buf[0], 3, ImGuiInputTextFlags_CharsDecimal))
	{
		if (height_text_buf[0] != 0)
		{
			if (!FilterUnwantedDecimals(height_text_buf))
			{
				m_height = std::stoi(height_text_buf);
			}
			else
			{
				if (m_height > 9)
				{
					int tmp = m_height;
					height_text_buf[1] = tmp % 10 + '0';
					tmp /= 10;
					height_text_buf[0] = tmp + '0';
				}
				else
				{
					height_text_buf[0] = m_height + '0';
				}
			}
		}
	}

	//Mine count
	ImVec2 mine_count_text_size = ImGui::CalcTextSize("Mine Count");
	ImGui::Dummy(ImVec2(1, mine_count_text_size.y));
	CenterCursorX(mine_count_text_size.x);
	ImGui::Text("Mine Count");

	static char mine_count_text_buf[3] = {'4','0',0};
	CenterCursorX(grid_size_input_box);
	ImGui::SetNextItemWidth(grid_size_input_box);
	if (ImGui::InputText("##mine_count_input", &mine_count_text_buf[0], 3, ImGuiInputTextFlags_CharsDecimal))
	{
		if (mine_count_text_buf[0] != 0)
		{
			if (!FilterUnwantedDecimals(mine_count_text_buf))
				m_minecount = std::stoi(mine_count_text_buf);
			else
			{
				if (m_minecount > 9)
				{
					int tmp = m_minecount;
					mine_count_text_buf[1] = tmp % 10 + '0';
					tmp /= 10;
					mine_count_text_buf[0] = tmp + '0';
				}
				else
				{
					mine_count_text_buf[0] = m_minecount + '0';
				}
			}
		}
	}

	//Difficulty
	float mine_ratio = m_minecount;
	mine_ratio /= (m_width * m_height);
	ImGui::Dummy(ImVec2(1, mine_count_text_size.y*2));
	if (mine_ratio < 0.25f)
	{
		CenterCursorX(ImGui::CalcTextSize("Easy").x);
		ImGui::TextColored(RGB2_IMVEC4(42, 234, 74), "Easy");
	}
	else if (mine_ratio < 0.50f)
	{
		CenterCursorX(ImGui::CalcTextSize("Medium").x);
		ImGui::TextColored(RGB2_IMVEC4(224, 125, 33), "Medium");
	}
	else
	{
		CenterCursorX(ImGui::CalcTextSize("Hard").x);
		ImGui::TextColored(RGB2_IMVEC4(224, 33, 33), "Hard");
	}
	
	//Start
	ImVec2 start_button_size;
	start_button_size.x = size.x * 0.80;
	start_button_size.y = ImGui::CalcTextSize("START").y * 3.5;
	CenterCursorX(start_button_size.x);
	ImGui::SetCursorPosY(size.y - start_button_size.y -  (start_button_size.y/2));
	if (ImGui::Button("START", start_button_size))
	{
		if ((m_width * m_height) <= m_minecount)
		{
			MessageBoxA(NULL, "Too many mines!!", "Error", MB_ICONERROR);
		}
		else
		{
			if (m_show_continue)
			{
				MS::DeleteSave();
			}
			mp_panel_comm->SetMinefield(MS::GetMineField(m_width, m_height, m_minecount));
			mp_panel_comm->SwitchPanel(this->id, MS::PanelId::Grid);
		}
	}

	IMAF::End();
}

bool MS::MenuPanel::FilterUnwantedDecimals(char buf[3])
{
	bool filter = false;
	for (int i = 0; i < 2; i++)
	{
		switch (buf[i])
		{
		case '.':
			filter = true;
			break;
		case '+':
			filter = true;
			break;
		case '-':
			filter = true;
			break;
		case '*':
			filter = true;
			break;
		case '/':
			filter = true;
			break;
		}
		if (filter)
		{
			buf[0] = 0;
			buf[1] = 0;
			break;
		}
	}
	return filter;
}