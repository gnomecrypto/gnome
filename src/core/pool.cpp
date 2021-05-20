#include "pool.h"

gnome::pool::pool(): transactions(std::vector<transaction>())
{
}

void gnome::pool::add_transaction(const transaction& transaction)
{
	transactions.push_back(transaction);
}

bool gnome::pool::verify() const
{
	for (const auto& t : transactions)
	{
		if (!transaction::verify(t))
		{
			return false;
		}
	}
	return true;
}
