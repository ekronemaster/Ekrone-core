// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Ekrone Devs
// Copyright (c) 2018-2023 Ekrone Network & Ekrone Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "INode.h"
#include "ITransaction.h"
#include "CryptoNoteProtocol/ICryptoNoteProtocolQuery.h"
#include "CryptoNoteProtocol/ICryptoNoteProtocolObserver.h"
#include "CryptoNoteCore/ICore.h"
#include "CryptoNoteCore/ICoreObserver.h"
#include "Common/ObserverManager.h"
#include "BlockchainExplorer/BlockchainExplorerDataBuilder.h"

#include <thread>
#include <boost/asio.hpp>

namespace cn {

class core;

class InProcessNode : public INode, public cn::ICryptoNoteProtocolObserver, public cn::ICoreObserver {
public:
  InProcessNode(cn::ICore& core, cn::ICryptoNoteProtocolQuery& protocol);

  InProcessNode(const InProcessNode&) = delete;
  InProcessNode(InProcessNode&&) = delete;

  InProcessNode& operator=(const InProcessNode&) = delete;
  InProcessNode& operator=(InProcessNode&&) = delete;

  virtual ~InProcessNode();

  virtual void init(const Callback& callback) override;
  virtual bool shutdown() override;

  virtual bool addObserver(INodeObserver* observer) override;
  virtual bool removeObserver(INodeObserver* observer) override;

  virtual size_t getPeerCount() const override;
  virtual uint32_t getLastLocalBlockHeight() const override;
  virtual uint32_t getLastKnownBlockHeight() const override;
  virtual uint32_t getLocalBlockCount() const override;
  virtual uint32_t getKnownBlockCount() const override;
  virtual uint64_t getLastLocalBlockTimestamp() const override;

  virtual void getNewBlocks(std::vector<crypto::Hash>&& knownBlockIds, std::vector<cn::block_complete_entry>& newBlocks, uint32_t& startHeight, const Callback& callback) override;
  virtual void getTransactionOutsGlobalIndices(const crypto::Hash& transactionHash, std::vector<uint32_t>& outsGlobalIndices, const Callback& callback) override;
  virtual void getRandomOutsByAmounts(std::vector<uint64_t>&& amounts, uint64_t outsCount,
      std::vector<cn::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& result, const Callback& callback) override;
  virtual void relayTransaction(const cn::Transaction& transaction, const Callback& callback) override;
  virtual void queryBlocks(std::vector<crypto::Hash>&& knownBlockIds, uint64_t timestamp, std::vector<BlockShortEntry>& newBlocks,
    uint32_t& startHeight, const Callback& callback) override;
  virtual void getPoolSymmetricDifference(std::vector<crypto::Hash>&& knownPoolTxIds, crypto::Hash knownBlockId, bool& isBcActual,
          std::vector<std::unique_ptr<ITransactionReader>>& newTxs, std::vector<crypto::Hash>& deletedTxIds, const Callback& callback) override;
  virtual void getMultisignatureOutputByGlobalIndex(uint64_t amount, uint32_t gindex, MultisignatureOutput& out, const Callback& callback) override;


  virtual void getBlocks(const std::vector<uint32_t>& blockHeights, std::vector<std::vector<BlockDetails>>& blocks, const Callback& callback) override;
  virtual void getBlocks(const std::vector<crypto::Hash>& blockHashes, std::vector<BlockDetails>& blocks, const Callback& callback) override;
  virtual void getTransaction(const crypto::Hash &transactionHash, cn::Transaction &transaction, const Callback &callback) override;
  virtual void getBlocks(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t blocksNumberLimit, std::vector<BlockDetails>& blocks, uint32_t& blocksNumberWithinTimestamps, const Callback& callback) override;
  virtual void getTransactions(const std::vector<crypto::Hash>& transactionHashes, std::vector<TransactionDetails>& transactions, const Callback& callback) override;
  virtual void getTransactionsByPaymentId(const crypto::Hash& paymentId, std::vector<TransactionDetails>& transactions, const Callback& callback) override;
  virtual void getPoolTransactions(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t transactionsNumberLimit, std::vector<TransactionDetails>& transactions, uint64_t& transactionsNumberWithinTimestamps, const Callback& callback) override;
  virtual void isSynchronized(bool& syncStatus, const Callback& callback) override;

private:
  virtual void peerCountUpdated(size_t count) override;
  virtual void lastKnownBlockHeightUpdated(uint32_t height) override;
  virtual void blockchainSynchronized(uint32_t topHeight) override;
  virtual void blockchainUpdated() override;
  virtual void poolUpdated() override;

  void getNewBlocksAsync(std::vector<crypto::Hash>& knownBlockIds, std::vector<cn::block_complete_entry>& newBlocks, uint32_t& startHeight, const Callback& callback);
  std::error_code doGetNewBlocks(std::vector<crypto::Hash>&& knownBlockIds, std::vector<cn::block_complete_entry>& newBlocks, uint32_t& startHeight);

  void getTransactionOutsGlobalIndicesAsync(const crypto::Hash& transactionHash, std::vector<uint32_t>& outsGlobalIndices, const Callback& callback);
  std::error_code doGetTransactionOutsGlobalIndices(const crypto::Hash& transactionHash, std::vector<uint32_t>& outsGlobalIndices);

  void getRandomOutsByAmountsAsync(std::vector<uint64_t>& amounts, uint64_t outsCount,
      std::vector<cn::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& result, const Callback& callback);
  std::error_code doGetRandomOutsByAmounts(std::vector<uint64_t>&& amounts, uint64_t outsCount,
      std::vector<cn::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& result);

  void relayTransactionAsync(const cn::Transaction& transaction, const Callback& callback);
  std::error_code doRelayTransaction(const cn::Transaction& transaction);

  void queryBlocksLiteAsync(std::vector<crypto::Hash>& knownBlockIds, uint64_t timestamp, std::vector<BlockShortEntry>& newBlocks, uint32_t& startHeight,
          const Callback& callback);
  std::error_code doQueryBlocksLite(std::vector<crypto::Hash>&& knownBlockIds, uint64_t timestamp, std::vector<BlockShortEntry>& newBlocks, uint32_t& startHeight);

  void getPoolSymmetricDifferenceAsync(std::vector<crypto::Hash>&& knownPoolTxIds, crypto::Hash knownBlockId, bool& isBcActual,
          std::vector<std::unique_ptr<ITransactionReader>>& newTxs, std::vector<crypto::Hash>& deletedTxIds, const Callback& callback);

  void getOutByMSigGIndexAsync(uint64_t amount, uint32_t gindex, MultisignatureOutput& out, const Callback& callback);

  void getBlocksAsync(const std::vector<uint32_t>& blockHeights, std::vector<std::vector<BlockDetails>>& blocks, const Callback& callback);
  std::error_code doGetBlocks(const std::vector<uint32_t>& blockHeights, std::vector<std::vector<BlockDetails>>& blocks);

  void getBlocksAsync(const std::vector<crypto::Hash>& blockHashes, std::vector<BlockDetails>& blocks, const Callback& callback);
  std::error_code doGetBlocks(const std::vector<crypto::Hash>& blockHashes, std::vector<BlockDetails>& blocks);

  void getBlocksAsync(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t blocksNumberLimit, std::vector<BlockDetails>& blocks, uint32_t& blocksNumberWithinTimestamps, const Callback& callback);
  std::error_code doGetBlocks(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t blocksNumberLimit, std::vector<BlockDetails>& blocks, uint32_t& blocksNumberWithinTimestamps);

  void getTransactionsAsync(const std::vector<crypto::Hash>& transactionHashes, std::vector<TransactionDetails>& transactions, const Callback& callback);
  std::error_code doGetTransactions(const std::vector<crypto::Hash>& transactionHashes, std::vector<TransactionDetails>& transactions);

  void getPoolTransactionsAsync(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t transactionsNumberLimit, std::vector<TransactionDetails>& transactions, uint64_t& transactionsNumberWithinTimestamps, const Callback& callback);
  std::error_code doGetPoolTransactions(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t transactionsNumberLimit, std::vector<TransactionDetails>& transactions, uint64_t& transactionsNumberWithinTimestamps);

  void getTransactionsByPaymentIdAsync(const crypto::Hash& paymentId, std::vector<TransactionDetails>& transactions, const Callback& callback);
  std::error_code doGetTransactionsByPaymentId(const crypto::Hash& paymentId, std::vector<TransactionDetails>& transactions);

  void isSynchronizedAsync(bool& syncStatus, const Callback& callback);
  std::error_code doIsSynchronized(bool& syncStatus);

  void getTransactionAsync(const crypto::Hash &transactionHash, cn::Transaction &transaction, const Callback &callback);
  std::error_code doGetTransaction(const crypto::Hash &transactionHash, cn::Transaction &transaction);
  void workerFunc();
  bool doShutdown();

  enum State {
    NOT_INITIALIZED,
    INITIALIZED
  };

  State state;
  cn::ICore& core;
  cn::ICryptoNoteProtocolQuery& protocol;
  tools::ObserverManager<INodeObserver> observerManager;

  boost::asio::io_service ioService;
  std::unique_ptr<std::thread> workerThread;
  std::unique_ptr<boost::asio::io_service::work> work;

  BlockchainExplorerDataBuilder blockchainExplorerDataBuilder;

  mutable std::mutex mutex;
};

} //namespace cn
