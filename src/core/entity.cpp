/***************************************
** Tsunagari Tile Engine              **
** entity.cpp                         **
** Copyright 2011-2013 Michael Reiley **
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

#include "core/entity.h"

#include "core/area.h"
#include "core/client-conf.h"
#include "core/display-list.h"
#include "core/images.h"
#include "core/jsons.h"
#include "core/log.h"
#include "core/resources.h"
#include "core/world.h"
#include "os/c.h"
#include "util/assert.h"
#include "util/math2.h"
#include "util/move.h"
#include "util/string2.h"

#define CHECK(x)      \
    if (!(x)) {       \
        return false; \
    }

static StringView directions[][3] = {
        {"up-left", "up", "up-right"},
        {"left", "stance", "right"},
        {"down-left", "down", "down-right"},
};


/*
 * JSON DESCRIPTOR CODE BELOW
 */

static bool
parseDescriptor(Entity* e) noexcept;
static bool
parseSprite(Entity* e, JsonValue sprite) noexcept;
static bool
parsePhases(Entity* e, JsonValue phases, TiledImage tiles) noexcept;
static bool
parsePhase(Entity* e,
           StringView name,
           JsonValue phase,
           TiledImage tiles) noexcept;
static bool
parseSounds(Entity* e, JsonValue sounds) noexcept;
static bool
parseSound(Entity* e, StringView name, StringView path) noexcept;
static bool
parseScripts(Entity* e, JsonValue scripts) noexcept;
static bool
parseScript(Entity* e, StringView name, StringView path) noexcept;
// static static bool
// setScript(Entity* e, StringView trigger, ScriptRef& script) noexcept;

static bool
parseDescriptor(Entity* e) noexcept {
    Optional<JsonDocument> document = loadJson(e->descriptor);
    if (!document) {
        return false;
    }

    JsonValue root = document->root;
    CHECK(root.isObject());

    JsonValue speedValue = root["speed"];
    JsonValue spriteValue = root["sprite"];
    JsonValue soundsValue = root["sounds"];
    JsonValue scriptsValue = root["scripts"];

    CHECK(speedValue.isNumber() || speedValue.isNull());
    CHECK(spriteValue.isObject() || spriteValue.isNull());
    CHECK(soundsValue.isObject() || soundsValue.isNull());
    CHECK(scriptsValue.isObject() || scriptsValue.isNull());

    if (speedValue.isNumber()) {
        e->tilesPerSecond = static_cast<float>(speedValue.toNumber());

        if (e->area) {
            assert_(e->area->grid.tileDim.x == e->area->grid.tileDim.y);
            e->pixelsPerSecond = e->tilesPerSecond * e->area->grid.tileDim.x;
        }
    }
    if (spriteValue.isObject()) {
        CHECK(parseSprite(e, spriteValue));
    }
    if (soundsValue.isObject()) {
        CHECK(parseSounds(e, soundsValue));
    }
    if (scriptsValue.isObject()) {
        CHECK(parseScripts(e, scriptsValue));
    }
    return true;
}

static bool
parseSprite(Entity* e, JsonValue sprite) noexcept {
    JsonValue sheetValue = sprite["sheet"];
    JsonValue phasesValue = sprite["phases"];

    CHECK(sheetValue.isObject());
    CHECK(phasesValue.isObject());

    JsonValue sheet = sheetValue;

    JsonValue tilewidthValue = sheetValue["tile_width"];
    JsonValue tileheightValue = sheetValue["tile_height"];
    JsonValue pathValue = sheetValue["path"];

    CHECK(tilewidthValue.isNumber());
    CHECK(tileheightValue.isNumber());
    CHECK(pathValue.isString());

    e->imgsz.x = tilewidthValue.toInt();
    e->imgsz.y = tileheightValue.toInt();
    StringView path = pathValue.toString();

    TiledImage tiles = tilesLoad(path, e->imgsz.x, e->imgsz.y);
    CHECK(TILES_VALID(tiles));

    return parsePhases(e, phasesValue, tiles);
}

static bool
parsePhases(Entity* e, JsonValue phases, TiledImage tiles) noexcept {
    for (JsonNode& node : phases) {
        CHECK(node.value.isObject());
        CHECK(parsePhase(e, node.key, node.value, tiles));
    }
    return true;
}

static Vector<int>
intArrayToVector(JsonValue array) noexcept {
    Vector<int> v;
    for (JsonNode& node : array) {
        if (node.value.isNumber()) {
            v.push_back(node.value.toInt());
        }
    }
    return v;
}

static bool
parsePhase(Entity* e,
           StringView name,
           JsonValue phase,
           TiledImage tiles) noexcept {
    // Each phase requires a 'name' and a 'frame' or 'frames'. Additionally,
    // 'speed' is required if 'frames' is found.

    JsonValue frameValue = phase["frame"];
    JsonValue framesValue = phase["frames"];
    JsonValue speedValue = phase["speed"];

    CHECK(frameValue.isNumber() || frameValue.isNull());
    CHECK(framesValue.isArray() || framesValue.isNull());
    CHECK(speedValue.isNumber() || speedValue.isNull());

    CHECK(frameValue.isNumber() || framesValue.isArray());
    CHECK(!frameValue.isNumber() || !framesValue.isArray());
    CHECK(!framesValue.isArray() || speedValue.isNumber());

    int nTiles = tiles.numTiles;

    Animation animation;

    if (frameValue.isNumber()) {
        int frame = frameValue.toInt();
        if (frame >= nTiles) {
            logErr(e->descriptor,
                   "<phase> frame attribute index out of bounds");
            return false;
        }
        Image image = tileAt(tiles, frame);
        animation = Animation(image);
    }
    else if (framesValue.isArray()) {
        if (!speedValue.isNumber()) {
            // Cannot get to this point because of CHECKs above.
            // logErr(descriptor,
            //        "<phase> speed attribute must be present and "
            //        "must be decimal");
            // return false;
        }
        float fps = static_cast<float>(speedValue.toNumber());
        assert_(fps != 0.0f);

        Vector<Image> images;
        for (JsonNode& node : framesValue) {
            JsonValue frameValue = node.value;

            CHECK(frameValue.isNumber());
            int frame = frameValue.toInt();

            if (frame < 0 || nTiles < frame) {
                logErr(e->descriptor,
                       "<phase> frames attribute index out of bounds");
                return false;
            }
            images.push_back(tileAt(tiles, frame));
        }
        assert_(images.size() > 0);

        time_t frameTime = static_cast<time_t>(1000.0 / fps);
        animation = Animation(move_(images), frameTime);
    }
    else {
        // Cannot get to this point because of CHECKs above.
        // logErr(descriptor,
        //        "<phase> frames attribute not an int or int ranges");
        // return false;
        assert_(false);
    }

    if (name == "stance") {
        e->phaseStance = animation;
    }
    else if (name == "down") {
        e->phaseDown = animation;
    }
    else if (name == "left") {
        e->phaseLeft = animation;
    }
    else if (name == "up") {
        e->phaseUp = animation;
    }
    else if (name == "right") {
        e->phaseRight = animation;
    }
    else if (name == "moving up") {
        e->phaseMovingUp = animation;
    }
    else if (name == "moving right") {
        e->phaseMovingRight = animation;
    }
    else if (name == "moving down") {
        e->phaseMovingDown = animation;
    }
    else if (name == "moving left") {
        e->phaseMovingLeft = animation;
    }
    else {
        logErr(e->descriptor, "unknown phase");
    }

    return true;
}

static bool
parseSounds(Entity* e, JsonValue sounds) noexcept {
    for (JsonNode& node : sounds) {
        CHECK(node.value.isString());
        CHECK(parseSound(e, node.key, node.value.toString()));
    }
    return true;
}

static bool
parseSound(Entity* e, StringView name, StringView path) noexcept {
    if (!path.size) {
        logErr(e->descriptor, "sound path is empty");
        return false;
    }

    if (name == "step") {
        e->soundPathStep = path;
    }
    else {
        logErr(e->descriptor, String() << "unknown entity sound " << name);
        return false;
    }
    return true;
}

static bool
parseScripts(Entity* e, JsonValue scripts) noexcept {
    for (JsonNode& node : scripts) {
        CHECK(node.value.isString());
        CHECK(parseScript(e, node.key, node.value.toString()));
    }
    return true;
}

static bool
parseScript(Entity* e, StringView /*name*/, StringView path) noexcept {
    if (!path.size) {
        logErr(e->descriptor, "script path is empty");
        return false;
    }

    // ScriptRef script = Script::create(filename);
    // if (!script || !script->validate()) {
    //     return false;
    // }

    // if (!setScript(trigger, script)) {
    //     logErr(e->descriptor,
    //            "unrecognized script trigger: " + trigger);
    //     return false;
    // }

    return true;
}

/*
static bool
setScript(Entity* e, StringView trigger, ScriptRef& script) noexcept {
    if (trigger == "on_tick") {
        tickScript = script;
        return true;
    }
    if (trigger == "on_turn") {
        turnScript = script;
        return true;
    }
    if (trigger == "on_tile_entry") {
        tileEntryScript = script;
        return true;
    }
    if (trigger == "on_tile_exit") {
        tileExitScript = script;
        return true;
    }
    if (trigger == "on_delete") {
        deleteScript = script;
        return true;
    }
    return false;
}
*/


/*
 * PUBLIC ENTITY CODE BELOW
 */

bool
Entity::init(StringView descriptor, StringView initialPhase) noexcept {
    this->descriptor = descriptor;
    CHECK(parseDescriptor(this));
    setPhase(initialPhase);
    return true;
}

void
Entity::destroy() noexcept {
    dead = true;
    if (area) {
        area->requestRedraw();
    }
}

void
Entity::draw(DisplayList* display) noexcept {
    redraw = false;
    if (!phase) {
        return;
    }

    time_t now = World::time();

    // TODO: Don't add to DisplayList if not on-screen.

    // X-axis is centered on tile.
    float maxX = (area->grid.tileDim.x + imgsz.x) / 2 + r.x;
    float minX = maxX - imgsz.x;
    // Y-axis is aligned with bottom of tile.
    float maxY = area->grid.tileDim.y + r.y;
    float minY = maxY - imgsz.y;

    display->items.push_back(
            DisplayItem{phase->setFrame(now), rvec2{minX, minY}});
}

bool
Entity::needsRedraw(icube& visiblePixels) noexcept {
    if (!phase) {
        // Entity is invisible.
        return false;
    }


    if (!redraw) {
        // Entity has not moved and has not changed phase.
        time_t now = World::time();
        if (!phase->needsRedraw(now)) {
            // Entity's animation does not need an update.
            return false;
        }
    }

    // At this point, we know the entity is visible, and has either moved since
    // the last frame or it wants to update its animation frame. Now we check
    // if it is on-screen.

    // X-axis is centered on tile.
    int maxX = (area->grid.tileDim.x + imgsz.x) / 2 + static_cast<int>(r.x);
    int minX = maxX - imgsz.x;
    // Y-axis is aligned with bottom of tile.
    int maxY = area->grid.tileDim.y + static_cast<int>(r.y);
    int minY = maxY - imgsz.y;

    if (visiblePixels.x2 < minX || maxX < visiblePixels.x1) {
        return false;
    }
    if (visiblePixels.y2 < minY || maxY < visiblePixels.y1) {
        return false;
    }

    return true;
}

bool
Entity::isDead() noexcept {
    return dead;
}


void
Entity::tick(time_t dt) noexcept {
    for (auto& fn : onTickFns) {
        fn(dt);
    }
}

void
Entity::turn() noexcept {
    for (auto& fn : onTurnFns) {
        fn();
    }
}

StringView
Entity::getFacing() noexcept {
    return directionStr(facing);
}

bool
Entity::setPhase(StringView name) noexcept {
    enum SetPhaseResult res;
    res = _setPhase(name);
    if (res == PHASE_NOTFOUND) {
        res = _setPhase("stance");
        if (res == PHASE_NOTFOUND) {
            logErr(descriptor, String() << "phase '" << name << "' not found");
        }
    }
    return res == PHASE_CHANGED;
}

ivec2
Entity::getImageSize() noexcept {
    return imgsz;
}

void
Entity::setAnimationStanding() noexcept {
    setPhase(getFacing());
}

void
Entity::setAnimationMoving() noexcept {
    setPhase(String() << "moving " << getFacing());
}


rcoord
Entity::getPixelCoord() noexcept {
    return r;
}

Area*
Entity::getArea() noexcept {
    return area;
}

void
Entity::setArea(Area* area) noexcept {
    this->area = area;
    calcDraw();

    assert_(area->grid.tileDim.x == area->grid.tileDim.y);
    pixelsPerSecond = tilesPerSecond * area->grid.tileDim.x;
}

float
Entity::getSpeedInPixels() noexcept {
    float tileWidth = static_cast<float>(area->grid.tileDim.x);
    return getSpeedInTiles() * tileWidth;
}

float
Entity::getSpeedInTiles() noexcept {
    return tilesPerSecond;
}

void
Entity::setFrozen(bool b) noexcept {
    frozen = b;
}

void
Entity::attach(OnTickFn fn) noexcept {
    onTickFns.push_back(fn);
}

void
Entity::attach(OnTurnFn fn) noexcept {
    onTurnFns.push_back(fn);
}

void
Entity::calcDraw() noexcept {
    if (area) {
        ivec2 tile = area->grid.tileDim;

        // X-axis is centered on tile.
        doff.x = static_cast<float>((tile.x - imgsz.x) / 2);
        // Y-axis is aligned with bottom of tile.
        doff.y = static_cast<float>(tile.y - imgsz.y);
    }
}

void
Entity::setFacing(ivec2 facing) noexcept {
    assert_(facing.x == bound(facing.x, -1, 1));
    assert_(facing.y == bound(facing.y, -1, 1));
    this->facing = facing;
}

StringView
Entity::directionStr(ivec2 facing) noexcept {
    return directions[facing.y + 1][facing.x + 1];
}

enum SetPhaseResult
Entity::_setPhase(StringView name) noexcept {
    Animation* newPhase = nullptr;
    if (name == "stance") {
        newPhase = &phaseStance;
    }
    else if (name == "down") {
        newPhase = &phaseDown;
    }
    else if (name == "left") {
        newPhase = &phaseLeft;
    }
    else if (name == "up") {
        newPhase = &phaseUp;
    }
    else if (name == "right") {
        newPhase = &phaseRight;
    }
    else if (name == "moving up") {
        newPhase = &phaseMovingUp;
    }
    else if (name == "moving right") {
        newPhase = &phaseMovingRight;
    }
    else if (name == "moving down") {
        newPhase = &phaseMovingDown;
    }
    else if (name == "moving left") {
        newPhase = &phaseMovingLeft;
    }
    else {
        return PHASE_NOTFOUND;
    }

    if (phase != newPhase) {
        time_t now = World::time();
        phase = newPhase;
        phase->restart(now);
        phaseName = name;
        redraw = true;
        return PHASE_CHANGED;
    }
    return PHASE_NOTCHANGED;
}

void
Entity::setDestinationCoordinate(rcoord destCoord) noexcept {
    // Set z right away so that we're on-level with the square we're
    // entering.
    r.z = destCoord.z;

    this->destCoord = destCoord;
    angleToDest =
            static_cast<float>(atan2(destCoord.y - r.y, destCoord.x - r.x));
}

void
Entity::moveTowardDestination(time_t dt) noexcept {
    if (!moving) {
        return;
    }

    redraw = true;

    float traveledPixels = pixelsPerSecond * static_cast<float>(dt) / 1000.0f;
    float toDestPixels = r.distanceTo(destCoord);
    if (toDestPixels > traveledPixels) {
        // The destination has not been reached yet.
        r.x += static_cast<float>(cos(angleToDest)) * traveledPixels;
        r.y += static_cast<float>(sin(angleToDest)) * traveledPixels;
    }
    else {
        // We have arrived at the destination.
        r = destCoord;
        moving = false;
        arrived();

        // If arrived() starts a new movement, rollover unused traveled
        // pixels and leave the the moving animation.
        if (moving) {
            float percent = 1.0f - toDestPixels / traveledPixels;
            time_t rem = static_cast<time_t>(percent * static_cast<float>(dt));
            moveTowardDestination(rem);
        }
        else {
            setAnimationStanding();
        }
    }
}

void
Entity::arrived() noexcept {
    // for (auto& fn : onArrivedFns)
    //     fn();
}
