#ifndef SCHEMA_HPP
#define SCHEMA_HPP

#include <string>
#include <vector>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>

namespace dbc
{

enum field_type
{
	ft_byte, ft_int, ft_uint, ft_float, ft_bool, ft_string, ft_loc,
};

struct field
{
	field_type type;
	std::string name;
	boost::optional<int> array;
};

struct schema
{
	std::string name;
	std::string file;
	std::vector<field> fields;
};

} // namespace dbc


BOOST_FUSION_ADAPT_STRUCT(
	dbc::field,
	(dbc::field_type, type)
	(std::string, name)
	(boost::optional<int>, array)
)

BOOST_FUSION_ADAPT_STRUCT(
	dbc::schema,
	(std::string, name)
	(std::string, file)
	(std::vector<dbc::field>, fields)
)

namespace dbc
{
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template<typename Iterator>
struct parser: qi::grammar<Iterator, dbc::schema(), ascii::space_type>
{
	parser() :
		parser::base_type(schema)
	{
		using namespace boost::spirit;

		type.add //
		("byte", ft_byte) //
		("int", ft_int) //
		("uint", ft_uint) //
		("float", ft_float) //
		("bool", ft_bool) //
		("string", ft_string) //
		("loc", ft_loc);

		field %= type >> +(char_ - '[' - ';') >> -('[' >> int_ >> ']') >> ';';

		filename %= '(' >> lexeme['"' >> +(char_ - '"') >> '"'] >> ')';

		schema %= lit("schema") >> +(char_ - '(') >> filename //
			>> '{' >> +field >> '}';
	}

	qi::symbols<char, dbc::field_type> type;
	qi::rule<Iterator, std::string(), ascii::space_type> filename;
	qi::rule<Iterator, dbc::schema(), ascii::space_type> schema;
	qi::rule<Iterator, dbc::field(), ascii::space_type> field;
};

} // namespace dbc

#endif /* SCHEMA_HPP */
