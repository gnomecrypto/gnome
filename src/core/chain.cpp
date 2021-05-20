#include "chain.h"

gnome::chain::chain()
{
	data = std::vector<block>({block::genesis()});
}

gnome::block gnome::chain::append(const payload_t &payload)
{
	const auto next = block::create(data.back(), payload);
	data.push_back(next);
	return next;
}

const gnome::block &gnome::chain::front() const
{
	return data.front();
}

const gnome::block &gnome::chain::back() const
{
	return data.back();
}

const gnome::block &gnome::chain::get(const size_t index) const
{
	return data[(data.size() + index) % data.size()];
}

size_t gnome::chain::size() const
{
	return data.size();
}

bool gnome::chain::replace(const chain &other)
{
	if (other.size() <= size())
	{
		return false;
	}
	else if (!is_valid(other))
	{
		return false;
	}
	data = std::vector<block>(other.data);
	return true;
}

json::value gnome::chain::to_json() const
{
	json::array o;
	for(const auto &b : data)
	{
		o.push_back(b.to_json());
	}
	return o;
}

bool gnome::chain::is_valid(const chain &chain)
{
	if (chain.front() != block::genesis())
	{
		return false;
	}
	for (size_t i = 1; i < chain.size(); ++i)
	{
		auto &curr = chain.get(i);
		auto &prev = chain.get(i - 1);
		if (!prev.is_previous(curr) || !curr.is_valid())
		{
			return false;
		}
	}
	return true;
}
