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

#include <filesystem>
#include <string>

class cIGZString;

namespace StringEncoding
{
	std::wstring Utf8ToUtf16(const char* const utf8Str, int utf8StrLength);
	std::wstring Utf8ToUtf16(const cIGZString& gzStr);

	std::string Utf16ToUtf8(const wchar_t* const utf16Str, int utf16StrLength);
	std::string Utf16ToUtf8(const std::filesystem::path& path);
}