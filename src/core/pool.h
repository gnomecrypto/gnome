#ifndef __GNOME_POOL_H__
#define __GNOME_POOL_H__

#include "transaction.h"

#include <vector>

namespace gnome
{
	class pool
	{
		std::vector<transaction> transactions;

	public:

		pool();

		void add_transaction(const transaction& transaction);

		bool verify() const;
	};
}

#endif//__GNOME_POOL_H__
