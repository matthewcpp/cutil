#ifndef CUTIL_TRAIT_H
#define CUTIL_TRAIT_H

/** \file trait.h */

#include <stddef.h>

/**
Trait function for comparing two elements.
For a trait describing a type T, the parameters passed into this function will be of T*.
The function should not modify the data pointed to by the parameters.
\param a Pointer to the first item that is to be compared.
\param b Pointer to the second item that is to be compared.
\param user_data User specified data attached to the trait object.
\returns integer value indicating the result of the comparison.  < 0 indicates that the the first item is less than the second.  0 indicates that the items are equal.  1 indicates that the first item is greater than the second.
*/
typedef int (*cutil_trait_compare_func)(void* a, void* b, void* user_data);

/**
Trait function for destroying an element.
For a trait describing type T, the paramter passed into this function will be of T*.
This function will be called before a container removes an item.
\param item Pointer to the item that will be removed.
\param user_data User specified data attached to the trait object.
*/
typedef void (*cutil_trait_pre_destroy_func)(void* item, void* user_data);

/**
Trait function for copying an element.
For a trait describing type T, the paramter passed into this function will be of T*.
For a trait describing a type T, the src parameter passed into this function will be of T*.
This function will be called when an element is added to a collection.
\param dest the address of memory that src should be copied to.  It's size will be equal to the sie paramter of the trait object.
\param src pointer to the element that should be copied into dest.
\param user_data User specified data attached to the trait object.
*/
typedef void (*cutil_trait_copy_func)(void* dest, void* src, void* user_data);

/**
The trait structure describes an arbitrary type of data.
This structure is used by cntainers to work with generic data.
All members of this structure are optional except for the size.
Some containers require specific functions are defined for a type.
Using an incompatable trait will result in the constructor for the container failing if all required functions are not present.
*/
typedef struct {
	/** Function used for comparing two elements. This member is optional and may be NULL. */
	cutil_trait_compare_func compare_func;

	/** Function used to free any resources that an element may be holding. This member is optional and may be NULL. */
	cutil_trait_pre_destroy_func pre_destroy_func;

	/** Function used to copy an element. This member is optional and may be NULL. */
	cutil_trait_copy_func copy_func;

	/** The size in bytes of a single element. This member is required.  */
	size_t size;

	/** Arbitrary data that will be passed to trait functions. This member is optional. */
	void* user_data;
} cutil_trait;

/**
Returns a trait describing a signed integer
Subsequent calls to this function will return the same trait.
\see cutil_trait_destroy()
*/
cutil_trait* cutil_trait_int();

/**
Returns a trait describing an unsigned integer
Subsequent calls to this function will return the same trait.
\see cutil_trait_destroy()
*/
cutil_trait* cutil_trait_uint();

/**
Returns a trait describing a floating point value
Subsequent calls to this function will return the same trait.
\see cutil_trait_destroy()
*/
cutil_trait* cutil_trait_float();

/**
Returns a trait describing a generic pointer.
Pointed to data will not be copied when inseted into a container.
The caller should take care to free resources consumed by pointed to data before the pointer goes out of scope.
Subsequent calls to this function will return the same trait object.
\see cutil_trait_destroy()
*/
cutil_trait* cutil_trait_ptr();

/**
Returns a trait describing a basic cstring.
cstring data will be copied when a container using this trait receives a new cstring.
When retrieving cstrings from a container, unless explicitly stated, the data is copied and returned as a char*.  It is the responsibility of the caller to free the memory pointed to by the result.
The pointed to string will be freed when the pointer goes out of scope.
Subsequent calls to this function will return the same trait object.
\see cutil_trait_destroy()
*/
cutil_trait* cutil_trait_cstring();


/**
Releases all resources used by the built in default trait functions (cutil_trait_xxx).
User created traits will not be destroyed by this function.
After calling this method all traits that were created using the cutil_trait_xxx functions will be invalid.
*/
void cutil_trait_destroy();

#endif
