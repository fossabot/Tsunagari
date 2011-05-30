/******************************
** Tsunagari Tile Engine     **
** area.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <boost/foreach.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "area.h"
#include "entity.h"
#include "resourcer.h"
#include "sprite.h"
#include "tile.h"

Area::Area(Resourcer* rc, Entity* player, const std::string descriptor)
	: rc(rc), player(player), descriptor(descriptor)
{
}

Area::~Area()
{
}

bool Area::init()
{
	if (!processDescriptor()) // Try to load in descriptor.
		return false;
	
	Sprite* s = new Sprite(rc, "grass.sprite");
	Sprite* s2 = new Sprite(rc, "grass.sprite");
	if (!s->init() || !s2->init())
		return false;
	Tile* t = new Tile(s, true, coord(0, 0, 0));
	Tile* t2 = new Tile(s2, true, coord(1, 0, 0));


	matrix.resize(1);
	matrix[0].resize(1);
	matrix[0][0].resize(2);
	matrix[0][0][0] = t;
	matrix[0][0][1] = t2;
	
	return true;
}

void Area::buttonDown(const Gosu::Button btn)
{
	if (btn == Gosu::kbRight)
		player->moveByTile(coord(1, 0, 0));
	else if (btn == Gosu::kbLeft)
		player->moveByTile(coord(-1, 0, 0));
	else if (btn == Gosu::kbUp)
		player->moveByTile(coord(0, -1, 0));
	else if (btn == Gosu::kbDown)
		player->moveByTile(coord(0, 1, 0));
}

void Area::draw()
{
	BOOST_FOREACH(grid_t g, matrix)
		BOOST_FOREACH(row_t r, g)
			BOOST_FOREACH(Tile* t, r)
				t->draw();
	player->draw();
}

bool Area::needsRedraw() const
{
	return player->needsRedraw();
}

bool Area::processDescriptor()
{
	const xmlNode* root = rc->getXMLDoc(descriptor);
	if (!root)
		return false;

	xmlNode* node = root->xmlChildrenNode; // <area>
	node = node->xmlChildrenNode; // decend into children of <area>
	while (node != NULL) {
		if (!xmlStrncmp(node->name, BAD_CAST("name"), 5)) {
			const xmlChar* str = xmlNodeGetContent(node);
			xml.name = (const char*)str;
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("author"), 7)) {
			const xmlChar* str = xmlNodeGetContent(node);
			xml.author = (const char*)str;
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("tileset"), 8)) {
			const xmlChar* str = xmlNodeGetContent(node);
			xml.tileset = (const char*)str;
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("tileset"), 8)) {
			const xmlChar* str = xmlNodeGetContent(node);
			xml.tileset = (const char*)str;
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("music"), 6)) {
			// TODO
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("event"), 6)) {
			// TODO Area-wide event
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("map"), 4)) {
			// TODO create TileMatrix somehow
			// perhaps pass xmlNode to TileMatrix::init() method??
		}
		node = node->next;
	}
	return true;
}

coord_t Area::getDimensions()
{
	return dim;
}

Tile* Area::getTile(coord_t c)
{
	return matrix[c.z][c.y][c.x];
}

