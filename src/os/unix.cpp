/*************************************
** Tsunagari Tile Engine            **
** os/unix.cpp                      **
** Copyright 2016-2019 Paul Merrill **
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

#include "os/os.h"

#define _DARWIN_USE_64_BIT_INODE

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/dir.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "util/vector.h"

char dirSeparator = '/';

Optional<uint64_t> getFileSize(String&& path) {
    struct stat status;
    if (stat(path.null(), &status)) {
        return Optional<uint64_t>();
    }
    return Optional<uint64_t>(static_cast<uint64_t>(status.st_size));
}

bool isDir(String&& path) {
    struct stat status;
    if (stat(path.null(), &status)) {
        return false;
    }
    return S_ISDIR(status.st_mode);
}

void makeDirectory(String&& path) {
    mkdir(path.null(), 0777);
}

vector<String> listDir(String&& path) {
    DIR* dir = nullptr;
    struct dirent* entry = nullptr;
    vector<String> names;

    if ((dir = opendir(path.null())) == nullptr) {
        return names;
    }

    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_ino == 0) {
            // Ignore unlinked files.
            continue;
        }
        if (entry->d_name[0] == '.') {
            // Ignore hidden files and directories.
            continue;
        }
        if ((entry->d_type & (DT_DIR | DT_REG)) == 0) {
            // Ignore odd files.
            continue;
        }
        names.push_back(entry->d_name);
    }

    closedir(dir);

    return names;
}

bool writeFile(String&& path, size_t length, void* data) {
    int fd = open(path.null(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd == -1) {
        return false;
    }
    ssize_t written = write(fd, data, length);
    if (written != length) {
        close(fd);
        return false;
    }
    close(fd);
    return true;
}

bool writeFileVec(String&& path, size_t count, size_t* lengths, void** datas) {
    int fd = open(path.null(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd == -1) {
        return false;
    }

    ssize_t total = 0;
    vector<iovec> ios;

    ios.reserve(count);
    for (size_t i = 0; i < count; i++) {
        total += lengths[i];
        ios.push_back({datas[i], lengths[i]});
    }

    ssize_t written = writev(fd, ios.data(), static_cast<int>(ios.size()));
    if (written != total) {
        close(fd);
        return false;
    }

    close(fd);
    return true;
}

/*
bool writeFileVec(String&& path, size_t count, size_t* lengths, void** datas) {
    int fd = open(path.null(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd == -1) {
        return false;
    }
    for (size_t i = 0; i < count; i++) {
        size_t length = lengths[i];
        void* data = datas[i];
        ssize_t written = write(fd, data, length);
        if (written != length) {
            close(fd);
            return false;
        }
    }
    close(fd);
    return true;
}
*/

static bool isaTTY() {
    static bool checked = false;
    static bool tty = false;

    if (!checked) {
        tty = isatty(0) != 0;
    }
    return tty;
}

void setTermColor(TermColor color) {
    if (!isaTTY()) {
        return;
    }

    // VT100 terminal control codes from:
    //   http://www.termsys.demon.co.uk/vtansi.htm

    const char escape = 27;

    switch (color) {
    case TC_RESET:
        printf("%c[0m", escape);
        break;
    case TC_GREEN:
        printf("%c[32m", escape);
        break;
    case TC_YELLOW:
        printf("%c[33m", escape);
        break;
    case TC_RED:
        printf("%c[31m", escape);
        break;
    }
}

Optional<MappedFile> MappedFile::fromPath(String&& path) {
	int fd = open(path.null(), O_RDONLY);
	if (fd == -1) {
		return Optional<MappedFile>();
	}

	struct stat st;
	fstat(fd, &st);

	if (st.st_size == 0) {
		close(fd);
		return Optional<MappedFile>();
	}

	// Cannot open files >4 GB on 32-bit operating systems since they will fail
	// the mmap.
	if (sizeof(long long) > sizeof(size_t)) {
		if (st.st_size > static_cast<long long>(SIZE_MAX)) {
			close(fd);
			return Optional<MappedFile>();
		}
	}

	char* map = reinterpret_cast<char*>(mmap(nullptr,
		static_cast<size_t>(st.st_size),
		PROT_READ, MAP_SHARED, fd, 0));
	size_t len = static_cast<size_t>(st.st_size);

	if (map == MAP_FAILED) {
		close(fd);
		return Optional<MappedFile>();
	}

	// FIXME: Close the fd now or at destruction?
	return Optional<MappedFile>(MappedFile(map, len));
}

MappedFile::MappedFile() : map(reinterpret_cast<char*>(MAP_FAILED)), len(0) {}

MappedFile::MappedFile(MappedFile&& other) { *this = move_(other); }

MappedFile::MappedFile(char* map, size_t len) : map(map), len(len) {}

MappedFile::~MappedFile() {
	if (map != MAP_FAILED) {
		munmap(map, len);
	}
}

MappedFile& MappedFile::operator=(MappedFile&& other) {
	map = other.map;
	len = other.len;
	other.map = reinterpret_cast<char*>(MAP_FAILED);
	other.len = 0;
	return *this;
}
