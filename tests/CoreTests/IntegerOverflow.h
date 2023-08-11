// Copyright (c) 2012-2017 The Cryptonote developers
// Copyright (c) 2018-2023 Ekrone Network & Ekrone Devs
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once
#include "Chaingen.h"

struct gen_uint_overflow_base : public test_chain_unit_base
{
  gen_uint_overflow_base();

  bool check_tx_verification_context(const cn::tx_verification_context& tvc, bool tx_added, size_t event_idx, const cn::Transaction& tx);
  bool check_block_verification_context(const cn::block_verification_context& bvc, size_t event_idx, const cn::Block& block);

  bool mark_last_valid_block(cn::core& c, size_t ev_index, const std::vector<test_event_entry>& events);

private:
  size_t m_last_valid_block_event_idx;
};

struct gen_uint_overflow_1 : public gen_uint_overflow_base
{
  bool generate(std::vector<test_event_entry>& events) const;
};

struct gen_uint_overflow_2 : public gen_uint_overflow_base
{
  bool generate(std::vector<test_event_entry>& events) const;
};
