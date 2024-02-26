///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-region-census, a DLL Plugin for SimCity 4 that
// adds a region census UI.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <filesystem>

class cISC4App;

class FileSystem
{
public:

	static FileSystem& GetInstance();

	const std::filesystem::path& GetModuleFolderPath() const;

	std::filesystem::path GetRegionCensusExportDirectory() const;

	void PostAppInit();

private:

	FileSystem();

	std::filesystem::path moduleFolderPath;
	std::filesystem::path regionCensusReportsRoot;
	std::filesystem::path currentRegionCensusOutputPath;
};

