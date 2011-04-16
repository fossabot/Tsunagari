/******************************
** Tsunagari Tile Engine     **
** tile.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#include "entity.h"
#include "event.h"
#include "resourcer.h"

class Tile
{
public:
	Tile(Resourcer* rc, Gosu::Image img, int size, bool walkable, coord_t coords, map<string,string>* events);
	bool canWalk();
	void draw();
	void activate(Entity* entity);
	void enter(Entity* entity);
	void leave(Entity* entity);
	
private:
	Event* loadEvent(string name);
};
