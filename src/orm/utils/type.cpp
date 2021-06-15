#include "orm/utils/type.hpp"

#include <QRegularExpression>

#include "orm/runtimeerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils
{

QString Type::prettyFunction(const QString &function)
{
    /* I can leave RegEx here because this function is used only during throwing
       exceptions, so there would not be any performance benefit. */
#ifdef __GNUG__
    QRegularExpression re(QStringLiteral(
                              "(?:.* )?(?:.*::)?(\\w+)(?:<.*>)?::(\\w+)\\(.*\\)"));
#elif _MSC_VER
    QRegularExpression re(QStringLiteral(
                              "(?:.*::)?(\\w+)(?:<.*>)?::(\\w+)(?:$|::<lambda)"));
#else
    throw RuntimeError("Unsupported compiler in Utils::Type::prettyFunction().");
#endif

    const auto match = re.match(function);

    // This should never happen, but who knows 🤔
    Q_ASSERT_X(match.hasMatch(), "regex match",
               "Can not get the function name in Utils::Type::prettyFunction().");
    Q_ASSERT_X(re.captureCount() == 2, "regex match",
               "Can not get the function name in Utils::Type::prettyFunction().");

    return QStringLiteral("%1::%2").arg(match.captured(1), match.captured(2));
}

QString
Type::classPureBasenameInternal(const std::type_info &typeInfo, const bool withNamespace)
{
#ifdef _MSC_VER
    return classPureBasenameMsvc(typeInfo.name(), withNamespace);
#elif __GNUG__
    // Demangle a type name
    int status;
    const auto typeName_ = abi::__cxa_demangle(typeInfo.name(), nullptr, nullptr,
                                               &status);
    const QString typeName(typeName_);
    // CUR check by valgrind silverqx
    free(typeName_);

    // CUR throw on status != 0 silverqx

    return classPureBasenameGcc(typeName, withNamespace);
#else
    throw RuntimeError(
                "Unsupported compiler in Utils::Type::classPureBasenameInternal().");
#endif
}

QString
Type::classPureBasenameMsvc(const QString &className, const bool withNamespace)
{
    auto findBeginWithoutNS = [&className]
    {
        return className.indexOf(QChar(' ')) + 1;
    };

    // Find the beginning of the class name
    auto itBegin = className.cbegin();

    // Include the namespace in the result
    if (withNamespace)
        itBegin += findBeginWithoutNS();

    // Doesn't contain the namespace
    else if (auto toBegin = className.lastIndexOf(QStringLiteral("::"));
             toBegin == -1
    )
        itBegin += findBeginWithoutNS();

    // Have the namespace and :: found, +2 to point after
    else
        itBegin += toBegin + 2;

    // Find the end of the class name
    auto itEnd = std::find_if(itBegin, className.cend(),
                              [](const QChar ch)
    {
        // The class name can end with < or space, anything else
        return ch == QChar('<') || ch == QChar(' ');
    });

    return QStringView(itBegin, itEnd).toString();
}

QString
Type::classPureBasenameGcc(const QString &className, const bool withNamespace)
{
    // Find the beginning of the class name
    auto itBegin = className.cbegin();

    if (!withNamespace)
        // Have the namespace and :: found, +2 to point after
        if (auto toBegin = className.lastIndexOf(QStringLiteral("::")); toBegin != -1)
            itBegin += toBegin + 2;

    // Find the end of the class name
    auto itEnd = std::find_if(itBegin, className.cend(),
                              [](const QChar ch)
    {
        // The class name can end with <, * or space, anything else
        return ch == QChar('<') || ch == QChar(' ') || ch == QChar('*');
    });

    return QStringView(itBegin, itEnd).toString();
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
