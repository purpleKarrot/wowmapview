#ifndef WOW_RESOURCE_HPP
#define WOW_RESOURCE_HPP

#include <string>
#include <boost/flyweight.hpp>
#include <boost/flyweight/key_value.hpp>

namespace wow
{

template<typename Instance>
struct resource
{
	typedef boost::flyweights::flyweight< //
		boost::flyweights::key_value<std::string, Instance> > type;
};

} // namespace wow

#endif /* WOW_RESOURCE_HPP */
