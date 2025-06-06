#ifndef ITEM_SUBSELECT_INCLUDED
#define ITEM_SUBSELECT_INCLUDED

/* Copyright (c) 2002, 2025, Oracle and/or its affiliates.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is designed to work with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have either included with
   the program or referenced in the documentation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/* subselect Item */

#include <assert.h>
#include <sys/types.h>

#include <cstddef>
#include <memory>  // unique_ptr
#include <vector>

#include "field_types.h"  // enum_field_types
#include "my_alloc.h"     // Destroy_only

#include "my_inttypes.h"
#include "my_table_map.h"
#include "my_time.h"
#include "mysql/udf_registration_types.h"
#include "mysql_time.h"
#include "sql/comp_creator.h"
#include "sql/enum_query_type.h"
#include "sql/item.h"                    // Item_result_field
#include "sql/iterators/row_iterator.h"  // IWYU pragma: keep
#include "sql/parse_location.h"          // POS
#include "sql/parse_tree_node_base.h"
#include "sql/sql_const.h"
#include "sql/sql_opt_exec_shared.h"
#include "template_utils.h"

class Comp_creator;
class Field;
class Item_func_not_all;
class Item_in_optimizer;
class JOIN;
class Json_wrapper;
class PT_subquery;
class Query_result_interceptor;
class Query_result_subquery;
class Query_result_union;
class Query_block;
class Query_expression;
class String;
class SubqueryWithResult;
class THD;
class Temp_table_param;
class my_decimal;
class subselect_indexsubquery_engine;
struct AccessPath;
class Table_ref;

template <class T>
class List;

/* base class for subselects */

class Item_subselect : public Item_result_field {
  typedef Item_result_field super;

 private:
  bool value_assigned; /* value already assigned to subselect */
  /**
      Whether or not execution of this subselect has been traced by
      optimizer tracing already. If optimizer trace option
      REPEATED_SUBSELECT is disabled, this is used to disable tracing
      after the first one.
  */
  bool traced_before;

 public:
  /*
    Used inside Item_subselect::fix_fields() according to this scenario:
      > Item_subselect::fix_fields
        > subquery->prepare
          > query_block->prepare
            (Here we realize we need to do the rewrite and set
             substitution= some new Item, eg. Item_in_optimizer )
          < query_block->prepare
        < subquery->prepare
        *ref= substitution;
      < Item_subselect::fix_fields
  */
  Item *substitution;

  /* unit of subquery */
  Query_expression *unit;
  /**
     If !=NO_PLAN_IDX: this Item is in the condition attached to the JOIN_TAB
     having this index in the parent JOIN.
  */
  int in_cond_of_tab;

  // For EXPLAIN.
  enum enum_engine_type { OTHER_ENGINE, INDEXSUBQUERY_ENGINE, HASH_SJ_ENGINE };
  enum_engine_type engine_type() const;

  // For EXPLAIN. Only valid if engine_type() == HASH_SJ_ENGINE.
  const TABLE *get_table() const;
  const Index_lookup &index_lookup() const;
  join_type get_join_type() const;

  void create_iterators(THD *thd);
  virtual AccessPath *root_access_path() const { return nullptr; }

 protected:
  /**
    We need this method, because some compilers do not allow 'this'
    pointer in constructor initialization list, but we need to pass a pointer
    to subselect Item class to Query_result_interceptor's constructor.
  */
  void init(Query_block *select, Query_result_subquery *result);

  /// The inner part of the subquery.
  unique_ptr_destroy_only<SubqueryWithResult> subquery;

  /// Only relevant for Item_in_subselect; optimized structure used for
  /// execution in place of running the entire subquery.
  subselect_indexsubquery_engine *indexsubquery_engine = nullptr;

  /// cache of used tables
  table_map used_tables_cache;
  /// allowed number of columns (1 for scalar subqueries)
  uint max_columns;
  /// where subquery is placed
  enum_parsing_context parsing_place;
  /// work with 'substitution'
  bool have_to_be_excluded;

 public:
  /// cache of used tables, used to identify tables added for IN subquery
  table_map m_subquery_used_tables{0};
  /// subquery is transformed
  bool changed;

  enum subs_type {
    UNKNOWN_SUBS,
    SINGLEROW_SUBS,
    EXISTS_SUBS,
    IN_SUBS,
    ALL_SUBS,
    ANY_SUBS
  };

  Item_subselect();
  explicit Item_subselect(const POS &pos);

 private:
  /// Accumulate properties from underlying query expression
  void accumulate_properties();
  /// Accumulate properties from underlying query block
  void accumulate_properties(Query_block *select);
  /// Accumulate properties from a selected expression within a query block.
  void accumulate_expression(Item *item);
  /// Accumulate properties from a condition or GROUP/ORDER within a query
  /// block.
  void accumulate_condition(Item *item);

 public:
  /// Accumulate used tables
  void accumulate_used_tables(table_map add_tables) {
    used_tables_cache |= add_tables;
    m_subquery_used_tables |= add_tables;
  }

  virtual subs_type substype() const { return UNKNOWN_SUBS; }

  void cleanup() override;
  virtual void reset() { null_value = true; }
  virtual bool select_transformer(THD *thd, Query_block *select) = 0;
  bool assigned() const { return value_assigned; }
  void assigned(bool a) { value_assigned = a; }
  enum Type type() const override;
  bool is_null() override { return update_null_value() || null_value; }
  bool fix_fields(THD *thd, Item **ref) override;
  void fix_after_pullout(Query_block *parent_query_block,
                         Query_block *removed_query_block) override;
  virtual bool exec(THD *thd);
  bool resolve_type(THD *) override;
  table_map used_tables() const override { return used_tables_cache; }
  table_map not_null_tables() const override { return 0; }
  Item *get_tmp_table_item(THD *thd) override;
  void update_used_tables() override;
  void print(const THD *thd, String *str,
             enum_query_type query_type) const override;

  void set_indexsubquery_engine(subselect_indexsubquery_engine *eng) {
    indexsubquery_engine = eng;
  }

  /*
    True if this subquery has been already evaluated. Implemented only for
    single select and union subqueries only.
  */
  bool is_evaluated() const;
  bool is_uncacheable() const;

  /*
    Used by max/min subquery to initialize value presence registration
    mechanism. Engine call this method before rexecution query.
  */
  virtual void reset_value_registration() {}
  enum_parsing_context place() { return parsing_place; }
  bool walk(Item_processor processor, enum_walk walk, uchar *arg) override;
  bool explain_subquery_checker(uchar **arg) override;
  bool inform_item_in_cond_of_tab(uchar *arg) override;
  bool clean_up_after_removal(uchar *arg) override;

  const char *func_name() const override {
    assert(0);
    return "subselect";
  }

  bool check_function_as_value_generator(uchar *args) override {
    Check_function_as_value_generator_parameters *func_arg =
        pointer_cast<Check_function_as_value_generator_parameters *>(args);
    func_arg->err_code = func_arg->get_unnamed_function_error_code();
    return true;
  }

  /// argument used by walk method collect_scalar_subqueries ("css")
  struct Collect_subq_info {
    ///< accumulated all subq (or aggregates) found
    std::vector<Item_subselect *> list;
    Query_block *m_query_block{nullptr};
    Collect_subq_info(Query_block *owner) : m_query_block(owner) {}
    bool contains(Query_expression *candidate) {
      for (auto sq : list) {
        if (sq->unit == candidate) return true;
      }
      return false;
    }
  };

  bool collect_subqueries(uchar *) override;
  Item *replace_item_field(uchar *arg) override;
  Item *replace_item_view_ref(uchar *arg) override;
  Item *replace_item(Item_transformer t, uchar *arg);

  friend class Query_result_interceptor;
  friend class Item_in_optimizer;
  friend bool Item_field::fix_fields(THD *, Item **);
  friend int Item_field::fix_outer_field(THD *, Field **, Item **);
  friend bool Item_ref::fix_fields(THD *, Item **);
  friend void Item_ident::fix_after_pullout(Query_block *parent_query_block,
                                            Query_block *removed_query_block);

 private:
  bool subq_opt_away_processor(uchar *arg) override;

 protected:
  uint unit_cols() const;
};

/* single value subselect */

class Item_singlerow_subselect : public Item_subselect {
 protected:
  Item_cache *value, **row;
  bool no_rows;  ///< @c no_rows_in_result
 public:
  Item_singlerow_subselect(Query_block *query_block);
  Item_singlerow_subselect()
      : Item_subselect(), value(nullptr), row(nullptr), no_rows(false) {}

  void cleanup() override;
  subs_type substype() const override { return SINGLEROW_SUBS; }

  void reset() override;
  bool select_transformer(THD *thd, Query_block *select) override;
  void store(uint i, Item *item);
  double val_real() override;
  longlong val_int() override;
  String *val_str(String *) override;
  my_decimal *val_decimal(my_decimal *) override;
  bool val_json(Json_wrapper *result) override;
  bool get_date(MYSQL_TIME *ltime, my_time_flags_t fuzzydate) override;
  bool get_time(MYSQL_TIME *ltime) override;
  bool val_bool() override;
  enum Item_result result_type() const override;
  bool resolve_type(THD *) override;

  /*
    Mark the subquery as having no rows.
    If there are aggregate functions (in the outer query),
    we need to generate a NULL row. @c return_zero_rows().
  */
  void no_rows_in_result() override;

  uint cols() const override { return unit_cols(); }

  /**
    @note that this returns the i-th element of the SELECT list.
    To check for nullability, look at this->maybe_null and not
    element_index[i]->maybe_null, since the selected expressions are
    always NULL if the subquery is empty.
  */
  Item *element_index(uint i) override {
    return reinterpret_cast<Item *>(row[i]);
  }
  Item **addr(uint i) override { return (Item **)row + i; }
  bool check_cols(uint c) override;
  bool null_inside() override;
  void bring_value() override;

  bool collect_scalar_subqueries(uchar *) override;
  virtual bool is_maxmin() const { return false; }

  /**
    Argument for walk method replace_scalar_subquery
  */
  struct Scalar_subquery_replacement {
    Item_singlerow_subselect *m_target;  ///< subquery to be replaced with field
    Field *m_field;                      ///< the replacement field
    Query_block *m_outer_query_block;    ///< The transformed query block.
    Query_block *m_inner_query_block;    ///< The immediately surrounding query
                                       ///< block. This will be the transformed
                                       ///< block or a subquery of it
    bool m_add_coalesce{false};
    Scalar_subquery_replacement(Item_singlerow_subselect *target, Field *field,
                                Query_block *select, bool add_coalesce)
        : m_target(target),
          m_field(field),
          m_outer_query_block(select),
          m_inner_query_block(select),
          m_add_coalesce(add_coalesce) {}
  };

  Item *replace_scalar_subquery(uchar *arge) override;
  /**
    This method is used to implement a special case of semantic tree
    rewriting, mandated by a SQL:2003 exception in the specification.
    The only caller of this method is handle_sql2003_note184_exception(),
    see the code there for more details.
    Note that this method breaks the object internal integrity, by
    removing it's association with the corresponding Query_block,
    making this object orphan from the parse tree.
    No other method, beside the destructor, should be called on this
    object, as it is now invalid.
    @return the Query_block structure that was given in the constructor.
  */
  Query_block *invalidate_and_restore_query_block();
  std::optional<ContainedSubquery> get_contained_subquery(
      const Query_block *outer_query_block) override;
  friend class Query_result_scalar_subquery;
};

/* used in static ALL/ANY optimization */
class Item_maxmin_subselect final : public Item_singlerow_subselect {
 protected:
  bool max;
  bool was_values;  // Set if we have found at least one row
 public:
  Item_maxmin_subselect(Item_subselect *parent, Query_block *query_block,
                        bool max, bool ignore_nulls);
  void print(const THD *thd, String *str,
             enum_query_type query_type) const override;
  void cleanup() override;
  bool any_value() { return was_values; }
  void register_value() { was_values = true; }
  void reset_value_registration() override { was_values = false; }
  bool is_maxmin() const override { return true; }
};

/* exists subselect */

/**
  Strategy which will be used to handle this subquery: flattening to a
  semi-join, conversion to a derived table, rewrite of IN to EXISTS...
  Sometimes the strategy is first only a candidate, then the real decision
  happens in a second phase. Other times the first decision is final.
 */
enum class Subquery_strategy : int {
  /// Nothing decided yet
  UNSPECIFIED,
  /// Candidate for rewriting IN(subquery) to EXISTS, or subquery
  /// materialization
  CANDIDATE_FOR_IN2EXISTS_OR_MAT,
  /// Candidate for semi-join flattening
  CANDIDATE_FOR_SEMIJOIN,
  /// Candidate for rewriting to joined derived table
  CANDIDATE_FOR_DERIVED_TABLE,
  /// Semi-join flattening
  SEMIJOIN,
  /// Rewrite to joined derived table
  DERIVED_TABLE,
  /// Evaluate as EXISTS subquery (possibly after rewriting from another type)
  SUBQ_EXISTS,
  /// Subquery materialization (HASH_SJ_ENGINE)
  SUBQ_MATERIALIZATION,
  /// Subquery has been deleted, probably because it was always false
  DELETED,
};

class Item_exists_subselect : public Item_subselect {
  typedef Item_subselect super;

 protected:
  /// value of this item (boolean: exists/not-exists)
  bool value{false};

 public:
  /// Priority of this predicate in the convert-to-semi-join-nest process.
  int sj_convert_priority{0};
  /// Execution strategy chosen for this Item
  Subquery_strategy strategy{Subquery_strategy::UNSPECIFIED};
  /// Used by the transformation to derived table
  enum_condition_context outer_condition_context{enum_condition_context::ANDS};

  /**
    Used by subquery optimizations to keep track about where this subquery
    predicate is located, and whether it is a candidate for transformation.
      (Table_ref*) 1 - the predicate is an AND-part of the WHERE
      join nest pointer    - the predicate is an AND-part of ON expression
                             of a join nest
      NULL                 - for all other locations. It also means that the
                             predicate is not a candidate for transformation.
    See also THD::emb_on_expr_nest.

    As for the second case above (the join nest pointer), note that this value
    may change if scalar subqueries are transformed to derived tables,
    cf. transform_scalar_subqueries_to_join_with_derived, due to the need to
    build new join nests. The change is performed in Query_block::nest_derived.
  */
  Table_ref *embedding_join_nest{nullptr};

  Item_exists_subselect(Query_block *select);

  Item_exists_subselect() : Item_subselect() {}

  explicit Item_exists_subselect(const POS &pos) : super(pos) {}

  void notify_removal() override { strategy = Subquery_strategy::DELETED; }

  bool select_transformer(THD *, Query_block *) override {
    strategy = Subquery_strategy::SUBQ_EXISTS;
    return false;
  }
  subs_type substype() const override { return EXISTS_SUBS; }
  bool is_bool_func() const override { return true; }
  void reset() override { value = false; }

  enum Item_result result_type() const override { return INT_RESULT; }
  /*
    The item is
    ([NOT] IN/EXISTS) [ IS [NOT] TRUE|FALSE ]
  */
  enum Bool_test value_transform = BOOL_IDENTITY;
  bool with_is_op() const {
    switch (value_transform) {
      case BOOL_IS_TRUE:
      case BOOL_IS_FALSE:
      case BOOL_NOT_TRUE:
      case BOOL_NOT_FALSE:
        return true;
      default:
        return false;
    }
  }
  /// True if the IS TRUE/FALSE wasn't explicit in the query
  bool implicit_is_op = false;
  Item *truth_transformer(THD *, enum Bool_test test) override;
  bool translate(bool &null_v, bool v);
  void apply_is_true() override {
    bool had_is = with_is_op();
    truth_transformer(nullptr, BOOL_IS_TRUE);
    if (!had_is && value_transform == BOOL_IS_TRUE)
      implicit_is_op = true;  // needn't be written by EXPLAIN
  }
  /// True if the Item has decided that it can do antijoin
  bool can_do_aj = false;
  bool choose_semijoin_or_antijoin();
  longlong val_int() override;
  double val_real() override;
  String *val_str(String *) override;
  my_decimal *val_decimal(my_decimal *) override;
  bool val_bool() override;
  bool get_date(MYSQL_TIME *ltime, my_time_flags_t fuzzydate) override {
    return get_date_from_int(ltime, fuzzydate);
  }
  bool get_time(MYSQL_TIME *ltime) override { return get_time_from_int(ltime); }
  bool resolve_type(THD *thd) override;
  void print(const THD *thd, String *str,
             enum_query_type query_type) const override;

  friend class Query_result_exists_subquery;
};

/**
  Representation of IN subquery predicates of the form
  "left_expr IN (SELECT ...)".

  @details
  This class has:
   - A "subquery execution engine" (as a subclass of Item_subselect) that allows
     it to evaluate subqueries. (and this class participates in execution by
     having was_null variable where part of execution result is stored.
   - Transformation methods (todo: more on this).

  This class is not used directly, it is "wrapped" into Item_in_optimizer
  which provides some small bits of subquery evaluation.
*/

class Item_in_subselect : public Item_exists_subselect {
  typedef Item_exists_subselect super;

 public:
  Item *left_expr;

 protected:
  /**
    Cache of the left operand of the subquery predicate. Allocated in the
    runtime memory root, for each execution, thus need not be freed.
  */
  List<Cached_item> *left_expr_cache;
  bool left_expr_cache_filled;  ///< Whether left_expr_cache holds a value
  /** The need for expr cache may be optimized away, @sa init_left_expr_cache.
   */
  bool need_expr_cache;

 private:
  /**
    In the case of

       x COMP_OP (SELECT1 UNION SELECT2 ...)

    - the subquery transformation is done on SELECT1; this requires wrapping
      'x' with more Item layers, and injecting that in a condition in SELECT1.

    - the same transformation is done on SELECT2; but the wrapped 'x' doesn't
      need to be created again, the one created for SELECT1 could be reused

    - to achieve this, the wrapped 'x' is stored in member
      'm_injected_left_expr' when it is created for SELECT1, and is later
      reused for SELECT2.

    This will refer to a cached value which is reevaluated once for each
    candidate row, cf. setup in #single_value_transformer.
  */
  Item_ref *m_injected_left_expr;

  /**
    Pointer to the created Item_in_optimizer; it is stored for the same
    reasons as 'm_injected_left_expr'.
  */
  Item_in_optimizer *optimizer;
  bool was_null;

 protected:
  /**
     True if naked IN is allowed to exchange FALSE for UNKNOWN.
     Because this is about the naked IN, there is no public ignore_unknown(),
     intentionally, so that callers don't get it wrong.
  */
  bool abort_on_null;

 private:
  /**
     This bundles several pieces of information useful when doing the
     IN->EXISTS transform. If this transform has not been done, pointer is
     NULL.
  */
  struct In2exists_info {
    /**
       True: if IN->EXISTS has been done and has added a condition to the
       subquery's WHERE clause.
    */
    bool added_to_where;
    /**
       True: if subquery was dependent (correlated) before IN->EXISTS
       was done.
    */
    bool dependent_before;
    /**
       True: if subquery was dependent (correlated) after IN->EXISTS
       was done.
    */
    bool dependent_after;
  } *in2exists_info;

  bool mark_as_outer(Item *left_row, size_t col);

 public:
  /* Used to trigger on/off conditions that were pushed down to subselect */
  bool *pushed_cond_guards;

  Item_func_not_all *upper_item;  // point on NOT/NOP before ALL/SOME subquery

 private:
  PT_subquery *pt_subselect;

 public:
  bool in2exists_added_to_where() const {
    return in2exists_info && in2exists_info->added_to_where;
  }

  /// Is reliable only if IN->EXISTS has been done.
  bool dependent_before_in2exists() const {
    return in2exists_info->dependent_before;
  }

  bool *get_cond_guard(int i) {
    return pushed_cond_guards ? pushed_cond_guards + i : nullptr;
  }
  void set_cond_guard_var(int i, bool v) {
    if (pushed_cond_guards) pushed_cond_guards[i] = v;
  }

  Item_in_subselect(Item *left_expr, Query_block *query_block);
  Item_in_subselect(const POS &pos, Item *left_expr,
                    PT_subquery *pt_subquery_arg);

  Item_in_subselect()
      : Item_exists_subselect(),
        left_expr(nullptr),
        left_expr_cache(nullptr),
        left_expr_cache_filled(false),
        need_expr_cache(true),
        m_injected_left_expr(nullptr),
        optimizer(nullptr),
        was_null(false),
        abort_on_null(false),
        in2exists_info(nullptr),
        pushed_cond_guards(nullptr),
        upper_item(nullptr) {}

  bool itemize(Parse_context *pc, Item **res) override;

  void cleanup() override;
  subs_type substype() const override { return IN_SUBS; }

  void reset() override {
    value = false;
    null_value = false;
    was_null = false;
  }
  bool select_transformer(THD *thd, Query_block *select) override;
  bool select_in_like_transformer(THD *thd, Query_block *select,
                                  Comp_creator *func);
  bool single_value_transformer(THD *thd, Query_block *select,
                                Comp_creator *func);
  bool row_value_transformer(THD *thd, Query_block *select);
  bool single_value_in_to_exists_transformer(THD *thd, Query_block *select,
                                             Comp_creator *func);
  bool row_value_in_to_exists_transformer(THD *thd, Query_block *select);
  bool subquery_allows_materialization(THD *thd, Query_block *query_block,
                                       const Query_block *outer);
  bool walk(Item_processor processor, enum_walk walk, uchar *arg) override;
  Item *transform(Item_transformer transformer, uchar *arg) override;
  Item *compile(Item_analyzer analyzer, uchar **arg_p,
                Item_transformer transformer, uchar *arg_t) override;

  bool exec(THD *thd) override;
  longlong val_int() override;
  double val_real() override;
  String *val_str(String *) override;
  my_decimal *val_decimal(my_decimal *) override;
  bool val_bool() override;
  bool test_limit();
  void print(const THD *thd, String *str,
             enum_query_type query_type) const override;
  bool fix_fields(THD *thd, Item **ref) override;
  void fix_after_pullout(Query_block *parent_query_block,
                         Query_block *removed_query_block) override;
  void update_used_tables() override;
  bool init_left_expr_cache(THD *thd);

  /**
     Once the decision to use IN->EXISTS has been taken, performs some last
     steps of this transformation.
  */
  bool finalize_exists_transform(THD *thd, Query_block *select);
  /**
     Once the decision to use materialization has been taken, performs some
     last steps of this transformation.
  */
  bool finalize_materialization_transform(THD *thd, JOIN *join);
  AccessPath *root_access_path() const override;
  std::optional<ContainedSubquery> get_contained_subquery(
      const Query_block *outer_query_block) override;

  friend class Item_ref_null_helper;
  friend class Item_is_not_null_test;
  friend class Item_in_optimizer;
  friend class subselect_indexsubquery_engine;
  friend class subselect_hash_sj_engine;

 private:
  bool val_bool_naked();
};

/// ALL/ANY/SOME subselect.
class Item_allany_subselect final : public Item_in_subselect {
 public:
  chooser_compare_func_creator func_creator;
  Comp_creator *func;
  bool all;

  Item_allany_subselect(Item *left_expr, chooser_compare_func_creator fc,
                        Query_block *select, bool all);

  // only ALL subquery has upper not
  subs_type substype() const override { return all ? ALL_SUBS : ANY_SUBS; }
  bool select_transformer(THD *thd, Query_block *select) override;
  void print(const THD *thd, String *str,
             enum_query_type query_type) const override;
};

class SubqueryWithResult {
 public:
  SubqueryWithResult(Query_expression *u, Query_result_interceptor *res,
                     Item_subselect *si);
  /**
    Cleanup subquery after complete query execution, free all resources.
  */
  void cleanup();
  bool prepare(THD *thd);
  void fix_length_and_dec(Item_cache **row);
  /**
    Execute the subquery

    SYNOPSIS
      exec()

    DESCRIPTION
      Execute the subquery. The result of execution is subquery value that is
      captured by previously set up Query_result-based 'sink'.

    RETURN
      false - OK
      true  - Execution error.
  */
  bool exec(THD *thd);
  void print(const THD *thd, String *str, enum_query_type query_type);
  bool change_query_result(THD *thd, Item_subselect *si,
                           Query_result_subquery *result);
  Query_block *single_query_block() const;  // Only if unit is simple.

  enum Item_result type() const { return res_type; }
  enum_field_types field_type() const { return res_field_type; }
  bool may_be_null() const { return maybe_null; }

#ifndef NDEBUG
  /**
     @returns the internal Item. Defined only in debug builds, because should
     be used only for debug asserts.
  */
  const Item_subselect *get_item() const { return item; }
#endif

 private:
  Query_result_interceptor *result; /* results storage class */
  Item_subselect *item;             /* item, that use this subquery */
  enum Item_result res_type;        /* type of results */
  enum_field_types res_field_type;  /* column type of the results */
  /**
    True if at least one of the columns returned by the subquery may
    be null, or if a single-row subquery may return zero rows.
  */
  bool maybe_null;

  Query_expression *unit; /* corresponding unit structure */

  void set_row(const mem_root_deque<Item *> &item_list, Item_cache **row,
               bool never_empty);

  friend class subselect_hash_sj_engine;
};

/**
  A subquery execution engine that evaluates the subquery by doing index
  lookups in a single table's index.

  This engine is used to resolve subqueries in forms

    outer_expr IN (SELECT tbl.key FROM tbl WHERE subq_where)

  or, row-based:

    (oe1, .. oeN) IN (SELECT key_part1, ... key_partK
                      FROM tbl WHERE subqwhere)

  i.e. the subquery is a single table SELECT without GROUP BY, aggregate
  functions, etc.
*/
class subselect_indexsubquery_engine {
 protected:
  Query_result_union *result = nullptr; /* results storage class */
  /// Table which is read, using one of eq_ref, ref, ref_or_null.
  TABLE *table{nullptr};
  Table_ref *table_ref{nullptr};
  Index_lookup ref;
  join_type type{JT_UNKNOWN};
  Item *cond;     /* The WHERE condition of subselect */
  ulonglong hash; /* Hash value calculated by RefIterator, when needed. */
  /*
    The "having" clause. This clause (further referred to as "artificial
    having") was inserted by subquery transformation code. It contains
    Item(s) that have a side-effect: they record whether the subquery has
    produced a row with NULL certain components. We need to use it for cases
    like
      (oe1, oe2) IN (SELECT t.key, t.no_key FROM t1)
    where we do index lookup on t.key=oe1 but need also to check if there
    was a row such that t.no_key IS NULL.
  */
  Item *having;

  Item_in_subselect *item; /* item that uses this engine */

 public:
  enum enum_engine_type { INDEXSUBQUERY_ENGINE, HASH_SJ_ENGINE };

  subselect_indexsubquery_engine(TABLE *table, Table_ref *table_ref,
                                 const Index_lookup &ref,
                                 enum join_type join_type,
                                 Item_in_subselect *subs, Item *where,
                                 Item *having_arg)
      : table(table),
        table_ref(table_ref),
        ref(ref),
        type(join_type),
        cond(where),
        having(having_arg),
        item(subs) {}
  virtual ~subselect_indexsubquery_engine() = default;
  virtual bool exec(THD *thd);
  virtual void print(const THD *thd, String *str, enum_query_type query_type);
  virtual enum_engine_type engine_type() const { return INDEXSUBQUERY_ENGINE; }
  virtual void cleanup() {}
  virtual void create_iterators(THD *) {}
};

/*
  This function is actually defined in sql_parse.cc, but it depends on
  chooser_compare_func_creator defined in this file.
 */
Item *all_any_subquery_creator(Item *left_expr,
                               chooser_compare_func_creator cmp, bool all,
                               Query_block *select);

/**
  Compute an IN predicate via a hash semi-join. The subquery is materialized
  during the first evaluation of the IN predicate. The IN predicate is executed
  via the functionality inherited from subselect_indexsubquery_engine.
*/

class subselect_hash_sj_engine final : public subselect_indexsubquery_engine {
 private:
  /* true if the subquery was materialized into a temp table. */
  bool is_materialized;
  // true if we know for sure that there are zero rows in the table.
  // Set only after is_materialized is true.
  bool has_zero_rows = false;
  /**
     Existence of inner NULLs in materialized table:
     By design, other values than IRRELEVANT_OR_FALSE are possible only if the
     subquery has only one inner expression.
  */
  enum nulls_exist {
    /// none, or they don't matter
    NEX_IRRELEVANT_OR_FALSE = 0,
    /// they matter, and we don't know yet if they exists
    NEX_UNKNOWN = 1,
    /// they matter, and we know there exists at least one.
    NEX_TRUE = 2
  };
  enum nulls_exist mat_table_has_nulls;
  Query_expression *const unit;
  unique_ptr_destroy_only<RowIterator> m_iterator;
  AccessPath *m_root_access_path;

  /// Saved result object, must be restored after use
  Query_result_interceptor *saved_result{nullptr};

 public:
  subselect_hash_sj_engine(Item_in_subselect *in_predicate,
                           Query_expression *unit_arg)
      : subselect_indexsubquery_engine(nullptr, nullptr, {}, JT_UNKNOWN,
                                       in_predicate, nullptr, nullptr),
        is_materialized(false),
        unit(unit_arg) {}
  ~subselect_hash_sj_engine() override;

  bool setup(THD *thd, const mem_root_deque<Item *> &tmp_columns);
  void cleanup() override;
  bool exec(THD *thd) override;
  void print(const THD *thd, String *str, enum_query_type query_type) override;
  enum_engine_type engine_type() const override { return HASH_SJ_ENGINE; }

  TABLE *get_table() const { return table; }
  const Index_lookup &index_lookup() const { return ref; }
  enum join_type get_join_type() const { return type; }
  AccessPath *root_access_path() const { return m_root_access_path; }
  void create_iterators(THD *thd) override;
};

/**
  Removes every predicate injected by IN->EXISTS.

  This function is different from others:
  - it wants to remove all traces of IN->EXISTS (for
  materialization)
  - remove_subq_pushed_predicates() and remove_additional_cond() want to
  remove only the conditions of IN->EXISTS which index lookup already
  satisfies (they are just an optimization).

  If there are no in2exists conditions, it will return the exact same
  pointer. If it returns a new Item, the old Item is left alone, so it
  can be reused in other settings.

  @param conds  Condition; may be nullptr.
  @returns      new condition
 */
Item *remove_in2exists_conds(Item *conds);

/// Returns whether the Item is an IN-subselect.
bool IsItemInSubSelect(Item *item);

#endif /* ITEM_SUBSELECT_INCLUDED */
