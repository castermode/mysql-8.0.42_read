/*****************************************************************************

Copyright (c) 2014, 2025, Oracle and/or its affiliates.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2.0, as published by the
Free Software Foundation.

This program is designed to work with certain software (including
but not limited to OpenSSL) that is licensed under separate terms,
as designated in a particular file or component or in included license
documentation.  The authors of MySQL hereby grant you an additional
permission to link the program and your derivative works with the
separately licensed software that they have either included with
the program or referenced in the documentation.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License, version 2.0,
for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

*****************************************************************************/

/** @file include/gis0rtree.h
 R-tree header file

 Created 2013/03/27 Jimmy Yang and Allen Lai
 ***********************************************************************/

#ifndef gis0rtree_h
#define gis0rtree_h

#include "univ.i"

#include "btr0cur.h"
#include "btr0types.h"
#include "data0type.h"
#include "data0types.h"
#include "dict0types.h"
#include "gis0geo.h"
#include "gis0type.h"
#include "hash0hash.h"
#include "mem0mem.h"
#include "page0page.h"
#include "que0types.h"
#include "rem0types.h"
#include "row0types.h"
#include "trx0types.h"
#include "ut0vec.h"
#include "ut0wqueue.h"

/* Define it for rtree search mode checking. */
static inline bool RTREE_SEARCH_MODE(page_cur_mode_t mode) {
  return mode >= PAGE_CUR_CONTAIN && mode <= PAGE_CUR_RTREE_GET_FATHER;
}

/* Geometry data header */
constexpr uint32_t GEO_DATA_HEADER_SIZE = 4;
/** Builds a Rtree node pointer out of a physical record and a page number.
@return own: node pointer
@param[in] index index
@param[in] mbr mbr of lower page
@param[in] rec record for which to build node pointer
@param[in] page_no page number to put in node pointer
@param[in] heap memory heap where pointer created */
dtuple_t *rtr_index_build_node_ptr(const dict_index_t *index,
                                   const rtr_mbr_t *mbr, const rec_t *rec,
                                   page_no_t page_no, mem_heap_t *heap);

/** Splits an R-tree index page to halves and inserts the tuple. It is assumed
 that mtr holds an x-latch to the index tree. NOTE: the tree x-latch is
 released within this function! NOTE that the operation of this
 function must always succeed, we cannot reverse it: therefore enough
 free disk space (2 pages) must be guaranteed to be available before
 this function is called.
 @return inserted record */
rec_t *rtr_page_split_and_insert(
    uint32_t flags,        /*!< in: undo logging and locking flags */
    btr_cur_t *cursor,     /*!< in/out: cursor at which to insert; when the
                           function returns, the cursor is positioned
                           on the predecessor of the inserted record */
    ulint **offsets,       /*!< out: offsets on inserted record */
    mem_heap_t **heap,     /*!< in/out: pointer to memory heap, or NULL */
    const dtuple_t *tuple, /*!< in: tuple to insert */
    mtr_t *mtr);           /*!< in: mtr */

/** Sets the child node mbr in a node pointer.
@param[in]      index   index
@param[in]      block   buffer block
@param[out]     mbr     MBR encapsulates the page
@param[in]      heap    heap for the memory allocation */
static inline void rtr_page_cal_mbr(const dict_index_t *index,
                                    const buf_block_t *block, rtr_mbr_t *mbr,
                                    mem_heap_t *heap);

/** Find the next matching record. This function will first exhaust
the copied record listed in the rtr_info->matches vector before
moving to next page
@param[in]      tuple           Data tuple; NOTE: n_fields_cmp in tuple
                                must be set so that it cannot get compared
                                to the node ptr page number field!
@param[in]      mode            Cursor search mode
@param[in]      sel_mode        Select mode: SELECT_ORDINARY,
                                SELECT_SKIP_LOKCED, or SELECT_NO_WAIT
@param[in]      cursor          Persistent cursor; NOTE that the function
                                may release the page latch
@param[in]      cur_level       Current level
@param[in]      mtr             Mini-transaction
@return true if there is next qualified record found, otherwise(if
exhausted) false */
bool rtr_pcur_move_to_next(const dtuple_t *tuple, page_cur_mode_t mode,
                           select_mode sel_mode, btr_pcur_t *cursor,
                           ulint cur_level, mtr_t *mtr);

/** Searches the right position in rtree for a page cursor.
@param[in] block Buffer block
@param[in] index Index descriptor
@param[in] tuple Data tuple
@param[in] mode Page_cur_l, page_cur_le, page_cur_g, or page_cur_ge
@param[in,out] cursor Page cursor
@param[in,out] rtr_info Search stack */
bool rtr_cur_search_with_match(const buf_block_t *block, dict_index_t *index,
                               const dtuple_t *tuple, page_cur_mode_t mode,
                               page_cur_t *cursor, rtr_info_t *rtr_info);

/** Calculate the area increased for a new record
@return area increased */
double rtr_rec_cal_increase(
    const dtuple_t *dtuple, /*!< in: data tuple to insert, which
                            cause area increase */
    const rec_t *rec,       /*!< in: physical record which differs from
                            dtuple in some of the common fields, or which
                            has an equal number or more fields than
                            dtuple */
    const ulint *offsets,   /*!< in: array returned by rec_get_offsets() */
    double *area,           /*!< out: increased area */
    const dd::Spatial_reference_system *srs); /*!< in: SRS of R-tree */

/** Following the right link to find the proper block for insert.
@return the proper block.
@param[in] btr_cur btr cursor
@param[in] mtr mtr */
dberr_t rtr_ins_enlarge_mbr(btr_cur_t *btr_cur, mtr_t *mtr);

/**                                                                        */
void rtr_get_father_node(
    dict_index_t *index,   /*!< in: index */
    ulint level,           /*!< in: the tree level of search */
    const dtuple_t *tuple, /*!< in: data tuple; NOTE: n_fields_cmp in
                           tuple must be set so that it cannot get
                           compared to the node ptr page number field! */
    btr_cur_t *sea_cur,    /*!< in: search cursor */
    btr_cur_t *cursor,     /*!< in/out: tree cursor; the cursor page is
                           s- or x-latched */
    page_no_t page_no,     /*!< in: current page no */
    mtr_t *mtr);           /*!< in: mtr */

/** Push a nonleaf index node to the search path
@param[in,out]  path            search path
@param[in]      pageno          pageno to insert
@param[in]      seq_no          Node sequence num
@param[in]      level           index level
@param[in]      child_no        child page no
@param[in]      cursor          position cursor
@param[in]      mbr_inc         MBR needs to be enlarged */
static inline void rtr_non_leaf_stack_push(rtr_node_path_t *path,
                                           page_no_t pageno, node_seq_t seq_no,
                                           ulint level, page_no_t child_no,
                                           btr_pcur_t *cursor, double mbr_inc);

/** Push a nonleaf index node to the search path for insertion
@param[in]      index   index descriptor
@param[in,out]  path    search path
@param[in]      level   index level
@param[in]      block   block of the page
@param[in]      rec     positioned record
@param[in]      mbr_inc MBR needs to be enlarged */
void rtr_non_leaf_insert_stack_push(dict_index_t *index, rtr_node_path_t *path,
                                    ulint level, const buf_block_t *block,
                                    const rec_t *rec, double mbr_inc);

/** Allocates a new Split Sequence Number.
 @return new SSN id */
static inline node_seq_t rtr_get_new_ssn_id(
    dict_index_t *index); /*!< in: the index struct */

/** Get the current Split Sequence Number.
 @return current SSN id */
static inline node_seq_t rtr_get_current_ssn_id(
    dict_index_t *index); /*!< in/out: the index struct */

/** Create a RTree search info structure
@param[in] need_prdt Whether predicate lock is needed
@param[in] init_matches Whether to initiate the "matches" structure for
collecting matched leaf records
@param[in] cursor Tree search cursor
@param[in] index Index struct */
rtr_info_t *rtr_create_rtr_info(bool need_prdt, bool init_matches,
                                btr_cur_t *cursor, dict_index_t *index);

/** Update a btr_cur_t with rtr_info
@param[in,out] cursor Tree cursor
@param[in] rtr_info Rtr_info to set to the cursor */
void rtr_info_update_btr(btr_cur_t *cursor, rtr_info_t *rtr_info);

/** Update a btr_cur_t with rtr_info */
void rtr_init_rtr_info(
    /****************/
    rtr_info_t *rtr_info, /*!< in: rtr_info to set to the
                          cursor */
    bool need_prdt,       /*!< in: Whether predicate lock is
                          needed */
    btr_cur_t *cursor,    /*!< in: tree search cursor */
    dict_index_t *index,  /*!< in: index structure */
    bool reinit);         /*!< in: Whether this is a reinit */

/** Clean up Rtree cursor */
void rtr_clean_rtr_info(rtr_info_t *rtr_info, /*!< in: RTree search info */
                        bool free_all); /*!< in: need to free rtr_info itself */

/** Get the bounding box content from an index record
@param[in] rec Data tuple
@param[in] offsets Offsets array
@param[out] mbr Mbr */
void rtr_get_mbr_from_rec(const rec_t *rec, const ulint *offsets,
                          rtr_mbr_t *mbr);

/** Get the bounding box content from a MBR data record
@param[in] dtuple Data tuple
@param[out] mbr Mbr to fill */
void rtr_get_mbr_from_tuple(const dtuple_t *dtuple, rtr_mbr *mbr);

/* Get the rtree page father.
@param[in]      offsets         work area for the return value
@param[in]      index           rtree index
@param[in]      block           child page in the index
@param[in]      mtr             mtr
@param[in]      sea_cur         search cursor, contains information
                                about parent nodes in search
@param[in]      cursor          cursor on node pointer record,
                                its page x-latched */
void rtr_page_get_father(dict_index_t *index, buf_block_t *block, mtr_t *mtr,
                         btr_cur_t *sea_cur, btr_cur_t *cursor);

/** Returns the father block to a page. It is assumed that mtr holds
 an X or SX latch on the tree.
 @return rec_get_offsets() of the node pointer record */
ulint *rtr_page_get_father_block(
    ulint *offsets,      /*!< in: work area for the return value */
    mem_heap_t *heap,    /*!< in: memory heap to use */
    dict_index_t *index, /*!< in: b-tree index */
    buf_block_t *block,  /*!< in: child page in the index */
    mtr_t *mtr,          /*!< in: mtr */
    btr_cur_t *sea_cur,  /*!< in: search cursor, contains information
                         about parent nodes in search */
    btr_cur_t *cursor);  /*!< out: cursor on node pointer record,
                         its page x-latched */
/** Store the parent path cursor
 @return number of cursor stored */
ulint rtr_store_parent_path(
    const buf_block_t *block, /*!< in: block of the page */
    btr_cur_t *btr_cur,       /*!< in/out: persistent cursor */
    ulint latch_mode,
    /*!< in: latch_mode */
    ulint level, /*!< in: index level */
    mtr_t *mtr); /*!< in: mtr */

/** Initializes and opens a persistent cursor to an index tree. It should be
 closed with btr_pcur::close. Mainly called by row_search_index_entry()
 @param[in] index index
 @param[in] level level in the btree
 @param[in] tuple tuple on which search done
 @param[in] mode PAGE_CUR_L, ...; NOTE that if the search is made using a unique
 prefix of a record, mode should be PAGE_CUR_LE, not PAGE_CUR_GE, as the latter
 may end up on the previous page from the record!
 @param[in] latch_mode BTR_SEARCH_LEAF, ...
 @param[in] cursor memore buffer for persistent cursor
 @param[in] location location where called
 @param[in] mtr mtr */
void rtr_pcur_open_low(dict_index_t *index, ulint level, const dtuple_t *tuple,
                       page_cur_mode_t mode, ulint latch_mode,
                       btr_pcur_t *cursor, ut::Location location, mtr_t *mtr);

static inline void rtr_pcur_open(dict_index_t *i, const dtuple_t *t,
                                 page_cur_mode_t md, ulint l, btr_pcur_t *c,
                                 ut::Location loc, mtr_t *m) {
  rtr_pcur_open_low(i, 0, t, md, l, c, loc, m);
}

struct btr_cur_t;

/** Returns the R-Tree node stored in the parent search path
@param[in]      btr_cur         persistent cursor
@param[in]      level           index level of buffer page
@param[in]      is_insert       whether insert operation
@return pointer to R-Tree cursor component */
static inline node_visit_t *rtr_get_parent_node(btr_cur_t *btr_cur, ulint level,
                                                ulint is_insert);

/** Returns the R-Tree cursor stored in the parent search path
@param[in]      btr_cur         persistent cursor
@param[in]      level           index level of buffer page
@param[in]      is_insert       whether insert operation
@return pointer to R-Tree cursor component */
static inline btr_pcur_t *rtr_get_parent_cursor(btr_cur_t *btr_cur, ulint level,
                                                ulint is_insert);

/** Copy recs from a page to new_block of rtree.
Differs from page_copy_rec_list_end, because this function does not
touch the lock table and max trx id on page or compress the page.

IMPORTANT: The caller will have to update IBUF_BITMAP_FREE
if new_block is a compressed leaf page in a secondary index.
This has to be done either within the same mini-transaction,
or by invoking ibuf_reset_free_bits() before mtr_commit().

@param[in] new_block Index page to copy to
@param[in] block Index page of rec
@param[in] rec Record on page
@param[in] index Record descriptor
@param[in,out] heap Heap memory
@param[in] rec_move Recording records moved
@param[in] max_move Num of rec to move
@param[out] num_moved Num of rec to move
@param[in] mtr Mini-transaction */
void rtr_page_copy_rec_list_end_no_locks(buf_block_t *new_block,
                                         buf_block_t *block, rec_t *rec,
                                         dict_index_t *index, mem_heap_t *heap,
                                         rtr_rec_move_t *rec_move,
                                         ulint max_move, ulint *num_moved,
                                         mtr_t *mtr);

/** Copy recs till a specified rec from a page to new_block of rtree.
@param[in] new_block Index page to copy to
@param[in] block Index page of rec
@param[in] rec Record on page
@param[in] index Record descriptor
@param[in,out] heap Heap memory
@param[in] rec_move Recording records moved
@param[in] max_move Num of rec to move
@param[out] num_moved Num of rec to move
@param[in] mtr Mini-transaction */
void rtr_page_copy_rec_list_start_no_locks(
    buf_block_t *new_block, buf_block_t *block, rec_t *rec, dict_index_t *index,
    mem_heap_t *heap, rtr_rec_move_t *rec_move, ulint max_move,
    ulint *num_moved, mtr_t *mtr);

/** Merge 2 mbrs and update the the mbr that cursor is on.
@param[in,out] cursor Cursor
@param[in] cursor2 The other cursor
@param[in] offsets Rec offsets
@param[in] offsets2 Rec offsets
@param[in] child_page The child page.
@param[in] mtr Mini-transaction */
dberr_t rtr_merge_and_update_mbr(btr_cur_t *cursor, btr_cur_t *cursor2,
                                 ulint *offsets, ulint *offsets2,
                                 page_t *child_page, mtr_t *mtr);

/** Deletes on the upper level the node pointer to a page.
@param[in] sea_cur Search cursor, contains information about parent nodes in
search
@param[in] mtr Mini-transaction
*/
void rtr_node_ptr_delete(btr_cur_t *sea_cur, mtr_t *mtr);

/** Check two MBRs are identical or need to be merged
@param[in] cursor Cursor
@param[in] cursor2 The other cursor
@param[in] offsets Rec offsets
@param[in] offsets2 Rec offsets
@param[out] new_mbr Mbr to update */
bool rtr_merge_mbr_changed(btr_cur_t *cursor, btr_cur_t *cursor2,
                           ulint *offsets, ulint *offsets2, rtr_mbr_t *new_mbr);

/** Update the mbr field of a spatial index row.
 @return true if successful */
bool rtr_update_mbr_field(
    btr_cur_t *cursor,  /*!< in: cursor pointed to rec.*/
    ulint *offsets,     /*!< in: offsets on rec. */
    btr_cur_t *cursor2, /*!< in/out: cursor pointed to rec
                        that should be deleted.
                        this cursor is for btr_compress to
                        delete the merged page's father rec.*/
    page_t *child_page, /*!< in: child page. */
    rtr_mbr_t *new_mbr, /*!< in: the new mbr. */
    rec_t *new_rec,     /*!< in: rec to use */
    mtr_t *mtr);        /*!< in: mtr */

/** Check whether a Rtree page is child of a parent page
 @return true if there is child/parent relationship */
bool rtr_check_same_block(
    dict_index_t *index,  /*!< in: index tree */
    btr_cur_t *cur,       /*!< in/out: position at the parent entry
                          pointing to the child if successful */
    buf_block_t *parentb, /*!< in: parent page to check */
    buf_block_t *childb,  /*!< in: child Page */
    mem_heap_t *heap);    /*!< in: memory heap */

/** Sets pointer to the data and length in a field.
@param[out]     data    data
@param[in]      mbr     data */
static inline void rtr_write_mbr(byte *data, const rtr_mbr_t *mbr);

/** Sets pointer to the data and length in a field.
@param[in]      data    data
@param[out]     mbr     data */
static inline void rtr_read_mbr(const byte *data, rtr_mbr_t *mbr);

/** Check whether a discarding page is in anyone's search path
@param[in] index Index
@param[in,out] cursor Cursor on the page to discard: not on the root page
@param[in] block Block of page to be discarded */
void rtr_check_discard_page(dict_index_t *index, btr_cur_t *cursor,
                            buf_block_t *block);

/** Reinitialize a RTree search info
@param[in,out]  cursor          tree cursor
@param[in]      index           index struct
@param[in]      need_prdt       Whether predicate lock is needed */
static inline void rtr_info_reinit_in_cursor(btr_cur_t *cursor,
                                             dict_index_t *index,
                                             bool need_prdt);

/** Estimates the number of rows in a given area.
@param[in]      index   index
@param[in]      tuple   range tuple containing mbr, may also be empty tuple
@param[in]      mode    search mode
@return estimated number of rows */
int64_t rtr_estimate_n_rows_in_range(dict_index_t *index, const dtuple_t *tuple,
                                     page_cur_mode_t mode);

#include "gis0rtree.ic"
#endif
