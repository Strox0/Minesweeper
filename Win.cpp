#include "Win.h"
#include "WndManager.h"
#include "IMAF/Application.h"


MS::WinPanel::WinPanel(IPanelComm* panel_comm, long long seconds) : mp_panel_comm(panel_comm), m_seconds(seconds), m_asset_win("./assets/win.png"), m_time_text("Time: ")
{
	int minutes = m_seconds / 60;
	if (minutes != 0)
	{
		m_time_text += std::to_string(minutes);
		m_time_text += " minutes ";
		m_time_text += std::to_string(m_seconds - (minutes * 60));
		m_time_text += " seconds";
	}
	else
	{
		m_time_text += std::to_string(m_seconds);
		m_time_text += " seconds";
	}
}

void MS::WinPanel::UiRender()
{
	const int titlebar_height = IMAF::Val(40);
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 pos = viewport->Pos;
	pos.y += titlebar_height;
	ImGui::SetNextWindowPos(pos);
	ImVec2 size = viewport->Size;
	size.y -= titlebar_height;
	ImGui::SetNextWindowSize(size);

	IMAF::Begin("win", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

	ImVec2 text_size = ImGui::CalcTextSize(m_time_text.c_str());
	ImVec2 text_pos;
	text_pos.y = text_size.y * 2;
	text_pos.x = 0 + (size.x - text_size.x) / 2;
	ImGui::SetCursorPos(text_pos);
	ImGui::Text(m_time_text.c_str());

	ImVec2 img_size = m_asset_win.GetSize();
	float ratio = img_size.y / img_size.x;
	img_size.x = size.x * 0.6;
	img_size.y = img_size.x * ratio;

	ImVec2 img_pos = pos;
	img_pos.x += (size.x - img_size.x) / 2;
	img_pos.y += (size.y - img_size.y - titlebar_height) / 2;
	m_asset_win.SetImagePosition(img_pos);
	m_asset_win.DisplayImGuiImage(img_size, ImVec2(0, 0), ImVec2(-1, -1), true);

	ImVec2 btn_size;
	btn_size.x = img_size.x;
	btn_size.y = ImGui::CalcTextSize("PLAY AGAIN").y * 3.5;

	ImVec2 btn_pos;
	btn_pos.y = img_pos.y + img_size.y + ImGui::GetStyle().ItemSpacing.y - pos.y;
	btn_pos.x = (size.x - btn_size.x) / 2;
	ImGui::SetCursorPos(btn_pos);
	if (ImGui::Button("PLAY AGAIN", btn_size))
	{
		mp_panel_comm->SwitchPanel(this->id, MS::PanelId::Menu);
	}

	IMAF::End();
}
