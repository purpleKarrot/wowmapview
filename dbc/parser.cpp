#include "parser.hpp"
#include <iterator>
#include <iostream>
#include <fstream>

int field_size[] = { 1, 4, 4, 4, 4, 4, 32 };

//ft_char, ft_int, ft_uint, ft_float, ft_bool, ft_string, ft_loc,

const char* type_names[] = { "unsigned char", "int", "unsigned int", "float",
	"bool", "const char*", "const char*" };

int main(int argc, char* argv[])
{
	std::string input;

	if (argc > 1)
	{
		std::ifstream fin(argv[1]);
		input = std::string(std::istream_iterator<char>(fin),
			std::istream_iterator<char>());
	}
	else
	{
		input = std::string(std::istreambuf_iterator<char>(std::cin),
			std::istreambuf_iterator<char>());
	}

	typedef std::string::const_iterator iterator;
	iterator begin(input.begin()), end(input.end());

	dbc::parser<iterator> parser;
	dbc::schema schema;

	std::cout << "#include \"dbcfile.h\"\n\n";

	while (phrase_parse(begin, end, parser, schema, boost::spirit::ascii::space))
	{
		std::cout << "struct " << schema.name << "Record: RecordBase\n{\n\t"
			<< schema.name
			<< "Record(const char* offset, const char* string) :\n"
				"\t\tRecordBase(offset, string)\n\t{\n\t}\n";

		std::size_t field_count = 0;
		std::size_t record_size = 0;

		for (std::vector<dbc::field>::iterator i = schema.fields.begin(); i
			!= schema.fields.end(); ++i)
		{
			const char* type = type_names[i->type];
			std::string name = i->name;
			const char* arg = i->type == dbc::ft_loc ? "int locale = 0"
				: i->array ? "int idx" : "";

			std::cout << "\n\t" << type << ' ' << name << '(' << arg
				<< ") const\n\t{\n\t\t";

			if (i->type == dbc::ft_loc)
				std::cout << "assert(locale >= 0 && locale < 8);\n\t\t";
			else if (i->array)
				std::cout << "assert(idx >= 0 && idx < " << i->array.get()
					<< ");\n\t\t";

			std::cout << "return get<" << type << "> (" << record_size;

			if (i->type == dbc::ft_loc)
				std::cout << " + locale * 4";
			else if (i->array)
				std::cout << " + idx * " << field_size[i->type];

			std::cout << ");\n\t}\n";

			if (!i->array)
			{
				field_count += 1;
				record_size += field_size[i->type];
			}
			else
			{
				field_count += i->array.get();
				record_size += field_size[i->type] * i->array.get();
			}
		}

		std::cout << "};\n\nstruct " << schema.name << ": DBCFileTemplate<"
			<< schema.name << "Record>\n{\n"
			"\tvoid open()\n"
			"\t{\n"
			"\t\tload(\"" << schema.file << "\");\n"
			"\t\tcheck_field_count(" << field_count << ");\n"
			"\t\tcheck_record_size(" << record_size << ");\n"
			"\t}\n"
			"};\n" << std::flush;
	}

	return 0;
}
