/***************************************
** Tsunagari Tile Engine              **
** player.h                           **
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

#ifndef SRC_CORE_PLAYER_H_
#define SRC_CORE_PLAYER_H_

#include "core/character.h"
#include "core/vec.h"

struct Exit;

class Player : public Character {
 public:
    static Player&
    instance() noexcept;

    Player() noexcept;
    void
    destroy() noexcept final;

    //! Smooth continuous movement.
    void
    startMovement(ivec2 delta) noexcept;
    void
    stopMovement(ivec2 delta) noexcept;

    //! Move the player by dx, dy. Not guaranteed to be smooth if called
    //! on each update().
    void
    moveByTile(ivec2 delta) noexcept;

    //! Try to use an object in front of the player.
    void
    useTile() noexcept;

    void
    setFrozen(bool b) noexcept final;

 protected:
    void
    arrived() noexcept final;

    void
    takeExit(Exit& exit) noexcept;

 private:
    //! Stores intent to move continuously in some direction.
    ivec2 velocity;

    //! Stack storing depressed keyboard keys in the form of movement vectors.
    ivec2 movements[8];
    size_t numMovements;
};

#endif  // SRC_CORE_PLAYER_H_
