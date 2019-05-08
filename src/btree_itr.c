#include "btree.h"
#include "btree_private.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define ITR_POS_UNINIT INT_MAX

_btree_node*  _itr_find_next_leaf_node(_btree_node* node);
void _itr_set_next_parent_node(cutil_btree_itr* itr);


cutil_btree_itr* cutil_btree_itr_create(cutil_btree* btree) {
    cutil_btree_itr* itr = malloc(sizeof(cutil_btree_itr));
    itr->node = NULL;
    itr->btree = btree;
    itr->node_pos = ITR_POS_UNINIT;

    return itr;
}

void cutil_btree_itr_destroy(cutil_btree_itr* itr) {
    free(itr);
}

void _find_starting_node_pos(cutil_btree_itr* itr) {
    if (cutil_btree_size(itr->btree) > 0) {
        itr->node = _itr_find_next_leaf_node(itr->btree->root);
    }

    itr->node_pos = 0;
}

_btree_node*  _itr_find_next_leaf_node(_btree_node* node) {
    _btree_node* leaf = node;

    while (leaf->branches[0]) {
        leaf = leaf->branches[0];
    }

    return leaf;
}

void _itr_set_next_parent_node(cutil_btree_itr* itr) {
    do {
        itr->node_pos = itr->node->position;
        itr->node = itr->node->parent;
    } while ((itr->node != NULL) && (itr->node_pos >= itr->node->item_count));
}

int cutil_btree_itr_next(cutil_btree_itr* itr) {
    if (itr->node_pos == ITR_POS_UNINIT) {
        if (cutil_btree_size(itr->btree) > 0) {
            _find_starting_node_pos(itr);
            return 1;
        }
        else {
            return 0;
        }
    }
    else {
        itr->node_pos += 1;

        if (_node_is_leaf(itr->node)) {
            /* check if all items in leaf node explored, if so, return to parent */
            if (itr->node_pos >= itr->node->item_count) {
                _itr_set_next_parent_node(itr);
            }
        }
        else {
            /* explore the next branch in this tree */
            if (itr->node->branches[itr->node_pos]) {
                itr->node = _itr_find_next_leaf_node(itr->node->branches[itr->node_pos]);
                itr->node_pos = 0;
            }
            else { /* no branches left, head up */
                _itr_set_next_parent_node(itr);
            }
        }
    }

    if (itr->node) {
        return 1;
    }
    else {
        return 0;
    }
}

int cutil_btree_itr_get_key(cutil_btree_itr* itr, void* key) {
    if (itr->node) {
        void* key_ptr = _node_get_key(itr->node, itr->btree->key_trait, itr->node_pos);
        memcpy(key, key_ptr, itr->btree->key_trait->size);

        return 1;
    }
    else {
        return 0;
    }
}

int cutil_btree_itr_get_value(cutil_btree_itr* itr, void* value) {
    if (itr->node) {
        void* value_ptr = _node_get_value(itr->node, itr->btree->value_trait, itr->node_pos);
        memcpy(value, value_ptr, itr->btree->value_trait->size);
        return 1;
    }
    else {
        return 0;
    }
}
