// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Ekrone Devs
// Copyright (c) 2018-2023 Ekrone Network & Ekrone Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

namespace cn {

class ICoreObserver {
public:
  virtual ~ICoreObserver() {};
  virtual void blockchainUpdated() {};
  virtual void poolUpdated() {};
};

}
