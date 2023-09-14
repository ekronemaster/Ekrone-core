// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2019, The Ekrone Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <array>
#include <memory>
#include <cstdint>

#include <boost/optional.hpp>

#include "inode.h"
#include "itransaction.h"

namespace cryptonote
{

    struct BlockchainInterval
    {
        uint32_t startHeight;
        std::vector<crypto::Hash> blocks;
    };

    struct CompleteBlock
    {
        crypto::Hash blockHash;
        boost::optional<cryptonote::BlockTemplate> block;
        // first transaction is always coinbase
        std::list<std::shared_ptr<ITransactionReader>> transactions;
    };

}
