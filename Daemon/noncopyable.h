#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace noncopyable {

struct NonCopyable
{
    NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable(const NonCopyable &&) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &&) = delete;
};

} //namespace
#endif // NONCOPYABLE_H
