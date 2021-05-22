include(../include/include.pri)

SOURCES += \
    $$PWD/orm/basegrammar.cpp \
    $$PWD/orm/concerns/detectslostconnections.cpp \
    $$PWD/orm/concerns/hasconnectionresolver.cpp \
    $$PWD/orm/connectors/connectionfactory.cpp \
    $$PWD/orm/connectors/connector.cpp \
    $$PWD/orm/connectors/mysqlconnector.cpp \
    $$PWD/orm/connectors/sqliteconnector.cpp \
    $$PWD/orm/databaseconnection.cpp \
    $$PWD/orm/databasemanager.cpp \
    $$PWD/orm/db.cpp \
    $$PWD/orm/logquery.cpp \
    $$PWD/orm/mysqlconnection.cpp \
    $$PWD/orm/ormtypes.cpp \
    $$PWD/orm/query/expression.cpp \
    $$PWD/orm/query/grammars/grammar.cpp \
    $$PWD/orm/query/grammars/mysqlgrammar.cpp \
    $$PWD/orm/query/grammars/sqlitegrammar.cpp \
    $$PWD/orm/query/joinclause.cpp \
    $$PWD/orm/query/processors/mysqlprocessor.cpp \
    $$PWD/orm/query/processors/processor.cpp \
    $$PWD/orm/query/processors/sqliteprocessor.cpp \
    $$PWD/orm/query/querybuilder.cpp \
    $$PWD/orm/queryerror.cpp \
    $$PWD/orm/schema/grammars/mysqlschemagrammar.cpp \
    $$PWD/orm/schema/grammars/schemagrammar.cpp \
    $$PWD/orm/schema/grammars/sqliteschemagrammar.cpp \
    $$PWD/orm/schema/mysqlschemabuilder.cpp \
    $$PWD/orm/schema/schemabuilder.cpp \
    $$PWD/orm/schema/sqliteschemabuilder.cpp \
    $$PWD/orm/sqlerror.cpp \
    $$PWD/orm/sqliteconnection.cpp \
    $$PWD/orm/support/configurationoptionsparser.cpp \
    $$PWD/orm/tiny/model.cpp \
    $$PWD/orm/tiny/modelnotfounderror.cpp \
    $$PWD/orm/tiny/relationnotfounderror.cpp \
    $$PWD/orm/tiny/relationnotloadederror.cpp \
    $$PWD/orm/tiny/relations/relation.cpp \
    $$PWD/orm/utils/attribute.cpp \
    $$PWD/orm/utils/string.cpp \
    $$PWD/orm/utils/type.cpp \
