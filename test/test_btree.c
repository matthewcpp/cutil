#include "testing.h"
#include "test_suites.h"

#include "btree.h"

#include <stdlib.h>

cutil_btree *g_btree = NULL;

void btree_test() {
	cutil_btree_insert(g_btree, 'A');
	cutil_btree_insert(g_btree, 'C');
	cutil_btree_insert(g_btree, 'G');
	cutil_btree_insert(g_btree, 'N');
	cutil_btree_insert(g_btree, 'H');
	cutil_btree_insert(g_btree, 'E');
	cutil_btree_insert(g_btree, 'K');
	cutil_btree_insert(g_btree, 'Q');
	cutil_btree_insert(g_btree, 'M');
	cutil_btree_insert(g_btree, 'F');
	cutil_btree_insert(g_btree, 'W');
	cutil_btree_insert(g_btree, 'L');
	cutil_btree_insert(g_btree, 'T');
	cutil_btree_insert(g_btree, 'Z');
	cutil_btree_insert(g_btree, 'D');
	cutil_btree_insert(g_btree, 'P');
	cutil_btree_insert(g_btree, 'R');
	cutil_btree_insert(g_btree, 'X');
	cutil_btree_insert(g_btree, 'Y');
	cutil_btree_insert(g_btree, 'S');
}

void btree_before_each() {
	g_btree = cutil_btree_create();
}

void btree_after_each() {
	cutil_btree_destroy(g_btree);
	g_btree = NULL;
}

void add_btree_tests() {
	cutil_testing_suite("btree");
	cutil_testing_suite_before_each(&btree_before_each);
	cutil_testing_suite_after_each(&btree_after_each);

	CUTIL_TESTING_ADD(btree_test);
}