/*************************************
** Tsunagari Tile Engine            **
** main.cpp                         **
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

#include "os/c.h"
#include "os/mutex.h"
#include "os/os.h"
#include "pack/pack-reader.h"
#include "pack/pack-writer.h"
#include "pack/walker.h"
#include "util/int.h"
#include "util/noexcept.h"
#include "util/string-view.h"
#include "util/string.h"

static String exe;
static bool verbose = false;

static void
usage() noexcept {
    const char* me = exe.null();
    fprintf(stderr,
            "usage: %s create [-v] <output-archive> [input-file]...\n"
            "       %s list <input-archive>\n"
            "       %s extract [-v] <input-archive>\n",
            me, me, me);
}

struct CreateArchiveContext {
    PackWriter* pack;
    Mutex packMutex;
};

static void
addFile(CreateArchiveContext& ctx, StringView path) noexcept {
    String data;
    bool ok = readFile(path, data);

    if (!ok) {
        if (verbose) {
            printf("%s", (String() << "Skipped " << path << ": file not found\n\0").null());
        }
        return;
    }

    if (verbose) {
        printf("%s", (String() << "Added " << path << ": " << data.size << " bytes\n").null());
    }

    // Write the file path to the pack file with '/' instead of '\\' on Windows.
    String standardizedPath;

    if (dirSeparator != '/') {
        standardizedPath = path;

        for (size_t i = 0; i < path.size; i++) {
            if (standardizedPath[i] == dirSeparator) {
                standardizedPath[i] = '/';
            }
        }

        path = standardizedPath;
    }

    LockGuard guard(ctx.packMutex);
    packWriterAddBlob(ctx.pack, path, static_cast<uint32_t>(data.size),
                      data.data);

    data.reset();  // Don't delete data pointer.
}

static bool
createArchive(StringView archivePath, Vector<StringView> paths) noexcept {
    CreateArchiveContext ctx;
    ctx.pack = makePackWriter();

    walk(static_cast<Vector<StringView>&&>(paths),
         [&](StringView path) noexcept { addFile(ctx, path); });

    if (verbose) {
        printf("%s", (String() << "Writing to " << archivePath << "\n").null());
    }

    bool ok = packWriterWriteToFile(ctx.pack, archivePath);

    destroyPackWriter(ctx.pack);
    return ok;
}

static bool
listArchive(StringView archivePath) noexcept {
    PackReader* pack = PackReader::fromFile(archivePath);

    if (pack) {
        String output;

        for (BlobIndex i = 0; i < pack->size(); i++) {
            StringView blobPath = pack->getBlobPath(i);
            uint64_t blobSize = pack->getBlobSize(i);

            // Print file paths with '\\' Windows.
            String standardizedPath;

            if (dirSeparator != '/') {
                standardizedPath = blobPath;

                for (size_t i = 0; i < blobPath.size; i++) {
                    if (standardizedPath[i] == '/') {
                        standardizedPath[i] = dirSeparator;
                    }
                }

                blobPath = standardizedPath;
            }

            output << blobPath << ": " << blobSize << " bytes\n";
        }

        printf("%s", output.null());

        delete pack;
        return true;
    }
    else {
        fprintf(stderr,
                "%s",
                (String() << exe << ": " << archivePath << ": not found\n")
                        .null());

        delete pack;
        return false;
    }
}

static bool
getParentPath(StringView path, StringView& parent) noexcept {
    StringPosition sep = path.rfind(dirSeparator);
    if (sep == SV_NOT_FOUND) {
        return false;
    }
    else {
        parent = path.substr(0, sep);
        return true;
    }
}

static void
createDirs(StringView path) noexcept {
    StringView parentPath;
    if (getParentPath(path, parentPath)) {
        // Make sure parentPath's parent exists.
        createDirs(parentPath);

        makeDirectory(parentPath);
    }
}

static void
putFile(StringView path, uint32_t size, void* data) noexcept {
    createDirs(path);

    // TODO: Propagate error up.
    writeFile(path, size, data);
}

static bool
extractArchive(StringView archivePath) noexcept {
    PackReader* pack = PackReader::fromFile(archivePath);

    if (pack) {
        Vector<void*> blobDatas;
        for (BlobIndex i = 0; i < pack->size(); i++) {
            blobDatas.push_back(pack->getBlobData(i));
        }

        for (BlobIndex i = 0; i < pack->size(); i++) {
            StringView blobPath = pack->getBlobPath(i);
            uint32_t blobSize = pack->getBlobSize(i);
            void* blobData = blobDatas[i];

            // Change file paths to use '\\' on Windows.
            String standardizedPath;

            if (dirSeparator != '/') {
                standardizedPath = blobPath;

                for (size_t i = 0; i < blobPath.size; i++) {
                    if (standardizedPath[i] == '/') {
                        standardizedPath[i] = dirSeparator;
                    }
                }

                blobPath = standardizedPath;
            }

            if (verbose) {
                printf("%s", (String() << "Extracting " << blobPath << ": " << blobSize << " bytes\n").null());
            }

            putFile(blobPath, blobSize, blobData);
        }

        delete pack;
        return true;
    }
    else {
        fprintf(stderr,
                "%s: %s: not found\n",
                exe.null(),
                String(archivePath).null());

        delete pack;
        return false;
    }
}

int
main(int argc, char* argv[]) noexcept {
    exe = argv[0];
    StringPosition dir = exe.view().rfind(dirSeparator);
    if (dir == SV_NOT_FOUND) {
        exe = exe.view().substr(dir + 1);
    }

    if (argc == 1) {
        usage();
        return 0;
    }
    if (argc == 2) {
        usage();
        return 1;
    }

    StringView command = argv[1];
    Vector<StringView> args;

    for (int i = 2; i < argc; i++) {
        args.push_back(argv[i]);
    }

    int exitCode;

    if (command == "create") {
        if (args.size > 0 && args[0] == "-v") {
            verbose = true;
            args.erase(0);
        }

        if (args.size < 2) {
            usage();
            return 1;
        }

        // The first argument is the archive, the rest are files to add to it.
        StringView archivePath = args[0];
        args.erase(0);

        return createArchive(archivePath,
                             static_cast<Vector<StringView>&&>(args)) ? 0 : 1;
    }
    else if (command == "list") {
        verbose = true;

        if (args.size != 1) {
            usage();
            return 1;
        }

        exitCode = listArchive(args[0]) ? 0 : 1;
    }
    else if (command == "extract") {
        if (args.size > 0 && args[0] == "-v") {
            verbose = true;
            args.erase(0);
        }

        if (args.size != 1) {
            usage();
            return 1;
        }

        exitCode = extractArchive(args[0]) ? 0 : 1;
    }
    else {
        usage();
        return 1;
    }

    return exitCode;
}
