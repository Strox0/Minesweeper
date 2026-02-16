#pragma once
#include "IMAF/Panel.h"
#include "IMAF/Image.h"

namespace MS
{
	class IPanelComm;

	class WinPanel : public IMAF::Panel
	{
	public:
		WinPanel(IPanelComm* panel_comm, long long seconds);

		void UiRender() override;

	private:
		long long m_seconds;
		std::string m_time_text;
		IPanelComm* mp_panel_comm;
		IMAF::Image m_asset_win;
	};
}