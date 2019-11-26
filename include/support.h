#ifndef MMPL_SUPPORT_H
#define MMPL_SUPPORT_H

#ifdef NDEBUG

#define MMPL_RUNTIME_ASSERT(expr)

#else  // NDEBUG

/**
 * @brief Wraps runtime assert to mask out durin non-debug builds
 */
#include <cassert>
#define MMPL_RUNTIME_ASSERT(expr) assert(expr)

#endif  // NDEBUG


#ifndef MMPL_DISABLE_CLEARER_STATIC_ASSERT_MESSAGES
/**
 * @brief Wraps static_assert message to make it easier to find in build errors
 */
#define MMPL_STATIC_ASSERT_MSG(msg) "[\n\n" msg "\n\n]"

#else  // MMPL_DISABLE_CLEARER_STATIC_ASSERT_MESSAGES

#define MMPL_STATIC_ASSERT_MSG(msg) msg

#endif  // MMPL_DISABLE_CLEARER_STATIC_ASSERT_MESSAGES

#endif  // MMPL_SUPPORT_H
