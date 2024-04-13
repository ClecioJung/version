#include <stdio.h>
#include <stdlib.h>

#define VERSION_IMPLEMENTATION
#include "version.h"

#define ARRAY_LEN(xs) (sizeof(xs)/sizeof(xs[0]))

static int compare(const void *ptr1, const void *ptr2)
{
    const char *str1 = *(const char **)ptr1;
    const char *str2 = *(const char **)ptr2;
    Semantic_Version ver1, ver2;
    // Errors are not handled in here, for simplicity :)
    parse_semantic_version(str1, &ver1);
    parse_semantic_version(str2, &ver2);
    return compare_semantic_versions(ver1, ver2);
}

static char spec_to_char(const Semantic_Version_Spec spec)
{
    switch (spec) {
        case VERSION_EQUAL:
            return '=';
        case VERSION_GREATER_PRE_RELEASE:
            return '@';
        case VERSION_GREATER_PATCH:
            return '~';
        case VERSION_GREATER_MINOR:
            return '^';
        case VERSION_GREATER_MAJOR:
            return '>';
    }
    return '\0';
}

static const char *bool_to_str(const int value)
{
    return value ? "true" : "false";
}

int main(void)
{
    const char *versions[] = {
        "2.0.0.rc.1",
        "1.2.3",
        "1.3.4",
        "2",
        "1.2+8",
        "1.2.3-alpha.beta",
        "1.2.3-alpha.1",
        "1.2.3-rc.9",
        "4",
        "1.2.3-rc.10",
        "1.2.3-beta",
        "1.0",
        "1.1.6+8",
        "1.2.3-rc.11",
        "1.2.3-alpha",
        "1.3.0",
        "1.2-alpha",
        "1.2.3-beta+2",
        "1.2.3-rc.11.test",
        "1.2.3-rc.1",
        "1.2",
        "1.2.3-rc.2",
        "1.2.3-rc.test",
        "1.1.6",
        "3.0.0",
        "1.5.0.rc.1",
        "1.2.4",
        "1.2.3-beta",
    };
    const struct {
        Semantic_Version_Spec spec;
        char *expected;
    } specifications[] = {
        { VERSION_EQUAL, "1.2.3" },
        { VERSION_EQUAL, "1.2.3-rc.2" },
        { VERSION_GREATER_MAJOR, "1.2.3" },
        { VERSION_GREATER_MINOR, "1.2.3" },
        { VERSION_GREATER_PATCH, "1.2.3" },
        { VERSION_GREATER_PATCH, "1.2.3-rc.2" },
        { VERSION_GREATER_PRE_RELEASE, "1.2.3-rc.2" },
    };
    // Sort the array of versions in ascending order
    qsort (versions, ARRAY_LEN(versions), sizeof(versions[0]), compare);
    // Print the header
    printf("%-20s", "Version");
    for (size_t j = 0; j < ARRAY_LEN(specifications); j++) {
        char formatted_spec[20];
        char c = spec_to_char(specifications[j].spec);
        snprintf(formatted_spec, sizeof(formatted_spec), "%c%s", c, specifications[j].expected);
        printf("%-15s", formatted_spec);
    }
    printf("\n");
    // Print the table
    for (size_t i = 0; i < ARRAY_LEN(versions); i++) {
        printf("%-20s", versions[i]);
        for (size_t j = 0; j < ARRAY_LEN(specifications); j++) {
            Semantic_Version parsed_version, expected_version;
            parse_semantic_version(versions[i], &parsed_version);
            parse_semantic_version(specifications[j].expected, &expected_version);
            int result = is_version_compatible(specifications[j].spec, expected_version, parsed_version);
            printf("%-15s", bool_to_str(result));
        }
        printf("\n");
    }
    return 0;
}
