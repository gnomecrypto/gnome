#ifndef __GNOME_APP_UTILS_H__
#define __GNOME_APP_UTILS_H__

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>

inline boost::property_tree::ptree read_json(const std::string &data)
{
	auto copy(data);
	boost::algorithm::trim(copy);
	std::stringstream ss;
	if (copy.empty())
	{
		copy = "{}";
	}
	ss << copy;
	boost::property_tree::ptree tree;
	read_json(ss, tree);
	return tree;
}

#endif