#include "Grid.h"
#include "imgui.h"
#include "IMAF/Application.h"
#include "Save.h"
#include <Windows.h>
#include "WndManager.h"

MS::GridPanel::GridPanel(IPanelComm* panel_comm, std::shared_ptr<MS::MineField> minefield) :
	m_minefield(minefield),
	m_asset_flag("./assets/flag.png"),
	m_asset_flag_green("./assets/flag_green.png"),
	m_asset_mine("./assets/mine.png"),
	m_asset_one("./assets/one.png"),
	m_asset_two("./assets/two.png"),
	m_asset_three("./assets/three.png"),
	m_asset_four("./assets/four.png"),
	m_asset_five("./assets/five.png"),
	m_asset_six("./assets/six.png"),
	m_asset_seven("./assets/seven.png"),
	m_asset_eight("./assets/eight.png"),
	m_gameover(false),
	m_lose(false),
	m_flag_green(false),
	m_show_cheat(false),
	mp_panel_comm(panel_comm),
	m_start_time(std::chrono::steady_clock::now()),
	m_mine_counter(minefield->num_mines),
	m_thread([this]()
		{
			while (!this->m_destroy)
			{
				if (GetAsyncKeyState(VK_MBUTTON) & 0x01)
					this->m_flag_green = !this->m_flag_green;
				if (GetAsyncKeyState(VK_INSERT) & 0x01)
					this->m_show_cheat = !this->m_show_cheat;
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	)
{
	mp_panel_comm->SetMineCount(m_mine_counter);
	for (size_t i = 0; i < m_minefield->fields.size(); i++)
	{
		if (m_minefield->fields[i].n == 0)
			MS::FloodFill(m_minefield, i);
		if (m_minefield->fields[i].state == MS::FieldState::Flagged)
			--m_mine_counter;
	}
}

MS::GridPanel::~GridPanel()
{
	m_destroy = true;
	m_thread.join();
	if (!m_gameover)
		MS::SaveField(m_minefield);
	else
		MS::DeleteSave();
}

void MS::GridPanel::UiRender()
{
	const int titlebar_height = IMAF::Val(40);
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 pos = viewport->Pos;
	pos.y += titlebar_height;
	ImGui::SetNextWindowPos(pos);
	ImVec2 size = viewport->Size;
	size.y -= titlebar_height;
	ImGui::SetNextWindowSize(size);

	IMAF::Begin("Minesweeper grid", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

	//if (false)
	//{
	//	std::string f = MS::StringifyMineField(m_minefield, true);
	//	ImGui::Text(f.c_str());
	//}

	const float spacing = IMAF::Val(25.f);

	//restart button
	ImVec2 btn_size = ImGui::CalcTextSize("Restart?");
	btn_size.x *= 2;
	btn_size.y *= 2;
	ImVec2 btn_pos;
	btn_pos.x = size.x - btn_size.x - spacing;
	btn_pos.y = size.y - btn_size.y - spacing;

	ImVec2 square_size;
	square_size.x = size.x - (btn_size.x + spacing + spacing) * 2;
	//square_size.x = size.x - spacing * 2;
	square_size.y = size.y - spacing * 2;
	int cell_size = 0;

	{
		ImVec2 cell_size_inter;
		cell_size_inter.x = square_size.x / m_minefield->width;
		cell_size_inter.y = square_size.y / m_minefield->height;
		cell_size = cell_size_inter.x > cell_size_inter.y ? cell_size_inter.y : cell_size_inter.x;

		square_size.x = cell_size * m_minefield->width;
		square_size.y = cell_size * m_minefield->height;
	}

	ImDrawList* drawlist = ImGui::GetWindowDrawList();

	//Draw outer square
	ImVec2 square_min;
	square_min.x = pos.x + (size.x - square_size.x) / 2;
	square_min.y = pos.y + (size.y - square_size.y) / 2;
	ImVec2 square_max;
	square_max.x = square_min.x + square_size.x;
	square_max.y = square_min.y + square_size.y;
	drawlist->AddRectFilled(square_min, square_max, IM_COL32(255, 255, 255, 10));
	drawlist->AddRect(square_min, square_max, IM_COL32(109, 113, 118, 255), 0, 0, IMAF::Val(1.5f));

	//Darw Column lines
	for (size_t i = 0; i < m_minefield->width - 1; i++)
	{
		ImVec2 p_min;
		p_min.x = square_min.x + cell_size * (i + 1);
		p_min.y = square_min.y;
		ImVec2 p_max;
		p_max.x = p_min.x;
		p_max.y = square_max.y;
		drawlist->AddLine(p_min, p_max, IM_COL32(109, 113, 118, 255), IMAF::Val(1.f));
	}

	for (size_t i = 0; i < m_minefield->height - 1; i++)
	{
		ImVec2 p_min;
		p_min.x = square_min.x;
		p_min.y = square_min.y + cell_size * (i + 1);
		ImVec2 p_max;
		p_max.x = square_max.x;
		p_max.y = p_min.y;
		drawlist->AddLine(p_min, p_max, IM_COL32(109, 113, 118, 255), IMAF::Val(1.f));
	}

	ImVec2 cursor = ImGui::GetMousePos();
	
	square_max.x -= IMAF::Val(1.5f);
	square_max.y -= IMAF::Val(1.5f);

	if (ImGui::IsMouseHoveringRect(square_min, square_max) && !m_gameover)
	{
		cursor.x -= square_min.x;
		cursor.y -= square_min.y;

		int cell_index_x = ((int)cursor.x / cell_size);
		int cell_index_y = ((int)cursor.y / cell_size);
		if (!m_show_cheat)
		{
			ImVec2 cell_min;
			cell_min.x = cell_index_x * cell_size + square_min.x + IMAF::Val(1.f);
			cell_min.y = cell_index_y * cell_size + square_min.y + IMAF::Val(1.f);
			ImVec2 cell_max;
			cell_max.x = cell_min.x + cell_size - IMAF::Val(1.f);
			cell_max.y = cell_min.y + cell_size - IMAF::Val(1.f);
			drawlist->AddRectFilled(cell_min, cell_max, IM_COL32(79, 33, 127, 200));
		}
		else
		{
			ImVec2 topc;
			topc.x = cell_index_x * cell_size + square_min.x + IMAF::Val(1.f);
			topc.y = cell_index_y * cell_size + square_min.y + IMAF::Val(1.f);

			ImVec2 image_pos = topc;
			image_pos.x += cell_size * 0.15;
			image_pos.y += cell_size * 0.15;
			ImVec2 image_size(0, 0);
			image_size.x = cell_size * 0.7;
			image_size.y = cell_size * 0.7;

			switch (m_minefield->fields[cell_index_y * m_minefield->width + cell_index_x].state)
			{
			case MS::FieldState::Revealed:
				break;
			case MS::FieldState::Hidden:
			case MS::FieldState::Flagged:
			case MS::FieldState::GreenFlag:
			{
				switch (m_minefield->fields[cell_index_y * m_minefield->width + cell_index_x].n)
				{
				case -1:
				{
					m_asset_mine.SetImagePosition(image_pos);
					m_asset_mine.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
					break;
				}
				case 1:
				{
					m_asset_one.SetImagePosition(image_pos);
					m_asset_one.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
					break;
				}
				case 2:
				{
					m_asset_two.SetImagePosition(image_pos);
					m_asset_two.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
					break;
				}
				case 3:
				{
					m_asset_three.SetImagePosition(image_pos);
					m_asset_three.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
					break;
				}
				case 4:
				{
					m_asset_four.SetImagePosition(image_pos);
					m_asset_four.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
					break;
				}
				case 5:
				{
					m_asset_five.SetImagePosition(image_pos);
					m_asset_five.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
					break;
				}
				case 6:
				{
					m_asset_six.SetImagePosition(image_pos);
					m_asset_six.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
					break;
				}
				case 7:
				{
					m_asset_seven.SetImagePosition(image_pos);
					m_asset_seven.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
					break;
				}
				case 8:
				{
					m_asset_eight.SetImagePosition(image_pos);
					m_asset_eight.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
					break;
				}
				case 9:
				{
					m_asset_flag.SetImagePosition(image_pos);
					m_asset_flag.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
					break;
				}
				}
				break;
			}
			}
		}

		if (m_minefield->fields[cell_index_y * m_minefield->width + cell_index_x].state != MS::FieldState::Revealed)
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
			{
				if (!m_timer.Start(150))
				{
					if (m_minefield->fields[cell_index_y * m_minefield->width + cell_index_x].state == MS::FieldState::Flagged)
					{
						m_minefield->fields[cell_index_y * m_minefield->width + cell_index_x].state = MS::FieldState::Hidden;
						m_mine_counter++;
					}
					else if (m_minefield->fields[cell_index_y * m_minefield->width + cell_index_x].state == MS::FieldState::GreenFlag)
					{
						m_minefield->fields[cell_index_y * m_minefield->width + cell_index_x].state = MS::FieldState::Hidden;
					}
					else
					{
						
						if (m_flag_green)
							m_minefield->fields[cell_index_y * m_minefield->width + cell_index_x].state = MS::FieldState::GreenFlag;
						else
						{
							m_minefield->fields[cell_index_y * m_minefield->width + cell_index_x].state = MS::FieldState::Flagged;
							m_mine_counter--;
						}
					}
				}
				mp_panel_comm->SetMineCount(m_mine_counter);
			}
			else if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				m_minefield->fields[cell_index_y * m_minefield->width + cell_index_x].state = MS::FieldState::Revealed;
				if (m_minefield->fields[cell_index_y * m_minefield->width + cell_index_x].n == -1)
				{
					m_gameover = true;
					m_lose = true;
					for (auto& i : m_minefield->fields)
					{
						i.state = MS::FieldState::Revealed;
					}
				}
				else if (m_minefield->fields[cell_index_y * m_minefield->width + cell_index_x].n == 0)
				{
					MS::FloodFill(m_minefield, cell_index_y * m_minefield->width + cell_index_x);
				}
				m_gameover = true;
				for (const auto& i : m_minefield->fields)
				{
					if (i.state != MS::FieldState::Revealed && i.n != -1)
						m_gameover = false;
				}
			}
		}
	}

	for (size_t i = 0; i < m_minefield->fields.size(); i++)
	{
		int cell_index_x = i % m_minefield->width;
		int cell_index_y = i / m_minefield->width;

		ImVec2 topc;
		topc.x = cell_index_x * cell_size + square_min.x + IMAF::Val(1.f);
		topc.y = cell_index_y * cell_size + square_min.y + IMAF::Val(1.f);

		ImVec2 image_pos = topc;
		image_pos.x += cell_size * 0.15;
		image_pos.y += cell_size * 0.15;
		ImVec2 image_size(0, 0);
		image_size.x = cell_size * 0.7;
		image_size.y = cell_size * 0.7;

		switch (m_minefield->fields[i].state)
		{
		case MS::FieldState::Hidden:
		{
			continue;
		}
		case MS::FieldState::Flagged:
		{
			m_asset_flag.SetImagePosition(image_pos);
			m_asset_flag.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
			break;
		}
		case MS::FieldState::GreenFlag:
		{
			m_asset_flag_green.SetImagePosition(image_pos);
			m_asset_flag_green.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
			break;
		}
		case MS::FieldState::Revealed:
		{
			ImVec2 pmax = topc;
			pmax.x += cell_size;
			pmax.y += cell_size;
			drawlist->AddRectFilled(topc, pmax, IM_COL32(255, 255, 255, 30));

			switch (m_minefield->fields[i].n)
			{
			case -1:
			{
				m_asset_mine.SetImagePosition(image_pos);
				m_asset_mine.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
				break;
			}
			case 1:
			{
				m_asset_one.SetImagePosition(image_pos);
				m_asset_one.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
				break;
			}
			case 2:
			{
				m_asset_two.SetImagePosition(image_pos);
				m_asset_two.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
				break;
			}
			case 3:
			{
				m_asset_three.SetImagePosition(image_pos);
				m_asset_three.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
				break;
			}
			case 4:
			{
				m_asset_four.SetImagePosition(image_pos);
				m_asset_four.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
				break;
			}
			case 5:
			{
				m_asset_five.SetImagePosition(image_pos);
				m_asset_five.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
				break;
			}
			case 6:
			{
				m_asset_six.SetImagePosition(image_pos);
				m_asset_six.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
				break;
			}
			case 7:
			{
				m_asset_seven.SetImagePosition(image_pos);
				m_asset_seven.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
				break;
			}
			case 8:
			{
				m_asset_eight.SetImagePosition(image_pos);
				m_asset_eight.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
				break;
			}
			case 9:
			{
				m_asset_flag.SetImagePosition(image_pos);
				m_asset_flag.DisplayImGuiImage(image_size, ImVec2(0, 0), ImVec2(-1, -1), true);
				break;
			}
			}
			break;
		}
		}
	}

	if (m_gameover)
	{
		if (m_lose)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, RGB2_IMVEC4(218, 55, 68));
			ImGui::SetCursorPos(btn_pos);
			if (ImGui::Button("Restart?", btn_size))
			{
				std::shared_ptr<MS::MineField> newmf = MS::GetMineField(m_minefield->width, m_minefield->height, m_minefield->num_mines);
				m_minefield = newmf;
				m_lose = false;
				m_gameover = false;
				for (size_t i = 0; i < m_minefield->fields.size(); i++)
				{
					if (m_minefield->fields[i].n == 0)
						MS::FloodFill(m_minefield, i);
				}
			}
			ImGui::PopStyleColor();
		}
		else
		{
			std::chrono::time_point end = std::chrono::steady_clock::now();
			std::chrono::seconds dur = std::chrono::duration_cast<std::chrono::seconds>(end - m_start_time);

			mp_panel_comm->SetTime(dur.count());
			mp_panel_comm->SwitchPanel(this->id, MS::PanelId::Win);
		}
	}

	IMAF::End();
}
