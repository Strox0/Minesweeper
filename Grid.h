#pragma once
#include "IMAF/Panel.h"
#include <memory>
#include <vector>
#include "Mines.h"
#include "IMAF/Image.h"
#include "Timer.h"
#include <atomic>
#include <thread>
#include <chrono>

namespace MS
{
	class IPanelComm;

	class GridPanel : public IMAF::Panel
	{
	public:
		GridPanel(IPanelComm* panel_comm, std::shared_ptr<MS::MineField> minefield);
		~GridPanel();
		void UiRender() override;
		
	private:
		std::shared_ptr<MS::MineField> m_minefield;
		IPanelComm* mp_panel_comm;
		std::chrono::steady_clock::time_point m_start_time;
		bool m_gameover;
		bool m_lose;
		int32_t m_mine_counter;
		std::atomic_bool m_flag_green;
		std::atomic_bool m_destroy;
		std::atomic_bool m_show_cheat;
		std::thread m_thread;
		MS::MutexTimer m_timer;

		IMAF::Image m_asset_flag;
		IMAF::Image m_asset_flag_green;
		IMAF::Image m_asset_mine;
		IMAF::Image m_asset_one;
		IMAF::Image m_asset_two;
		IMAF::Image m_asset_three;
		IMAF::Image m_asset_four;
		IMAF::Image m_asset_five;
		IMAF::Image m_asset_six;
		IMAF::Image m_asset_seven;
		IMAF::Image m_asset_eight;
	};
}
