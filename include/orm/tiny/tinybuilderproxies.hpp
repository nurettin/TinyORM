#pragma once
#ifndef ORM_TINY_TINYBUILDERPROXIES_HPP
#define ORM_TINY_TINYBUILDERPROXIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtSql/QSqlQuery>

#include "orm/ormconcepts.hpp"
#include "orm/tiny/tinytypes.hpp"
#include "orm/tiny/utils/attribute.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

namespace Query
{
    class JoinClause;
}

namespace Tiny
{

    /*! Contains proxy methods to the QueryBuilder. */
    template<typename Model>
    class BuilderProxies
    {
        /*! Alias for the attribute utils. */
        using AttributeUtils = Orm::Tiny::Utils::Attribute;
        /*! JoinClause alias. */
        using JoinClause = Orm::Query::JoinClause;

    public:
        /*! Default constructor. */
        inline BuilderProxies() = default;
        /*! Default destructor. */
        inline ~BuilderProxies() = default;

        /*! Copy constructor. */
        inline BuilderProxies(const BuilderProxies &) = default;
        /*! Deleted copy assignment operator (not needed). */
        BuilderProxies &operator=(const BuilderProxies &) = delete;

        /*! Move constructor. */
        inline BuilderProxies(BuilderProxies &&) noexcept = default;
        /*! Deleted move assignment operator (not needed). */
        BuilderProxies &operator=(BuilderProxies &&) = delete;

        /* Retrieving results */
        /*! Concatenate values of a given column as a string. */
        QString implode(const QString &column, const QString &glue = "") const;

        /* Insert, Update, Delete */
        /*! Insert a new record into the database. */
        std::optional<QSqlQuery>
        insert(const QVector<AttributeItem> &values) const;
        /*! Insert new records into the database. */
        std::optional<QSqlQuery>
        insert(const QVector<QVector<AttributeItem>> &values) const;
        /*! Insert new records into the database (multi insert). */
        std::optional<QSqlQuery>
        insert(const QVector<QString> &columns, QVector<QVector<QVariant>> values) const;

        /*! Insert a new record and get the value of the primary key. */
        quint64 insertGetId(const QVector<AttributeItem> &values,
                            const QString &sequence = "") const;

        /*! Insert a new record into the database while ignoring errors. */
        std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVector<AttributeItem> &values) const;
        /*! Insert new records into the database while ignoring errors. */
        std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVector<QVector<AttributeItem>> &values) const;
        /*! Insert new records into the database while ignoring errors (multi insert). */
        std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVector<QString> &columns,
                       QVector<QVector<QVariant>> values) const;

        /*! Update records in the database. */
        std::tuple<int, QSqlQuery>
        update(const QVector<UpdateItem> &values) const;

        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> remove() const;
        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> deleteModels() const;

        /*! Run a truncate statement on the table. */
        void truncate() const;

        /* Select */
        /*! Retrieve the "count" result of the query. */
        quint64 count(const QVector<Column> &columns = {ASTERISK}) const;
        /*! Retrieve the "count" result of the query. */
        template<typename = void>
        quint64 count(const Column &column) const;
        /*! Retrieve the minimum value of a given column. */
        QVariant min(const Column &column) const;
        /*! Retrieve the maximum value of a given column. */
        QVariant max(const Column &column) const;
        /*! Retrieve the sum of the values of a given column. */
        QVariant sum(const Column &column) const;
        /*! Retrieve the average of the values of a given column. */
        QVariant avg(const Column &column) const;
        /*! Alias for the "avg" method. */
        QVariant average(const Column &column) const;

        /*! Execute an aggregate function on the database. */
        QVariant aggregate(const QString &function,
                           const QVector<Column> &columns = {ASTERISK}) const;

        /*! Determine if any rows exist for the current query. */
        bool exists() const;
        /*! Determine if no rows exist for the current query. */
        bool doesntExist() const;

        /*! Execute the given callback if no rows exist for the current query. */
        bool existsOr(const std::function<void()> &callback) const;
        /*! Execute the given callback if rows exist for the current query. */
        bool doesntExistOr(const std::function<void()> &callback) const;

        /*! Execute the given callback if no rows exist for the current query. */
        template<typename R>
        std::pair<bool, R> existsOr(const std::function<R()> &callback) const;
        template<typename R>
        /*! Execute the given callback if rows exist for the current query. */
        std::pair<bool, R> doesntExistOr(const std::function<R()> &callback) const;

        /*! Set the columns to be selected. */
        TinyBuilder<Model> &select(const QVector<Column> &columns = {ASTERISK});
        /*! Set the column to be selected. */
        TinyBuilder<Model> &select(const Column &column);
        /*! Add new select columns to the query. */
        TinyBuilder<Model> &addSelect(const QVector<Column> &columns);
        /*! Add a new select column to the query. */
        TinyBuilder<Model> &addSelect(const Column &column);

        /*! Set a select subquery on the query. */
        template<Queryable T>
        TinyBuilder<Model> &select(T &&query, const QString &as);
        /*! Add a select subquery to the query. */
        template<Queryable T>
        TinyBuilder<Model> &addSelect(T &&query, const QString &as);

        /*! Add a subselect expression to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &selectSub(T &&query, const QString &as);
        /*! Add a new "raw" select expression to the query. */
        TinyBuilder<Model> &selectRaw(const QString &expression,
                                      const QVector<QVariant> &bindings = {});

        /*! Force the query to only return distinct results. */
        TinyBuilder<Model> &distinct();
        /*! Force the query to only return distinct results. */
        TinyBuilder<Model> &distinct(const QStringList &columns);
        /*! Force the query to only return distinct results. */
        TinyBuilder<Model> &distinct(QStringList &&columns);

        /* Joins */
        /*! Add a join clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &join(T &&table, const QString &first,
                                 const QString &comparison, const QString &second,
                                 const QString &type = INNER, bool where = false);
        /*! Add an advanced join clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &join(T &&table,
                                 const std::function<void(JoinClause &)> &callback,
                                 const QString &type = INNER);
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &joinWhere(T &&table, const QString &first,
                                      const QString &comparison,
                                      const QVariant &second,
                                      const QString &type = INNER);

        /*! Add a left join to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &leftJoin(T &&table, const QString &first,
                                     const QString &comparison, const QString &second);
        /*! Add an advanced left join to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &leftJoin(T &&table,
                                     const std::function<void(JoinClause &)> &callback);
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &leftJoinWhere(T &&table, const QString &first,
                                          const QString &comparison,
                                          const QVariant &second);

        /*! Add a right join to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &rightJoin(T &&table, const QString &first,
                                      const QString &comparison, const QString &second);
        /*! Add an advanced right join to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &rightJoin(T &&table,
                                      const std::function<void(JoinClause &)> &callback);
        /*! Add a "right join where" clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &rightJoinWhere(T &&table, const QString &first,
                                           const QString &comparison,
                                           const QVariant &second);

        /*! Add a "cross join" clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &crossJoin(T &&table, const QString &first,
                                      const QString &comparison, const QString &second);
        /*! Add an advanced "cross join" clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &crossJoin(T &&table,
                                      const std::function<void(JoinClause &)> &callback);
        /*! Add a "cross join" clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &crossJoin(T &&table);

        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &joinSub(T &&query, const QString &as, const QString &first,
                                    const QString &comparison, const QVariant &second,
                                    const QString &type = INNER, bool where = false);
        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &joinSub(T &&query, const QString &as,
                                    const std::function<void(JoinClause &)> &callback,
                                    const QString &type = INNER);

        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &
        leftJoinSub(T &&query, const QString &as, const QString &first,
                    const QString &comparison, const QVariant &second);
        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &
        leftJoinSub(T &&query, const QString &as,
                    const std::function<void(JoinClause &)> &callback);

        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &
        rightJoinSub(T &&query, const QString &as, const QString &first,
                     const QString &comparison, const QVariant &second);
        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &
        rightJoinSub(T &&query, const QString &as,
                     const std::function<void(JoinClause &)> &callback);

        /* General where */
        /*! Add a basic where clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &where(const Column &column, const QString &comparison,
                                  T &&value, const QString &condition = AND);
        /*! Add an "or where" clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &orWhere(const Column &column, const QString &comparison,
                                    T &&value);
        /*! Add a basic equal where clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &whereEq(const Column &column, T &&value,
                                    const QString &condition = AND);
        /*! Add an equal "or where" clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &orWhereEq(const Column &column, T &&value);

        /* General where not */
        /*! Add a basic "where not" clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &whereNot(const Column &column, const QString &comparison,
                                     T &&value, const QString &condition = AND);
        /*! Add an "or where not" clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &orWhereNot(const Column &column, const QString &comparison,
                                       T &&value);
        /*! Add a basic equal "where not" clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &whereNotEq(const Column &column, T &&value,
                                       const QString &condition = AND);
        /*! Add an equal "or where not" clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &orWhereNotEq(const Column &column, T &&value);

        /* Nested where */
        /*! Add a nested where clause to the query. */
        TinyBuilder<Model> &
        where(const std::function<void(TinyBuilder<Model> &)> &callback,
              const QString &condition = AND);
        /*! Add a nested "or where" clause to the query. */
        TinyBuilder<Model> &
        orWhere(const std::function<void(TinyBuilder<Model> &)> &callback);
        /*! Add a nested "where not" clause to the query. */
        TinyBuilder<Model> &
        whereNot(const std::function<void(TinyBuilder<Model> &)> &callback,
                 const QString &condition = AND);
        /*! Add a nested "or where not" clause to the query. */
        TinyBuilder<Model> &
        orWhereNot(const std::function<void(TinyBuilder<Model> &)> &callback);

        /* Array where */
        /*! Add a vector of basic where clauses to the query. */
        TinyBuilder<Model> &where(const QVector<WhereItem> &values,
                                  const QString &condition = AND);
        /*! Add a vector of basic "or where" clauses to the query. */
        TinyBuilder<Model> &orWhere(const QVector<WhereItem> &values);
        /*! Add a vector of basic "where not" clauses to the query. */
        TinyBuilder<Model> &whereNot(const QVector<WhereItem> &values,
                                     const QString &condition = AND,
                                     const QString &defaultCondition = "");
        /*! Add a vector of basic "or where not" clauses to the query. */
        TinyBuilder<Model> &orWhereNot(const QVector<WhereItem> &values,
                                       const QString &defaultCondition = "");

        /* where column */
        /*! Add a vector of where clauses comparing two columns to the query. */
        TinyBuilder<Model> &whereColumn(const QVector<WhereColumnItem> &values,
                                        const QString &condition = AND);
        /*! Add a vector of "or where" clauses comparing two columns to the query. */
        TinyBuilder<Model> &orWhereColumn(const QVector<WhereColumnItem> &values);

        /*! Add a "where" clause comparing two columns to the query. */
        TinyBuilder<Model> &whereColumn(const Column &first, const QString &comparison,
                                        const Column &second,
                                        const QString &condition = AND);
        /*! Add a "or where" clause comparing two columns to the query. */
        TinyBuilder<Model> &orWhereColumn(const Column &first, const QString &comparison,
                                          const Column &second);
        /*! Add an equal "where" clause comparing two columns to the query. */
        TinyBuilder<Model> &whereColumnEq(const Column &first, const Column &second,
                                          const QString &condition = AND);
        /*! Add an equal "or where" clause comparing two columns to the query. */
        TinyBuilder<Model> &orWhereColumnEq(const Column &first, const Column &second);

        /* where IN */
        /*! Add a "where in" clause to the query. */
        TinyBuilder<Model> &whereIn(const Column &column,
                                    const QVector<QVariant> &values,
                                    const QString &condition = AND, bool nope = false);
        /*! Add an "or where in" clause to the query. */
        TinyBuilder<Model> &orWhereIn(const Column &column,
                                      const QVector<QVariant> &values);
        /*! Add a "where not in" clause to the query. */
        TinyBuilder<Model> &whereNotIn(const Column &column,
                                       const QVector<QVariant> &values,
                                       const QString &condition = AND);
        /*! Add an "or where not in" clause to the query. */
        TinyBuilder<Model> &orWhereNotIn(const Column &column,
                                         const QVector<QVariant> &values);

        /* where null */
        /*! Add a "where null" clause to the query. */
        TinyBuilder<Model> &whereNull(const QVector<Column> &columns = {ASTERISK},
                                      const QString &condition = AND, bool nope = false);
        /*! Add an "or where null" clause to the query. */
        TinyBuilder<Model> &orWhereNull(const QVector<Column> &columns = {ASTERISK});
        /*! Add a "where not null" clause to the query. */
        TinyBuilder<Model> &whereNotNull(const QVector<Column> &columns = {ASTERISK},
                                         const QString &condition = AND);
        /*! Add an "or where not null" clause to the query. */
        TinyBuilder<Model> &orWhereNotNull(const QVector<Column> &columns = {ASTERISK});

        /*! Add a "where null" clause to the query. */
        TinyBuilder<Model> &whereNull(const Column &column,
                                      const QString &condition = AND, bool nope = false);
        /*! Add an "or where null" clause to the query. */
        TinyBuilder<Model> &orWhereNull(const Column &column);
        /*! Add a "where not null" clause to the query. */
        TinyBuilder<Model> &whereNotNull(const Column &column,
                                         const QString &condition = AND);
        /*! Add an "or where not null" clause to the query. */
        TinyBuilder<Model> &orWhereNotNull(const Column &column);

        /* where sub-queries */
        /*! Add a basic where clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &where(C &&column, const QString &comparison, V &&value,
                                  const QString &condition = AND);
        /*! Add an "or where" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &orWhere(C &&column, const QString &comparison, V &&value);
        /*! Add a basic equal where clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &whereEq(C &&column, V &&value,
                                    const QString &condition = AND);
        /*! Add an equal "or where" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &orWhereEq(C &&column, V &&value);

        /* where not sub-queries */
        /*! Add a basic "where not" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &whereNot(C &&column, const QString &comparison, V &&value,
                                     const QString &condition = AND);
        /*! Add an "or where not" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &orWhereNot(C &&column, const QString &comparison, V &&value);
        /*! Add a basic equal "where not" clause to the query with a full sub-select
            column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &whereNotEq(C &&column, V &&value,
                                       const QString &condition = AND);
        /*! Add an equal "or where not" clause to the query with a full sub-select
            column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &orWhereNotEq(C &&column, V &&value);

        /*! Add a full sub-select to the "where" clause. */
        template<WhereValueSubQuery T>
        TinyBuilder<Model> &whereSub(const Column &column, const QString &comparison,
                                     T &&query, const QString &condition = AND);

        /* where exists */
        /*! Add an exists clause to the query. */
        TinyBuilder<Model> &
        whereExists(const std::function<void(QueryBuilder &)> &callback,
                    const QString &condition = AND, bool nope = false);
        /*! Add an or exists clause to the query. */
        TinyBuilder<Model> &
        orWhereExists(const std::function<void(QueryBuilder &)> &callback,
                      bool nope = false);
        /*! Add a where not exists clause to the query. */
        TinyBuilder<Model> &
        whereNotExists(const std::function<void(QueryBuilder &)> &callback,
                       const QString &condition = AND);
        /*! Add a where not exists clause to the query. */
        TinyBuilder<Model> &
        orWhereNotExists(const std::function<void(QueryBuilder &)> &callback);

        /* where raw */
        /*! Add a raw "where" clause to the query. */
        TinyBuilder<Model> &whereRaw(const QString &sql,
                                     const QVector<QVariant> &bindings = {},
                                     const QString &condition = AND);
        /*! Add a raw "or where" clause to the query. */
        TinyBuilder<Model> &orWhereRaw(const QString &sql,
                                       const QVector<QVariant> &bindings = {});

        /* Group by and having */
        /*! Add a "group by" clause to the query. */
        TinyBuilder<Model> &groupBy(const QVector<Column> &groups);
        /*! Add a "group by" clause to the query. */
        TinyBuilder<Model> &groupBy(const Column &group);
        /*! Add a "group by" clause to the query. */
        template<ColumnConcept ...Args>
        TinyBuilder<Model> &groupBy(Args &&...groups);

        /*! Add a raw "groupBy" clause to the query. */
        TinyBuilder<Model> &groupByRaw(const QString &sql,
                                       const QVector<QVariant> &bindings = {});

        /*! Add a "having" clause to the query. */
        TinyBuilder<Model> &having(const Column &column, const QString &comparison,
                                   const QVariant &value,
                                   const QString &condition = AND);
        /*! Add an "or having" clause to the query. */
        TinyBuilder<Model> &orHaving(const Column &column, const QString &comparison,
                                     const QVariant &value);

        /*! Add a raw "having" clause to the query. */
        TinyBuilder<Model> &havingRaw(const QString &sql,
                                      const QVector<QVariant> &bindings = {},
                                      const QString &condition = AND);
        /*! Add a raw "or having" clause to the query. */
        TinyBuilder<Model> &orHavingRaw(const QString &sql,
                                        const QVector<QVariant> &bindings = {});

        /* Ordering */
        /*! Add an "order by" clause to the query. */
        TinyBuilder<Model> &orderBy(const Column &column,
                                    const QString &direction = ASC);
        /*! Add a descending "order by" clause to the query. */
        TinyBuilder<Model> &orderByDesc(const Column &column);

        /*! Add an "order by" clause to the query with a subquery ordering. */
        template<Queryable T>
        TinyBuilder<Model> &orderBy(T &&query, const QString &direction = ASC);
        /*! Add a descending "order by" clause to the query with a subquery ordering. */
        template<Queryable T>
        TinyBuilder<Model> &orderByDesc(T &&query);

        /*! Put the query's results in random order. */
        TinyBuilder<Model> &inRandomOrder(const QString &seed = "");
        /*! Add a raw "order by" clause to the query. */
        TinyBuilder<Model> &orderByRaw(const QString &sql,
                                       const QVector<QVariant> &bindings = {});

        /*! Add an "order by" clause for a timestamp to the query. */
        TinyBuilder<Model> &latest(const Column &column = "");
        /*! Add an "order by" clause for a timestamp to the query. */
        TinyBuilder<Model> &oldest(const Column &column = "");
        /*! Remove all existing orders. */
        TinyBuilder<Model> &reorder();
        /*! Remove all existing orders and optionally add a new order. */
        TinyBuilder<Model> &reorder(const Column &column,
                                    const QString &direction = ASC);

        /*! Set the "limit" value of the query. */
        TinyBuilder<Model> &limit(int value);
        /*! Alias to set the "limit" value of the query. */
        TinyBuilder<Model> &take(int value);
        /*! Set the "offset" value of the query. */
        TinyBuilder<Model> &offset(int value);
        /*! Alias to set the "offset" value of the query. */
        TinyBuilder<Model> &skip(int value);
        /*! Set the limit and offset for a given page. */
        TinyBuilder<Model> &forPage(int page, int perPage = 30);

        /*! Constrain the query to the previous "page" of results before a given ID. */
        TinyBuilder<Model> &
        forPageBeforeId(int perPage = 30, const QVariant &lastId = {},
                        const QString &column = Orm::Constants::ID,
                        bool prependOrder = false);
        /*! Constrain the query to the next "page" of results after a given ID. */
        TinyBuilder<Model> &
        forPageAfterId(int perPage = 30, const QVariant &lastId = {},
                       const QString &column = Orm::Constants::ID,
                       bool prependOrder = false);

        /* Others */
        /*! Increment a column's value by a given amount. */
        template<typename T = std::size_t> requires std::is_arithmetic_v<T>
        std::tuple<int, QSqlQuery>
        increment(const QString &column, T amount = 1,
                  const QVector<UpdateItem> &extra = {}) const;
        /*! Decrement a column's value by a given amount. */
        template<typename T = std::size_t> requires std::is_arithmetic_v<T>
        std::tuple<int, QSqlQuery>
        decrement(const QString &column, T amount = 1,
                  const QVector<UpdateItem> &extra = {}) const;

        /* Pessimistic Locking */
        /*! Lock the selected rows in the table for updating. */
        TinyBuilder<Model> &lockForUpdate();
        /*! Share lock the selected rows in the table. */
        TinyBuilder<Model> &sharedLock();
        /*! Lock the selected rows in the table. */
        TinyBuilder<Model> &lock(bool value = true);
        /*! Lock the selected rows in the table. */
        TinyBuilder<Model> &lock(const char *value);
        /*! Lock the selected rows in the table. */
        TinyBuilder<Model> &lock(const QString &value);
        /*! Lock the selected rows in the table. */
        TinyBuilder<Model> &lock(QString &&value);

        /* Debugging */
        /*! Dump the current SQL and bindings. */
        void dump(bool replaceBindings = true, bool simpleBindings = false) const;
        /*! Die and dump the current SQL and bindings. */
        void dd(bool replaceBindings = true, bool simpleBindings = false) const;

        /* Others proxy methods, not added to the Model and Relation */
        /*! Add an "exists" clause to the query. */
        TinyBuilder<Model> &
        addWhereExistsQuery(const std::shared_ptr<QueryBuilder> &query,
                            const QString &condition = AND, bool nope = false);

        /*! Merge an array of where clauses and bindings. */
        TinyBuilder<Model> &mergeWheres(const QVector<WhereConditionItem> &wheres,
                                        const QVector<QVariant> &bindings);
        /*! Merge an array of where clauses and bindings. */
        TinyBuilder<Model> &mergeWheres(QVector<WhereConditionItem> &&wheres,
                                        QVector<QVariant> &&bindings);

    private:
        /*! Static cast this to a child's instance TinyBuilder type. */
        const TinyBuilder<Model> &builder() const;
        /*! Static cast this to a child's instance TinyBuilder type, const version. */
        TinyBuilder<Model> &builder();
        /*! Get a base query builder instance. */
        QueryBuilder &toBase() const;
    };

    /* public */

    /* Retrieving results */

    template<typename Model>
    QString
    BuilderProxies<Model>::implode(const QString &column, const QString &glue) const
    {
        return toBase().implode(column, glue);
    }

    /* Insert, Update, Delete */

    template<typename Model>
    std::optional<QSqlQuery>
    BuilderProxies<Model>::insert(const QVector<AttributeItem> &values) const
    {
        return toBase().insert(AttributeUtils::convertVectorToMap(values));
    }

    template<typename Model>
    std::optional<QSqlQuery>
    BuilderProxies<Model>::insert(const QVector<QVector<AttributeItem>> &values) const
    {
        return toBase().insert(AttributeUtils::convertVectorsToMaps(values));
    }

    template<typename Model>
    std::optional<QSqlQuery>
    BuilderProxies<Model>::insert(
            const QVector<QString> &columns, QVector<QVector<QVariant>> values) const
    {
        return toBase().insert(columns, std::move(values));
    }

    // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
    template<typename Model>
    quint64
    BuilderProxies<Model>::insertGetId(const QVector<AttributeItem> &values,
                                       const QString &sequence) const
    {
        return toBase().insertGetId(AttributeUtils::convertVectorToMap(values),
                                    sequence);
    }

    template<typename Model>
    std::tuple<int, std::optional<QSqlQuery>>
    BuilderProxies<Model>::insertOrIgnore(const QVector<AttributeItem> &values) const
    {
        return toBase().insertOrIgnore(AttributeUtils::convertVectorToMap(values));
    }

    template<typename Model>
    std::tuple<int, std::optional<QSqlQuery>>
    BuilderProxies<Model>::insertOrIgnore(
            const QVector<QVector<AttributeItem>> &values) const
    {
        return toBase().insertOrIgnore(AttributeUtils::convertVectorsToMaps(values));
    }

    template<typename Model>
    std::tuple<int, std::optional<QSqlQuery>>
    BuilderProxies<Model>::insertOrIgnore(
            const QVector<QString> &columns, QVector<QVector<QVariant>> values) const
    {
        return toBase().insertOrIgnore(columns, std::move(values));
    }

    template<typename Model>
    std::tuple<int, QSqlQuery>
    BuilderProxies<Model>::update(const QVector<UpdateItem> &values) const
    {
        return toBase().update(builder().addUpdatedAtColumn(values));
    }

    // FUTURE add onDelete (and similar) callback feature silverqx
    template<typename Model>
    std::tuple<int, QSqlQuery> BuilderProxies<Model>::remove() const
    {
        return toBase().deleteRow();
    }

    template<typename Model>
    std::tuple<int, QSqlQuery> BuilderProxies<Model>::deleteModels() const
    {
        return remove();
    }

    template<typename Model>
    void BuilderProxies<Model>::truncate() const
    {
        toBase().truncate();
    }

    /* Select */

    template<typename Model>
    quint64 BuilderProxies<Model>::count(const QVector<Column> &columns) const
    {
        return toBase().count(columns);
    }

    template<typename Model>
    template<typename>
    quint64 BuilderProxies<Model>::count(const Column &column) const
    {
        return toBase().count(QVector<Column> {column});
    }

    template<typename Model>
    QVariant BuilderProxies<Model>::min(const Column &column) const
    {
        return toBase().min(column);
    }

    template<typename Model>
    QVariant BuilderProxies<Model>::max(const Column &column) const
    {
        return toBase().max(column);
    }

    template<typename Model>
    QVariant BuilderProxies<Model>::sum(const Column &column) const
    {
        return toBase().sum(column);
    }

    template<typename Model>
    QVariant BuilderProxies<Model>::avg(const Column &column) const
    {
        return toBase().avg(column);
    }

    template<typename Model>
    QVariant BuilderProxies<Model>::average(const Column &column) const
    {
        return toBase().avg(column);
    }

    template<typename Model>
    QVariant
    BuilderProxies<Model>::aggregate(const QString &function,
                                     const QVector<Column> &columns) const
    {
        return toBase().aggregate(function, columns);
    }

    template<typename Model>
    bool BuilderProxies<Model>::exists() const
    {
        return toBase().exists();
    }

    template<typename Model>
    bool BuilderProxies<Model>::doesntExist() const
    {
        return toBase().doesntExist();
    }

    template<typename Model>
    bool BuilderProxies<Model>::existsOr(const std::function<void()> &callback) const
    {
        return toBase().existsOr(callback);
    }

    template<typename Model>
    bool
    BuilderProxies<Model>::doesntExistOr(const std::function<void()> &callback) const
    {
        return toBase().doesntExistOr(callback);
    }

    template<typename Model>
    template<typename R>
    std::pair<bool, R>
    BuilderProxies<Model>::existsOr(const std::function<R()> &callback) const
    {
        return toBase().template existsOr<R>(callback);
    }

    template<typename Model>
    template<typename R>
    std::pair<bool, R>
    BuilderProxies<Model>::doesntExistOr(const std::function<R()> &callback) const
    {
        return toBase().template doesntExistOr<R>(callback);
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::select(const QVector<Column> &columns)
    {
        toBase().select(columns);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::select(const Column &column)
    {
        toBase().select(column);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::addSelect(const QVector<Column> &columns)
    {
        toBase().addSelect(columns);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::addSelect(const Column &column)
    {
        toBase().addSelect(column);
        return builder();
    }

    template<typename Model>
    template<Queryable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::select(T &&query, const QString &as)
    {
        toBase().select(std::forward<T>(query), as);
        return builder();
    }

    template<typename Model>
    template<Queryable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::addSelect(T &&query, const QString &as)
    {
        toBase().addSelect(std::forward<T>(query), as);
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::selectSub(T &&query, const QString &as)
    {
        toBase().selectSub(std::forward<T>(query), as);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::selectRaw(const QString &expression,
                                     const QVector<QVariant> &bindings)
    {
        toBase().selectRaw(expression, bindings);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::distinct()
    {
        toBase().distinct();
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::distinct(const QStringList &columns)
    {
        toBase().distinct(columns);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::distinct(QStringList &&columns)
    {
        toBase().distinct(std::move(columns));
        return builder();
    }

    /* Joins */

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::join(
            T &&table, const QString &first, const QString &comparison,
            const QString &second, const QString &type, const bool where)
    {
        toBase().join(std::forward<T>(table), first, comparison, second, type, where);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::join(
            T &&table, const std::function<void(JoinClause &)> &callback,
            const QString &type)
    {
        toBase().join(std::forward<T>(table), callback, type);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::joinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second, const QString &type)
    {
        toBase().joinWhere(std::forward<T>(table), first, comparison, second, type);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::leftJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second)
    {
        toBase().leftJoin(std::forward<T>(table), first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::leftJoin(
            T &&table, const std::function<void(JoinClause &)> &callback)
    {
        toBase().leftJoin(std::forward<T>(table), callback);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::leftJoinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second)
    {
        toBase().leftJoinWhere(std::forward<T>(table), first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::rightJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second)
    {
        toBase().rightJoin(std::forward<T>(table), first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::rightJoin(
            T &&table, const std::function<void(JoinClause &)> &callback)
    {
        toBase().rightJoin(std::forward<T>(table), callback);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::rightJoinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second)
    {
        toBase().rightJoinWhere(std::forward<T>(table), first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::crossJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second)
    {
        toBase().crossJoin(std::forward<T>(table), first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::crossJoin(
            T &&table, const std::function<void(JoinClause &)> &callback)
    {
        toBase().crossJoin(std::forward<T>(table), callback);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::crossJoin(T &&table)
    {
        toBase().crossJoin(std::forward<T>(table));
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::joinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second, const QString &type,
            const bool where)
    {
        toBase().joinSub(std::forward<T>(query), as, first, comparison, second, type,
                         where);
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::joinSub(
            T &&query, const QString &as,
            const std::function<void(JoinClause &)> &callback, const QString &type)
    {
        toBase().joinSub(std::forward<T>(query), as, callback, type);
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::leftJoinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second)
    {
        toBase().leftJoinSub(std::forward<T>(query), as, first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::leftJoinSub(
            T &&query, const QString &as,
            const std::function<void(JoinClause &)> &callback)
    {
        toBase().joinSub(std::forward<T>(query), as, callback, LEFT);
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::rightJoinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second)
    {
        toBase().rightJoinSub(std::forward<T>(query), as, first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::rightJoinSub(
            T &&query, const QString &as,
            const std::function<void(JoinClause &)> &callback)
    {
        toBase().joinSub(std::forward<T>(query), as, callback, RIGHT);
        return builder();
    }

    /* General where */

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::where(
            const Column &column, const QString &comparison, T &&value,
            const QString &condition)
    {
        toBase().where(column, comparison, std::forward<T>(value), condition);
        return builder();
    }

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhere(
            const Column &column, const QString &comparison, T &&value)
    {
        toBase().orWhere(column, comparison, std::forward<T>(value));
        return builder();
    }

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereEq(
            const Column &column, T &&value, const QString &condition)
    {
        toBase().whereEq(column, std::forward<T>(value), condition);
        return builder();
    }

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereEq(const Column &column, T &&value)
    {
        toBase().orWhereEq(column, std::forward<T>(value));
        return builder();
    }

    /* General where not */

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNot(
            const Column &column, const QString &comparison, T &&value,
            const QString &condition)
    {
        toBase().whereNot(column, comparison, std::forward<T>(value), condition);
        return builder();
    }

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNot(
            const Column &column, const QString &comparison, T &&value)
    {
        toBase().orWhereNot(column, comparison, std::forward<T>(value));
        return builder();
    }

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotEq(
            const Column &column, T &&value, const QString &condition)
    {
        toBase().whereNotEq(column, std::forward<T>(value), condition);
        return builder();
    }

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotEq(const Column &column, T &&value)
    {
        toBase().orWhereNotEq(column, std::forward<T>(value));
        return builder();
    }

    /* Nested where */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::where(
            const std::function<void(TinyBuilder<Model> &)> &callback,
            const QString &condition)
    {
        // Ownership of a unique_ptr()
        auto query = builder().m_model.newQueryWithoutRelationships();

        std::invoke(callback, *query);

        toBase().addNestedWhereQuery(query->getQuerySharedPointer(), condition);

        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhere(
            const std::function<void(TinyBuilder<Model> &)> &callback)
    {
        return where(callback, OR);
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNot(
            const std::function<void(TinyBuilder<Model> &)> &callback,
            const QString &condition)
    {
        return where(callback, SPACE_IN.arg(condition, NOT));
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNot(
            const std::function<void(TinyBuilder<Model> &)> &callback)
    {
        return where(callback, SPACE_IN.arg(OR, NOT));
    }

    /* Array where */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::where(
            const QVector<WhereItem> &values, const QString &condition)
    {
        toBase().where(values, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhere(const QVector<WhereItem> &values)
    {
        toBase().orWhere(values);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNot(
            const QVector<WhereItem> &values, const QString &condition,
            const QString &defaultCondition)
    {
        toBase().whereNot(values, condition, defaultCondition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNot(
            const QVector<WhereItem> &values, const QString &defaultCondition)
    {
        toBase().orWhereNot(values, defaultCondition);
        return builder();
    }

    /* where column */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereColumn(
            const QVector<WhereColumnItem> &values, const QString &condition)
    {
        toBase().whereColumn(values, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereColumn(const QVector<WhereColumnItem> &values)
    {
        toBase().orWhereColumn(values);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereColumn(
            const Column &first, const QString &comparison, const Column &second,
            const QString &condition)
    {
        toBase().whereColumn(first, comparison, second, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereColumn(
            const Column &first, const QString &comparison, const Column &second)
    {
        toBase().orWhereColumn(first, comparison, second);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereColumnEq(
            const Column &first, const Column &second, const QString &condition)
    {
        toBase().whereColumnEq(first, second, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereColumnEq(const Column &first, const Column &second)
    {
        toBase().orWhereColumnEq(first, second);
        return builder();
    }

    /* where IN */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereIn(
            const Column &column, const QVector<QVariant> &values,
            const QString &condition, const bool nope)
    {
        toBase().whereIn(column, values, condition, nope);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereIn(
            const Column &column, const QVector<QVariant> &values)
    {
        toBase().orWhereIn(column, values);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotIn(
            const Column &column, const QVector<QVariant> &values,
            const QString &condition)
    {
        toBase().whereNotIn(column, values, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotIn(
            const Column &column, const QVector<QVariant> &values)
    {
        toBase().orWhereNotIn(column, values);
        return builder();
    }

    /* where null */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNull(
            const QVector<Column> &columns, const QString &condition, const bool nope)
    {
        toBase().whereNull(columns, condition, nope);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNull(const QVector<Column> &columns)
    {
        toBase().orWhereNull(columns);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotNull(
            const QVector<Column> &columns, const QString &condition)
    {
        toBase().whereNotNull(columns, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotNull(const QVector<Column> &columns)
    {
        toBase().orWhereNotNull(columns);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNull(
            const Column &column, const QString &condition, const bool nope)
    {
        toBase().whereNull(column, condition, nope);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNull(const Column &column)
    {
        toBase().orWhereNull(column);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotNull(const Column &column, const QString &condition)
    {
        toBase().whereNotNull(column, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotNull(const Column &column)
    {
        toBase().orWhereNotNull(column);
        return builder();
    }

    /* where sub-queries */

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::where(
            C &&column, const QString &comparison, V &&value, const QString &condition)
    {
        toBase().where(std::forward<C>(column), comparison, std::forward<V>(value),
                       condition);
        return builder();
    }

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhere(C &&column, const QString &comparison, V &&value)
    {
        toBase().where(std::forward<C>(column), comparison, std::forward<V>(value), OR);
        return builder();
    }

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereEq(C &&column, V &&value, const QString &condition)
    {
        toBase().where(std::forward<C>(column), EQ, std::forward<V>(value), condition);
        return builder();
    }

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereEq(C &&column, V &&value)
    {
        toBase().where(std::forward<C>(column), EQ, std::forward<V>(value), OR);
        return builder();
    }

    /* where not sub-queries */

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNot(
            C &&column, const QString &comparison, V &&value, const QString &condition)
    {
        toBase().whereNot(std::forward<C>(column), comparison, std::forward<V>(value),
                          condition);
        return builder();
    }

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNot(C &&column, const QString &comparison, V &&value)
    {
        toBase().orWhereNot(std::forward<C>(column), comparison, std::forward<V>(value));
        return builder();
    }

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotEq(C &&column, V &&value, const QString &condition)
    {
        toBase().whereNotEq(std::forward<C>(column), std::forward<V>(value), condition);
        return builder();
    }

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotEq(C &&column, V &&value)
    {
        toBase().orWhereNotEq(std::forward<C>(column), std::forward<V>(value));
        return builder();
    }

    template<typename Model>
    template<WhereValueSubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereSub(
            const Column &column, const QString &comparison, T &&query,
            const QString &condition)
    {
        toBase().whereSub(column, comparison, std::forward<T>(query), condition);
        return builder();
    }

    /* where exists */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereExists(
            const std::function<void(QueryBuilder &)> &callback,
            const QString &condition, const bool nope)
    {
        toBase().whereExists(callback, condition, nope);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereExists(
            const std::function<void(QueryBuilder &)> &callback, const bool nope)
    {
        toBase().whereExists(callback, OR, nope);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotExists(
            const std::function<void(QueryBuilder &)> &callback,
            const QString &condition)
    {
        toBase().whereExists(callback, condition, true);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotExists(
            const std::function<void(QueryBuilder &)> &callback)
    {
        toBase().whereExists(callback, OR, true);
        return builder();
    }

    /* where raw */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereRaw(
            const QString &sql, const QVector<QVariant> &bindings,
            const QString &condition)
    {
        toBase().whereRaw(sql, bindings, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereRaw(
            const QString &sql, const QVector<QVariant> &bindings)
    {
        toBase().whereRaw(sql, bindings, OR);
        return builder();
    }

    /* Group by and having */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::groupBy(const QVector<Column> &groups)
    {
        toBase().groupBy(groups);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::groupBy(const Column &group)
    {
        toBase().groupBy(group);
        return builder();
    }

    template<typename Model>
    template<ColumnConcept ...Args>
    TinyBuilder<Model> &
    BuilderProxies<Model>::groupBy(Args &&...groups)
    {
        toBase().groupBy(QVector<Column> {std::forward<Args>(groups)...});
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::groupByRaw(
            const QString &sql, const QVector<QVariant> &bindings)
    {
        toBase().groupByRaw(sql, bindings);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::having(
            const Column &column, const QString &comparison, const QVariant &value,
            const QString &condition)
    {
        toBase().having(column, comparison, value, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orHaving(
            const Column &column, const QString &comparison, const QVariant &value)
    {
        toBase().orHaving(column, comparison, value);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::havingRaw(
            const QString &sql, const QVector<QVariant> &bindings,
            const QString &condition)
    {
        toBase().havingRaw(sql, bindings, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orHavingRaw(
            const QString &sql, const QVector<QVariant> &bindings)
    {
        toBase().havingRaw(sql, bindings, OR);
        return builder();
    }

    /* Ordering */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orderBy(const Column &column, const QString &direction)
    {
        toBase().orderBy(column, direction);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orderByDesc(const Column &column)
    {
        toBase().orderByDesc(column);
        return builder();
    }

    template<typename Model>
    template<Queryable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orderBy(T &&query, const QString &direction)
    {
        toBase().orderBy(std::forward<T>(query), direction);
        return builder();
    }

    template<typename Model>
    template<Queryable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orderByDesc(T &&query)
    {
        toBase().orderBy(std::forward<T>(query), DESC);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::inRandomOrder(const QString &seed)
    {
        toBase().inRandomOrder(seed);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orderByRaw(
            const QString &sql, const QVector<QVariant> &bindings)
    {
        toBase().orderByRaw(sql, bindings);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::latest(const Column &column)
    {
        toBase().latest(builder().getCreatedAtColumnForLatestOldest(column));
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::oldest(const Column &column)
    {
        toBase().oldest(builder().getCreatedAtColumnForLatestOldest(column));
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::reorder()
    {
        toBase().reorder();
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::reorder(const Column &column, const QString &direction)
    {
        toBase().reorder(column, direction);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::limit(const int value)
    {
        toBase().limit(value);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::take(const int value)
    {
        return limit(value);
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::offset(const int value)
    {
        toBase().offset(value);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::skip(const int value)
    {
        return offset(value);
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::forPage(const int page, const int perPage)
    {
        toBase().forPage(page, perPage);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::forPageBeforeId(const int perPage, const QVariant &lastId,
                                           const QString &column, const bool prependOrder)
    {
        toBase().forPageBeforeId(perPage, lastId, column, prependOrder);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::forPageAfterId(const int perPage, const QVariant &lastId,
                                          const QString &column, const bool prependOrder)
    {
        toBase().forPageAfterId(perPage, lastId, column, prependOrder);
        return builder();
    }

    /* Others */

    template<typename Model>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    BuilderProxies<Model>::increment(
            const QString &column, const T amount, const QVector<UpdateItem> &extra) const
    {
        return toBase().increment(column, amount, builder().addUpdatedAtColumn(extra));
    }

    template<typename Model>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    BuilderProxies<Model>::decrement(
            const QString &column, const T amount, const QVector<UpdateItem> &extra) const
    {
        return toBase().decrement(column, amount, builder().addUpdatedAtColumn(extra));
    }

    /* Pessimistic Locking */

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::lockForUpdate()
    {
        toBase().lock(true);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::sharedLock()
    {
        toBase().lock(false);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::lock(const bool value)
    {
        toBase().lock(value);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::lock(const char *value)
    {
        toBase().lock(value);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::lock(const QString &value)
    {
        toBase().lock(value);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::lock(QString &&value)
    {
        toBase().lock(std::move(value));
        return builder();
    }

    /* Debugging */

    template<typename Model>
    void BuilderProxies<Model>::dump(const bool replaceBindings,
                                     const bool simpleBindings) const
    {
        toBase().dump(replaceBindings, simpleBindings);
    }

    template<typename Model>
    void BuilderProxies<Model>::dd(const bool replaceBindings,
                                   const bool simpleBindings) const
    {
        toBase().dd(replaceBindings, simpleBindings);
    }

    /* Others proxy methods, not added to the Model and Relation */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::addWhereExistsQuery(
            const std::shared_ptr<QueryBuilder> &query, const QString &condition,
            const bool nope)
    {
        toBase().addWhereExistsQuery(query, condition, nope);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::mergeWheres(
            const QVector<WhereConditionItem> &wheres, const QVector<QVariant> &bindings)
    {
        toBase().mergeWheres(wheres, bindings);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::mergeWheres(
            QVector<WhereConditionItem> &&wheres, QVector<QVariant> &&bindings)
    {
        toBase().mergeWheres(std::move(wheres), std::move(bindings));
        return builder();
    }

    /* private */

    template<typename Model>
    const TinyBuilder<Model> &BuilderProxies<Model>::builder() const
    {
        return static_cast<const TinyBuilder<Model> &>(*this);
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::builder()
    {
        return static_cast<TinyBuilder<Model> &>(*this);
    }

    template<typename Model>
    QueryBuilder &BuilderProxies<Model>::toBase() const
    {
        return builder().getQuery();
    }

} // namespace Tiny
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_TINYBUILDERPROXIES_HPP
