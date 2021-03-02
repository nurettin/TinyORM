#ifndef ORMTYPES_H
#define ORMTYPES_H

#include <QSharedPointer>
#include <QVariant>
#include <QVector>

#include "export.hpp"

// TODO divide OrmTypes to internal and types which user will / may need, so divide to two files silverqx
/* 👆 I have good idea hot to do that, public types will be tinytypes.hpp and private will be
   types.hpp, and divide it as most as possible when needed, so eg Reconnector type to
   types/reconnectortype.hpp. */
// TODO JSON_USE_IMPLICIT_CONVERSIONS=0 silverqx

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    enum struct BindingType
    {
        SELECT,
        FROM,
        JOIN,
        WHERE,
        GROUPBY,
        HAVING,
        ORDER,
        UNION,
        UNIONORDER,
    };

    // TODO have to be QMap<BindingType, QVector<BindingValue>> to correctly support Expressions silverqx
    using BindingsMap = QMap<BindingType, QVector<QVariant>>;

    enum struct WhereType
    {
        UNDEFINED = -1,
        BASIC,
        NESTED,
        COLUMN,
        IN_,
        NOT_IN,
        NULL_,
        NOT_NULL,
    };

namespace Query
{
    class Builder;
}
    using QueryBuilder = Query::Builder;

    struct WhereConditionItem
    {
        QString                      column;
        QVariant                     value       {};
        QString                      comparison  {"="};
        QString                      condition   {"and"};
        WhereType                    type        {WhereType::UNDEFINED};
        QSharedPointer<QueryBuilder> nestedQuery {nullptr};
        QVector<QVariant>            values      {};
    };

    enum struct HavingType
    {
        UNDEFINED = -1,
        BASIC,
    };
    struct HavingConditionItem
    {
        QString    column;
        QVariant   value;
        QString    comparison {"="};
        QString    condition  {"and"};
        HavingType type       {HavingType::UNDEFINED};
    };

    struct AssignmentListItem
    {
        QString  column;
        QVariant value;
    };

    class AssignmentList final : public QVector<AssignmentListItem>
    {
    public:
        // Inherit all base class constructors, wow 😲✨
        using QVector<AssignmentListItem>::QVector;

        AssignmentList(const QVariantHash &variantHash);
    };

    struct OrderByItem
    {
        QString column;
        QString direction {"asc"};
    };

    struct SHAREDLIB_EXPORT UpdateItem
    {
        QString  column;
        QVariant value;
    };

    struct ResultItem
    {
        QString  column;
        QVariant value;
    };

    // TODO types, also divide types by namespace, eg AttributeItem is only used in the Orm::Tiny namespace, so an user can use 'using namespace Orm::Tiny' in model files, it is not possible now, because he has to use symbols from an Orm namespace too silverqx
    // TODO pretty print in the debugger silverqx
    struct SHAREDLIB_EXPORT AttributeItem
    {
        QString  key;
        QVariant value;
    };

    SHAREDLIB_EXPORT bool operator==(const AttributeItem &lhs, const AttributeItem &rhs);

    struct SHAREDLIB_EXPORT WhereItem
    {
        QString  column;
        QVariant value;
        QString  comparison {"="};
        QString  condition  {};

        operator AttributeItem() const;
    };

    struct WhereColumnItem
    {
        QString first;
        QString second;
        QString comparison {"="};
        QString condition  {};
    };

    // TODO types, belongs to Orm::Tiny namespace silverqx
    /*! Eager load relation item. */
    struct WithItem
    {
        QString               name;
        std::function<void()> constraints {nullptr};
    };

//    bool operator==(const WithItem &lhs, const WithItem &rhs);

    /*! Tag for BaseModel::getRelation() family methods to return Related type directly ( not container type ). */
    struct One {};
    // TODO Many internal type only for now silverqx
    /*! Tag for BaseModel::getRelationshipFromMethod() to return QVector<Related> type ( 'Many' relation types ). */
    struct Many {};

    /*! Options parameter type used in Model save() method. */
    struct SHAREDLIB_EXPORT SaveOptions
    {
        /*! Indicates if timestamps of parent models should be touched. */
        bool touch = true;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#ifdef TINYORM_COMMON_NAMESPACE
Q_DECLARE_METATYPE(TINYORM_COMMON_NAMESPACE::Orm::WhereConditionItem);
#else
Q_DECLARE_METATYPE(Orm::WhereConditionItem);
#endif

#endif // ORMTYPES_H
