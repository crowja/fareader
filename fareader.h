/**
 *  @file fareader.h
 *  @version 0.9.0
 *  @date Sun Feb 16, 2020 07:47:25 PM CST
 *  @copyright 2019-2020 John A. Crow <crowja@gmail.com>
 *  @license Unlicense <http://unlicense.org/>
 */

#ifndef FAREADER_H
#define FAREADER_H

struct fareader;

/**
 *  @brief Create and return a new fareader object.
 *  @param fname Filename to read from, stdin if NULL.
 */
struct fareader *fareader_new(char *fname);

/**
 *  @brief Clean up and free an fareader structure.
 *  @param p Pointer to an fareader object.
 */
void        fareader_free(struct fareader **pp);

/**
 *  @brief Extend the internal read buffer size.
 *  @details There is an internal read buffer, a varstr, for the sequence text
 *  with default size and extension. fareader_bufferesize() provides a way to
 *  modify this directly.
 *  @param p Pointer to an fareader object.
 *  @param size Size of read buffer, if possible.
 *  @param extend Additive length of extension of the buffer.
 */
int         fareader_buffersize(struct fareader *p, unsigned size, unsigned extend);

/**
 * @brief Return the next sequence.
 * @details Return the next sequence from the stream opened by the constructor.
 * @param p Pointer to an fareader object.
 * @param h Pointer to a string representing the FASTA header.
 * @param s Pointer to a string representing the sequence text.
 */
int         fareader_next(struct fareader *p, char **h, char **s);

/**
 *  @brief Return the version of this package.
 *  @details FIXME longer description here ...
 */
const char *fareader_version(void);

#endif
