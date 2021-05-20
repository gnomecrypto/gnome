#pragma once

#include "crypto.h"
#include "utils.h"

namespace gnome
{
	class wallet
	{
		amount_t balance;
		const keypair_t keys;

	public:

		wallet(const payload_t &secret)
			: balance(0)
			, keys(eddsa_keypair(secret))
		{}

		hash_t sign(const payload_t &data) const;

		hash_t get_address() const;

		amount_t get_balance() const;

		std::string to_json() const;
	};
}
