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

#include "DataExport.h"
#include "Logger.h"
#include "CityDataCSVWriter.h"
#include "FileSystem.h"
#include "cIGZString.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"
#include "SC4NotificationDialog.h"

static const uint32_t kExportMessageGroupID = 0x42944A38;
static const uint32_t kExportMessageSuccessInstanceID = 0x6DEDCBA6;
static const uint32_t kExportMessageFailedInstanceID = 0x6DEDCBA7;

namespace
{
	void DisplayExportStatusMessage(const cIGZString& message)
	{
		SC4NotificationDialog::ShowDialog(message, cRZBaseString("Region Census"));
	}

	void DisplayExportSuccessMessage(const std::string& exportedFileName)
	{
		cRZAutoRefCount<cIGZString> text;

		if (StringResourceManager::GetLocalizedString(
			StringResourceKey(kExportMessageGroupID, kExportMessageSuccessInstanceID),
			text.AsPPObj()))
		{
			cRZBaseString message(*text);

			// Append to output file name to the end of the success message.
			// This matches the behavior of SC4's city save success message.

			char formattedFileName[1024]{};

			int formattedTextLength = std::snprintf(
				formattedFileName,
				sizeof(formattedFileName),
				" (%s)",
				exportedFileName.c_str());

			message.Append(formattedFileName, formattedTextLength);

			DisplayExportStatusMessage(message);
		}
	}

	void DisplayExportFailedMessage()
	{
		cRZAutoRefCount<cIGZString> message;

		if (StringResourceManager::GetLocalizedString(
			StringResourceKey(kExportMessageGroupID, kExportMessageSuccessInstanceID),
			message.AsPPObj()))
		{
			DisplayExportStatusMessage(*message);
		}
	}

	std::string MakeRegionCensusFileName(const std::string& fileExtension)
	{
		time_t rawTime;
		time(&rawTime);

		tm timeInfo;
		localtime_s(&timeInfo, &rawTime);

		std::ostringstream os;

		// The file name uses the format: Census-2024-1-1-18-30-00.<extension>.
		os << "Census-" << std::put_time(&timeInfo, "%Y-%m-%d-%H-%M-%S");

		if (fileExtension.length() > 0)
		{
			if (fileExtension[0] != '.')
			{
				os << '.';
			}

			os << fileExtension;
		}

		return os.str();
	}
}

void DataExport::WriteRegionCensusCSVFile(const std::vector<RegionalCityData>& data)
{
	try
	{
		const std::string fileName = MakeRegionCensusFileName(".csv");

		const FileSystem& fileSystem = FileSystem::GetInstance();

		std::filesystem::path outputPath = fileSystem.GetRegionCensusExportDirectory();
		outputPath /= fileName;

		CityDataCSVWriter csvWriter(outputPath);
		csvWriter.Write(data);

		DisplayExportSuccessMessage(fileName);
	}
	catch (const std::exception& e)
	{
		Logger::GetInstance().WriteLine(LogLevel::Error, e.what());
		DisplayExportFailedMessage();
	}
}
