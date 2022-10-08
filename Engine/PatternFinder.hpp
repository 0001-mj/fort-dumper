#pragma once

#include <cstdint>
#include <windows.h>
#include <vector>

/// <summary>
/// Searches for the first pattern in the module.
/// </summary>
/// <param name="module">The module to scan.</param>
/// <param name="pattern">The pattern (Example: "\x12\xAB\x34")</param>
/// <param name="mask">The mask (Example: "x?x")</param>
/// <returns>The address of the found pattern or -1 if the pattern was not found.</returns>
uintptr_t FindPattern(HMODULE module, const unsigned char* pattern, const char* mask);

/// <summary>
/// Searches for the first pattern in the memory region.
/// </summary>
/// <param name="start">The start address of the memory region to scan.</param>
/// <param name="length">The length of the memory region.</param>
/// <param name="pattern">The pattern (Example: "\x12\xAB\x34")</param>
/// <param name="mask">The mask (Example: "x?x")</param>
/// <returns>The address of the found pattern or -1 if the pattern was not found.</returns>
uintptr_t FindPattern(uintptr_t start, size_t length, const unsigned char* pattern, const char* mask);

static uintptr_t FindPattern(const char* signature, bool bRelative = false, uint32_t offset = 0)
{
	uintptr_t base_address = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
	static auto patternToByte = [](const char* pattern)
	{
		auto bytes = std::vector<int>{};
		const auto start = const_cast<char*>(pattern);
		const auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current)
		{
			if (*current == '?')
			{
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else { bytes.push_back(strtoul(current, &current, 16)); }
		}
		return bytes;
	};

	const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
	const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);

	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto patternBytes = patternToByte(signature);
	const auto scanBytes = reinterpret_cast<std::uint8_t*>(base_address);

	const auto s = patternBytes.size();
	const auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i)
	{
		bool found = true;
		for (auto j = 0ul; j < s; ++j)
		{
			if (scanBytes[i + j] != d[j] && d[j] != -1)
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			uintptr_t address = reinterpret_cast<uintptr_t>(&scanBytes[i]);
			if (bRelative)
			{
				address = ((address + offset + 4) + *(int32_t*)(address + offset));
				return address;
			}
			return address;
		}
	}

	return NULL;
}