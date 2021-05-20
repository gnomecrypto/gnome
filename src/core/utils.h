#ifndef __GNOME_UTILS_H__
#define __GNOME_UTILS_H__

#include <cassert>
#include <chrono>
#include <iomanip>
#include <ios>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include <openssl/rand.h>

namespace gnome
{
	typedef unsigned char byte_t;
	typedef long long time_t;
	typedef std::string hash_t;
	typedef std::string uuid_t;
	typedef std::string payload_t;
	typedef std::stringstream stream_t;
	typedef unsigned long long amount_t;

	template<typename ... Args>
	std::string format(const std::string &fmt, Args ...args)
	{
		const int fullsize = std::snprintf(nullptr, 0, fmt.c_str(), args...) + 1;
		if (fullsize <= 0)
		{
			throw std::runtime_error("Error during formatting.");
		}
		const auto size = static_cast<size_t>(fullsize);
		const auto buff = std::make_unique<char[]>(size);
		std::snprintf(buff.get(), size, fmt.c_str(), args...);
		return std::string(buff.get(), buff.get() + size - 1);
	}

	inline time_t get_timestamp()
	{
		return std::chrono::system_clock::now().time_since_epoch().count();
	}

	inline hash_t bytes2hex(const byte_t *const data, size_t size)
	{
		stream_t ss;
		for (size_t i = 0; i < size; ++i)
		{
			ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(data[i]);
		}
		return ss.str();
	}

	inline std::vector<byte_t> hex2bytes(const hash_t &hash)
	{
		assert(hash.size() % 2 == 0);
		auto data = std::vector<byte_t>(hash.size() / 2);
		for (size_t i = 0; i < hash.size(); i += 2)
		{
			const auto num = strtoul(hash.substr(i, 2).c_str(), nullptr, 16);
			data.push_back(static_cast<byte_t>(num));
		}
		return data;
	}

	inline hash_t random_hex(const size_t length = 1)
	{
		const auto data = new byte_t[length];
		const auto rand = RAND_get_rand_method();
		int rc = RAND_bytes(data, sizeof(data));
		auto result = bytes2hex(data, length);
		delete[] data;
		return result;
	}

	inline uuid_t uuid()
	{
		return format("%s-%s-4%s-%s-%s", random_hex(8), random_hex(4), random_hex(3), random_hex(4), random_hex(12));
	}
}

#endif//__GNOME_UTILS_H__
