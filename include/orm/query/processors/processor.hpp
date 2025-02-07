#pragma once
#ifndef ORM_QUERY_PROCESSORS_PROCESSOR_HPP
#define ORM_QUERY_PROCESSORS_PROCESSOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QStringList>
#include <QtGlobal>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Types
{
    class SqlQuery;
}

namespace Query::Processors
{

    /*! Base processor class, process sql result. */
    class SHAREDLIB_EXPORT Processor
    {
        Q_DISABLE_COPY(Processor)

        /*! Alias for the SqlQuery. */
        using SqlQuery = Orm::Types::SqlQuery;

    public:
        /*! Default constructor. */
        inline Processor() = default;
        /*! Virtual destructor, this class is used so can not be pure. */
        inline virtual ~Processor() = default;

        /*! Process the results of a column listing query. */
        virtual QStringList processColumnListing(SqlQuery &query) const;
    };

} // namespace Query::Processors
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_PROCESSORS_PROCESSOR_HPP
