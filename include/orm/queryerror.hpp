#ifndef QUERYERROR_H
#define QUERYERROR_H

#include "orm/sqlerror.hpp"

class QSqlQuery;

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class QueryError : public SqlError
    {
    public:
        QueryError(const char *message, const QSqlQuery &query,
                   const QVector<QVariant> &bindings);
        QueryError(const QString &message, const QSqlQuery &query,
                   const QVector<QVariant> &bindings);

        /*! Get the SQL for the query. */
        const QString &getSql() const;
        /*! Get the bindings for the query. */
        const QVector<QVariant> &getBindings() const;

    protected:
        /*! Format the Qt SQL error message. */
        QString formatMessage(const char *message, const QSqlQuery &query);

        /*! The SQL for the query. */
        const QString m_sql;
        /*! The bindings for the query. */
        const QVector<QVariant> m_bindings;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // QUERYERROR_H
