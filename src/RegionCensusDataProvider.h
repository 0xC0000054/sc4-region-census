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
#include "IRegionCensusDataProvider.h"

class cISC4Region;

class RegionCensusDataProvider final : public IRegionCensusDataProvider
{
public:

	RegionCensusDataProvider();

	const RegionCensusTotals& GetRegionCensusTotals() const;

	const std::vector<RegionalCityData> GetRegionalCityData() const;

	void Clear();

	void PostRegionInit(cISC4Region* pRegion);

	void PreRegionShutdown();

private:

	bool initialized;
	RegionCensusTotals regionCensusTotals;
	std::vector<RegionalCityData> regionalCityData;
};

