/*************************************
** Tsunagari Tile Engine            **
** pack-reader.h                    **
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

#ifndef SRC_PACK_PACK_READER_H_
#define SRC_PACK_PACK_READER_H_

#include "util/int.h"
#include "util/noexcept.h"
#include "util/string-view.h"

typedef uint32_t BlobIndex;
typedef uint32_t BlobSize;

static BlobIndex BLOB_NOT_FOUND = UINT32_MAX;

class PackReader {
 public:
    static PackReader*
    fromFile(StringView path) noexcept;
    virtual ~PackReader() noexcept {}

    virtual BlobIndex
    size() noexcept = 0;

    virtual BlobIndex
    findIndex(StringView path) noexcept = 0;

    virtual StringView
    getBlobPath(BlobIndex index) noexcept = 0;
    virtual BlobSize
    getBlobSize(BlobIndex index) noexcept = 0;
    virtual void*
    getBlobData(BlobIndex index) noexcept = 0;
};

#endif  // SRC_PACK_PACK_READER_H_
