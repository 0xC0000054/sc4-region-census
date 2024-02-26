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
#include "cRZBaseWinProc.h"
#include "IRegionCensusWinManager.h"

class cIGZWin;
class IRegionCensusDataProvider;

class RegionCensusWin final : public cRZBaseWinProc
{
public:

	RegionCensusWin(IRegionCensusWinManager* pWimMgr);

	bool Create();

	bool Destroy();

	bool WindowCreated() const;

	void ToggleWindowVisibility();

private:

	bool DoWinProcMessage(cIGZWin*, cGZMessage&) override;

	void SetRegionCensusTotals(const IRegionCensusDataProvider& censusDataProvider);

	cIGZWin* pRegionCensusWin;
	IRegionCensusWinManager* pWinMgr;
};

