#ifndef __GNOME_BLOCK__
#define __GNOME_BLOCK__

#include "utils.h"

#include <boost/json.hpp>

namespace json = boost::json;

namespace gnome
{
	struct block
	{
		const time_t timestamp;
		const hash_t prev_hash;
		const hash_t curr_hash;
		const payload_t payload;
		const hash_t validator;
		const hash_t signature;

		block(
			time_t timestamp, // The block's creation time, in milliseconds since the epoch.
			hash_t prev_hash, // Hash of the previous block.
			hash_t curr_hash, // Hash of the current block.
			payload_t payload, // Data contained by this block.
			hash_t validator, // Address of the miner who produced this block.
			hash_t signature // Encrypted block hash signed by the validator.
		);

		// returns true if this block is the valid descendant to the other block
		bool is_next(const block &other) const;

		// returns true if this block is the valid predecessor to the other block
		bool is_previous(const block &other) const;

		// returns true if this block is locally valid, that is, its hash is correct
		bool is_valid() const;

		json::value to_json() const;
		
		std::string to_string() const;

		static block create(const block &prev, const payload_t &payload);

		static hash_t hash(const block &block);

		static block genesis();

		static block none();
	};
	
	inline bool operator==(const block &lhs, const block &rhs)
	{
		return lhs.timestamp == rhs.timestamp
			&& lhs.prev_hash == rhs.prev_hash
			&& lhs.curr_hash == rhs.curr_hash
			&& lhs.payload == rhs.payload
			&& lhs.signature == rhs.signature;
	}

	inline bool operator!=(const block &lhs, const block &rhs)
	{
		return lhs.timestamp != rhs.timestamp
			|| lhs.prev_hash != rhs.prev_hash
			|| lhs.curr_hash != rhs.curr_hash
			|| lhs.payload != rhs.payload
			|| lhs.signature != rhs.signature;
	}
}

#endif//__GNOME_BLOCK__
