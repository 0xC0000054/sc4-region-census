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
#include <cstdint>
#include <map>
#include <string>
#include "cISC4Region.h"

enum class RCIGroup
{
	R1,
	R2,
	R3,
	Cs1,
	Cs2,
	Cs3,
	Co2,
	Co3,
	IR,
	ID,
	IM,
	IHT
};

struct RCIGroupInfo
{
	int32_t population;
	int32_t extrapolatedPopulation;
	float taxRate;
};

class RegionalCityData
{
public:

	RegionalCityData(
		cISC4RegionalCity* pRegionalCity,
		cISC4Region::eCityTileSize cityTileSize);

	const std::string& GetCityName() const;

	const std::string& GetMayorName() const;

	int8_t GetMayorRating() const;

	const std::string& GetDifficulty() const;

	const std::string& GetCityTileSize() const;

	int32_t GetResidentialPopulation() const;

	int32_t GetCommercialJobs() const;

	int32_t GetIndustrialJobs() const;

	float GetWorkforcePercentage() const;

	const RCIGroupInfo& GetRCIGroupInfo(RCIGroup group) const;

	float GetBudget() const;

	float GetIncome() const;

private:

	std::string cityName;
	std::string mayorName;
	std::string difficulty;
	std::string cityTileSize;
	int32_t resPop;
	int32_t comPop;
	int32_t indPop;
	float workforcePercentage;
	std::map<RCIGroup, RCIGroupInfo> rciGroupInfo;
	float budget;
	float income;
	int8_t mayorRating;
};