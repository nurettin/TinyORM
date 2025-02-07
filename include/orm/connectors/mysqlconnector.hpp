#pragma once
#ifndef ORM_CONNCECTORS_MYSQLCONNECTOR_HPP
#define ORM_CONNCECTORS_MYSQLCONNECTOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/connectors/connector.hpp"
#include "orm/connectors/connectorinterface.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Connectors
{

    /*! MySql connector. */
    class MySqlConnector final : public ConnectorInterface,
                                 public Connector
    {
        Q_DISABLE_COPY(MySqlConnector)

    public:
        /*! Default constructor. */
        inline MySqlConnector() = default;
        /*! Virtual destructor. */
        inline ~MySqlConnector() final = default;

        /*! Establish a database connection. */
        ConnectionName connect(const QVariantHash &config) const override;

        /*! Get the QSqlDatabase connection options for the current connector. */
        const QVariantHash &getConnectorOptions() const override;

    protected:
        /*! Set the connection transaction isolation level. */
        static void configureIsolationLevel(const QSqlDatabase &connection,
                                            const QVariantHash &config);
        /*! Set the connection character set and collation. */
        static void configureEncoding(const QSqlDatabase &connection,
                                      const QVariantHash &config);
        /*! Get the collation for the configuration. */
        static QString getCollation(const QVariantHash &config);
        /*! Set the timezone on the connection. */
        static void configureTimezone(const QSqlDatabase &connection,
                                      const QVariantHash &config);

        /*! Set the modes for the connection. */
        static void setModes(const QSqlDatabase &connection,
                             const QVariantHash &config);
        /*! Get the query to enable strict mode. */
        static QString strictMode(const QSqlDatabase &connection,
                                  const QVariantHash &config);
        /*! Get the MySql server version. */
        static QString getMySqlVersion(const QSqlDatabase &connection,
                                       const QVariantHash &config);
        /*! Set the custom modes on the connection. */
        static void setCustomModes(const QSqlDatabase &connection,
                                   const QVariantHash &config);

    private:
        /*! The default QSqlDatabase connection options for the MySqlConnector. */
        inline static const QVariantHash m_options {
            {QLatin1String("MYSQL_OPT_RECONNECT"), 0},
        };
    };

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNCECTORS_MYSQLCONNECTOR_HPP
