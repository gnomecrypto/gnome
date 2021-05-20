#ifndef __GNOME_TRANSACTION_H__
#define __GNOME_TRANSACTION_H__

#include "utils.h"
#include "wallet.h"

namespace gnome
{
	enum class transaction_type : byte_t
	{
		none,
		stake,
		miner,
		transaction
	};
	
	class transaction
	{
		struct input_t
		{
			const time_t timestamp;
			const hash_t from;
			const hash_t signature;

			input_t(time_t timestamp, hash_t from, hash_t signature);
			
			payload_t to_json() const;
		};

		struct output_t
		{
			const hash_t to;
			const amount_t amount;
			const amount_t fee;

			output_t(hash_t from, amount_t amount, amount_t fee);

			payload_t to_json() const;
		};
		
		const uuid_t uuid;
		const transaction_type type;
		const input_t input;
		const output_t output;

		transaction(transaction_type type, input_t input, output_t output);

	public:

		transaction(const transaction &other) = default;

		static transaction gen_transaction(const wallet &wallet, const hash_t &to, amount_t amount, transaction_type type);

		static transaction new_transaction(const wallet&sender, const hash_t &to, amount_t amount, transaction_type type);

		static bool verify(const transaction &transaction);

		static transaction none();
	};
}

#endif//__GNOME_TRANSACTION_H__
