/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Translation Table Entry (TTE) (TODO).
 *
 */

#ifndef _SPARC64_TTE_H
#define _SPARC64_TTE_H

/*
 * NOTE: I switched from structs to anonyous structs inside unions recently;
 * this makes it possible to convert easily between bitfields and uint64_t.
 */

/* The TTE Tag VA field contains <63:22> of the VA
 * Bits 21 through 13 are not maintained in the tag, since these bits are
 * used to index the smallest direct-mapped TSB of 64 entries.
 */
#define TTE_VA_SHIFT 22

/* SPARCv9 TTE (Translation Table Entry) Tag:
 *
 *  63  62 61 60       48 47   42 41                                             0
 * +---+-----+-----------+-------+-------------------------------------------------+
 * | G |  -  |  CONTEXT  |   -   |               VIRTUAL ADDRESS TAG               |
 * +---+-----+-----------+-------+-------------------------------------------------+
 *    - global (ignore context)
 *    - context identifier
 *    - virtual page number (VPN)
 */

/** I/D TTE Tag. */
typedef union {
  struct {
    uint64_t g      : 1;
    uint64_t        : 2;
    uint64_t ctx    : 13;
    uint64_t        : 6;
    uint64_t va     : 42;
  } __attribute__ ((packed));
  uint64_t value;
} tte_tag;


/* SPARCv9 TTE (Translation Table Entry) Data:
 *
 *  63  62  61  60    59  58   50 49  41 40 13 12   7  6    5   4    3   2   1   0
 * +---+------+-----+----+-------+------+-----+------+---+----+----+---+---+---+---+
 * | V | SIZE | NFO | IE | SOFT2 | DIAG | PA  | SOFT | L | CP | CV | E | P | W | G |
 * +---+------+-----+----+-------+------+-----+------+---+----+----+---+---+---+---+
 *    - valid
 *    - page size (0=8k, 1=64k, 2=512k 3=4M)
 *    - no fault only (0=consitent page, 1=streamable page)
 *    - invert endianness
 *    - software defined field
 *    - diagnostics NOTE: DIAG filed has been removed since US III
 *    - physical page number
 *    - software defined field
 *    - locked
 *    - cachable-in-physically-indexed-cache
 *    - cachable-in-virtually-indexed-cache
 *    - side-effect
 *    - privileged
 *    - writeable
 *    - global
 */

/** I/D TTE Data. */
typedef union {
  struct {
    uint64_t v      : 1;
    uint64_t size   : 2;
    uint64_t nfo    : 1;
    uint64_t ie     : 1;
    uint64_t soft2  : 9;
    uint64_t diag   : 9;
    uint64_t pa     : 28;
    uint64_t soft   : 6;
    uint64_t l      : 1;
    uint64_t cp     : 1;
    uint64_t cv     : 1;
    uint64_t e      : 1;
    uint64_t p      : 1;
    uint64_t w      : 1;
    uint64_t g      : 1;
  } __attribute__ ((packed));
  uint64_t value;
} tte_data;

#endif /* _SPARC64_TTE_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
