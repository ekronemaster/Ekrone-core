// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2019, The Ekrone Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <unordered_set>

#include "cryptonote_protocol/cryptonote_protocol_definitions.h"

namespace cryptonote
{
    struct PendingLiteBlock
    {
        NOTIFY_NEW_LITE_BLOCK_request request;
        std::unordered_set<crypto::Hash> missed_transactions;
    };
} // namespace cryptonote
