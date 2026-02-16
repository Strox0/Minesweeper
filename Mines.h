#pragma once
#include <vector>
#include <string>
#include <memory>

namespace MS
{
	enum class FieldState
	{
		Revealed,
		Hidden,
		Flagged,
		GreenFlag
	};

	struct Field
	{
		int n = 0;
		FieldState state = FieldState::Hidden;
	};

	struct MineField
	{
		size_t num_mines = 0;
		size_t width = 0;
		size_t height = 0;
		std::vector<Field> fields;
	};

	std::shared_ptr<MineField> GetMineField(size_t width, size_t height, size_t mine_count);
	std::shared_ptr<MineField> GetMineField(const std::vector<int>& field, size_t width, size_t height, size_t mine_count);
	std::string StringifyMineField(std::shared_ptr<MineField> mf, bool show_states);
	void FloodFill(std::shared_ptr<MineField> minefield, size_t cell_index);
}