#ifndef _VITORE_UNIQUE_HANDLE_HPP
#define _VITORE_UNIQUE_HANDLE_HPP

#include <glad/glad.h>

#include <utility>

template <void deleter(GLuint)>
struct UniqueHandle {
    GLuint handle;

    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;

    explicit UniqueHandle(GLuint handle):
        handle(handle) {}

    UniqueHandle(UniqueHandle&& other):
        handle(std::exchange(other.handle, 0)) {}

    UniqueHandle& operator=(UniqueHandle&& other) {
        std::swap(this->handle, other.handle);
        return *this;
    }

    ~UniqueHandle() {
        if (this->handle != 0)
            deleter(this->handle);
    }

    operator GLuint() const {
        return this->handle;
    }

    GLuint* operator&() {
        return &this->handle;
    }
};

#endif
