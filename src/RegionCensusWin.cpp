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

#include "RegionCensusWin.h"
#include "DataExport.h"
#include "Logger.h"
#include "cGZMessage.h"
#include "cIGZLanguageManager.h"
#include "cIGZLanguageUtility.h"
#include "cIGZWin.h"
#include "cIGZWinGen.h"
#include "cIGZWinText.h"
#include "cISC4App.h"
#include "cISC4Region.h"
#include "cISC4RegionalCity.h"
#include "cGZPersistResourceKey.h"
#include "cRZAutoRefCount.h"
#include "IRegionCensusWinManager.h"
#include "SC4UI.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"
#include "GZServPtrs.h"

namespace
{
	void SetWinProc(cIGZWin* pWin, cIGZWinProc* pWinProc)
	{
		cRZAutoRefCount<cIGZWinGen> winGen;

		if (pWin->QueryInterface(GZIID_cIGZWinGen, winGen.AsPPVoid()))
		{
			winGen->SetWinProc(pWinProc);
		}
	}

	enum class NumberFormat
	{
		Number,
		Money
	};

	void SetNumberText(
		cIGZWin* pWin,
		uint32_t controlID,
		cIGZLanguageUtility* pLanguageUtility,
		int64_t number,
		NumberFormat format)
	{
		if (pWin && pLanguageUtility)
		{
			cRZAutoRefCount<cIGZWinText> pWinText;

			if (pWin->GetChildAs(controlID, GZIID_cIGZWinText, pWinText.AsPPVoid()))
			{
				cRZBaseString text;
				bool result = false;

				if (format == NumberFormat::Money)
				{
					// The currently symbol string is the hexadecimal-escaped UTF-8 encoding
					// of the section symbol (§).
					// The \xC2 value is the first byte in a two byte UTF-8 sequence, and the
					// \xA7 value is the Unicode value of the section symbol (U+00A7).
					// See the following page for more information on UTF-8 encoding:
					// https://www.fileformat.info/info/unicode/utf8.htm
					//
					// UTF-8 is SC4's native string encoding.

					cRZBaseString currencySymbol("\xC2\xA7");

					result = pLanguageUtility->MakeMoneyString(number, text, &currencySymbol);
				}
				else
				{
					result = pLanguageUtility->MakeNumberString(number, text);
				}

				if (result)
				{
					pWinText->SetCaption(text);
				}
			}
		}
	}

	constexpr uint32_t kResTotalPopTextID = 0xC2FE60A0;
	constexpr uint32_t kRes1TotalPopTextID = 0xC2FE60A1;
	constexpr uint32_t kRes2TotalPopTextID = 0xC2FE60A2;
	constexpr uint32_t kRes3TotalPopTextID = 0xC2FE60A3;
	constexpr uint32_t kComTotalPopTextID = 0xC2FE60A4;
	constexpr uint32_t kCs1TotalPopTextID = 0xC2FE60A5;
	constexpr uint32_t kCs2TotalPopTextID = 0xC2FE60A6;
	constexpr uint32_t kCs3TotalPopTextID = 0xC2FE60A7;
	constexpr uint32_t kCo2TotalPopTextID = 0xC2FE60A8;
	constexpr uint32_t kCo3TotalPopTextID = 0xC2FE60A9;
	constexpr uint32_t kIndTotalPopTextID = 0xC2FE60AA;
	constexpr uint32_t kIATotalPopTextID = 0xC2FE60AB;
	constexpr uint32_t kIDTotalPopTextID = 0xC2FE60AC;
	constexpr uint32_t kIMTotalPopTextID = 0xC2FE60AD;
	constexpr uint32_t kIHTTotalPopTextID = 0xC2FE60AE;
	constexpr uint32_t kRegionTotalCitiesTextID = 0xC2FE60AF;
	constexpr uint32_t kRegionTotalDevelopedCitiesTextID = 0xC2FE60B0;
	constexpr uint32_t kRegionTotalUndevelopedCitiesTextID = 0xC2FE60B1;
	constexpr uint32_t kRegionTotalFundsTextID = 0xC2FE60B2;
	constexpr uint32_t kExportRegionCensusCSVButtonID = 0x7E6D20B7;
	constexpr uint32_t kCloseDialogButtonID = 0x17E2EA82;
}

RegionCensusWin::RegionCensusWin(IRegionCensusWinManager* pWimMgr)
	: pRegionCensusWin(nullptr),
	  pWinMgr(pWimMgr)
{
}

bool RegionCensusWin::Create()
{
	if (!pRegionCensusWin)
	{
		cIGZWin* pParentWin = pWinMgr->GetParentWin();

		// This is the TGI of our Region Census window.
		cGZPersistResourceKey key(0x00000000, 0x9cb6053f, 0x90e0199b);
		cRZAutoRefCount<cIGZWin> scriptWin;

		if (!SC4UI::CreateWindowFromScript(
			key,
			pParentWin,
			0x0B1E1F95,
			GZIID_cIGZWin,
			reinterpret_cast<void**>(&pRegionCensusWin)))
		{
			if (pRegionCensusWin)
			{
				pRegionCensusWin->Release();
				pRegionCensusWin = nullptr;
			}

			Logger::GetInstance().WriteLine(
				LogLevel::Error,
				"Failed to create the region census window, is RegionCensusUI.dat in the Plugins folder?");

			return false;
		}

		SetRegionCensusTotals(pWinMgr->GetRegionCensusData());

		pParentWin->ChildAdd(pRegionCensusWin);

		// This places the dialog in the top left of the region view screen.

		int32_t x = 2;
		int32_t y = 2;

		pRegionCensusWin->GZWinMoveTo(x, y);

		SetWinProc(pRegionCensusWin, this);

		pRegionCensusWin->PullToFront();
		pRegionCensusWin->ShowWindow();
	}

	return true;
}

bool RegionCensusWin::Destroy()
{
	if (pRegionCensusWin)
	{
		cIGZWin* pParentWin = pWinMgr->GetParentWin();

		pParentWin->ChildDelete(pRegionCensusWin);

		SetWinProc(pRegionCensusWin, nullptr);

		pRegionCensusWin->Release();
		pRegionCensusWin = nullptr;
	}

	return true;
}

bool RegionCensusWin::WindowCreated() const
{
	return pRegionCensusWin != nullptr;
}

void RegionCensusWin::ToggleWindowVisibility()
{
	if (pRegionCensusWin)
	{
		if (pRegionCensusWin->IsVisible())
		{
			pRegionCensusWin->HideWindow();
		}
		else
		{
			pRegionCensusWin->ShowWindow();
		}
	}
}

bool RegionCensusWin::DoWinProcMessage(cIGZWin* pWin, cGZMessage& msg)
{
	constexpr uint32_t kGZWin_MessageType_Command = 3;

	constexpr uint32_t kGZWin_Command_ButtonClicked = 0x287259F6;

	bool result = false;

	if (msg.dwMessageType == kGZWin_MessageType_Command
		&& msg.dwData1 == kGZWin_Command_ButtonClicked)
	{
		uint32_t controlID = msg.dwData2;

		if (controlID == kExportRegionCensusCSVButtonID)
		{
			const auto& censusDataProvider = pWinMgr->GetRegionCensusData();
			DataExport::WriteRegionCensusCSVFile(censusDataProvider.GetRegionalCityData());
			result = true;
		}
		else if (controlID == kCloseDialogButtonID)
		{
			pRegionCensusWin->HideWindow();
			result = true;
		}
	}


	return result;
}

void RegionCensusWin::SetRegionCensusTotals(const IRegionCensusDataProvider& censusDataProvider)
{
	const RegionCensusTotals& totals = censusDataProvider.GetRegionCensusTotals();

	cIGZLanguageManagerPtr pLM;

	if (pLM)
	{
		const uint32_t currentLanguageID = pLM->GetCurrentLanguage();
		cIGZLanguageUtility* pLanguageUtility = pLM->GetLanguageUtility(currentLanguageID);

		if (pLanguageUtility)
		{
			for (size_t i = 0; i < 19; i++)
			{
				uint32_t buttonID = 0;
				int64_t value = 0;
				NumberFormat format = NumberFormat::Number;

				switch (i)
				{
				case 0:
					buttonID = kResTotalPopTextID;
					value = totals.resPop;
					break;
				case 1:
					buttonID = kRes1TotalPopTextID;
					value = totals.res1Pop;
					break;
				case 2:
					buttonID = kRes2TotalPopTextID;
					value = totals.res2Pop;
					break;
				case 3:
					buttonID = kRes3TotalPopTextID;
					value = totals.res3Pop;
					break;
				case 4:
					buttonID = kComTotalPopTextID;
					value = totals.comPop;
					break;
				case 5:
					buttonID = kCs1TotalPopTextID;
					value = totals.cs1Pop;
					break;
				case 6:
					buttonID = kCs2TotalPopTextID;
					value = totals.cs2Pop;
					break;
				case 7:
					buttonID = kCs3TotalPopTextID;
					value = totals.cs3Pop;
					break;
				case 8:
					buttonID = kCo2TotalPopTextID;
					value = totals.co2Pop;
					break;
				case 9:
					buttonID = kCo3TotalPopTextID;
					value = totals.co3Pop;
					break;
				case 10:
					buttonID = kIndTotalPopTextID;
					value = totals.indPop;
					break;
				case 11:
					buttonID = kIATotalPopTextID;
					value = totals.iaPop;
					break;
				case 12:
					buttonID = kIDTotalPopTextID;
					value = totals.idPop;
					break;
				case 13:
					buttonID = kIMTotalPopTextID;
					value = totals.imPop;
					break;
				case 14:
					buttonID = kIHTTotalPopTextID;
					value = totals.ihtPop;
					break;
				case 15:
					buttonID = kRegionTotalCitiesTextID;
					value = totals.developedCities + totals.undevelopedCities;
					break;
				case 16:
					buttonID = kRegionTotalDevelopedCitiesTextID;
					value = totals.developedCities;
					break;
				case 17:
					buttonID = kRegionTotalUndevelopedCitiesTextID;
					value = totals.undevelopedCities;
					break;
				case 18:
					buttonID = kRegionTotalFundsTextID;
					value = totals.regionFunds;
					format = NumberFormat::Money;
					break;
				}

				SetNumberText(
					pRegionCensusWin,
					buttonID,
					pLanguageUtility,
					value,
					format);
			}
		}
	}
}