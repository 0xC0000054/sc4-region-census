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

struct RegionCensusTotals
{
	int64_t resPop;
	int64_t res1Pop;
	int64_t res2Pop;
	int64_t res3Pop;
	int64_t comPop;
	int64_t cs1Pop;
	int64_t cs2Pop;
	int64_t cs3Pop;
	int64_t co2Pop;
	int64_t co3Pop;
	int64_t indPop;
	int64_t iaPop;
	int64_t idPop;
	int64_t imPop;
	int64_t ihtPop;
	int64_t developedCities; // cities in Mayor mode
	int64_t undevelopedCities; // cities in God mode
	int64_t regionFunds;
};