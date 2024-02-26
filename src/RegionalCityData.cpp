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

#include "RegionalCityData.h"
#include "cISC4RegionalCity.h"
#include "cRZBaseString.h"

#include <Windows.h>
#include <wil/resource.h>

#include <array>
#include <functional>

using namespace std::placeholders; // For _1, _2, etc.

namespace
{
	static constexpr std::array<std::pair<uint32_t, RCIGroup>, 12> RCITypes
	{
		std::pair(0x1010, RCIGroup::R1),
		std::pair(0x1020, RCIGroup::R2),
		std::pair(0x1030, RCIGroup::R3),
		std::pair(0x3110, RCIGroup::Cs1),
		std::pair(0x3120, RCIGroup::Cs2),
		std::pair(0x3130, RCIGroup::Cs3),
		std::pair(0x3320, RCIGroup::Co2),
		std::pair(0x3330, RCIGroup::Co3),
		std::pair(0x4100, RCIGroup::IR),
		std::pair(0x4200, RCIGroup::ID),
		std::pair(0x4300, RCIGroup::IM),
		std::pair(0x4400, RCIGroup::IHT),
	};

	std::string GetString(std::function<bool(cIGZString&)> func)
	{
		std::string value;

		cRZBaseString gzStr;

		if (func(gzStr))
		{
			value = gzStr.ToChar();
		}

		return value;
	}
}

RegionalCityData::RegionalCityData(
	cISC4RegionalCity* pRegionalCity,
	cISC4Region::eCityTileSize tileSize)
{
	cityName = GetString(std::bind(&cISC4RegionalCity::GetCityName, pRegionalCity, _1));
	mayorName = GetString(std::bind(&cISC4RegionalCity::GetMayorName, pRegionalCity, _1));

	switch (tileSize)
	{
	case cISC4Region::eCityTileSize::Small:
		cityTileSize = "Small";
		break;
	case cISC4Region::eCityTileSize::Medium:
		cityTileSize = "Medium";
		break;
	case cISC4Region::eCityTileSize::Large:
		cityTileSize = "Large";
		break;
	}

	switch (pRegionalCity->GetDifficultyLevel())
	{
	case 0:
		difficulty = "Easy";
		break;
	case 1:
		difficulty = "Medium";
		break;
	case 2:
		difficulty = "Hard";
		break;
	}

	mayorRating = pRegionalCity->GetMayorRating();
	resPop = pRegionalCity->GetPopulation();
	comPop = pRegionalCity->GetCommercialJobs();
	indPop = pRegionalCity->GetIndustrialJobs();
	workforcePercentage = pRegionalCity->GetWorkforcePercentage()->percentage * 100.0f;
	budget = pRegionalCity->GetBudget();
	income = pRegionalCity->GetIncome();

	for (const auto& item : RCITypes)
	{
		const uint32_t sc4RCIGroup = item.first;

		RCIGroupInfo info{};

		info.population = pRegionalCity->GetPopulation(sc4RCIGroup);
		info.extrapolatedPopulation = pRegionalCity->GetExtrapolatedPopulation(sc4RCIGroup);
		info.taxRate = pRegionalCity->GetTaxRate(sc4RCIGroup);

		rciGroupInfo.emplace(item.second, info);
	}
}

const std::string& RegionalCityData::GetCityName() const
{
	return cityName;
}

const std::string& RegionalCityData::GetMayorName() const
{
	return mayorName;
}

int8_t RegionalCityData::GetMayorRating() const
{
	return mayorRating;
}

const std::string& RegionalCityData::GetDifficulty() const
{
	return difficulty;
}

const std::string& RegionalCityData::GetCityTileSize() const
{
	return cityTileSize;
}

int32_t RegionalCityData::GetResidentialPopulation() const
{
	return resPop;
}

int32_t RegionalCityData::GetCommercialJobs() const
{
	return comPop;
}

int32_t RegionalCityData::GetIndustrialJobs() const
{
	return indPop;
}

float RegionalCityData::GetWorkforcePercentage() const
{
	return workforcePercentage;
}

const RCIGroupInfo& RegionalCityData::GetRCIGroupInfo(RCIGroup group) const
{
	return rciGroupInfo.at(group);
}

float RegionalCityData::GetBudget() const
{
	return budget;
}

float RegionalCityData::GetIncome() const
{
	return income;
}
