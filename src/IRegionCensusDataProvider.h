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
#include "RegionCensusTotals.h"
#include "RegionalCityData.h"
#include <vector>

class IRegionCensusDataProvider
{
public:

	virtual const RegionCensusTotals& GetRegionCensusTotals() const = 0;

	virtual const std::vector<RegionalCityData> GetRegionalCityData() const = 0;
};