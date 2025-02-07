#include <QCoreApplication>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/utils/helpers.hpp"
#include "orm/utils/nullvariant.hpp"

#include "databases.hpp"

#include "models/datetime.hpp"

using Orm::Constants::ID;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::TZ00;
using Orm::Constants::UTC;

using Orm::DB;
using Orm::QtTimeZoneConfig;
using Orm::QtTimeZoneType;
using Orm::Utils::NullVariant;

using Orm::Tiny::ConnectionOverride;
using Orm::Utils::Helpers;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

using Models::Datetime;

class tst_Model_QDateTime : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;

    /* QDateTime with/without timezone */
    /* Server timezone UTC */
    void create_QDateTime_UtcTimezone_DatetimeAttribute_UtcOnServer() const;
    void create_QDateTime_0200Timezone_DatetimeAttribute_UtcOnServer() const;
    void create_QString_DatetimeAttribute_UtcOnServer() const;
    void create_QDateTime_UtcTimezone_TimestampAttribute_UtcOnServer() const;
    void create_QDateTime_0200Timezone_TimestampAttribute_UtcOnServer() const;
    void create_QString_TimestampAttribute_UtcOnServer() const;

    /* Server timezone +02:00 */
    void create_QDateTime_UtcTimezone_DatetimeAttribute_0200OnServer() const;
    void create_QDateTime_0200Timezone_DatetimeAttribute_0200OnServer() const;
    void create_QString_DatetimeAttribute_0200OnServer() const;
    void create_QDateTime_UtcTimezone_TimestampAttribute_0200OnServer() const;
    void create_QDateTime_0200Timezone_TimestampAttribute_0200OnServer() const;
    void create_QString_TimestampAttribute_0200OnServer() const;

    /* QDate */
    /* Server timezone UTC */
    void create_QDate_UtcTimezone_DateAttribute_UtcOnServer() const;
    void create_QString_DateAttribute_UtcOnServer() const;

    /* Server timezone +02:00 */
    void create_QDate_UtcTimezone_DateAttribute_0200OnServer() const;
    void create_QString_DateAttribute_0200OnServer() const;

    /* Null values QDateTime / QDate */
    /* Server timezone UTC */
    void create_QDateTime_Null_DatetimeAttribute_UtcOnServer() const;
    void create_QDate_Null_DateAttribute_UtcOnServer() const;

    /* Server timezone +02:00 */
    void create_QDateTime_Null_DatetimeAttribute_0200OnServer() const;
    void create_QDate_Null_DateAttribute_0200OnServer() const;

    /* QtTimeZoneType::DontConvert */
    /* Server timezone UTC */
    void create_QDateTime_0300Timezone_DatetimeAttribute_UtcOnServer_DontConvert() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Set the MySQL/PostgreSQL timezone session variable to the UTC value. */
    inline static void setUtcTimezone(const QString &connection = {});
    /*! Get the UTC time zone string. */
    static const QString &utcTimezoneString(const QString &connection);
    /*! Are MySQL time zone table populated? */
    static bool mysqlTimezoneTablesNotPopulated(const QString &connection);

    /*! Set the database timezone session variable to +02:00 value by connection. */
    inline static void set0200Timezone(const QString &connection = {});
    /*! Set the MySQL timezone session variable to the +02:00 value. */
    inline static void set0200TimezoneForMySQL(const QString &connection = {});
    /*! Set the PostgreSQL timezone session variable to the +02:00 value. */
    inline static void setEUBratislavaTimezoneForPSQL(const QString &connection = {});

    /*! Set the MySQL/PostgreSQL timezone session variable to the given value. */
    static void setTimezone(const QString &timeZone, QtTimeZoneConfig &&qtTimeZone,
                            const QString &connection);
    /*! Get a SQL query string to set a database time zone session variable. */
    static QString getSetTimezoneQueryString(const QString &connection);

    /*! Get the QTimeZone +02:00 instance for MySQL/PostgreSQL. */
    inline static const QTimeZone &timezone0200(const QString &connection = {});

    /*! Restore the database after a QDateTime-related test. */
    static void restore(quint64 lastId, bool restoreTimezone = false,
                        const QString &connection = {});
};

/*! QString constant "datetime" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(QString, datetime_, ("datetime"))
/*! QString constant "timestamp" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(QString, timestamp, ("timestamp"))
/*! QString constant "date" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(QString, date, ("date"))

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Model_QDateTime::initTestCase_data() const
{
    const auto &connections = Databases::createConnections();

    if (connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                           .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

/* QDateTime with/without timezone */

/* Server timezone UTC */

void tst_Model_QDateTime::
create_QDateTime_UtcTimezone_DatetimeAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC);

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::
create_QDateTime_0200Timezone_DatetimeAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QDateTime({2022, 8, 28}, {13, 14, 15},
                                         timezone0200(connection));

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 11:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {11, 14, 15}, Qt::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::create_QString_DatetimeAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QString("2022-08-28 13:14:15");

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::
create_QDateTime_UtcTimezone_TimestampAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*timestamp] = QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC);

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*timestamp);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::
create_QDateTime_0200Timezone_TimestampAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*timestamp] = QDateTime({2022, 8, 28}, {13, 14, 15},
                                         timezone0200(connection));

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*timestamp);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 11:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {11, 14, 15}, Qt::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::create_QString_TimestampAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*timestamp] = QString("2022-08-28 13:14:15");

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*timestamp);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

/* Server timezone +02:00 */

void tst_Model_QDateTime::
create_QDateTime_UtcTimezone_DatetimeAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC);

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 15:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {15, 14, 15},
                                                timezone0200(connection));
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::
create_QDateTime_0200Timezone_DatetimeAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QDateTime({2022, 8, 28}, {13, 14, 15},
                                         timezone0200(connection));

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                timezone0200(connection));
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::create_QString_DatetimeAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QString("2022-08-28 13:14:15");

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                timezone0200(connection));
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::
create_QDateTime_UtcTimezone_TimestampAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*timestamp] = QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC);

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*timestamp);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 15:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {15, 14, 15},
                                                 timezone0200(connection));
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::
create_QDateTime_0200Timezone_TimestampAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*timestamp] = QDateTime({2022, 8, 28}, {13, 14, 15},
                                         timezone0200(connection));

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*timestamp);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                 timezone0200(connection));
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::create_QString_TimestampAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*timestamp] = QString("2022-08-28 13:14:15");

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*timestamp);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                 timezone0200(connection));
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

/* QDate */

/* Server timezone UTC */

void tst_Model_QDateTime::create_QDate_UtcTimezone_DateAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*date] = QDate(2022, 8, 28);

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*date);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::create_QString_DateAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        // Will use isStandardDateFormat()
        datetime[*date] = QString("2022-08-28");

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*date);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

/* Server timezone +02:00 */

void tst_Model_QDateTime::create_QDate_UtcTimezone_DateAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*date] = QDate(2022, 8, 28);

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*date);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::create_QString_DateAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        // Will use isStandardDateFormat()
        datetime[*date] = QString("2022-08-28");

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*date);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true, connection);
}

/* Null values QDateTime / QDate */

/* Server timezone UTC */

void tst_Model_QDateTime::create_QDateTime_Null_DatetimeAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = NullVariant::QDateTime();

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QVERIFY(attribute.isValid() && attribute.isNull());
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QDateTime);

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(datetimeDbVariant.isNull());

        /* Following is not true for QSQLITE driver:
           TinyORM Model fixes this and returns the QDateTime instead of QString.
           Would be possible to detect whether it's QSQLITE driver and a null attribute
           and on the base of that return the QVariant(QMetaType::QDateTime), but
           I have decided to leave it as it's now, I will not override this behavior.
           QSQLITE driver simply returns QVariant(QMetaType::QString) for all null
           values. */
        if (DB::driverName(connection) == QSQLITE)
            QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QString);
        else
            QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* TZ is irrelevant for null values, but I will check them anyway, if something
           weird happens and TZ changes then test fail, so I will know about that. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime();

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::create_QDate_Null_DateAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*date] = NullVariant::QDate();

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*date);
        QVERIFY(attribute.isValid() && attribute.isNull());
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QDate);

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(dateDbVariant.isNull());

        /* Following is not true for QSQLITE driver:
           TinyORM Model fixes this and returns the QDate instead of QString.
           Would be possible to detect whether it's QSQLITE driver and a null attribute
           and on the base of that return the QVariant(QMetaType::QDate), but
           I have decided to leave it as it's now, I will not override this behavior.
           QSQLITE driver simply returns QVariant(QMetaType::QString) for all null
           values. */
        if (DB::driverName(connection) == QSQLITE)
            QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QString);
        else
            QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

/* Server timezone +02:00 */

void tst_Model_QDateTime::create_QDateTime_Null_DatetimeAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = NullVariant::QDateTime();

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QVERIFY(attribute.isValid() && attribute.isNull());
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QDateTime);

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(datetimeDbVariant.isNull());

        /* Following is not true for QSQLITE driver:
           TinyORM Model fixes this and returns the QDateTime instead of QString.
           Would be possible to detect whether it's QSQLITE driver and a null attribute
           and on the base of that return the QVariant(QMetaType::QDateTime), but
           I have decided to leave it as it's now, I will not override this behavior.
           QSQLITE driver simply returns QVariant(QMetaType::QString) for all null
           values. */
        if (DB::driverName(connection) == QSQLITE)
            QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QString);
        else
            QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* TZ is irrelevant for null values, but I will check them anyway, if something
           weird happens and TZ changes then test fail, so I will know about that. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime();

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::create_QDate_Null_DateAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*date] = NullVariant::QDate();

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*date);
        QVERIFY(attribute.isValid() && attribute.isNull());
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QDate);

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(dateDbVariant.isNull());

        /* Following is not true for QSQLITE driver:
           TinyORM Model fixes this and returns the QDate instead of QString.
           Would be possible to detect whether it's QSQLITE driver and a null attribute
           and on the base of that return the QVariant(QMetaType::QDate), but
           I have decided to leave it as it's now, I will not override this behavior.
           QSQLITE driver simply returns QVariant(QMetaType::QString) for all null
           values. */
        if (DB::driverName(connection) == QSQLITE)
            QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QString);
        else
            QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true, connection);
}

/* QtTimeZoneType::DontConvert */

/* Server timezone UTC */

void tst_Model_QDateTime::
create_QDateTime_0300Timezone_DatetimeAttribute_UtcOnServer_DontConvert() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    DB::setQtTimeZone(QtTimeZoneConfig {QtTimeZoneType::DontConvert}, connection);
    QCOMPARE(DB::qtTimeZone(connection),
             QtTimeZoneConfig {QtTimeZoneType::DontConvert});

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QDateTime({2022, 8, 28}, {13, 14, 15},
                                         QTimeZone("UTC+03:00"));

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(Helpers::qVariantTypeId(attribute), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);

    DB::setQtTimeZone(QtTimeZoneConfig {QtTimeZoneType::QtTimeSpec,
                                        QVariant::fromValue(Qt::UTC)}, connection);
    QCOMPARE(DB::qtTimeZone(connection),
             (QtTimeZoneConfig {QtTimeZoneType::QtTimeSpec,
                                QVariant::fromValue(Qt::UTC)}));
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

void tst_Model_QDateTime::setUtcTimezone(const QString &connection)
{
    setTimezone(utcTimezoneString(connection),
                {QtTimeZoneType::QtTimeSpec, QVariant::fromValue(Qt::UTC)},
                connection);
}

const QString &tst_Model_QDateTime::utcTimezoneString(const QString &connection)
{
    const auto driverName = DB::driverName(connection);

    if (driverName == QMYSQL && mysqlTimezoneTablesNotPopulated(connection))
        return TZ00;

    return UTC;
}

bool
tst_Model_QDateTime::mysqlTimezoneTablesNotPopulated(const QString &connection)
{
    auto qtQuery = DB::select(
                       QStringLiteral("select count(*) from `mysql`.`time_zone_name`"),
                       {}, connection);

    if (!qtQuery.first())
        return true;

    const auto tzTableRows = qtQuery.value(0);

    // If 0 then TZ tables are empty
    return tzTableRows.value<quint64>() == 0;
}

void tst_Model_QDateTime::set0200Timezone(const QString &connection)
{
    const auto driverName = DB::driverName(connection);

    if (driverName == QMYSQL)
        set0200TimezoneForMySQL(connection);

    else if (driverName == QPSQL)
        setEUBratislavaTimezoneForPSQL(connection);

    // Do nothing for QSQLITE
}

/*! Time zone +02:00 for MySQL. */
Q_GLOBAL_STATIC_WITH_ARGS(QTimeZone, TimeZone0200, (QByteArray("UTC+02:00")))

/*! Time zone Europe/Bratislava for PostgreSQL. */
Q_GLOBAL_STATIC_WITH_ARGS(QTimeZone, TimeZoneEUBratislava,
                          (QByteArray("Europe/Bratislava")))

void tst_Model_QDateTime::set0200TimezoneForMySQL(const QString &connection)
{
    setTimezone(QStringLiteral("+02:00"),
                {QtTimeZoneType::QTimeZone, QVariant::fromValue(*TimeZone0200)},
                connection);
}

void tst_Model_QDateTime::setEUBratislavaTimezoneForPSQL(const QString &connection)
{
    setTimezone(QStringLiteral("Europe/Bratislava"),
                {QtTimeZoneType::QTimeZone, QVariant::fromValue(*TimeZoneEUBratislava)},
                connection);
}

void tst_Model_QDateTime::setTimezone(
        const QString &timeZone, Orm::QtTimeZoneConfig &&qtTimeZone,
        const QString &connection)
{
    const auto qtQuery = DB::unprepared(
                             getSetTimezoneQueryString(connection).arg(timeZone),
                             connection);

    QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());

    DB::setQtTimeZone(std::move(qtTimeZone), connection);
}

QString tst_Model_QDateTime::getSetTimezoneQueryString(const QString &connection)
{
    const auto driverName = DB::driverName(connection);

    if (driverName == QMYSQL)
        return QStringLiteral("set time_zone=\"%1\";");

    if (driverName == QPSQL)
        return QStringLiteral("set time zone '%1';");

    Q_UNREACHABLE();
}

const QTimeZone &tst_Model_QDateTime::timezone0200(const QString &connection)
{
    const auto driverName = DB::driverName(connection);

    if (driverName == QMYSQL || driverName == QSQLITE)
        return *TimeZone0200;

    if (driverName == QPSQL)
        return *TimeZoneEUBratislava;

    Q_UNREACHABLE();
}

void tst_Model_QDateTime::restore(const quint64 lastId, const bool restoreTimezone,
                                  const QString &connection)
{
    const auto affected = Datetime::destroy(lastId);

    QCOMPARE(affected, 1);

    if (!restoreTimezone)
        return;

    /* Restore also the MySQL/PostgreSQL timezone session variable to auto tests
       default UTC value. */
    if (!connection.isEmpty() && DB::driverName(connection) != QSQLITE)
        setUtcTimezone(connection);
}

QTEST_MAIN(tst_Model_QDateTime)

#include "tst_model_qdatetime.moc"
