#pragma once
#ifndef MODELS_TYPE_HPP
#define MODELS_TYPE_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::CastItem;
using Orm::Tiny::Model;

// NOLINTNEXTLINE(bugprone-exception-escape)
class Type final : public Model<Type>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"types"};

    /*! The attributes that should be cast. */
    std::unordered_map<QString, CastItem> u_casts {};
};

} // namespace Models

#endif // MODELS_TYPE_HPP
