/****************************
** Tsunagari Tile Engine   **
** python-bindings.cpp     **
** Copyright 2012 OmegaSDG **
****************************/

#include <boost/python/module.hpp>

#include "area.h" // for exportArea
#include "entity.h" // for exportEntity
#include "music.h" // for exportMusic
#include "random.h" // for exportRandom
#include "resourcer.h" // for exportResourcer
#include "sound.h" // for exportSound
#include "tile.h" // for exportTile
#include "timer.h" // for exportTimer
#include "vec.h" // for exportVecs

BOOST_PYTHON_MODULE(tsunagari)
{
	exportArea();
	exportEntity();
	exportMusic();
	exportResourcer();
	exportSound();
	exportTile();
	exportVecs();
	exportRandom();
	exportTimer();
}

// *sigh* If only it weren't static.
void pythonInitBindings()
{
	inittsunagari();
}
