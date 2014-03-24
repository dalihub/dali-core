#ifndef _ASSERT_H_
#define _ASSERT_H_
#include <stdio.h>
#include <stdlib.h>

#define assert(exp) \
    if (!(exp)) { \
        fprintf(stderr, \
            "Assert fail in %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, \
            "Following expression is not true:\n" \
                "%s\n", #exp); \
        return 1; \
    }

#define assert_eq(var, ref) \
    if (var != ref) { \
        fprintf(stderr, \
            "Assert fail in %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, \
            "Values \"%s\" and \"%s\" are not equal:\n" \
                "%s == %d, %s == %d\n", \
            #var, #ref, #var, (int)var, #ref, (int)ref); \
        return 1; \
    }

#define assert_neq(var, ref) \
    if (var == ref) { \
        fprintf(stderr, \
            "Assert fail in %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, \
            "Values \"%s\" and \"%s\" are equal:\n" \
                "%s == %s == %d\n", \
            #var, #ref, #var, #ref, (int)ref); \
        return 1; \
    }

#define assert_gt(var, ref) \
    if (var <= ref) { \
        fprintf(stderr, \
            "Assert fail in %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, \
            "Value \"%s\" is not greater than \"%s\":\n" \
                "%s == %d, %s == %d\n", \
            #var, #ref, #var, var, #ref, ref); \
        return 1; \
    }

#define assert_geq(var, ref) \
    if (var < ref) { \
        fprintf(stderr, \
            "Assert fail in %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, \
            "Value \"%s\" is not greater or equal to \"%s\":\n" \
                "%s == %d, %s == %d\n", \
            #var, #ref, #var, var, #ref, ref); \
        return 1; \
    }

#define assert_lt(var, ref) \
    if (var >= ref) { \
        fprintf(stderr, \
            "Assert fail in %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, \
            "Value \"%s\" is not lower than \"%s\":\n" \
                "%s == %d, %s == %d\n", \
            #var, #ref, #var, var, #ref, ref); \
        return 1; \
    }

#define assert_leq(var, ref) \
    if (var > ref) { \
        fprintf(stderr, \
            "Assert fail in %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, \
            "Value \"%s\" is not lower or equal to \"%s\":\n" \
                "%s == %d, %s == %d\n", \
            #var, #ref, #var, var, #ref, ref); \
        return 1; \
    }

#endif //  _ASSERT_H_
