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
#include "cIGZWinProcFilterW32.h"
#include "cIGZWinMessageFilter.h"
#include "IRegionCensusWinManager.h"
#include "RegionCensusDataProvider.h"
#include "RegionCensusWin.h"
#include <Windows.h>

class cIGZMessage2Standard;
class cIGZMessageServer2;

class RegionCensusWinManager final :
	private cIGZWinProcFilterW32,
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
	// cIGZWinProcFilterW32

	LRESULT FilterMessage(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParram,
		LPARAM lParam,
		bool& handled) override;

	// IRegionCensusWinManager

	const IRegionCensusDataProvider& GetRegionCensusData() override;

	cIGZWin* GetParentWin() override;

	// cIGZMessageTarget2

	bool DoMessage(cIGZMessage2* pMsg) override;

	// Private members

	void PostRegionInit();

	void PreRegionShutdown();

	void ProcessCheatCode(cIGZMessage2Standard* pStandardMsg);

	void ToggleRegionCensusWindowVisibility();

	bool UpdateWindowMessageHookRegistration(bool registerHook);

	uint32_t refCount;
	RegionCensusDataProvider regionCensusDataProvider;
	RegionCensusWin regionCensusWin;
	cIGZWin* pRegionScreen;
	bool initialized;
};

