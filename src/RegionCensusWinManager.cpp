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

#include "RegionCensusWinManager.h"
#include "FileSystem.h"
#include "Logger.h"
#include "GZCLSIDDefs.h"
#include "GZWinKeyAcceleratorUtil.h"
#include "cGZMessage.h"
#include "cGZPersistResourceKey.h"
#include "cIGZCanvas.h"
#include "cIGZCanvasW32.h"
#include "cIGZCheatCodeManager.h"
#include "cIGZMessage2Standard.h"
#include "cIGZMessageServer2.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZWin.h"
#include "cISC4App.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"

#include <functional>
#include <string>
#include <vector>

static constexpr uint32_t kSC4MessagePostRegionInit = 0xCBB5BB45;
static constexpr uint32_t kSC4MessagePreRegionShutdown = 0x8BB5BB46;
static constexpr uint32_t kMessageCheatIssued = 0x230E27AC;

static constexpr uint32_t kToggleRegionCensusDialogCheatID = 0x8651AE4A;

static const uint32_t kGZWin_WinSC4App = 0x6104489A;
static const uint32_t kGZWin_SC4WinRegionScreen = 0xEA659793;

RegionCensusWinManager::RegionCensusWinManager()
	: refCount(0),
	  regionCensusDataProvider(),
	  regionCensusWin(this),
	  pRegionScreen(nullptr),
	  initialized(false)
{
}

bool RegionCensusWinManager::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZCLSID::kcIGZMessageTarget2)
	{
		*ppvObj = static_cast<cIGZMessageTarget2*>(this);

		AddRef();

		return true;
	}
	else if (riid == GZIID_cIGZUnknown)
	{
		*ppvObj = static_cast<cIGZUnknown*>(static_cast<cIGZMessageTarget2*>(this));

		AddRef();

		return true;
	}

	return false;
}

uint32_t RegionCensusWinManager::AddRef()
{
	return ++refCount;
}

uint32_t RegionCensusWinManager::Release()
{
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}

bool RegionCensusWinManager::RegisterMessages(cIGZMessageServer2* pMsgServ)
{
	std::vector<uint32_t> requiredNotifications;
	requiredNotifications.push_back(kSC4MessagePostRegionInit);
	requiredNotifications.push_back(kSC4MessagePreRegionShutdown);

	for (uint32_t messageID : requiredNotifications)
	{
		if (!pMsgServ->AddNotification(this, messageID))
		{
			return false;
		}
	}

	return true;
}

const IRegionCensusDataProvider& RegionCensusWinManager::GetRegionCensusData()
{
	return regionCensusDataProvider;
}

cIGZWin* RegionCensusWinManager::GetParentWin()
{
	return pRegionScreen;
}

bool RegionCensusWinManager::DoMessage(cIGZMessage2* pMessage)
{
	cIGZMessage2Standard* pStandardMsg = static_cast<cIGZMessage2Standard*>(pMessage);
	uint32_t dwType = pStandardMsg->GetType();

	switch (dwType)
	{
	case kSC4MessagePostRegionInit:
		PostRegionInit();
		break;
	case kSC4MessagePreRegionShutdown:
		PreRegionShutdown();
		break;
	case kMessageCheatIssued:
		ProcessCheatCode(pStandardMsg);
		break;
	}

	return true;
}

void RegionCensusWinManager::PostRegionInit()
{
	if (!initialized)
	{
		initialized = true;

		Logger& logger = Logger::GetInstance();

		cISC4AppPtr pSC4App;

		if (pSC4App)
		{
			cIGZWin* mainWindow = pSC4App->GetMainWindow();

			if (mainWindow)
			{
				cIGZWin* pWinSC4App = mainWindow->GetChildWindowFromID(kGZWin_WinSC4App);

				if (pWinSC4App)
				{
					// We increment the reference count on the window we get from GetChildWindowFromID
					// because this window reference to perform clean up when a region shutdown
					// message is received.
					pRegionScreen = pWinSC4App->GetChildWindowFromID(kGZWin_SC4WinRegionScreen);

					if (!pRegionScreen)
					{
						logger.WriteLine(LogLevel::Error, "The region screen window pointer was null.");
						return;
					}

					pRegionScreen->AddRef();
				}
				else
				{
					logger.WriteLine(LogLevel::Error, "The SC4App window pointer was null.");
					return;
				}
			}
			else
			{
				logger.WriteLine(LogLevel::Error, "The main window pointer was null.");
				return;
			}

			regionCensusDataProvider.PostRegionInit(pSC4App->GetRegion());

			cIGZCheatCodeManager* pCheatMgr = pSC4App->GetCheatCodeManager();

			if (pCheatMgr)
			{
				pCheatMgr->AddNotification2(this, 0);
				pCheatMgr->RegisterCheatCode(kToggleRegionCensusDialogCheatID, cRZBaseString("RegionCensus"));
			}
			else
			{
				logger.WriteLine(LogLevel::Error, "The cheat manager pointer was null.");
			}

			if (!UpdateWindowMessageHookRegistration(true))
			{
				logger.WriteLine(
					LogLevel::Error,
					"Failed to set the middle click message hook.");
			}
		}
		else
		{
			logger.WriteLine(LogLevel::Error, "The SC4 application pointer was null.");
		}
	}
}

void RegionCensusWinManager::PreRegionShutdown()
{
	if (initialized)
	{
		initialized = false;

		cISC4AppPtr pSC4App;

		if (pSC4App)
		{
			cIGZCheatCodeManager* pCheatMgr = pSC4App->GetCheatCodeManager();

			if (pCheatMgr)
			{
				pCheatMgr->RemoveNotification2(this, 0);
				pCheatMgr->UnregisterCheatCode(kToggleRegionCensusDialogCheatID);
			}
		}

		if (pRegionScreen)
		{
			regionCensusWin.Destroy();

			pRegionScreen->Release();
			pRegionScreen = nullptr;
		}

		regionCensusDataProvider.PreRegionShutdown();
		UpdateWindowMessageHookRegistration(false);
	}
}

void RegionCensusWinManager::ProcessCheatCode(cIGZMessage2Standard* pStandardMsg)
{
	uint32_t cheatID = static_cast<uint32_t>(pStandardMsg->GetData1());

	if (cheatID == kToggleRegionCensusDialogCheatID)
	{
		ToggleRegionCensusWindowVisibility();
	}
}

void RegionCensusWinManager::ToggleRegionCensusWindowVisibility()
{
	if (regionCensusWin.WindowCreated())
	{
		regionCensusWin.ToggleWindowVisibility();
	}
	else
	{
		regionCensusWin.Create();
	}
}

bool RegionCensusWinManager::UpdateWindowMessageHookRegistration(bool registerHook)
{
	bool result = false;

	cIGZCanvasPtr pCanvas;

	if (pCanvas)
	{
		cRZAutoRefCount<cIGZCanvasW32> pCanvasW32;

		if (pCanvas->QueryInterface(GZIID_cIGZCanvasW32, pCanvasW32.AsPPVoid()))
		{
			result = pCanvasW32->AddWinProcFilter(this, registerHook);
		}
	}

	return result;
}

LRESULT RegionCensusWinManager::FilterMessage(HWND hWnd, UINT uMsg, WPARAM wParram, LPARAM lParam, bool& handled)
{
	handled = false;

	if (uMsg == WM_MBUTTONUP)
	{
		handled = true;
		ToggleRegionCensusWindowVisibility();
	}

	return 0;
}
