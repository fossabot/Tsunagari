/***************************************
** Tsunagari Tile Engine              **
** tile-grid.cpp                      **
** Copyright 2011-2015 Michael Reiley **
** Copyright 2011-2019 Paul Merrill   **
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

#include "core/tile-grid.h"

#include "core/log.h"
#include "util/assert.h"
#include "util/math2.h"
#include "util/string.h"

int
TileGrid::getTileType(icoord phys) noexcept {
    int x = dim.x;
    int y = dim.y;

    if (loopX) {
        phys.x = wrap(0, phys.x, x);
    }

    if (loopY) {
        phys.y = wrap(0, phys.y, y);
    }

    int idx = (phys.z * y + phys.y) * x + phys.x;
    return types[idx];
}

void
TileGrid::setTileType(icoord phys, int type) noexcept {
    int x = dim.x;
    int y = dim.y;

    if (loopX) {
        phys.x = wrap(0, phys.x, x);
    }

    if (loopY) {
        phys.y = wrap(0, phys.y, y);
    }

    int idx = (phys.z * y + phys.y) * x + phys.x;
    types[idx] = type;
}

Tile*
TileGrid::getTile(icoord phys) noexcept {
    int x = dim.x;
    int y = dim.y;

    if (loopX) {
        phys.x = wrap(0, phys.x, dim.x);
    }

    if (loopY) {
        phys.y = wrap(0, phys.y, dim.y);
    }

    int idx = (phys.z * y + phys.y) * x + phys.x;
    return &grid[idx];
}

Tile*
TileGrid::getTile(vicoord virt) noexcept {
    return getTile(virt2phys(virt));
}

Tile*
TileGrid::getTile(rcoord virt) noexcept {
    return getTile(virt2phys(virt));
}

const Tile*
TileGrid::getTile(icoord phys) const noexcept {
    int x = dim.x;
    int y = dim.y;

    if (loopX) {
        phys.x = wrap(0, phys.x, dim.x);
    }

    if (loopY) {
        phys.y = wrap(0, phys.y, dim.y);
    }

    assert_(inBounds(phys));

    int idx = (phys.z * y + phys.y) * x + phys.x;
    return &grid[idx];
}

const Tile*
TileGrid::getTile(vicoord virt) const noexcept {
    return getTile(virt2phys(virt));
}

const Tile*
TileGrid::getTile(rcoord virt) const noexcept {
    return getTile(virt2phys(virt));
}


ivec3
TileGrid::getDimensions() const noexcept {
    return dim;
}

ivec2
TileGrid::getTileDimensions() const noexcept {
    return tileDim;
}


bool
TileGrid::doesLoopInX() const noexcept {
    return loopX;
}

bool
TileGrid::doesLoopInY() const noexcept {
    return loopY;
}


bool
TileGrid::inBounds(icoord phys) const noexcept {
    return (loopX || (0 <= phys.x && phys.x < dim.x)) &&
           (loopY || (0 <= phys.y && phys.y < dim.y)) &&
           (0 <= phys.z && phys.z < dim.z);
}

bool
TileGrid::inBounds(vicoord virt) const noexcept {
    return inBounds(virt2phys(virt));
}

bool
TileGrid::inBounds(rcoord virt) const noexcept {
    return inBounds(virt2phys(virt));
}


vicoord
TileGrid::phys2virt_vi(icoord phys) const noexcept {
    return vicoord{phys.x, phys.y, indexDepth(phys.z)};
}

rcoord
TileGrid::phys2virt_r(icoord phys) const noexcept {
    return rcoord{static_cast<double>(phys.x * tileDim.x),
                  static_cast<double>(phys.y * tileDim.y),
                  indexDepth(phys.z)};
}

icoord
TileGrid::virt2phys(vicoord virt) const noexcept {
    return icoord{static_cast<int>(virt.x),
                  static_cast<int>(virt.y),
                  depthIndex(virt.z)};
}

icoord
TileGrid::virt2phys(rcoord virt) const noexcept {
    return icoord{static_cast<int>(virt.x) / tileDim.x,
                  static_cast<int>(virt.y) / tileDim.y,
                  depthIndex(virt.z)};
}

rcoord
TileGrid::virt2virt(vicoord virt) const noexcept {
    return rcoord{static_cast<double>(virt.x * tileDim.x),
                  static_cast<double>(virt.y * tileDim.y),
                  virt.z};
}

vicoord
TileGrid::virt2virt(rcoord virt) const noexcept {
    return vicoord{static_cast<int>(virt.x) / tileDim.x,
                   static_cast<int>(virt.y) / tileDim.y,
                   virt.z};
}


int
TileGrid::depthIndex(double depth) const noexcept {
    auto it = depth2idx.find(depth);
    if (it == depth2idx.end()) {
        Log::fatal("TileGrid",
                   String() << "Attempt to access invalid layer: " << depth);
    }
    return it.value();
}

double
TileGrid::indexDepth(int idx) const noexcept {
    assert_(0 <= idx && idx <= dim.z);
    return idx2depth[(size_t)idx];
}
