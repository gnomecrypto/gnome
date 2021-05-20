#include "block.h"

#include "crypto.h"

gnome::block::block(
	const time_t timestamp, 
	const hash_t prev_hash, 
	const hash_t curr_hash, 
	const payload_t payload,
	const hash_t validator, 
	const hash_t signature
)
: timestamp(timestamp)
, prev_hash(prev_hash)
, curr_hash(curr_hash)
, payload(payload)
, validator(validator)
, signature(signature)
{}

bool gnome::block::is_next(const block &other) const
{
	return prev_hash == other.curr_hash;
}

bool gnome::block::is_previous(const block& other) const
{
	return curr_hash == other.prev_hash;
}

bool gnome::block::is_valid() const
{
	return curr_hash == hash(*this);
}

json::value gnome::block::to_json() const
{
	return json::object({
		{ "timestamp", timestamp },
		{ "prev_hash", prev_hash },
		{ "curr_hash", curr_hash },
		{ "payload", payload },
		{ "validator", validator },
		{ "signature", signature }
	});
}

std::string gnome::block::to_string() const
{
	return format(
		"BLOCK\n"
		"Timestamp: %d\n"
		"Prev Hash: %s\n"
		"Curr Hash: %s\n"
		"  Payload: %s\n"
		"Validator: %s\n"
		"Signature: %s",
		timestamp,
		prev_hash,
		curr_hash,
		payload,
		validator,
		signature
	);
}

gnome::block gnome::block::create(const block &prev, const payload_t &payload)
{
	const auto last_hash = prev.curr_hash;
	const auto timestamp = get_timestamp();
	const auto curr_hash = hash256(timestamp, last_hash, payload);
	return block(timestamp, last_hash, curr_hash, payload, "", "");
}

gnome::hash_t gnome::block::hash(const block &block)
{
	return hash256(block.timestamp, block.prev_hash, block.payload);
}

gnome::block gnome::block::genesis()
{
	return block(0, "", "", "", "", hash256(0, "", ""));
}

gnome::block gnome::block::none()
{
	return block(0, "", "", "", "", hash256(0, "", ""));
}
