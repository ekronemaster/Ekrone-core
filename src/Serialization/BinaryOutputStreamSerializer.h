// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Ekrone Devs
// Copyright (c) 2018-2023 Ekrone Network & Ekrone Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "Common/IOutputStream.h"
#include "ISerializer.h"
#include "SerializationOverloads.h"

namespace cn {

class BinaryOutputStreamSerializer : public ISerializer {
public:
  BinaryOutputStreamSerializer(common::IOutputStream& strm) : stream(strm) {}
  virtual ~BinaryOutputStreamSerializer() {}

  virtual ISerializer::SerializerType type() const override;

  virtual bool beginObject(common::StringView name) override;
  virtual void endObject() override;

  virtual bool beginArray(size_t& size, common::StringView name) override;
  virtual void endArray() override;

  virtual bool operator()(uint8_t& value, common::StringView name) override;
  virtual bool operator()(int16_t& value, common::StringView name) override;
  virtual bool operator()(uint16_t& value, common::StringView name) override;
  virtual bool operator()(int32_t& value, common::StringView name) override;
  virtual bool operator()(uint32_t& value, common::StringView name) override;
  virtual bool operator()(int64_t& value, common::StringView name) override;
  virtual bool operator()(uint64_t& value, common::StringView name) override;
  virtual bool operator()(double& value, common::StringView name) override;
  virtual bool operator()(bool& value, common::StringView name) override;
  virtual bool operator()(std::string& value, common::StringView name) override;
  virtual bool binary(void* value, size_t size, common::StringView name) override;
  virtual bool binary(std::string& value, common::StringView name) override;

  template<typename T>
  bool operator()(T& value, common::StringView name) {
    return ISerializer::operator()(value, name);
  }

private:
  void checkedWrite(const char* buf, size_t size);
  common::IOutputStream& stream;
};

}
