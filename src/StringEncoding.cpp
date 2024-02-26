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

#include "StringEncoding.h"
#include "cIGZString.h"

#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"

#include <stdexcept>

namespace
{
	void ThrowWin32Error(const char* const methodName, uint32_t lastError)
	{
		char buffer[1024]{};

		std::snprintf(
			buffer,
			sizeof(buffer),
			"%s returned error code 0x%08X.",
			methodName,
			lastError);

		throw std::runtime_error(buffer);
	}
}

std::wstring StringEncoding::Utf8ToUtf16(const char* const utf8Str, int utf8StrLength)
{
	int convertedSize = MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8Str,
		utf8StrLength,
		nullptr,
		0);

	if (convertedSize <= 0)
	{
		ThrowWin32Error("MultiByteToWideChar", GetLastError());
	}

	std::wstring utf16Str(convertedSize, L' ');

	convertedSize = MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8Str,
		utf8StrLength,
		utf16Str.data(),
		static_cast<int>(utf16Str.size()));

	if (convertedSize <= 0)
	{
		ThrowWin32Error("MultiByteToWideChar", GetLastError());
	}

	return utf16Str;
}

std::wstring StringEncoding::Utf8ToUtf16(const cIGZString& gzStr)
{
	return Utf8ToUtf16(gzStr.ToChar(), static_cast<int>(gzStr.Strlen()));
}

std::string StringEncoding::Utf16ToUtf8(const wchar_t* const utf16Str, int utf16StrLength)
{
	int convertedSize = WideCharToMultiByte(
		CP_UTF8,
		0,
		utf16Str,
		utf16StrLength,
		nullptr,
		0,
		nullptr,
		nullptr);

	if (convertedSize <= 0)
	{
		ThrowWin32Error("WideCharToMultiByte", GetLastError());
	}

	std::string utf8Str(convertedSize, L' ');

	convertedSize = WideCharToMultiByte(
		CP_UTF8,
		0,
		utf16Str,
		utf16StrLength,
		utf8Str.data(),
		static_cast<int>(utf8Str.size()),
		nullptr,
		nullptr);

	if (convertedSize <= 0)
	{
		ThrowWin32Error("WideCharToMultiByte", GetLastError());
	}

	return utf8Str;
}

std::string StringEncoding::Utf16ToUtf8(const std::filesystem::path& path)
{
	const std::wstring& nativePath = path.native();

	return Utf16ToUtf8(nativePath.c_str(), static_cast<int>(nativePath.size()));
}
