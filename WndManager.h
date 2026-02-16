#pragma once
#include "IMAF/Application.h"
#include "GLFW/glfw3.h"
#include <atomic>
#include "Menu.h"
#include "Grid.h"
#include "Mines.h"
#include "Win.h"

namespace MS
{
	enum class PanelId
	{
		Menu,
		Grid,
		Win
	};

	class IPanelComm
	{
	public:
		virtual void SwitchPanel(uint64_t panel_id, PanelId target_panel) = 0;
		virtual void SetMinefield(std::shared_ptr<MS::MineField> minefield) = 0;
		virtual void SetTime(long long seconds) = 0;
		virtual void SetMineCount(int32_t mines) = 0;
	};

	class WndMgr final : public IPanelComm
	{
	public:
		WndMgr(const IMAF::AppProperties& props);

		static void _DefCostumTitleBarDraw(const IMAF::AppProperties* props, GLFWwindow* window);
		void DefCostumTitleBarDraw(const IMAF::AppProperties* props, GLFWwindow* window);

		void SwitchPanel(uint64_t panel_id, PanelId target_panel) override;
		void SetMinefield(std::shared_ptr<MS::MineField> minefield) override;
		void SetTime(long long seconds) override;
		void SetMineCount(int32_t mines) override;

		void Run();

	private:
		IMAF::AppProperties m_props;
		IMAF::Application m_app;
		std::atomic<float> m_titlebar_height;
		PanelId m_current_panel;
		uint64_t m_current_id;
		int32_t m_mines;
		long long m_seconds;
		std::shared_ptr<MS::GridPanel> mp_grid;
		std::shared_ptr<MS::MenuPanel> mp_menu;
		std::shared_ptr<MS::MineField> mp_minefield;
		std::shared_ptr<MS::WinPanel> mp_win;
	};
}