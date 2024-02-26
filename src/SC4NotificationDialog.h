#///////////////////////////////////////////////////////////////////////////////
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
#include "cIGZString.h"

// SC4's standard notification dialog with an OK button.
// TGI 0, 0x96A006B0, 0xCA8CBF0F
namespace SC4NotificationDialog
{
	void ShowDialog(const cIGZString& message, const cIGZString& caption);
};

