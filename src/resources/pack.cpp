/********************************
** Tsunagari Tile Engine       **
** pack.cpp                    **
** Copyright 2016 Paul Merrill **
********************************/

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

#include "resources/pack.h"

#include <stdlib.h>

#include <memory>
#include <mutex>

#include "core/formatter.h"
#include "core/log.h"
#include "core/measure.h"

#include "data/data-world.h"

static PackResources globalResources;

Resources& Resources::instance() {
    return globalResources;
}


PackResource::PackResource(void* data, size_t size)
    : _data(data), _size(size) {}

const void* PackResource::data() const {
    return _data.get();
}

size_t PackResource::size() const {
    return _size;
}


PackResources::PackResources() : initialized(false) {}

void PackResources::init() {
    const std::string& path = DataWorld::instance().datafile;

    pack = PackReader::fromFile(path);
    if (!pack) {
        Log::fatal("PackResources",
                   Formatter("%: could not open archive") % path);
    }
}

std::unique_ptr<Resource> PackResources::load(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex);

    TimeMeasure m("Mapped " + path);

    if (!initialized) {
        initialized = true;
        init();
    }

    const std::string fullPath = DataWorld::instance().datafile + "/" + path;

    PackReader::BlobIndex index = pack->findIndex(path);

    if (index == PackReader::BLOB_NOT_FOUND) {
        Log::err("PackResources", Formatter("%: file missing") % fullPath);
        return std::unique_ptr<Resource>();
    }

    uint64_t blobSize = pack->getBlobSize(index);

    // Will it fit in memory?
    if (blobSize > std::numeric_limits<size_t>::max()) {
        Log::err("PackResources", Formatter("%: file too large") % fullPath);
        return std::unique_ptr<Resource>();;
    }

    void* blobData = pack->getBlobData(index);

    return std::make_unique<PackResource>(blobData, blobSize);
}