#ifndef __GNOME_CHAIN__
#define __GNOME_CHAIN__

#include "utils.h"
#include "block.h"

#include <vector>

#include <boost/json.hpp>

namespace json = boost::json;

namespace gnome
{
	class chain
	{
		std::vector<block> data;
		
	public:
		chain();

		block append(const payload_t &payload);

		const block &front() const;

		const block &back() const;

		const block &get(size_t index) const;

		size_t size() const;

		bool replace(const chain &other);

		json::value to_json() const;

		static bool is_valid(const chain &chain);
	};
}

#endif//__GNOME_CHAIN__