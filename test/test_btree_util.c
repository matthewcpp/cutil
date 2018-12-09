#include "test_btree_util.h"

#include "test_settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

extern void read_btree(cutil_btree* btree, const char* data);

bool read_btree_from_file(cutil_btree* btree, const char* test_data_name) {
	const char* test_data_dir = cutil_test_get_data_directory();
	int path_size = snprintf(NULL, 0, "%s/%s.txt", test_data_dir, test_data_name);
	char*  path_str = malloc(path_size + 1);
	sprintf(path_str, "%s/%s.txt", test_data_dir, test_data_name);

	FILE*  file = fopen(path_str, "r");
	free(path_str);

	if (file) {
		long file_size = 0;
		fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		fseek(file, 0, SEEK_SET);

		char* file_data = malloc(file_size + 1);
		fread(file_data, 1, file_size, file);
		file_data[file_size] = 0;
		fclose(file);

		read_btree(btree, file_data);

		free(file_data);
		return true;
	}
	else {
		return false;
	}
}



void insert_char_sequence(cutil_btree* btree, const char* sequence) {
	size_t len = strlen(sequence);

	for (size_t i = 0; i < len; i++) {
		cutil_btree_insert(btree, (int)sequence[i]);
	}
}

bool confirm_forward_iteration_char_sequence(cutil_btree* btree, const char* expected_sequence) {
	cutil_btree_itr*  itr = cutil_btree_itr_create(btree);
	int val = 0;
	int count = 0;
	bool ok = true;

	while (cutil_btree_itr_next(itr, &val)) {
		if (expected_sequence[count++] != val) {
			ok = false;
			break;
		}
	}

	cutil_btree_itr_destroy(itr);
	return ok;
}
