#ifndef UTILS_STRING_H
#define UTILS_STRING_H

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils::String
{
    /*! Convert a string to snake case. */
    QString toSnake(const QString &string);

} // namespace Orm::Utils::String
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // UTILS_STRING_H
