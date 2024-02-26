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
#include "cIGZMessageTarget2.h"
#include "cIGZWinMessageFilter.h"
#include "IRegionCensusWinManager.h"
#include "RegionCensusDataProvider.h"
#include "RegionCensusWin.h"
#include <Windows.h>
#include "CAuxThunk.h"
#include "wil\resource.h"

class cIGZMessage2Standard;
class cIGZMessageServer2;

class RegionCensusWinManager final :
	private CAuxThunk<RegionCensusWinManager>,
	private cIGZMessageTarget2,
	private IRegionCensusWinManager
{
public:

	RegionCensusWinManager();

	bool QueryInterface(uint32_t riid, void** ppvObj) override;

	uint32_t AddRef() override;

	uint32_t Release() override;

	bool RegisterMessages(cIGZMessageServer2* pMsgServ);

private:

	const IRegionCensusDataProvider& GetRegionCensusData() override;

	cIGZWin* GetParentWin() override;

	bool DoMessage(cIGZMessage2* pMsg) override;

	void PostRegionInit();

	void PreRegionShutdown();

	void ProcessCheatCode(cIGZMessage2Standard* pStandardMsg);

	void ToggleRegionCensusWindowVisibility();

	LRESULT MouseMessageHookProc(int nCode, WPARAM wParam, LPARAM lParam);

	uint32_t refCount;
	RegionCensusDataProvider regionCensusDataProvider;
	RegionCensusWin regionCensusWin;
	cIGZWin* pRegionScreen;
	bool initialized;
	wil::unique_hhook mouseMessageHook;
};

