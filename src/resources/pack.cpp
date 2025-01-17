/*************************************
** Tsunagari Tile Engine            **
** pack.cpp                         **
** Copyright 2016-2020 Paul Merrill **
*************************************/

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

#include "core/log.h"
#include "core/measure.h"
#include "core/resources.h"
#include "data/data-world.h"
#include "os/mutex.h"
#include "pack/pack-reader.h"
#include "util/int.h"

static Mutex mutex;
static PackReader* pack = 0;

static bool
openPackFile() noexcept {
    if (pack) {
        return true;
    }

    StringView path = dataWorldDatafile;

    // TimeMeasure m("Opened " + path);

    pack = PackReader::fromFile(path);
    if (!pack) {
        logFatal("PackResources",
                 String() << path << ": could not open archive");
        return false;
    }

    return true;
}

static String
getFullPath(StringView path) noexcept {
    return String() << dataWorldDatafile << "/" << path;
}

bool
resourceLoad(StringView path, StringView& data) noexcept {
    LockGuard lock(mutex);

    if (!openPackFile()) {
        return false;
    }

    BlobIndex index = pack->findIndex(path);

    if (index == BLOB_NOT_FOUND) {
        logErr("PackResources",
               String() << getFullPath(path) << ": file missing");
        return false;
    }

    uint32_t blobSize = pack->getBlobSize(index);

    // Will it fit in memory?
    if (blobSize > static_cast<uint32_t>(INT32_MAX)) {
        logErr("PackResources",
               String() << getFullPath(path) << ": file too large");
        return false;
    }

    void* blob = pack->getBlobData(index);

    data.data = static_cast<char*>(blob);
    data.size = blobSize;
    return true;
}
