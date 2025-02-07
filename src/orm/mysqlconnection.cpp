#include "orm/mysqlconnection.hpp"

#ifdef TINYORM_MYSQL_PING
#  include <QDebug>
#endif
#include <QVersionNumber>
#include <QtSql/QSqlDriver>

#ifdef TINYORM_MYSQL_PING
#  ifdef __MINGW32__
#    include <mysql/errmsg.h>
#  elif __has_include(<errmsg.h>)
#    include <errmsg.h>
#  endif
#  if __has_include(<mysql/mysql.h>)
#    include <mysql/mysql.h>
#  elif __has_include(<mysql.h>)
#    include <mysql.h>
#  else
#    error Can not find <mysql.h> header file, install the MySQL C client library or \
disable TINYORM_MYSQL_PING preprocessor directive.
#  endif
#endif

#include "orm/query/grammars/mysqlgrammar.hpp"
#include "orm/query/processors/mysqlprocessor.hpp"
#include "orm/schema/grammars/mysqlschemagrammar.hpp"
#include "orm/schema/mysqlschemabuilder.hpp"
#include "orm/utils/configuration.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using ConfigUtils = Orm::Utils::Configuration;

namespace Orm
{

/* public */

MySqlConnection::MySqlConnection(
        std::function<Connectors::ConnectionName()> &&connection,
        QString &&database, QString &&tablePrefix, QtTimeZoneConfig &&qtTimeZone,
        QVariantHash &&config
)
    : DatabaseConnection(
          std::move(connection), std::move(database), std::move(tablePrefix),
          std::move(qtTimeZone), std::move(config))
    , m_isMaria(std::nullopt)
{
    /* We need to initialize a query grammar that is a very important part
       of the database abstraction, so we initialize it to the default value
       while starting. */
    useDefaultQueryGrammar();

    useDefaultPostProcessor();
}

std::unique_ptr<SchemaBuilder> MySqlConnection::getSchemaBuilder()
{
    if (!m_schemaGrammar)
        useDefaultSchemaGrammar();

    return std::make_unique<SchemaNs::MySqlSchemaBuilder>(*this);
}

/* Getters / Setters */

std::optional<QString> MySqlConnection::version()
{
    auto configVersionValue = ConfigUtils::getValidConfigVersion(m_config);

    /* Default values is std::nullopt if pretending and the database config. doesn't
       contain a valid version value. */
    if (m_pretending && !m_version && configVersionValue.isEmpty())
        return std::nullopt;

    // Return the cached value
    if (m_version)
        return m_version;

    // A user can provide the version through the configuration to save one DB query
    if (!configVersionValue.isEmpty())
        return m_version = std::move(configVersionValue);

    // Obtain and cache the database version value
    return m_version = selectOne(QStringLiteral("select version()")).value(0)
                       .value<QString>();
}

bool MySqlConnection::isMaria()
{
    // Default values is false if pretending and the config. version was not set manually
    if (m_pretending && !m_isMaria && !m_version &&
        !ConfigUtils::hasValidConfigVersion(m_config)
    )
        return false;

    // Return the cached value
    if (m_isMaria)
        return *m_isMaria;

    // Obtain a version from the database if needed
    version();

    // This should never happen 🤔 because of the condition at beginning
    if (!m_version)
        return false;

    // Cache the value
    m_isMaria = m_version->contains(QStringLiteral("MariaDB"));

    return *m_isMaria;
}

bool MySqlConnection::useUpsertAlias()
{
    // Default values is true if pretending and the config. version was not set manually
    if (m_pretending && !m_useUpsertAlias && !m_version &&
        !ConfigUtils::hasValidConfigVersion(m_config)
    )
        // FUTURE useUpsertAlias() default value to true after MySQL 8.0 will be end-of-life silverqx
        return false;

    // Return the cached value
    if (m_useUpsertAlias)
        return *m_useUpsertAlias;

    // Obtain a version from the database if needed
    version();

    // This should never happen 🤔 because of the condition at beginning
    if (!m_version)
        return false;

    /* The MySQL >=8.0.19 supports aliases in the VALUES and SET clauses
       of INSERT INTO ... ON DUPLICATE KEY UPDATE statement for the row to be
       inserted and its columns. It also generates warning if old style used.
       So set it to true to avoid this warning. */

    // Cache the value
    m_useUpsertAlias = QVersionNumber::fromString(*m_version) >=
                       QVersionNumber(8, 0, 19);

    return *m_useUpsertAlias;
}

#ifdef TINYORM_TESTS_CODE
void MySqlConnection::setConfigVersion(QString value) // NOLINT(performance-unnecessary-value-param)
{
    // Override it through the config., this ensure that more code branches will be tested
    const_cast<QVariantHash &>(m_config).insert(Version, std::move(value));

    // We need to reset these to recomputed them again
    m_version = std::nullopt;
    m_isMaria = std::nullopt;
    m_useUpsertAlias = std::nullopt;
}
#endif

/* Others */

bool MySqlConnection::pingDatabase()
{
#ifdef TINYORM_MYSQL_PING
    auto qtConnection = getQtConnection();

    const auto getMysqlHandle = [&qtConnection]() -> MYSQL *
    {
        auto driverHandle = qtConnection.driver()->handle();
        const auto *typeName = driverHandle.typeName();

        // MYSQL* for MySQL client and st_mysql* for MariaDB client
        if (qstrcmp(typeName, "MYSQL*") == 0 || qstrcmp(typeName, "st_mysql*") == 0)
            return *static_cast<MYSQL **>(driverHandle.data());

        return nullptr;
    };

    const auto mysqlPing = [getMysqlHandle]() -> bool
    {
        auto *mysqlHandle = getMysqlHandle();
        if (mysqlHandle == nullptr)
            return false;

        const auto ping = mysql_ping(mysqlHandle);
        const auto errNo = mysql_errno(mysqlHandle);

        /* So strange logic, because I want interpret CR_COMMANDS_OUT_OF_SYNC errno as
           successful ping. */
        if (ping != 0 && errNo == CR_COMMANDS_OUT_OF_SYNC) {
            // TODO log to file, how often this happen silverqx
            qWarning("mysql_ping() returned : CR_COMMANDS_OUT_OF_SYNC(%ud)", errNo);
            return true;
        }

        if (ping == 0)
            return true;
        if (ping != 0)
            return false;

        qWarning() << "Unknown behavior during mysql_ping(), this should never "
                      "happen :/";
        return false;
    };

    if (qtConnection.isOpen() && mysqlPing()) {
        logConnected();
        return true;
    }

    // The database connection was lost
    logDisconnected();

    // Database connection have to be closed manually
    // isOpen() check is called in MySQL driver
    qtConnection.close();

    // Reset in transaction state and the savepoints counter
    resetTransactions();

    return false;
#else
    throw Exceptions::RuntimeError(
                QStringLiteral(
                    "pingDatabase() method was disabled for the '%1' database driver, "
                    "if you want to use MySqlConnection::pingDatabase(), then "
                    "reconfigure the TinyORM project with the MYSQL_PING preprocessor "
                    "macro ( -DMYSQL_PING ) for cmake or with the 'mysql_ping' "
                    "configuration option ( \"CONFIG+=mysql_ping\" ) for qmake.")
                .arg(driverName()));
#endif
}

/* protected */

std::unique_ptr<QueryGrammar> MySqlConnection::getDefaultQueryGrammar() const
{
    // Ownership of a unique_ptr()
    auto grammar = std::make_unique<Query::Grammars::MySqlGrammar>();

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<SchemaGrammar> MySqlConnection::getDefaultSchemaGrammar() const
{
    // Ownership of a unique_ptr()
    auto grammar = std::make_unique<SchemaNs::Grammars::MySqlSchemaGrammar>();

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<QueryProcessor> MySqlConnection::getDefaultPostProcessor() const
{
    return std::make_unique<Query::Processors::MySqlProcessor>();
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

// TEST now add MariaDB tests, install mariadb add connection and run all the tests against mariadb too silverqx
