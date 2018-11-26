#pragma once

#include <stdbool.h>

void cutil_test_settings_init();
void cutil_test_settings_destroy();

bool cutil_test_settings_parse(int argc, const char** argv);
bool cutil_test_validate_settings();
const char* cutil_test_get_filter_string();
const char* cutil_test_get_data_directory();
