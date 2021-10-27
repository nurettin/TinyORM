#pragma once
#ifndef ORM_EXPRESSION_HPP
#define ORM_EXPRESSION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>
#include <QVector>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query
{

    /*! Expression in sql query. */
    class SHAREDLIB_EXPORT Expression
    {
    public:
        /*! Default constructor, needed by Q_DECLARE_METATYPE. */
        Expression();
        /*! Destructor. */
        ~Expression() = default;

        /*! Converting constructor from QVariant type. */
        explicit Expression(const QVariant &value);
        /*! Converting constructor from QVariant type. */
        explicit Expression(QVariant &&value);

        /*! Copy constructor. */
        Expression(const Expression &) = default;
        /*! Copy assignment operator. */
        Expression &operator=(const Expression &) = default;

        /*! Move constructor. */
        Expression(Expression &&) = default;
        /*! Move assignment operator. */
        Expression &operator=(Expression &&) = default;

        /*! Converting operator, QVariant(Expression). */
        operator QVariant() const; // NOLINT(google-explicit-constructor)

        /*! Obtain expression's value. */
        const QVariant &getValue() const;

        /*! Equality operator, the inequality operator is automatically generated. */
        bool operator==(const Expression &) const = default;

    private:
        /*! Expression's value. */
        QVariant m_value;
    };

    inline const QVariant &Expression::getValue() const
    {
        return m_value;
    }

} // namespace Orm::Query

TINYORM_END_COMMON_NAMESPACE

#ifdef TINYORM_COMMON_NAMESPACE
// NOLINTNEXTLINE(performance-no-int-to-ptr, misc-no-recursion)
Q_DECLARE_METATYPE(TINYORM_COMMON_NAMESPACE::Orm::Query::Expression)
#else
// NOLINTNEXTLINE(performance-no-int-to-ptr, misc-no-recursion)
Q_DECLARE_METATYPE(Orm::Query::Expression)
#endif

#endif // ORM_EXPRESSION_HPP
