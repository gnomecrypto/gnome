#include "transaction.h"
#include "currency.h"

gnome::transaction::input_t::
input_t(const time_t timestamp, const hash_t from, const hash_t signature)
	: timestamp(timestamp), from(from), signature(signature)
{}

gnome::payload_t gnome::transaction::input_t::to_json() const
{
	return format(
		R"({"time":%u,"from":"%s","signature":"%s"})",
		timestamp, from, signature
	);
}

gnome::transaction::output_t::
output_t(const hash_t from, const amount_t amount, const amount_t fee)
	: to(from), amount(amount), fee(fee)
{}

gnome::payload_t gnome::transaction::output_t::to_json() const
{
	return format(
		R"({"to":"%s","amount":%u,"fee":%u})",
		to, amount, fee
	);
}

gnome::transaction::transaction(const transaction_type type, const input_t input, const output_t output)
	: uuid(gnome::uuid()), type(type), input(input), output(output)
{}

class gnome::transaction gnome::transaction::gen_transaction(
	const wallet &wallet,
	const hash_t &to,
	const amount_t amount,
	const transaction_type type
)
{
	const auto output = output_t(to, amount - transaction_fee, transaction_fee);
	const auto input = input_t(get_timestamp(), wallet.get_address(), wallet.sign(output.to_json()));
	return transaction(type, input, output);
}

class gnome::transaction gnome::transaction::new_transaction(
	const wallet &sender,
	const hash_t &to,
	amount_t amount,
	transaction_type type
)
{
	if (amount + transaction_fee > sender.get_balance())
	{
		return none();
	}

	return gen_transaction(sender, to, amount, type);
}

bool gnome::transaction::verify(const transaction& transaction)
{
	return eddsa_verify(transaction.input.from, transaction.input.signature, transaction.output.to_json());
}

class gnome::transaction gnome::transaction::none()
{
	return transaction(
		transaction_type::none,
		input_t(0, "", ""),
		output_t("", 0, 0)
	);
}
