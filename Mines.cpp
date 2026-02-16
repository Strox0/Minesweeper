#include "Mines.h"
#include "Random.h"
#include <unordered_set>
#include <queue>
#include <Windows.h>

std::shared_ptr<MS::MineField> MS::GetMineField(size_t width, size_t height, size_t mine_count)
{
	std::shared_ptr<MS::MineField> mf = std::make_shared<MS::MineField>();
	mf->width = width;
	mf->height = height;
	mf->num_mines = mine_count;
	mf->fields.resize(height * width);

	RandomInt random(0, mf->fields.size() - 1);
	std::unordered_set<int> mines_place;

	for (size_t i = 0; i < mine_count; i++)
	{
		int64_t mine_pos = -1;
		do
		{
			mine_pos = random.GetNumber();
		} while (mines_place.contains(mine_pos));
		mines_place.insert(mine_pos);
		mf->fields[mine_pos].n = -1;
	}

	size_t curr_row = 0;
	size_t curr_column = 0;
	const size_t& num_cols = width;
	const size_t& row_max = height;

	for (size_t i = 0; i < mf->fields.size(); i++)
	{
		if (curr_column == num_cols)
		{
			curr_column = 0;
			curr_row++;
		}

		if (mf->fields[i].n == -1)
		{
			curr_column++;
			continue;
		}

		int mine_count = 0;

		if (curr_column != 0)
		{
			if (mf->fields[i - 1].n == -1)
				mine_count++;
		}

		if (curr_column != num_cols - 1)
		{
			if (mf->fields[i + 1].n == -1)
				mine_count++;
		}

		if (curr_row != 0)
		{
			if (mf->fields[i - num_cols].n == -1)
				mine_count++;
			if (curr_column != 0)
			{
				if (mf->fields[i - num_cols - 1].n == -1)
					mine_count++;
			}
			if (curr_column != num_cols - 1)
			{
				if (mf->fields[i - num_cols + 1].n == -1)
					mine_count++;
			}
		}

		if (curr_row != row_max - 1)
		{
			if (mf->fields[i + num_cols].n == -1)
				mine_count++;
			if (curr_column != 0)
			{
				if (mf->fields[i + num_cols - 1].n == -1)
					mine_count++;
			}
			if (curr_column != num_cols - 1)
			{
				if (mf->fields[i + num_cols + 1].n == -1)
					mine_count++;
			}
		}

		mf->fields[i].n = mine_count;
		curr_column++;
	}
	return mf;
}

std::shared_ptr<MS::MineField> MS::GetMineField(const std::vector<int>& field, size_t width, size_t height, size_t mine_count)
{
	std::shared_ptr<MS::MineField> mf = std::make_shared<MS::MineField>();
	mf->width = width;
	mf->height = height;
	mf->num_mines = mine_count;
	mf->fields.reserve(height * width);

	for (const auto& i : field)
	{
		MS::Field f;
		f.n = i;
		f.state = MS::FieldState::Hidden;
		mf->fields.push_back(f);
	}

	return mf;
}

std::string MS::StringifyMineField(std::shared_ptr<MineField> mf, bool show_states)
{
	std::string str;
	str.reserve(mf->height * mf->width * 2);

	for (size_t i = 0; i < mf->height; i++)
	{
		str += '[';
		for (size_t l = 0; l < mf->width; l++)
		{
			str += ' ';

			if (!show_states)
			{
				str += mf->fields[i * mf->width + l].n == -1 ? 'M' : mf->fields[i * mf->width + l].n + '0';
			}
			else
			{
				switch (mf->fields[i * mf->width + l].state)
				{
				case MS::FieldState::Hidden:
					str += 'H';
					break;
				case MS::FieldState::Flagged:
					str += 'F';
					break;
				case MS::FieldState::Revealed:
					str += 'R';
					break;
				}
			}
		}
		str += "]\n";
	}

	return str;
}

void MS::FloodFill(std::shared_ptr<MineField> minefield, size_t cell_index)
{
	std::queue<size_t> cells_queue;
	std::unordered_set<size_t> visited;

	auto PushIfFresh = [&](size_t index)
		{
			if (!visited.contains(index))
				cells_queue.push(index);
		};

	cells_queue.push(cell_index);

	while (!cells_queue.empty())
	{
		size_t i = cells_queue.front();
		cells_queue.pop();
		if (visited.contains(i))
			continue;
		else
			visited.insert(i);
		minefield->fields[i].state = MS::FieldState::Revealed;
		if (minefield->fields[i].n != 0)
			continue;
		int cell_index_x = i % minefield->width;
		int cell_index_y = i / minefield->width;
		
		bool in_last_column = (cell_index_x == minefield->width - 1);
		bool in_last_row = (cell_index_y == minefield->height - 1);
		bool in_first_column = cell_index_x == 0;
		bool in_first_row = cell_index_y == 0;

		if (!in_last_column)
		{
			PushIfFresh(i + 1);
			if (!in_last_row)
			{
				PushIfFresh(i + minefield->width + 1);
				PushIfFresh(i + minefield->width);
			}
			if (!in_first_row)
			{
				PushIfFresh(i - minefield->width);
				PushIfFresh(i - minefield->width + 1);
			}
		}
		if (!in_first_column)
		{
			PushIfFresh(i - 1);
			if (!in_last_row)
			{
				PushIfFresh(i + minefield->width - 1);
			}
			if (!in_first_row)
			{
				PushIfFresh(i - minefield->width - 1);
			}
		}
	}
}
