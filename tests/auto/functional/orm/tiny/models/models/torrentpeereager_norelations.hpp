#ifndef TORRENTPEEREAGER_NORELATIONS_H
#define TORRENTPEEREAGER_NORELATIONS_H

#include "orm/tiny/basemodel.hpp"

using Orm::Tiny::BaseModel;

class TorrentPeerEager_NoRelations final : public BaseModel<TorrentPeerEager_NoRelations>
{
    friend BaseModel;
    using BaseModel::BaseModel;

    /*! The table associated with the model. */
    QString u_table {"torrent_peers"};

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif
};

#endif // TORRENTPEEREAGER_NORELATIONS_H
