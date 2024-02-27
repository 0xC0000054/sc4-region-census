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

#include "RegionCensusDataProvider.h"
#include "Logger.h"
#include "cISC4App.h"
#include "cISC4Region.h"
#include "cISC4RegionalCity.h"
#include "GZServPtrs.h"
#include <array>
#include <memory>

RegionCensusDataProvider::RegionCensusDataProvider()
	: regionCensusTotals{},
	  initialized(false)
{
}

const RegionCensusTotals& RegionCensusDataProvider::GetRegionCensusTotals() const
{
	return regionCensusTotals;
}

const std::vector<RegionalCityData> RegionCensusDataProvider::GetRegionalCityData() const
{
	return regionalCityData;
}

void RegionCensusDataProvider::Clear()
{
	memset(&regionCensusTotals, 0, sizeof(regionCensusTotals));
	regionalCityData.clear();
}

void RegionCensusDataProvider::PostRegionInit(cISC4Region* pRegion)
{
	if (!initialized)
	{
		Logger& logger = Logger::GetInstance();

		initialized = true;
		if (pRegion)
		{
			try
			{
				eastl::vector<cISC4Region::cLocation> cityLocations;

				pRegion->GetCityLocations(cityLocations);

				size_t count = cityLocations.size();

				if (count > 0)
				{

					for (size_t i = 0; i < count; i++)
					{
						const cISC4Region::cLocation& location = cityLocations[i];

						// The city pointer should not be released.

						cISC4RegionalCity*& pRegionalCity = pRegion->GetCity(location.x, location.y);

						if (pRegionalCity)
						{
							if (pRegionalCity->GetEstablished())
							{
								regionCensusTotals.developedCities++;

								RegionalCityData cityData(pRegionalCity, location.cityTileSize);

								regionalCityData.push_back(cityData);

								regionCensusTotals.resPop += cityData.GetResidentialPopulation();
								regionCensusTotals.res1Pop += cityData.GetRCIGroupInfo(RCIGroup::R1).population;
								regionCensusTotals.res2Pop += cityData.GetRCIGroupInfo(RCIGroup::R2).population;
								regionCensusTotals.res3Pop += cityData.GetRCIGroupInfo(RCIGroup::R3).population;
								regionCensusTotals.comPop += cityData.GetCommercialJobs();
								regionCensusTotals.cs1Pop += cityData.GetRCIGroupInfo(RCIGroup::Cs1).population;
								regionCensusTotals.cs2Pop += cityData.GetRCIGroupInfo(RCIGroup::Cs2).population;
								regionCensusTotals.cs3Pop += cityData.GetRCIGroupInfo(RCIGroup::Cs3).population;
								regionCensusTotals.co2Pop += cityData.GetRCIGroupInfo(RCIGroup::Co2).population;
								regionCensusTotals.co3Pop += cityData.GetRCIGroupInfo(RCIGroup::Co3).population;
								regionCensusTotals.indPop += cityData.GetIndustrialJobs();
								regionCensusTotals.iaPop += cityData.GetRCIGroupInfo(RCIGroup::IR).population;
								regionCensusTotals.idPop += cityData.GetRCIGroupInfo(RCIGroup::ID).population;
								regionCensusTotals.imPop += cityData.GetRCIGroupInfo(RCIGroup::IM).population;
								regionCensusTotals.ihtPop += cityData.GetRCIGroupInfo(RCIGroup::IHT).population;
								regionCensusTotals.regionFunds += static_cast<int64_t>(cityData.GetBudget());
							}
							else
							{
								regionCensusTotals.undevelopedCities++;
							}
						}
					}
				}
			}
			catch (const std::exception& e)
			{
				logger.WriteLineFormatted(
					LogLevel::Error,
					"Error loading the region census data: %s",
					e.what());
				Clear();
			}
		}
		else
		{
			logger.WriteLine(
				LogLevel::Error,
				"Error loading the region census data, the region pointer was null.");
		}
	}
}

void RegionCensusDataProvider::PreRegionShutdown()
{
	if (initialized)
	{
		initialized = false;

		Clear();
	}
}
