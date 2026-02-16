#include "Save.h"
#include "Mines.h"
#include <filesystem>
#include <fstream>

std::shared_ptr<MS::MineField> MS::GetSavedField()
{
	std::filesystem::path file_path = MS::GetSaveFilePath();
	
	if (!std::filesystem::exists(file_path))
		nullptr;

	std::shared_ptr<MS::MineField> mf = std::make_shared<MS::MineField>();

	std::ifstream save_file(file_path, std::ios::binary | std::ios::in);

	save_file.read((char*)&*mf, sizeof(size_t) * 3);
	
	size_t buf_size = mf->height * mf->width * sizeof(Field);
	mf->fields.resize(buf_size / sizeof(Field));
	save_file.read((char*)mf->fields.data(), buf_size);

	save_file.close();

	return mf;
}

bool MS::SaveField(std::shared_ptr<MS::MineField> field)
{
	std::filesystem::path file_path = MS::GetSaveFilePath();

	if (std::filesystem::exists(file_path))
		std::filesystem::remove(file_path);

	std::fstream save_file(file_path, std::ios::binary | std::ios::out);

	save_file.write((const char*)&*field, sizeof(size_t) * 3);
	save_file.write((const char*)field->fields.data(), sizeof(Field) * field->fields.size());

	save_file.close();

	return false;
}


std::filesystem::path MS::GetSaveFilePath()
{
	std::filesystem::path file_path;
	{
		char* localAppDataPath = nullptr;
		size_t size = 0;
		errno_t err = _dupenv_s(&localAppDataPath, &size, "LOCALAPPDATA");
		if (err != 0 || localAppDataPath == nullptr)
			return std::filesystem::path();

		file_path = std::filesystem::path(localAppDataPath);
		free(localAppDataPath);
	}

	file_path /= "MyMineSweeper";
	if (!std::filesystem::exists(file_path))
		std::filesystem::create_directories(file_path);

	file_path /= "game.save";
	return file_path;
}

void MS::DeleteSave()
{
	std::filesystem::path fpath = MS::GetSaveFilePath();
	if (std::filesystem::exists(fpath))
	{
		std::filesystem::remove(fpath);
	}
}
