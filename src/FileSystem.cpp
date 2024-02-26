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

#include "FileSystem.h"
#include "StringEncoding.h"
#include "cISC4App.h"
#include "cISC4Region.h"
#include "cRZBaseString.h"
#include "GZServPtrs.h"

#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"

namespace
{
	std::filesystem::path GetDllFolderPath()
	{
		wil::unique_cotaskmem_string modulePath = wil::GetModuleFileNameW(wil::GetModuleInstanceHandle());

		std::filesystem::path temp(modulePath.get());

		return temp.parent_path();
	}

	std::wstring GetRegionName()
	{
		std::wstring regionName;

		cISC4AppPtr pSC4App;

		if (pSC4App)
		{
			cISC4Region* pRegion = pSC4App->GetRegion();

			if (pRegion)
			{
				// The following is an ugly hack to work around a problem with the cISC4Region class:
				//
				// The developers who wrote the class made the GetName and GetDirectoryName methods
				// return a pointer to the internal cRZString class that implements cIGZString instead
				// of a cIGZString pointer.
				//
				// To fix this we cast the char pointer returned by GetName to void**, the void** pointer
				// points to the start of the cRZSting vtable.
				// Then we cast that cRZString vtable pointer to a cIGZSting pointer.

				cIGZString* name = reinterpret_cast<cIGZString*>(reinterpret_cast<void**>(pRegion->GetName()));

				regionName = StringEncoding::Utf8ToUtf16(*name);
			}
		}

		return regionName;
	}
}

FileSystem& FileSystem::GetInstance()
{
	static FileSystem instance;

	return instance;
}

const std::filesystem::path& FileSystem::GetModuleFolderPath() const
{
	return moduleFolderPath;
}

std::filesystem::path FileSystem::GetRegionCensusExportDirectory() const
{
	std::filesystem::create_directory(regionCensusReportsRoot);

	std::filesystem::path exportDirectory = regionCensusReportsRoot;
	exportDirectory /= GetRegionName();

	std::filesystem::create_directory(exportDirectory);

	return exportDirectory;
}

void FileSystem::PostAppInit()
{
	cISC4AppPtr pSC4App;

	if (pSC4App)
	{
		// It appears that the only way to get the game's per-user folder path through the API
		// is to retrieve the per-user preferences file path and remove the file name.

		cRZBaseString appPreferencesFilePath;

		pSC4App->GetAppPreferencesFilePath(appPreferencesFilePath);

		std::filesystem::path appPrefsUtf16 = StringEncoding::Utf8ToUtf16(
			appPreferencesFilePath);

		std::filesystem::path userDataDir = appPrefsUtf16.remove_filename();

		regionCensusReportsRoot = userDataDir;
		regionCensusReportsRoot /= L"Region_Census";
	}
}

FileSystem::FileSystem()
	: moduleFolderPath(GetDllFolderPath()),
	  regionCensusReportsRoot()
{
}

