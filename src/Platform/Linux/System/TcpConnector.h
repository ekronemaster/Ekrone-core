// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Ekrone Devs
// Copyright (c) 2018-2023 Ekrone Network & Ekrone Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <cstdint>
#include <string>

namespace platform_system {

class Dispatcher;
class Ipv4Address;
class TcpConnection;

class TcpConnector {
public:
  TcpConnector();
  TcpConnector(Dispatcher& dispatcher);
  TcpConnector(const TcpConnector&) = delete;
  TcpConnector(TcpConnector&& other);
  ~TcpConnector();
  TcpConnector& operator=(const TcpConnector&) = delete;
  TcpConnector& operator=(TcpConnector&& other);
  TcpConnection connect(const Ipv4Address& address, uint16_t port);

private:
  void* context;
  Dispatcher* dispatcher;
};

}
