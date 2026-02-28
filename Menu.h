#pragma once
#include "IMAF/Panel.h"
#include "Mines.h"
#include <memory>

namespace MS
{
	class IPanelComm;

	class MenuPanel : public IMAF::Panel
	{
	public:
		MenuPanel(IPanelComm* panel_comm);

		void UiRender() override;

	private:
		bool FilterUnwantedDecimals(char buf[3]);

	private:
		std::shared_ptr<MineField> mp_minefield;
		IPanelComm* mp_panel_comm;
		bool m_show_continue = false;
		bool m_first_pass = true;
		int m_width;
		int m_height;
		int m_minecount;
	};

}