#ifndef EXPRESSION_H
#define EXPRESSION_H

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    // TODO rework saveing Expressions to the "BindingsMap m_bindings", see also todo at BindingsMap definition in ormtypes.h silverqx
    class Expression
    {
    public:
        Expression() = default;
        ~Expression() = default;
        Expression(const QVariant &value);

        Expression(const Expression &) = default;
        Expression &operator=(const Expression &) = default;

        operator QVariant() const;

        inline QVariant getValue() const
        { return m_value; }

    private:
        QVariant m_value;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#ifdef TINYORM_COMMON_NAMESPACE
Q_DECLARE_METATYPE(TINYORM_COMMON_NAMESPACE::Orm::Expression);
#else
Q_DECLARE_METATYPE(Orm::Expression);
#endif

#endif // EXPRESSION_H
