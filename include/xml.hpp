#ifndef HEADER_XML_HPP
#define HEADER_XML_HPP

#include <pugixml.hpp>
#include <string>

class XML {
	public:
		XML(std::string_view const& xml);

		std::string to_string();

		// from META-INF/container.xml
		std::string get_rootfile();

		// for rootfile
		std::string fix_metadata();
	private:
		pugi::xml_document doc;
};

#endif /* HEADER_XML_HPP */

