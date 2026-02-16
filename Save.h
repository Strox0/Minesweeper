#pragma once
#include <memory>
#include <filesystem>

namespace MS
{
	struct MineField;

	std::shared_ptr<MineField> GetSavedField();
	bool SaveField(std::shared_ptr<MineField> field);
	std::filesystem::path GetSaveFilePath();
	void DeleteSave();
}