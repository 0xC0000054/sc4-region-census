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

#include "CityDataCSVWriter.h"
#include <array>
#include <locale>
#include <stdio.h>

using namespace std::literals::string_view_literals;

static const std::u8string_view ColumnHeaderUtf8 =
u8"City Name,Mayor Name,Mayor Rating,Difficulty,Tile Size,Residential Pop.,Commercial Jobs,Industrial Jobs,Workforce Percentage,"
u8"R§ Pop.,R§ Extrapolated Pop.,R§ Tax Rate,R§§ Pop.,R§§ Extrapolated Pop.,R§§ Tax Rate,R§§§ Pop.,R§§§ Extrapolated Pop.,R§§§ Tax Rate,"
u8"Cs§ Pop.,Cs§ Extrapolated Pop.,Cs§ Tax Rate,Cs§§ Pop.,Cs§§ Extrapolated Pop.,Cs§§ Tax Rate,Cs§§§ Pop.,Cs§§§ Extrapolated Pop.,Cs§§§ Tax Rate,"
u8"Co§§ Pop.,Co§§ Extrapolated Pop.,Co§§ Tax Rate,Co§§§ Pop.,Co§§§ Extrapolated Pop.,Co§§§ Tax Rate,IR Pop.,IR Extrapolated Pop.,IR Tax Rate,"
u8"ID Pop.,ID Extrapolated Pop.,ID Tax Rate,IM Pop.,IM Extrapolated Pop.,IM Tax Rate,IHT Pop.,IHT Extrapolated Pop.,IHT Tax Rate,Budget,Income,"sv;

CityDataCSVWriter::CityDataCSVWriter(const std::filesystem::path& path)
{
	outStream.open(path, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
	outStream.imbue(std::locale::classic());
	outStream.setf(std::ios_base::fixed, std::ios_base::floatfield);
}

void CityDataCSVWriter::Write(const std::vector<RegionalCityData>& cityData)
{
	outStream.write(reinterpret_cast<const char*>(ColumnHeaderUtf8.data()), ColumnHeaderUtf8.size());
	outStream << std::endl;	// The line terminator marks the end of a CSV record.

	for (const RegionalCityData& data : cityData)
	{
		WriteString(data.GetCityName());
		WriteString(data.GetMayorName());
		WriteNumber(data.GetMayorRating());
		WriteString(data.GetDifficulty());
		WriteString(data.GetCityTileSize());
		WriteNumber(data.GetResidentialPopulation());
		WriteNumber(data.GetCommercialJobs());
		WriteNumber(data.GetIndustrialJobs());
		WritePercentage(data.GetWorkforcePercentage(), 2);
		WriteRCIGroupInfo(data.GetRCIGroupInfo(RCIGroup::R1));
		WriteRCIGroupInfo(data.GetRCIGroupInfo(RCIGroup::R2));
		WriteRCIGroupInfo(data.GetRCIGroupInfo(RCIGroup::R3));
		WriteRCIGroupInfo(data.GetRCIGroupInfo(RCIGroup::Cs1));
		WriteRCIGroupInfo(data.GetRCIGroupInfo(RCIGroup::Cs2));
		WriteRCIGroupInfo(data.GetRCIGroupInfo(RCIGroup::Cs3));
		WriteRCIGroupInfo(data.GetRCIGroupInfo(RCIGroup::Co2));
		WriteRCIGroupInfo(data.GetRCIGroupInfo(RCIGroup::Co3));
		WriteRCIGroupInfo(data.GetRCIGroupInfo(RCIGroup::IR));
		WriteRCIGroupInfo(data.GetRCIGroupInfo(RCIGroup::ID));
		WriteRCIGroupInfo(data.GetRCIGroupInfo(RCIGroup::IM));
		WriteRCIGroupInfo(data.GetRCIGroupInfo(RCIGroup::IHT));
		WriteCurrency(data.GetBudget());
		WriteCurrency(data.GetIncome());
		outStream << std::endl;
	}
}

void CityDataCSVWriter::WriteRCIGroupInfo(const RCIGroupInfo& info)
{
	WriteNumber(info.population);
	WriteNumber(info.extrapolatedPopulation);
	WritePercentage(info.taxRate, 1);
}

void CityDataCSVWriter::WriteCurrency(float value)
{
	std::streamsize oldPrecision = outStream.precision();

	// The \xC2\xA7 value is the UTF-8 encoding of the section symbol (§).
	outStream << "\"\xC2\xA7" << std::setprecision(2) << value << std::setprecision(oldPrecision) << "\",";
}

void CityDataCSVWriter::WriteNumber(int64_t value)
{
	outStream << '"' << value << "\",";
}

void CityDataCSVWriter::WritePercentage(float value, std::streamsize precision)
{
	std::streamsize oldPrecision = outStream.precision();

	outStream << '"' << std::setprecision(precision) << value << std::setprecision(oldPrecision) << "%\",";
}

void CityDataCSVWriter::WriteString(const std::string& value)
{
	outStream << '"' << value << "\",";
}
