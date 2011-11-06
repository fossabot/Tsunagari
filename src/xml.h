/******************************
** Tsunagari Tile Engine     **
** xml.h                     **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef XML_H
#define XML_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <libxml/tree.h>

class XMLDoc;

class XMLNode {
public:
	XMLNode();
	XMLNode(XMLDoc* doc, xmlNode* node);

	XMLNode childrenNode() const;
	XMLNode next() const;

	bool is(const char* name) const;

	std::string content() const;
	bool intContent(int* i) const;
	bool doubleContent(double* d) const;

	std::string attr(const std::string& name) const;
	bool intAttr(const std::string& name, int* i) const;
	bool doubleAttr(const std::string& name, double* d) const;

	//! Whether this is a valid node (non-NULL).
	operator bool() const;

private:
	XMLDoc* doc;
	xmlNode* node;
};

class XMLDoc {
public:
	XMLDoc();
	bool init(const std::string& path,
	          const std::string& data,
	          const std::string& dtdFile);

	XMLNode root();
	xmlNode* temporaryGetRoot() const;
	const std::string& path() const;

	//! Equivalent to doc::unique();
	bool unique() const;

	//! Equivalent to doc::use_count().
	long use_count() const;

	//! Signifies whether document is has been initialized, parsed
	//! correctly, and is valid.
	operator bool() const;

private:
	boost::shared_ptr<xmlDoc> doc;
	std::string path_;
};

//! Get XML attribute (memory-safe).
std::string readXmlAttribute(xmlNode* node, const std::string& attr);

//! Get XML element content (memory-safe).
std::string readXmlElement(xmlNode* node);

#endif
