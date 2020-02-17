/**
 *  @file varstr.h
 *  @version 3.1.1-dev0
 *  @date Sun Feb 16, 2020 07:32:15 PM CST
 *  @copyright 2018-2020 John A. Crow <crowja@gmail.com>
 *  @license Unlicense <http://unlicense.org/>
 */

#ifndef VARSTR_H
#define VARSTR_H

struct varstr;

/**
 *  @brief Create and return a new varstr object
 *  @returns a pointer to a varstr struct.
 */
struct varstr *varstr_new(void);

/**
 *  @brief Clean up and free a varstr structure
 *  @param[in, out] pp is a reference to a struct varstr * created by varstr_new().
 *    On success set to NULL.
 *  @returns 0.
 */
int         varstr_free(struct varstr **pp);

/**
 *  @brief Return the version of this package
 *  @returns a version string.
 */
const char *varstr_version(void);

/**
 *  @brief Sets parameters for the internal buffer.
 *  @details A varstr has an internal buffer that gets extended as needed. This
 *    method allows the user to modify the parameters controlling its current 
 *    size and its current (additive) extension. Typically there is no need to
 *    do this.
 *  @param[in, out] p is a struct varstr * created by varstr_new().
 *  @param[in] size. Attempt to set the internal buffer size to this. If the internal
 *    buffer requires more than this, it will retain its current size.
 *  @param[in] extend. Change the internal buffer (additive) extension to this.
 *  @returns 0 if successful, 1 otherwise.
 */
int         varstr_buffersize(struct varstr *p, unsigned size, unsigned extend);

/**
 *  @brief Concatenate a string to the end of a varstr object.
 *  @param[in, out] p is a struct varstr * created by varstr_new().
 *  @param[in] x is a string to concatenate.
 */
void        varstr_cat(struct varstr *p, char *x);

/**
 *  @brief Concatenate a character to the end of a varstr object.
 *  @param[in, out] p is a struct varstr * created by varstr_new().
 *  @param x FIXME
 */
void        varstr_catc(struct varstr *p, char x);

/**
 *  @brief Remove whitespace from the end of a varstr object.
 *  @param[in, out] p is a struct varstr * created by varstr_new().
 */
void        varstr_chomp(struct varstr *p);

/**
 *  @brief Remove all whitespace from a varstr.
 *  @param[in, out] p is a struct varstr * created by varstr_new().
 */
void        varstr_compact(struct varstr *p);

/**
 *  @brief Reset to the emptry varstr.
 *  @details Reset the varstr so that it represents the empty string.
 *  @param[in, out] p is a struct varstr * created by varstr_new().
 */
void        varstr_empty(struct varstr *p);

/**
 *  @brief Remove whitespace from both ends.
 *  @details Remove whitespace from the beginning and end of the string
 *    represented by a varstr.
 *  @param[in, out] p is a struct varstr * created by varstr_new().
 */
void        varstr_lrtrim(struct varstr *p);

void        varstr_ltrim(struct varstr *p);

/**
 *  @brief Set the extension size for a varstr object
 *  @param[in, out] p is a struct varstr * created by varstr_new().
 *  @param[in] extend is the size of the (additive) extension used when
 *    resizing the internal buffer.
 *  @returns the size of the new extension.
 */
unsigned    varstr_extend(struct varstr *p, unsigned extend);

/**
 *  @brief Initialize a varstr object
 *  @details Same as varstr_extend().
 */
unsigned    varstr_init(struct varstr *p, unsigned extend);

void        varstr_rtrim(struct varstr *p);

/**
 *  @brief Get the pointer to the internal buffer holding the string.
 *  @param[in] p is a struct varstr * created by varstr_new().
 *  @returns a pointer to the internal char * buffer. This should not be modified.
 */
char       *varstr_str(struct varstr *p);

/**
 *  @brief Get the string represented by the varstr.
 *  @details Similar to varstr_str() except a copy of the string is allocated
 *    and must be deallocated by the user. This prevents corruption of the 
 *    internals.
 *  @param[in] p is a struct varstr * created by varstr_new().
 *  @returns a copy of the string represented by the varstr.
 */
char       *varstr_to_s(struct varstr *p);

void        varstr_tolower(struct varstr *p);
void        varstr_toupper(struct varstr *p);

#endif
