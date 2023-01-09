#pragma once

#include "DataStruct.h"

#include <random>

NS_JYE_BEGIN

namespace Utility
{
	static unsigned int HashCode(const char* str)
	{
		size_t len = strlen(str);
		unsigned int hash = 0;
		for (size_t i = 0; i < len; ++i)
			hash = 65599 * hash + str[i];
		return hash ^ (hash >> 16);
	}

	static unsigned int HashCode(const String& str)
	{
		unsigned int hash = 0;
		for (size_t i = 0; i < str.size(); ++i)
			hash = 65599 * hash + str[i];
		return hash ^ (hash >> 16);
	}

	static unsigned int HashCode(const Vector<unsigned char>& buf)
	{
		unsigned int hash = 0;
		for (size_t i = 0; i < buf.size(); ++i)
			hash = 65599 * hash + buf[i];
		return hash ^ (hash >> 16);
	}

	static bool ReplaceString(String& str, const String& from, const String& to)
	{
		size_t start_pos = str.find(from);
		if (start_pos == String::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	static String& trim(String& s)
	{
		if (s.empty())
		{
			return s;
		}
		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
		return s;
	}

	static Vector<String> split(const String& s, const String& delim)
	{
		Vector<String> elems;
		size_t pos = 0;
		size_t len = s.length();
		size_t delim_len = delim.length();
		if (delim_len == 0) return elems;
		while (pos < len)
		{
			int find_pos = s.find(delim, pos);
			if (find_pos < 0)
			{
				elems.push_back(s.substr(pos, len - pos));
				break;
			}
			elems.push_back(s.substr(pos, find_pos - pos));
			pos = find_pos + delim_len;
		}
		return std::move(elems);
	}

	template<typename ... Args>
	String StringFormat(const String& format, Args ... args)
	{
		size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		return String(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}

	static String GetPathExt(const String& path)
	{
		String ext;
		size_t dotPos = path.find_last_of(".");
		if (dotPos != std::string::npos)
		{
			ext = path.substr(dotPos);
		}

		return ext;
	}

	static void ReplaceAllString(String& str, const String& from, const String& to)
	{
		size_t start_pos = 0;
		do
		{
			start_pos = str.find(from, start_pos);
			if (start_pos != std::string::npos)
			{
				str.replace(start_pos, from.length(), to);
				start_pos += to.size();//±‹√‚÷ÿ∏¥ÃÊªª◊‘º∫
			}
		} while (start_pos != std::string::npos);
	}

	static int ThreadsafeRand(const int& min, const int& max)
	{
		std::mt19937 generator(std::random_device{}());
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(generator);
	}

	static std::string RandomString(size_t length)
	{
		auto randchar = []() -> char
		{
			const char charset[] =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";
			const size_t max_index = (sizeof(charset) - 1);
			return charset[ThreadsafeRand(0, max_index)];
		};
		std::string str(length, 0);
		std::generate_n(str.begin(), length, randchar);
		return str;
	}

	static std::string GetFileSuffixString(const std::string& path)
	{
		const std::string fileName = path;
		size_t pos = fileName.rfind('.');
		if (pos == std::string::npos) {
			return "";
		}
		return fileName.substr(++pos);
	}
}

NS_JYE_END
