/*************************************
** Tsunagari Tile Engine            **
** file-type.cpp                    **
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

#include "pack/file-type.h"

#include "util/noexcept.h"
#include "util/string-view.h"

static const StringView textExtensions[] = {".json"};

static const StringView mediaExtensions[] = {".oga", ".png"};

FileType
determineFileType(StringView path) noexcept {
    StringPosition dot = path.rfind('.');
    if (dot == SV_NOT_FOUND) {
        return FT_UNKNOWN;
    }
    StringView extension = path.substr(dot);
    for (StringView textExtension : textExtensions) {
        if (extension == textExtension) {
            return FT_TEXT;
        }
    }
    for (StringView mediaExtension : mediaExtensions) {
        if (extension == mediaExtension) {
            return FT_MEDIA;
        }
    }
    return FT_UNKNOWN;
}
