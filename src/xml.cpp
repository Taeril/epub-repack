#include "xml.hpp"

#include <fmt/core.h>

class StrWriter : public pugi::xml_writer {
	public:
		void write(const void* data, size_t size) override {
			output.append(std::string(static_cast<const char*>(data), size));
		}

		std::string str(pugi::xml_document const& doc) {
			doc.print(*this);
			return output;
		}
	private:
		std::string output;
};

inline std::string doc_to_string(pugi::xml_document const& doc) {
	return StrWriter().str(doc);
}


XML::XML(std::string_view const& xml) {
	pugi::xml_parse_result result = doc.load_buffer(xml.data(), xml.size());
	if(!result) {
		fmt::print(stderr, "ERROR: {}", result.description());
		throw 1;
	}
}

std::string XML::to_string() {
	return doc_to_string(doc);
}

std::string XML::get_rootfile() {
	auto rootfile = doc.child("container").child("rootfiles").child("rootfile");
	if(!rootfile) throw 1;
	return rootfile.attribute("full-path").value();
}

std::string XML::fix_metadata() {
	auto metadata = doc.child("package").child("metadata");
	auto series = metadata.find_child_by_attribute("meta", "name", "calibre:series");
	auto index = metadata.find_child_by_attribute("meta", "name", "calibre:series_index");

	// Make sure that attributes starts with "name" and "content" and with that order.
	auto fix_name_content = [](pugi::xml_node& node) {
		std::string name = node.attribute("name").value();
		std::string content = node.attribute("content").value();
		node.remove_attribute("name");
		node.remove_attribute("content");
		node.prepend_attribute("content").set_value(content.c_str());
		node.prepend_attribute("name").set_value(name.c_str());
	};

	// normalize all <meta> tags
	for(auto& meta : metadata.children("meta")) {
		fix_name_content(meta);
	}

	// Make <meta> tags with names calibre:series and calibre:series_index lats two in <metadata>.
	// Also make it in that order.
	if(series) {
		//fix_name_content(series);
		metadata.append_move(series);
	}
	if(index) {
		//fix_name_content(index);
		metadata.append_move(index);
	}

	return doc_to_string(doc);
}

