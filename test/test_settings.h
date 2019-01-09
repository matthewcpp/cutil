#ifndef CUTIL_TEST_SETTINGS_H
#define CUTIL_TEST_SETTINGS_H

void cutil_test_settings_init();
void cutil_test_settings_destroy();

int cutil_test_settings_parse(int argc, const char** argv);
int cutil_test_validate_settings();
const char* cutil_test_get_filter_string();
const char* cutil_test_get_data_directory();

#endif
