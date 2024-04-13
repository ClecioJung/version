// MIT License

// Copyright (c) 2024 CLECIO JUNG <clecio.jung@gmail.com>

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//------------------------------------------------------------------------------
// HEADER
//------------------------------------------------------------------------------

#ifndef VERSION_NO_STDLIB
#include <stddef.h> // size_t
#include <stdlib.h> // strtoul, NULL
#include <stdio.h> // printf
#include <string.h> // strcspn, strlen, strncmp
#include <ctype.h> // isspace
#endif // VERSION_NO_STDLIB

#ifndef __VERSION_HEADER_ONLY_LIBRARY
#define __VERSION_HEADER_ONLY_LIBRARY

// This file is a header only library. In order to include it's implementation,
// define the macro VERSION_IMPLEMENTATION before including this file

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned int major;
    unsigned int minor;
    unsigned int patch;
    const char *pre_release;
    size_t pre_release_len;
    const char *build_info;
    size_t build_info_len;
} Semantic_Version;

typedef enum {
    VERSION_EQUAL = 0,
    VERSION_GREATER_PRE_RELEASE,
    VERSION_GREATER_PATCH,
    VERSION_GREATER_MINOR,
    VERSION_GREATER_MAJOR,
} Semantic_Version_Spec;

#if !defined(__GNUC__) && !defined(__attribute__)
#define __attribute__(a)
#define warn_unused_result
#define nonnull
#endif

int parse_semantic_version(const char *str, Semantic_Version *const version)
    __attribute__((nonnull));
void debug_print_semantic_version(const Semantic_Version version);
int compare_semantic_versions(const Semantic_Version version1, const Semantic_Version version2)
    __attribute__((warn_unused_result));
int is_version_compatible(const Semantic_Version_Spec spec, const Semantic_Version expected, const Semantic_Version version)
    __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif  // __VERSION_HEADER_ONLY_LIBRARY

//------------------------------------------------------------------------------
// SOURCE
//------------------------------------------------------------------------------

#ifdef VERSION_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

int parse_semantic_version(const char *str, Semantic_Version *const version)
{
    *version = (Semantic_Version){ 0 };
    while (isspace(*str))
        str++;
    for (int i = 0; i < 3; i++) {
        char *endptr;
        unsigned long value = strtoul(str, &endptr, 10);
        if (str == endptr) {
            // error
            return -1;
        }
        (&version->major)[i] = (unsigned int)value;
        str = endptr;
        if (i < 2) {
            if (*str != '.') break;
            str++;
        }
    }
    // pre-release info
    if (*str == '-') {
        version->pre_release = ++str;
        version->pre_release_len = strcspn(str, "+");
        str += version->pre_release_len;
    }
    // build info
    if (*str == '+') {
        version->build_info = ++str;
        version->build_info_len = strlen(str);
        str += version->build_info_len;
    }
    if (*str != '\0') {
        // error
        return -1;
    }
    return 0;
}

void debug_print_semantic_version(const Semantic_Version version)
{
    printf("Major: %u\n", version.major);
    printf("Minor: %u\n", version.minor);
    printf("Patch: %u\n", version.patch);
    if (version.pre_release != NULL && version.pre_release_len > 0) {
        printf("Pre-release info: %.*s\n", (int)version.pre_release_len, version.pre_release);
    }
    if (version.build_info != NULL && version.build_info_len > 0) {
        printf("Build info: %.*s\n", (int)version.build_info_len, version.build_info);
    }
}

static int compare_sized_strings(const char *str1, const char *str2, const int len1, const int len2)
{
    const size_t min_len = (size_t)(len1 < len2 ? len1 : len2);
    int result = strncmp(str1, str2, min_len);
    if (result == 0) {
        return (len1 - len2);
    }
    return result;
}

static int parse_identifier(const char **id, int *const integer, int *const len)
{
    char *endptr = NULL;
    *len = (int)strcspn(*id, ".+");
    *integer = (int)strtol(*id, &endptr, 10);
    const int id_is_int = (*id != endptr) && ((endptr-*id) == *len);
    // consume input
    *id += *len;
    if (**id != '\0')
        (*id)++;
    return id_is_int;
}

static int compare_identifiers(const char **id1, const char **id2)
{
    int int1, int2;
    int len1, len2;
    const char *const current_id1 = *id1;
    const char *const current_id2 = *id2;
    const int id1_is_int = parse_identifier(id1, &int1, &len1);
    const int id2_is_int = parse_identifier(id2, &int2, &len2);
    if (id1_is_int != id2_is_int) {
        // integers have lower precedence than alphanumeric identifiers
        return id2_is_int - id1_is_int;
    }
    if (id1_is_int) {
        // compare identifiers as integers
        return int1 - int2;
    }
    // compare identifiers as strings
    return compare_sized_strings(current_id1, current_id2, len1, len2);
}

static int compare_pre_release(const char *pre_release1, const char *pre_release2)
{
    if (pre_release1 == NULL)
        return (pre_release2 == NULL) ? 0 : 1;
    else if (pre_release2 == NULL)
        return -1;
    int result = 0;
    while (result == 0) {
        if (*pre_release1 == '\0' || *pre_release2 == '\0')
            return *pre_release1 - *pre_release2;
        result = compare_identifiers(&pre_release1, &pre_release2);
    }
    return result;
}

int compare_semantic_versions(const Semantic_Version version1, const Semantic_Version version2)
{
    int result = 0;
    for (int i = 0; result == 0 && i < 3; i++) {
        const int value1 = (int)(&version1.major)[i];
        const int value2 = (int)(&version2.major)[i];
        result = value1 - value2;
    }
    if (result == 0)
        return compare_pre_release(version1.pre_release, version2.pre_release);
    return result;
}

int is_version_compatible(const Semantic_Version_Spec spec, const Semantic_Version expected, const Semantic_Version version)
{
    const int cmp = compare_semantic_versions(version, expected);
    switch (spec) {
        case VERSION_EQUAL:
            return cmp == 0;
        case VERSION_GREATER_PRE_RELEASE:
            if (version.major != expected.major || version.minor != expected.minor || version.patch != expected.patch) {
                return 0;
            }
            return cmp >= 0;
        case VERSION_GREATER_PATCH:
            if (version.major != expected.major || version.minor != expected.minor) {
                return 0;
            }
            return cmp >= 0;
        case VERSION_GREATER_MINOR:
            if (version.major != expected.major) {
                return 0;
            }
            return cmp >= 0;
        case VERSION_GREATER_MAJOR:
            return cmp >= 0;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif // VERSION_IMPLEMENTATION

//------------------------------------------------------------------------------
// END
//------------------------------------------------------------------------------

// MIT License

// Copyright (c) 2024 CLECIO JUNG <clecio.jung@gmail.com>

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
