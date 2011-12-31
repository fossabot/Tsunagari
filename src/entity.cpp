/******************************
** Tsunagari Tile Engine     **
** entity.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <math.h>

#include <boost/foreach.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/Math.hpp>
#include <Gosu/Timing.hpp>

#include "area.h"
#include "config.h"
#include "entity.h"
#include "log.h"
#include "python.h"
#include "resourcer.h"
#include "window.h"
#include "xml.h"

#define ASSERT(x)  if (!(x)) return false

static std::string facings[][3] = {
	{"up-left",   "up",   "up-right"},
	{"left",      "",     "right"},
	{"down-left", "down", "down-right"},
};


Entity::Entity(Resourcer* rc, Area* area, ClientValues* conf)
	: rc(rc),
	  redraw(true),
	  speedMul(1.0),
	  moving(false),
	  area(NULL),
	  r(0.0, 0.0, 0.0),
	  conf(conf)
{
	if (area)
		setArea(area);
}

Entity::~Entity()
{
}

bool Entity::init(const std::string& descriptor)
{
	this->descriptor = descriptor;
	if (!processDescriptor())
		return false;

	// Set an initial phase.
	phase = &phases.begin()->second;
	return true;
}

void Entity::draw()
{
	int millis = GameWindow::getWindow().time();
	phase->updateFrame(millis);
	phase->frame()->draw(doff.x + r.x, doff.y + r.y, r.z);
	redraw = false;
}

bool Entity::needsRedraw() const
{
	int millis = GameWindow::getWindow().time();
	return redraw || phase->needsRedraw(millis);
}


static double angleFromXY(double x, double y)
{
	double angle = 0.0;

	// Moving at an angle
	if (x != 0 && y != 0) {
		angle = atan(y / x);
		if (y < 0 && x < 0)
			;
		else if (y < 0 && x > 0)
			angle += M_PI;
		else if (y > 0 && x < 0)
			angle += M_PI*2;
		else if (y > 0 && x > 0)
			angle += M_PI;
	}

	// Moving straight
	else {
		if (x < 0)
			angle = 0;
		else if (x > 0)
			angle = M_PI;
		else if (y < 0)
			angle = M_PI_2;
		else if (y > 0)
			angle = 3*M_PI_2;
	}

	return angle;
}

void Entity::update(unsigned long dt)
{
	switch (conf->moveMode) {
	case TURN:
		updateTurn(dt);
		break;
	case TILE:
		updateTile(dt);
		break;
	case NOTILE:
		updateNoTile(dt);
		break;
	}
}

void Entity::updateTurn(unsigned long)
{
	// Entities don't do anything in TILE mode.
}

void Entity::updateTile(unsigned long dt)
{
	if (!moving)
		return;

	redraw = true;
	double traveled = speed * (double)dt;
	double destDist = Gosu::distance(r.x, r.y, destCoord.x, destCoord.y);
	if (destDist <= traveled) {
		r = destCoord;
		moving = false;
		postMove();
		if (moving) {
			// Time rollover.
			double perc = 1.0 - destDist/traveled;
			unsigned long remt = (unsigned long)(perc * (double)dt);
			update(remt);
		}
	}
	else {
		double angle = angleFromXY(r.x - destCoord.x,
		                           destCoord.y - r.y);
		double dx = cos(angle);
		double dy = -sin(angle);

		// Fix inaccurate trig functions. (Why do we have to do this!??)
		if (-1e-10 < dx && dx < 1e-10)
			dx = 0.0;
		if (-1e-10 < dy && dy < 1e-10)
			dy = 0.0;

		// Save state of partial pixel travel in double.
		r.x += dx * traveled;
		r.y += dy * traveled;
	}
}

void Entity::updateNoTile(unsigned long)
{
	// TODO
}

std::string Entity::getFacing() const
{
	return facing;
}

bool Entity::setPhase(const std::string& name)
{
	boost::unordered_map<std::string, Animation>::iterator it;
	it = phases.find(name);
	if (it != phases.end()) {
		Animation* newPhase = &it->second;
		if (phase != newPhase) {
			phase = newPhase;
			redraw = true;
			return true;
		}
	}
	return false;
}

rcoord Entity::getPixelCoord() const
{
	return r;
}

icoord Entity::getTileCoords() const
{
	ivec2 tileDim = area->getTileDimensions();
	return icoord(
		(int)r.x / tileDim.x,
		(int)r.y / tileDim.y,
		area->depthIndex(r.z)
	);
}

void Entity::setTileCoords(icoord coords)
{
	// FIXME: security: bounds check
	redraw = true;
	const ivec2 tileDim = area->getTileDimensions();
	r = rcoord(
		coords.x * tileDim.x,
		coords.y * tileDim.y,
		coords.z
	);
}

void Entity::moveByTile(icoord delta)
{
	if (moving)
		return;
	calculateFacing(delta.x, delta.y);
	setPhase(facing);

	std::vector<icoord> tiles = frontTiles();
	BOOST_FOREACH(const icoord& tile, tiles) {
		if (canMove(tile)) {
			preMove();
			return;
		}
	}
}

void Entity::setArea(Area* a)
{
	area = a;
	calcDoff();
	setSpeed(speedMul); // Calculate new speed based on tile size.
}

void Entity::gotoRandomTile()
{
	const icoord map = area->getDimensions();
	icoord pos;
	Tile* tile;
	do {
		pos = icoord(rand() % map.x, rand() % map.y, 0);
		tile = &area->getTile(pos);
	} while (tile->hasFlag(nowalk));
	setTileCoords(pos);
}

void Entity::setSpeed(double multiplier)
{
	speedMul = multiplier;
	if (area) {
		double tilesPerSecond = area->getTileDimensions().x / 1000.0;
		speed = baseSpeed * speedMul * tilesPerSecond;
	}
}

std::vector<icoord> Entity::frontTiles() const
{
	std::vector<icoord> tiles;
	icoord normal = getTileCoords();
	normal += icoord(faceX, faceY, 0);
	tiles.push_back(normal);

	// If we are on a tile with the layermod property, we have access to
	// tiles on the new layer, too.
	const boost::optional<int> layermod = getTile().layermod;
	if (layermod) {
		icoord mod = normal;
		mod.z = area->depthIndex(*layermod);
		tiles.push_back(mod);
	}
	return tiles;
}

void Entity::calcDoff()
{
	// X-axis is centered on tile.
	doff.x = (area->getTileDimensions().x - imgw) / 2;
	// Y-axis is aligned with bottom of tile.
	doff.y = area->getTileDimensions().y - imgh - 1;
}

Tile& Entity::getTile() const
{
	return area->getTile(getTileCoords());
}

SampleRef Entity::getSound(const std::string& name)
{
	boost::unordered_map<std::string, SampleRef>::iterator it;

	it = sounds.find(name);
	if (it != sounds.end())
		return it->second;
	else
		return SampleRef();
}

void Entity::calculateFacing(int x, int y)
{
	if (x < 0)
		faceX = -1;
	else if (x == 0)
		faceX = 0;
	else
		faceX = 1;

	if (y < 0)
		faceY = -1;
	else if (y == 0)
		faceY = 0;
	else
		faceY = 1;

	facing = facings[faceY+1][faceX+1];
}

bool Entity::canMove(icoord dest)
{
	if (!area->tileExists(dest))
		// The tile is off the map.
		return false;
	ivec2 tileDim = area->getTileDimensions();
	destCoord = rcoord(
		dest.x * tileDim.x,
		dest.y * tileDim.y,
		area->indexDepth(dest.z)
	);
	destTile = &area->getTile(dest);
	return !destTile->hasFlag(nowalk);
}

void Entity::preMove()
{
	fromCoord = r;
	fromTile = &getTile();
	moving = true;

	// Set z right away so that we're on-level with the square we're entering.
	r.z = destCoord.z;

	// Start moving animation.
	switch (conf->moveMode) {
	case TURN:
		break;
	case TILE:
	case NOTILE:
		setPhase("moving " + facing);
		break;
	}

	// Process triggers.
	tileExitScript();
	fromTile->onLeaveScripts(rc, this);

	if (conf->moveMode == TURN) {
		// Movement is instantaneous.
		r = destCoord;
		postMove();
	}
}

void Entity::postMove()
{
	moving = false;

	// Stop moving animation.
	if (conf->moveMode != TURN)
		setPhase(facing);

	// Process triggers.
	destTile->onEnterScripts(rc, this);
	tileEntryScript();

	// TODO: move teleportation here
	/*
	 * if (onDoor()) {
	 * 	leaveTile();
	 * 	moveArea(getDoor());
	 * 	postMoveScript();
	 * 	enterTile();
	 * }
	 */
}

void Entity::tileExitScript()
{
	const std::string& name = scripts["tileexit"];
	if (name.size()) {
		pythonSetGlobal("entity", this);
		rc->runPythonScript(name);
	}
}

void Entity::tileEntryScript()
{
	const std::string& name = scripts["tileentry"];
	if (name.size()) {
		pythonSetGlobal("entity", this);
		rc->runPythonScript(name);
	}
}


/*
 * DESCRIPTOR CODE BELOW
 */

bool Entity::processDescriptor()
{
	XMLRef doc = rc->getXMLDoc(descriptor, "entity.dtd");
	if (!doc)
		return false;
	const XMLNode root = doc->root(); // <entity>
	if (!root)
		return false;

	for (XMLNode node = root.childrenNode(); node; node = node.next()) {
		if (node.is("speed")) {
			ASSERT(node.doubleContent(&baseSpeed));
			setSpeed(speedMul); // Calculate speed from tile size.
		} else if (node.is("sprite")) {
			ASSERT(processSprite(node.childrenNode()));
		} else if (node.is("sounds")) {
			ASSERT(processSounds(node.childrenNode()));
		} else if (node.is("scripts")) {
			ASSERT(processScripts(node.childrenNode()));
		}
	}
	return true;
}

bool Entity::processSprite(XMLNode node)
{
	TiledImage tiles;
	for (; node; node = node.next()) {
		if (node.is("sheet")) {
			std::string imageSheet = node.content();
			ASSERT(node.intAttr("tilewidth",  &imgw) &&
			       node.intAttr("tileheight", &imgh));
			ASSERT(rc->getTiledImage(tiles, imageSheet,
			       imgw, imgh, false));
		} else if (node.is("phases")) {
			ASSERT(processPhases(node.childrenNode(), tiles));
		}
	}
	return true;
}

bool Entity::processPhases(XMLNode node, const TiledImage& tiles)
{
	for (; node; node = node.next())
		if (node.is("phase"))
			ASSERT(processPhase(node, tiles));
	return true;
}

bool Entity::processPhase(const XMLNode node, const TiledImage& tiles)
{
	/* Each phase requires a 'name'. Additionally,
	 * one of either 'pos' or 'speed' is needed.
	 * If speed is used, we have sub-elements. We
	 * can't have both pos and speed.
	 */
	const std::string name = node.attr("name");
	if (name.empty()) {
		Log::err(descriptor, "<phase> name attribute is empty");
		return false;
	}

	const std::string posStr = node.attr("pos");
	const std::string speedStr = node.attr("speed");

	if (posStr.size() && speedStr.size()) {
		Log::err(descriptor, "pos and speed attributes in "
				"phase element are mutually exclusive");
		return false;
	} else if (posStr.empty() && speedStr.empty()) {
		Log::err(descriptor, "must have pos or speed attribute "
			       "in phase element");
		return false;
	}

	if (posStr.size()) {
		int pos;
		ASSERT(node.intAttr("pos", &pos));
		if (pos < 0 || (int)tiles.size() < pos) {
			Log::err(descriptor,
				"<phase></phase> index out of bounds");
			return false;
		}
		phases[name].addFrame(tiles[pos]);
	}
	else {
		int speed;
		ASSERT(node.intAttr("speed", &speed));

		int len = (int)(1000.0/speed);
		phases[name].setFrameLen(len);
		ASSERT(processMembers(node.childrenNode(),
		                      phases[name], tiles));
	}

	return true;
}

bool Entity::processMembers(XMLNode node, Animation& anim,
                            const TiledImage& tiles)
{
	for (; node; node = node.next())
		if (node.is("member"))
			ASSERT(processMember(node, anim, tiles));
	return true;
}

bool Entity::processMember(const XMLNode node, Animation& anim,
                           const TiledImage& tiles)
{
	int pos;
	ASSERT(node.intAttr("pos", &pos));
	if (pos < 0 || (int)tiles.size() < pos) {
		Log::err(descriptor, "<member></member> index out of bounds");
		return false;
	}
	anim.addFrame(tiles[pos]);
	return true;
}

bool Entity::processSounds(XMLNode node)
{
	for (; node; node = node.next())
		if (node.is("sound"))
			ASSERT(processSound(node));
	return true;
}

bool Entity::processSound(const XMLNode node)
{
	const std::string name = node.attr("name");
	const std::string filename = node.content();
	if (name.empty()) {
		Log::err(descriptor, "<sound> name attribute is empty");
		return false;
	} else if (filename.empty()) {
		Log::err(descriptor, "<sound></sound> is empty");
		return false;
	}

	SampleRef s = rc->getSample(filename);
	if (s)
		sounds[name] = s;
	return true;
}

bool Entity::processScripts(XMLNode node)
{
	for (; node; node = node.next())
		if (node.is("script"))
			ASSERT(processScript(node));
	return true;
}

bool Entity::processScript(const XMLNode node)
{
	const std::string trigger = node.attr("trigger");
	const std::string filename = node.content();
	if (trigger.empty()) {
		Log::err(descriptor, "<script> trigger attribute is empty");
		return false;
	} else if (filename.empty()) {
		Log::err(descriptor, "<script></script> is empty");
		return false;
	}

	if (rc->resourceExists(filename)) {
		scripts[trigger] = filename;
		return true;
	}
	else {
		Log::err(descriptor,
		         std::string("script not found: ") + filename);
		return false;
	}
}


void exportEntity()
{
	boost::python::class_<Entity>("Entity", boost::python::no_init)
		.add_property("animation", &Entity::getFacing, &Entity::setPhase)
		.def("getTileCoords", &Entity::getTileCoords)
		.def("gotoRandomTile", &Entity::gotoRandomTile)
		.def("setSpeed", &Entity::setSpeed);
}

