#include "wallet.h"

gnome::hash_t gnome::wallet::sign(const payload_t& data) const
{
	return eddsa_sign(keys.prv, data);
}

gnome::hash_t gnome::wallet::get_address() const
{
	return keys.pub;
}

gnome::amount_t gnome::wallet::get_balance() const
{
	return balance;
}

std::string gnome::wallet::to_json() const
{
	return format(
		R"({"address":"%s","balance":%u})",
		keys.pub, balance
	);
}
