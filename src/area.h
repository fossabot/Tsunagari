/******************************
** Tsunagari Tile Engine     **
** area.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef AREA_H
#define AREA_H

#include <string>

#include <Gosu/Gosu.hpp>

#include "common.h"
#include "entity.h"
#include "resourcer.h"
//#include "tilegrid.h"

class Entity;
class GameWindow;
class Resourcer;

class Area
{
public:
	Area(Resourcer* rc, Entity* player, const std::string filename);
	~Area();

	void buttonDown(const Gosu::Button btn);
	void draw();
	bool needsRedraw() const;
	coord_t translateCoords();

private:
//	void build(Resourcer* rc, yaml_t* yaml);
	Entity* player;
//	TileGrid* grid;
};

#endif
