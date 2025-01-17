/***************************************
** Tsunagari Tile Engine              **
** area.h                             **
** Copyright 2011-2015 Michael Reiley **
** Copyright 2011-2020 Paul Merrill   **
***************************************/

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// **********

#ifndef SRC_CORE_AREA_H_
#define SRC_CORE_AREA_H_

#include "core/animation.h"
#include "core/tile-grid.h"
#include "core/tile.h"
#include "core/vec.h"
#include "core/window.h"
#include "util/hashtable.h"
#include "util/string-view.h"
#include "util/string.h"
#include "util/vector.h"

#define ISOMETRIC_ZOFF_PER_TILE 0.001

class AreaJSON;
class Character;
class DataArea;
struct DisplayList;
class Entity;
class Overlay;
class Player;

//! An Area represents one map, or screen, in a World.
/*!
    The Area class manages a three-dimensional structure of Tiles and a set
    of Entities.

    The game's Viewport must be "focused" on an Area. Only one Area can be
    focused on at a time.

    The viewport will not scroll past the edge of an Area. (At least as of
    June 2012. :)
*/
class Area {
 public:
    //! Prepare game state for this Area to be in focus.
    void
    focus();

    //! Processes keyboard input, calling the Player object when necessary.
    void
    buttonDown(Key key);
    void
    buttonUp(Key key);

    //! Renders all visible Tiles and Entities within this Area.
    void
    draw(DisplayList* display);

    //! If false, drawing might be skipped. Saves CPU cycles when idle.
    bool
    needsRedraw();

    //! Inform the Area that a redraw is needed.
    void
    requestRedraw();

    /**
     * Update the game state within this Area as if dt milliseconds had
     * passed since the last call. Updates Entities, runs scripts, and
     * checks for Tile animation updates.
     */
    void
    tick(time_t dt);

    /**
     * Updates Entities, runs scripts, and checks for Tile animation
     * updates.
     */
    void
    turn();

    uint32_t
    getColorOverlay();
    void
    setColorOverlay(uint8_t a, uint8_t r, uint8_t g, uint8_t b);

    TileSet*
    getTileSet(StringView imagePath);

    //! Returns a physical cubic range of Tiles that are visible on-screen.
    //! Takes actual map size into account.
    icube
    visibleTiles();

    //! Returns true if a Tile exists at the specified coordinate.
    bool
    inBounds(Entity* ent);

    // Create an NPC and insert it into the Area. Returns a borrowed ref.
    Character*
    spawnNPC(StringView descriptor, vicoord coord, StringView phase);
    // Create an Overlay and insert it into the Area. Returns a borrowed ref.
    Overlay*
    spawnOverlay(StringView descriptor, vicoord coord, StringView phase);

    DataArea*
    getDataArea();

    void
    runScript(TileGrid::ScriptType type,
              icoord tile,
              Entity* triggeredBy) noexcept;

 public:
    TileGrid grid;

    bool ok = true;

 protected:
    //! Calculate frame to show for each type of tile
    void
    drawTiles(DisplayList* display, icube& tiles, int z);
    void
    drawEntities(DisplayList* display, icube& tiles, int z);

 protected:
    Hashmap<String, TileSet> tileSets;

    Vector<Animation> tileGraphics;
    Vector<bool> checkedForAnimation;
    Vector<bool> tilesAnimated;

    Vector<Character*> characters;
    Vector<Overlay*> overlays;

    bool beenFocused = false;
    bool redraw = true;
    uint32_t colorOverlayARGB = 0x00000000;

    DataArea* dataArea;

    Player* player;

    // The following contain filenames such that they may be loaded lazily.
    String descriptor;

    String name;
    String author;
    String musicPath;

    friend AreaJSON;
};

#endif  // SRC_CORE_AREA_H_
