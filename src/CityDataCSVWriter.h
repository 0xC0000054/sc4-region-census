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
#include "RegionalCityData.h"
#include <filesystem>
#include <fstream>
#include <vector>

class CityDataCSVWriter
{
public:

	CityDataCSVWriter(const std::filesystem::path& path);

	void Write(const std::vector<RegionalCityData>& cityData);

private:

	void WriteRCIGroupInfo(const RCIGroupInfo& info);

	void WriteCurrency(float value);

	void WriteNumber(int64_t value);

	void WritePercentage(float value, std::streamsize precision);

	void WriteString(const std::string& value);

	std::ofstream outStream;
};

