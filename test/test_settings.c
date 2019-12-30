#include "test_settings.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    char* filter_string;
    char* data_directory;
} cutil_test_settings;


cutil_test_settings* test_settings = NULL;

void cutil_test_settings_init() {
    if (!test_settings) {
        test_settings = (cutil_test_settings*)malloc(sizeof(cutil_test_settings));
        
        test_settings->filter_string = NULL;
        test_settings->data_directory = NULL;
    }
}

void cutil_test_settings_destroy() {
    if (test_settings) {
        if (test_settings->data_directory) {
            free(test_settings->data_directory);
        }

        if (test_settings->filter_string) {
            free(test_settings->filter_string);
        }

        free(test_settings);
        test_settings = NULL;
    }
}

int cutil_test_settings_parse(int argc, const char** argv) {
    int i = 1;

    while (i < argc) {
        if (strcmp(argv[i], "--cutil-test-filter") == 0) {
            i += 1;

            test_settings->filter_string = (char*)malloc(strlen(argv[i]) + 1);
            strcpy(test_settings->filter_string, argv[i]);
        }
        else if (strcmp(argv[i], "--cutil-test-data-dir") == 0) {
            i += 1;

            test_settings->data_directory = (char*)malloc(strlen(argv[i]) + 1);
            strcpy(test_settings->data_directory, argv[i]);
        }

        i += 1;
    }

    return cutil_test_validate_settings();
}

int cutil_test_validate_settings() {
    if (!test_settings->data_directory) {
        printf("You must specify a data directory using --cutil-test-data-dir");
        return 0;
    }

    return 1;
}

const char* cutil_test_get_filter_string() {
    return test_settings->filter_string;
}

const char* cutil_test_get_data_directory() {
    return test_settings->data_directory;
}
