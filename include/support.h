#ifndef MMPL_SUPPORT_H
#define MMPL_SUPPORT_H

#include <cassert>

#define MMPL_RUNTIME_ASSERT(expr) assert(expr)

#define MMPL_STATIC_ASSERT_MSG(msg) "[\n" msg "\n]"

#endif  // MMPL_SUPPORT_H
