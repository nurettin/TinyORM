#pragma once
#ifndef TINYUTILS_DATABASES_HPP
#define TINYUTILS_DATABASES_HPP

#include <memory>

#include "orm/support/databaseconfiguration.hpp"

#include "export.hpp"

namespace Orm
{
    class DatabaseManager;
}

namespace TestUtils
{
    /* I will not create a separate constants file for these two constants, define them
       here is ok because every test that will include databases.hpp will also use
       one of these constants. */
    inline const QString AutoTestSkipped =
            QStringLiteral("%1 autotest skipped, environment variables "
                           "for '%2' connection have not been defined.");
    inline const QString AutoTestSkippedAny =
            QStringLiteral("%1 autotest skipped, environment variables "
                           "for ANY connection have not been defined.");

    class TINYUTILS_EXPORT Databases
    {
        Q_DISABLE_COPY(Databases)

        /*! Type used for Database Connections map. */
        using ConfigurationsType = Orm::Support::DatabaseConfiguration
                                               ::ConfigurationsType;

    public:
        /*! MySQL connection name. */
        inline static const QString MYSQL      = "tinyorm_mysql_tests";
        /*! SQLite connection name. */
        inline static const QString SQLITE     = "tinyorm_sqlite_tests";
        /*! PostgreSQL connection name. */
        inline static const QString POSTGRESQL = "tinyorm_pgsql_tests";

        /*! Create all database connections which will be tested. */
        static const QStringList &
        createConnections(const QStringList &connections = {});
        /*! Create database connection. */
        static QString createConnection(const QString &connection);

        /*! Check whether all env. variables are empty. */
        static bool allEnvVariablesEmpty(const std::vector<const char *> &envVariables);

        /*! Get a reference to the database manager. */
        static const std::shared_ptr<Orm::DatabaseManager> &manager();

    private:
        /*! Create database configurations hash. */
        static const ConfigurationsType &
        createConfigurationsHash(const QStringList &connections);

        /*! Create MySQL configuration hash. */
        static std::pair<std::reference_wrapper<const QVariantHash>, bool>
        mysqlConfiguration();
        /*! Create SQLite configuration hash. */
        static std::pair<std::reference_wrapper<const QVariantHash>, bool>
        sqliteConfiguration();
        /*! Create PostgreSQL configuration hash. */
        static std::pair<std::reference_wrapper<const QVariantHash>, bool>
        postgresConfiguration();

        /*! Throw exception when database connections were already initialized. */
        static void throwIfConnectionsInitialized();
    };

} // namespace TestUtils

#endif // TINYUTILS_DATABASES_HPP
