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

class cIGZWin;

class IRegionCensusWinManager
{
public:

	virtual const IRegionCensusDataProvider& GetRegionCensusData() = 0;

	virtual cIGZWin* GetParentWin() = 0;
};