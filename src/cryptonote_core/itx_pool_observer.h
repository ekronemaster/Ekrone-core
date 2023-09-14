// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2019, The Ekrone Developers
//
// Please see the included LICENSE file for more information.

#pragma once

namespace cryptonote
{
    class ITxPoolObserver
    {
    public:
        virtual ~ITxPoolObserver()
        {
        }

        virtual void txDeletedFromPool() = 0;
    };
}
