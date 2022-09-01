#pragma once
#ifndef MODELS_TAGGED_HPP
#define MODELS_TAGGED_HPP

#include "orm/tiny/relations/basepivot.hpp"

namespace Models
{

using Orm::Tiny::Relations::BasePivot;

class Tagged final : public BasePivot<Tagged>
{
    friend Model;
    friend BasePivot;

    using BasePivot::BasePivot;

    /*! The table associated with the model. */
    QString u_table {"tag_torrent"};

    /*! Indicates if the ID is auto-incrementing. */
//    bool u_incrementing = true;

    // FUTURE u_connection can't be overridden, the same is true in Eloquent, add support to be able to override these silveqx
};

} // namespace Models

#endif // MODELS_TAGGED_HPP
