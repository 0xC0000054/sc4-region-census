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

#include "version.h"
#include "FileSystem.h"
#include "Logger.h"
#include "RegionCensusWinManager.h"

#include "cIGZCOM.h"
#include "cIGZCmdLine.h"
#include "cIGZFrameWork.h"
#include "cIGZMessageServer2.h"
#include "cIGZMessage2Standard.h"
#include "cIGZWin.h"
#include "cIGZWinGen.h"
#include "cIGZWinMgr.h"
#include "cISC4App.h"
#include "cISC4Region.h"
#include "cISC4RegionalCity.h"
#include "cRZCOMDllDirector.h"
#include "GZServPtrs.h"

#include <filesystem>
#include <map>

static constexpr uint32_t kRegionCensusDirectorID = 0x48B46DE2;

static constexpr std::string_view PluginLogFileName = "SC4RegionCensus.log";

class RegionCensusDllDirector final : public cRZCOMDllDirector
{
public:

	RegionCensusDllDirector() : regionCensusWinManager()
	{
		std::filesystem::path dllFolderPath = FileSystem::GetInstance().GetModuleFolderPath();

		std::filesystem::path logFilePath = dllFolderPath;
		logFilePath /= PluginLogFileName;

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogLevel::Error, false);
		logger.WriteLogFileHeader("SC4RegionCensus v" PLUGIN_VERSION_STR);
	}

	uint32_t GetDirectorID() const
	{
		return kRegionCensusDirectorID;
	}

	bool PostAppInit()
	{
		Logger& logger = Logger::GetInstance();

		cIGZMessageServer2Ptr pMsgServ;
		if (pMsgServ)
		{
			if (!regionCensusWinManager.RegisterMessages(pMsgServ))
			{
				logger.WriteLine(LogLevel::Error, "Failed to subscribe to the required notifications.");
				return false;
			}
		}
		else
		{
			logger.WriteLine(LogLevel::Error, "Failed to subscribe to the required notifications.");
			return false;
		}

		FileSystem::GetInstance().PostAppInit();

		return true;
	}

	bool OnStart(cIGZCOM* pCOM)
	{
		cIGZFrameWork* const pFramework = RZGetFrameWork();

		const cIGZFrameWork::FrameworkState state = pFramework->GetState();

		if (state < cIGZFrameWork::kStatePreAppInit)
		{
			pFramework->AddHook(this);
		}
		else
		{
			PreAppInit();
		}

		return true;
	}

private:

	RegionCensusWinManager regionCensusWinManager;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static RegionCensusDllDirector sDirector;
	return &sDirector;
}