#include <boost/foreach.hpp>

#include "area.h"
#include "entity-lua.h"
#include "script.h"
#include "tile.h"
#include "window.h"


void Tile::onEnterScripts(Resourcer* rc, Entity* triggeredBy)
{
	if (flags * hasOnEnter)
		runScripts(rc, triggeredBy, onEnter, events);
	if (type->flags * hasOnEnter)
		runScripts(rc, triggeredBy, onEnter, type->events);
}

void Tile::onLeaveScripts(Resourcer* rc, Entity* triggeredBy)
{
	if (type->flags * hasOnLeave)
		runScripts(rc, triggeredBy, onLeave, type->events);
	if (flags * hasOnLeave)
		runScripts(rc, triggeredBy, onLeave, events);
}

void Tile::runScripts(Resourcer* rc, Entity* entity,
                      const TileEventTrigger trigger,
                      const std::vector<TileEvent>& events)
{
	for (std::vector<TileEvent>::const_iterator i = events.begin(); i != events.end(); i++) {
//	BOOST_FOREACH(const TileEvent& e, events) {
		const TileEvent& e = *i;
		if (e.trigger == trigger) {
			Script s(rc);
			bindEntity(&s, entity, "entity");
			// TODO bindTile(script, tile, "tile");
			s.run(rc, e.script);
		}
	}
}


TileType::TileType()
	: flags(0x0)
{
}

TileType::TileType(TileSet& set)
	: flags(0x0)
{
	anim.addFrame(set.tiles.front());
	set.tiles.pop_front();
}

bool TileType::needsRedraw(const Area& area) const
{
	const int millis = GameWindow::getWindow().time();
	return anim.needsRedraw(millis) &&
	       visibleIn(area, area.visibleTiles());
}

bool TileType::visibleIn(const Area& area, const icube_t& tiles) const
{
	for (int z = tiles.z1; z != tiles.z2; z++) {
		for (int y = tiles.y1; y != tiles.y2; y++) {
			for (int x = tiles.x1; x != tiles.x2; x++) {
				const Tile& tile = area.getTile(icoord(x, y, z));
				if (tile.type == this)
					return true;
			}
		}
	}
	return false;
}

