// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2019 Conceal Network & Conceal Devs
// Copyright (c) 2017-2022 Ekrone Infinity Developers
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include "EkroneWallet.h"

#include <ctime>
#include <fstream>
#include <future>
#include <iomanip>
#include <thread>
#include <set>
#include <sstream>
#include <regex>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "Common/Base58.h"
#include "Common/CommandLine.h"
#include "Common/ColouredMsg.h"
#include "Common/SignalHandler.h"
#include "Common/StringTools.h"
#include "Common/PathTools.h"
#include "Common/Util.h"
#include "Common/DnsTools.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "CryptoNoteProtocol/CryptoNoteProtocolHandler.h"
#include "NodeRpcProxy/NodeRpcProxy.h"
#include "Rpc/CoreRpcServerCommandsDefinitions.h"
#include "Rpc/HttpClient.h"
#include "CryptoNoteCore/CryptoNoteTools.h"

#include "Wallet/WalletRpcServer.h"
#include "WalletLegacy/WalletLegacy.h"
#include "Wallet/LegacyKeysImporter.h"
#include "WalletLegacy/WalletHelper.h"
#include "Mnemonics/electrum-words.cpp"

#include "version.h"

#include <Logging/LoggerManager.h>

#if defined(WIN32)
#include <crtdbg.h>
#endif

using namespace cn;
using namespace logging;
using common::JsonValue;

namespace po = boost::program_options;

#define EXTENDED_LOGS_FILE "wallet_details.log"
#undef ERROR

namespace {

const command_line::arg_descriptor<std::string> arg_wallet_file = { "wallet-file", "Use wallet <arg>", "" };
const command_line::arg_descriptor<std::string> arg_generate_new_wallet = { "generate-new-wallet", "Generate new wallet and save it to <arg>", "" };
const command_line::arg_descriptor<std::string> arg_daemon_address = { "daemon-address", "Use daemon instance at <host>:<port>", "" };
const command_line::arg_descriptor<std::string> arg_daemon_host = { "daemon-host", "Use daemon instance at host <arg> instead of localhost", "" };
const command_line::arg_descriptor<std::string> arg_password = { "password", "Wallet password", "", true };
const command_line::arg_descriptor<uint16_t>    arg_daemon_port = { "daemon-port", "Use daemon instance at port <arg> instead of default", 0 };
const command_line::arg_descriptor<uint32_t>    arg_log_level = { "set_log", "", INFO, true };
const command_line::arg_descriptor<bool>        arg_testnet = { "testnet", "Used to deploy test nets. The daemon must be launched with --testnet flag", false };
const command_line::arg_descriptor< std::vector<std::string> > arg_command = { "command", "" };

bool parseUrlAddress(const std::string& url, std::string& address, uint16_t& port) {
  auto pos = url.find("://");
  size_t addrStart = 0;

  if (pos != std::string::npos) {
    addrStart = pos + 3;
  }

  auto addrEnd = url.find(':', addrStart);

  if (addrEnd != std::string::npos) {
    auto portEnd = url.find('/', addrEnd);
    port = common::fromString<uint16_t>(url.substr(
      addrEnd + 1, portEnd == std::string::npos ? std::string::npos : portEnd - addrEnd - 1));
  } else {
    addrEnd = url.find('/');
    port = 80;
  }

  address = url.substr(addrStart, addrEnd - addrStart);
  return true;
}


inline std::string interpret_rpc_response(bool ok, const std::string& status) {
  std::string err;
  if (ok) {
    if (status == CORE_RPC_STATUS_BUSY) {
      err = "daemon is busy. Please try later";
    } else if (status != CORE_RPC_STATUS_OK) {
      err = status;
    }
  } else {
    err = "possible lost connection to daemon";
  }
  return err;
}

template <typename IterT, typename ValueT = typename IterT::value_type>
class ArgumentReader {
public:

  ArgumentReader(IterT begin, IterT end) :
    m_begin(begin), m_end(end), m_cur(begin) {
  }

  bool eof() const {
    return m_cur == m_end;
  }

  ValueT next() {
    if (eof()) {
      throw std::runtime_error("unexpected end of arguments");
    }

    return *m_cur++;
  }

private:

  IterT m_cur;
  IterT m_begin;
  IterT m_end;
};

struct TransferCommand {
  const cn::Currency& m_currency;
  size_t fake_outs_count;
  std::vector<cn::WalletLegacyTransfer> dsts;
  std::vector<uint8_t> extra;
  uint64_t fee;
  std::map<std::string, std::vector<WalletLegacyTransfer>> aliases;
  std::vector<std::string> messages;
  uint64_t ttl;
  std::string m_remote_address;

  TransferCommand(const cn::Currency& currency, std::string remote_fee_address) :
    m_currency(currency), m_remote_address(remote_fee_address), fake_outs_count(0), fee(currency.minimumFee()), ttl(0) {
  }

/* This parses arguments from the transfer command */
  bool parseArguments(LoggerRef& logger, const std::vector<std::string> &args) {
    ArgumentReader<std::vector<std::string>::const_iterator> ar(args.begin(), args.end());

    try 
    {
      /* Parse the remaining arguments */
      while (!ar.eof()) 
      {
        auto arg = ar.next();

        if (arg.size() && arg[0] == '-') 
        {          
          const auto& value = ar.next();
          if (arg == "-p") {
            if (!createTxExtraWithPaymentId(value, extra)) {
              logger(ERROR, BRIGHT_RED) << "payment ID has invalid format: \"" << value << "\", expected 64-character string";
              return false;
            }
          } else if (arg == "-m") {
            messages.emplace_back(value);
          } else if (arg == "-ttl") {
            fee = 0;
            if (!common::fromString(value, ttl) || ttl < 1 || ttl * 60 > m_currency.mempoolTxLiveTime()) {
              logger(ERROR, BRIGHT_RED) << "TTL has invalid format: \"" << value << "\", " <<
                "enter time from 1 to " << (m_currency.mempoolTxLiveTime() / 60) << " minutes";
              return false;
            }
          }
        } else {

          /* Integrated address check */
          if (arg.length() == 186) {
            std::string paymentID;
            std::string spendPublicKey;
            std::string viewPublicKey;
            const uint64_t paymentIDLen = 64;

            /* Extract the payment id */
            std::string decoded;
            uint64_t prefix;
            if (tools::base_58::decode_addr(arg, prefix, decoded)) {
              paymentID = decoded.substr(0, paymentIDLen);
            }

            /* Validate and add the payment ID to extra */
            if (!createTxExtraWithPaymentId(paymentID, extra)) {
              logger(ERROR, BRIGHT_RED) << "Integrated payment ID has invalid format: \"" << paymentID << "\", expected 64-character string";
              return false;
            }

            /* create the address from the public keys */
            std::string keys = decoded.substr(paymentIDLen, std::string::npos);
            cn::AccountPublicAddress addr;
            cn::BinaryArray ba = common::asBinaryArray(keys);

            if (!cn::fromBinaryArray(addr, ba)) {
              return true;
            }

            std::string address = cn::getAccountAddressAsStr(cn::parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX, 
                                                                    addr);   
            arg = address;
          }

          WalletLegacyTransfer destination;
          WalletLegacyTransfer feeDestination;          
          cn::TransactionDestinationEntry de;
          std::string aliasUrl;

          if (!m_currency.parseAccountAddressString(arg, de.addr)) {
            aliasUrl = arg;
          }

          auto value = ar.next();
          bool ok = m_currency.parseAmount(value, de.amount);

          if (!ok || 0 == de.amount) {
            logger(ERROR, BRIGHT_RED) << "amount is wrong: " << arg << ' ' << value <<
              ", expected number from 0 to " << m_currency.formatAmount(std::numeric_limits<uint64_t>::max());
            return false;
          }

          if (aliasUrl.empty()) {
            destination.address = arg;
            destination.amount = de.amount;
            dsts.push_back(destination);
          } else {
            aliases[aliasUrl].emplace_back(WalletLegacyTransfer{"", static_cast<int64_t>(de.amount)});
          }

          /* Remote node transactions fees are 1000 X */
          if (!m_remote_address.empty()) {
            destination.address = m_remote_address;                     
            destination.amount = 10000;
            dsts.push_back(destination);
          }

        }
      }

      if (dsts.empty() && aliases.empty()) {
        logger(ERROR, BRIGHT_RED) << "At least one destination address is required";
        return false;
      }
    } catch (const std::exception& e) {
      logger(ERROR, BRIGHT_RED) << e.what();
      return false;
    }

    return true;
  }
};

JsonValue buildLoggerConfiguration(Level level, const std::string& logfile) {
  JsonValue loggerConfiguration(JsonValue::OBJECT);
  loggerConfiguration.insert("globalLevel", static_cast<int64_t>(level));

  JsonValue& cfgLoggers = loggerConfiguration.insert("loggers", JsonValue::ARRAY);

  JsonValue& consoleLogger = cfgLoggers.pushBack(JsonValue::OBJECT);
  consoleLogger.insert("type", "console");
  consoleLogger.insert("level", static_cast<int64_t>(TRACE));
  consoleLogger.insert("pattern", "");

  JsonValue& fileLogger = cfgLoggers.pushBack(JsonValue::OBJECT);
  fileLogger.insert("type", "file");
  fileLogger.insert("filename", logfile);
  fileLogger.insert("level", static_cast<int64_t>(TRACE));

  return loggerConfiguration;
}

std::error_code initAndLoadWallet(IWalletLegacy& wallet, std::istream& walletFile, const std::string& password) {
  WalletHelper::InitWalletResultObserver initObserver;
  std::future<std::error_code> f_initError = initObserver.initResult.get_future();

  WalletHelper::IWalletRemoveObserverGuard removeGuard(wallet, initObserver);
  wallet.initAndLoad(walletFile, password);
  auto initError = f_initError.get();

  return initError;
}

std::string tryToOpenWalletOrLoadKeysOrThrow(LoggerRef& logger, std::unique_ptr<IWalletLegacy>& wallet, const std::string& walletFile, const std::string& password) {
  std::string keys_file, walletFileName;
  WalletHelper::prepareFileNames(walletFile, keys_file, walletFileName);

  boost::system::error_code ignore;
  bool keysExists = boost::filesystem::exists(keys_file, ignore);
  bool walletExists = boost::filesystem::exists(walletFileName, ignore);
  if (!walletExists && !keysExists && boost::filesystem::exists(walletFile, ignore)) {
    boost::system::error_code renameEc;
    boost::filesystem::rename(walletFile, walletFileName, renameEc);
    if (renameEc) {
      throw std::runtime_error("failed to rename file '" + walletFile + "' to '" + walletFileName + "': " + renameEc.message());
    }

    walletExists = true;
  }

  if (walletExists) {
    logger(INFO, GREEN) << "Loading wallet...";
    std::ifstream walletFile;
    walletFile.open(walletFileName, std::ios_base::binary | std::ios_base::in);
    if (walletFile.fail()) {
      throw std::runtime_error("error opening wallet file '" + walletFileName + "'");
    }

    auto initError = initAndLoadWallet(*wallet, walletFile, password);

    walletFile.close();
    if (initError) { //bad password, or legacy format
      if (keysExists) {
        std::stringstream ss;
        cn::importLegacyKeys(keys_file, password, ss);
        boost::filesystem::rename(keys_file, keys_file + ".back");
        boost::filesystem::rename(walletFileName, walletFileName + ".back");

        initError = initAndLoadWallet(*wallet, ss, password);
        if (initError) {
          throw std::runtime_error("failed to load wallet: " + initError.message());
        }

        logger(INFO, GREEN) << "Storing wallet...";

        try {
          cn::WalletHelper::storeWallet(*wallet, walletFileName);
        } catch (std::exception& e) {
          logger(ERROR, BRIGHT_RED) << "Failed to store wallet: " << e.what();
          throw std::runtime_error("error saving wallet file '" + walletFileName + "'");
        }

        logger(INFO, BRIGHT_GREEN) << "Stored ok";
        return walletFileName;
      } else { // no keys, wallet error loading
        throw std::runtime_error("can't load wallet file '" + walletFileName + "', check password");
      }
    } else { //new wallet ok
      return walletFileName;
    }
  } else if (keysExists) { //wallet not exists but keys presented
    std::stringstream ss;
    cn::importLegacyKeys(keys_file, password, ss);
    boost::filesystem::rename(keys_file, keys_file + ".back");

    WalletHelper::InitWalletResultObserver initObserver;
    std::future<std::error_code> f_initError = initObserver.initResult.get_future();

    WalletHelper::IWalletRemoveObserverGuard removeGuard(*wallet, initObserver);
    wallet->initAndLoad(ss, password);
    auto initError = f_initError.get();

    removeGuard.removeObserver();
    if (initError) {
      throw std::runtime_error("failed to load wallet: " + initError.message());
    }

    logger(INFO, GREEN) << "Storing wallet...";

    try {
      cn::WalletHelper::storeWallet(*wallet, walletFileName);
    } catch(std::exception& e) {
      logger(ERROR, BRIGHT_RED) << "Failed to store wallet: " << e.what();
      throw std::runtime_error("error saving wallet file '" + walletFileName + "'");
    }

    logger(INFO, BRIGHT_GREEN) << "Stored ok";
    return walletFileName;
  } else { //no wallet no keys
    throw std::runtime_error("wallet file '" + walletFileName + "' is not found");
  }
}

const size_t TIMESTAMP_MAX_WIDTH = 19;
const size_t HASH_MAX_WIDTH = 64;
const size_t TOTAL_AMOUNT_MAX_WIDTH = 20;
const size_t FEE_MAX_WIDTH = 14;
const size_t BLOCK_MAX_WIDTH = 7;
const size_t UNLOCK_TIME_MAX_WIDTH = 11;

void printListTransfersHeader(LoggerRef& logger) {
  std::string header = common::makeCenteredString(TIMESTAMP_MAX_WIDTH, "timestamp (UTC)") + "  ";
  header += common::makeCenteredString(HASH_MAX_WIDTH, "hash") + "  ";
  header += common::makeCenteredString(TOTAL_AMOUNT_MAX_WIDTH, "total amount") + "  ";
  header += common::makeCenteredString(FEE_MAX_WIDTH, "fee") + "  ";
  header += common::makeCenteredString(BLOCK_MAX_WIDTH, "block") + "  ";
  header += common::makeCenteredString(UNLOCK_TIME_MAX_WIDTH, "unlock time");

  logger(INFO, BRIGHT_MAGENTA) << header;
  logger(INFO, BRIGHT_MAGENTA) << std::string(header.size(), '-');
}

void printListDepositsHeader(LoggerRef& logger) {
  std::string header = common::makeCenteredString(8, "ID") + " | ";
  header += common::makeCenteredString(20, "Amount") + " | ";
  header += common::makeCenteredString(20, "Interest") + " | ";
  header += common::makeCenteredString(16, "Unlock Height") + " | ";
  header += common::makeCenteredString(10, "State");

  logger(INFO) << "\n" << header;
  logger(INFO) << std::string(header.size(), '=');
}

void printListTransfersItem(LoggerRef& logger, const WalletLegacyTransaction& txInfo, IWalletLegacy& wallet, const Currency& currency) {
  std::vector<uint8_t> extraVec = common::asBinaryArray(txInfo.extra);

  crypto::Hash paymentId;
  std::string paymentIdStr = (getPaymentIdFromTxExtra(extraVec, paymentId) && paymentId != NULL_HASH ? common::podToHex(paymentId) : "");

  char timeString[TIMESTAMP_MAX_WIDTH + 1];
  time_t timestamp = static_cast<time_t>(txInfo.timestamp);
  if (std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::gmtime(&timestamp)) == 0) {
    throw std::runtime_error("time buffer is too small");
  }

  std::string rowColor = txInfo.totalAmount < 0 ? MAGENTA : GREEN;
  logger(INFO, rowColor)
    << std::setw(TIMESTAMP_MAX_WIDTH) << timeString
    << "  " << std::setw(HASH_MAX_WIDTH) << common::podToHex(txInfo.hash)
    << "  " << std::setw(TOTAL_AMOUNT_MAX_WIDTH) << currency.formatAmount(txInfo.totalAmount)
    << "  " << std::setw(FEE_MAX_WIDTH) << currency.formatAmount(txInfo.fee)
    << "  " << std::setw(BLOCK_MAX_WIDTH) << txInfo.blockHeight
    << "  " << std::setw(UNLOCK_TIME_MAX_WIDTH) << txInfo.unlockTime;

  if (!paymentIdStr.empty()) {
    logger(INFO, rowColor) << "payment ID: " << paymentIdStr;
  }

  if (txInfo.totalAmount < 0) {
    if (txInfo.transferCount > 0) {
      logger(INFO, rowColor) << "transfers:";
      for (TransferId id = txInfo.firstTransferId; id < txInfo.firstTransferId + txInfo.transferCount; ++id) {
        WalletLegacyTransfer tr;
        wallet.getTransfer(id, tr);
        logger(INFO, rowColor) << tr.address << "  " << std::setw(TOTAL_AMOUNT_MAX_WIDTH) << currency.formatAmount(tr.amount);
      }
    }
  }

  logger(INFO, rowColor) << " "; //just to make logger print one endline
}

std::string prepareWalletAddressFilename(const std::string& walletBaseName) {
  return walletBaseName + ".address";
}

bool writeAddressFile(const std::string& addressFilename, const std::string& address) {
  std::ofstream addressFile(addressFilename, std::ios::out | std::ios::trunc | std::ios::binary);
  if (!addressFile.good()) {
    return false;
  }

  addressFile << address;

  return true;
}

bool processServerAliasResponse(const std::string& s, std::string& address) {
  try {
  //   
  // Courtesy of Monero Project
		// make sure the txt record has "oa1:Ekr" and find it
		auto pos = s.find("oa1:Ekr");
		if (pos == std::string::npos)
			return false;
		// search from there to find "recipient_address="
		pos = s.find("recipient_address=", pos);
		if (pos == std::string::npos)
			return false;
		pos += 18; // move past "recipient_address="
		// find the next semicolon
		auto pos2 = s.find(";", pos);
		if (pos2 != std::string::npos)
		{
			// length of address == 95, we can at least validate that much here
			if (pos2 - pos == 98)
			{
				address = s.substr(pos, 98);
			} else {
				return false;
			}
		}
    }
	catch (std::exception&) {
		return false;
	}

	return true;
}


bool askAliasesTransfersConfirmation(const std::map<std::string, std::vector<WalletLegacyTransfer>>& aliases, const Currency& currency) {
  std::cout << "Would you like to send money to the following addresses?" << std::endl;

  for (const auto& kv: aliases) {
    for (const auto& transfer: kv.second) {
      std::cout << transfer.address << " " << std::setw(21) << currency.formatAmount(transfer.amount) << "  " << kv.first << std::endl;
    }
  }

  std::string answer;
  do {
    std::cout << "y/n: ";
    std::getline(std::cin, answer);
  } while (answer != "y" && answer != "Y" && answer != "n" && answer != "N");

  return answer == "y" || answer == "Y";
}

}

bool processServerFeeAddressResponse(const std::string& response, std::string& fee_address) {
    try {
        std::stringstream stream(response);
        JsonValue json;
        stream >> json;

        auto rootIt = json.getObject().find("fee_address");
        if (rootIt == json.getObject().end()) {
            return false;
        }

        fee_address = rootIt->second.getString();
    }
    catch (std::exception&) {
        return false;
    }

    return true;
}

std::string ekrone_wallet::get_commands_str(bool do_ext) {
  std::stringstream ss;
  ss << "";

  std::string usage;
  if (do_ext)
    usage = wallet_menu(true);
  else
    usage = wallet_menu(false);

  boost::replace_all(usage, "\n", "\n  ");
  usage.insert(0, "  ");

  ss << usage << std::endl;

  return ss.str();
}

bool ekrone_wallet::help(const std::vector<std::string> &args/* = std::vector<std::string>()*/) {
  success_msg_writer() << get_commands_str(false);
  return true;
}

bool ekrone_wallet::extended_help(const std::vector<std::string> &args/* = std::vector<std::string>()*/) {
  success_msg_writer() << get_commands_str(true);
  return true;
}

bool ekrone_wallet::exit(const std::vector<std::string> &args) {
  m_consoleHandler.requestStop();
  return true;
}

ekrone_wallet::ekrone_wallet(platform_system::Dispatcher& dispatcher, const cn::Currency& currency, logging::LoggerManager& log) :
  m_dispatcher(dispatcher),
  m_daemon_port(0),
  m_currency(currency),
  logManager(log),
  logger(log, "ekronewallet"),
  m_refresh_progress_reporter(*this),
  m_initResultPromise(nullptr),
  m_walletSynchronized(false) {
  m_consoleHandler.setHandler("create_integrated", boost::bind(&ekrone_wallet::create_integrated, this, _1), "create_integrated <payment_id> - Create an integrated address with a payment ID");
  m_consoleHandler.setHandler("export_keys", boost::bind(&ekrone_wallet::export_keys, this, _1), "Show the secret keys of the current wallet");
  m_consoleHandler.setHandler("balance", boost::bind(&ekrone_wallet::show_balance, this, _1), "Show current wallet balance");
  m_consoleHandler.setHandler("sign_message", boost::bind(&ekrone_wallet::sign_message, this, _1), "Sign a message with your wallet keys");
  m_consoleHandler.setHandler("verify_signature", boost::bind(&ekrone_wallet::verify_signature, this, _1), "Verify a signed message");
  m_consoleHandler.setHandler("incoming_transfers", boost::bind(&ekrone_wallet::show_incoming_transfers, this, _1), "Show incoming transfers");
  m_consoleHandler.setHandler("outgoing_transfers", boost::bind(&ekrone_wallet::show_outgoing_transfers, this, _1), "Show outgoing transfers");
  m_consoleHandler.setHandler("list_transfers", boost::bind(&ekrone_wallet::listTransfers, this, _1), "list_transfers <height> - Show all known transfers from a certain (optional) block height");
  m_consoleHandler.setHandler("payments", boost::bind(&ekrone_wallet::show_payments, this, _1), "payments <payment_id_1> [<payment_id_2> ... <payment_id_N>] - Show payments <payment_id_1>, ... <payment_id_N>");
  m_consoleHandler.setHandler("get_tx_proof", boost::bind(&ekrone_wallet::get_tx_proof, this, _1), "Generate a signature to prove payment: <txid> <address> [<txkey>]");
  m_consoleHandler.setHandler("bc_height", boost::bind(&ekrone_wallet::show_blockchain_height, this, _1), "Show blockchain height");
  m_consoleHandler.setHandler("show_dust", boost::bind(&ekrone_wallet::show_dust, this, _1), "Show the number of unmixable dust outputs");
  m_consoleHandler.setHandler("outputs", boost::bind(&ekrone_wallet::show_num_unlocked_outputs, this, _1), "Show the number of unlocked outputs available for a transaction");
  m_consoleHandler.setHandler("optimize", boost::bind(&ekrone_wallet::optimize_outputs, this, _1), "Combine many available outputs into a few by sending a transaction to self");
  m_consoleHandler.setHandler("optimize_all", boost::bind(&ekrone_wallet::optimize_all_outputs, this, _1), "Optimize your wallet several times so you can send large transactions");  
  m_consoleHandler.setHandler("transfer", boost::bind(&ekrone_wallet::transfer, this, _1),
    "transfer <addr_1> <amount_1> [<addr_2> <amount_2> ... <addr_N> <amount_N>] [-p payment_id]"
    " - Transfer <amount_1>,... <amount_N> to <address_1>,... <address_N>, respectively. ");
  m_consoleHandler.setHandler("set_log", boost::bind(&ekrone_wallet::set_log, this, _1), "set_log <level> - Change current log level, <level> is a number 0-4");
  m_consoleHandler.setHandler("address", boost::bind(&ekrone_wallet::print_address, this, _1), "Show current wallet public address");
  m_consoleHandler.setHandler("save", boost::bind(&ekrone_wallet::save, this, _1), "Save wallet synchronized data");
  m_consoleHandler.setHandler("reset", boost::bind(&ekrone_wallet::reset, this, _1), "Discard cache data and start synchronizing from the start");
  m_consoleHandler.setHandler("help", boost::bind(&ekrone_wallet::help, this, _1), "Show this help");
  m_consoleHandler.setHandler("ext_help", boost::bind(&ekrone_wallet::extended_help, this, boost::arg<1>()), "Show this help");
  m_consoleHandler.setHandler("exit", boost::bind(&ekrone_wallet::exit, this, _1), "Close wallet");  
  m_consoleHandler.setHandler("balance_proof", boost::bind(&ekrone_wallet::get_reserve_proof, this, _1), "all|<amount> [<message>] - Generate a signature proving that you own at least <amount>, optionally with a challenge string <message>. ");
  m_consoleHandler.setHandler("save_keys", boost::bind(&ekrone_wallet::save_keys_to_file, this, boost::arg<1>()), "Saves wallet private keys to \"<wallet_name>_ekrone_backup.txt\"");
  m_consoleHandler.setHandler("list_deposits", boost::bind(&ekrone_wallet::list_deposits, this, boost::arg<1>()), "Show all known deposits from this wallet");
  m_consoleHandler.setHandler("deposit", boost::bind(&ekrone_wallet::deposit, this, boost::arg<1>()), "deposit <months> <amount> - Create a deposit");
  m_consoleHandler.setHandler("withdraw", boost::bind(&ekrone_wallet::withdraw, this, boost::arg<1>()), "withdraw <id> - Withdraw a deposit");
  m_consoleHandler.setHandler("deposit_info", boost::bind(&ekrone_wallet::deposit_info, this, boost::arg<1>()), "deposit_info <id> - Get infomation for deposit <id>");
}

std::string ekrone_wallet::wallet_menu(bool do_ext)
{
  std::string menu_item;

  if (do_ext)
  {
    menu_item += "\t\tEkrone Infinity Wallet Extended Menu\n\n";
    menu_item += "[ ] = Optional arg\n";
    menu_item += "\"balance_proof <amount>\"                           - Generate a signature proving that you own at least <amount> | [<message>]\n";
    menu_item += "\"create_integrated <payment_id>\"                   - Create an integrated address with a payment ID.\n";
    menu_item += "\"get_tx_proof <txid> <address>\"                    - Generate a signature to prove payment | [<txkey>]\n";
    menu_item += "\"incoming_transfers\"                               - Show incoming transfers.\n";
    menu_item += "\"optimize\"                                         - Combine many available outputs into a few by sending a transaction to self.\n";
    menu_item += "\"optimize_all\"                                     - Optimize your wallet several times so you can send large transactions.\n";
    menu_item += "\"outputs\"                                          - Show the number of unlocked outputs available for a transaction.\n";
    menu_item += "\"payments <payment_id>\"                            - Show payments from payment ID. | [<payment_id_2> ... <payment_id_N>]\n";
    menu_item += "\"save_txs_to_file\"                                 - Saves all known transactions to <wallet_name>_ekrone_transactions.txt | [false] or [true] to include deposits (default: false)\n";
    menu_item += "\"set_log <level>\"                                  - Change current log level, default = 3, <level> is a number 0-4.\n";
    menu_item += "\"sign_message <message>\"                           - Sign a message with your wallet keys.\n";
    menu_item += "\"show_dust\"                                        - Show the number of unmixable dust outputs.\n";
    menu_item += "\"verify_signature <message> <address> <signature>\" - Verify a signed message.\n";
  }
  else
  {
    menu_item += "\t\tEkrone Infinity Wallet Menu\n\n";
    menu_item += "[ ] = Optional arg\n\n";
    menu_item += "\"help\"                        - Shows this help dialog.\n";
    menu_item += "\"ext_help\"                    - Shows this extended help dialog.\n\n";
    menu_item += "\"address\"                     - Shows wallet address.\n";
    menu_item += "\"balance\"                     - Shows wallet main and deposit balance.\n";
    menu_item += "\"bc_height\"                   - Shows current blockchain height.\n";
    menu_item += "\"deposit <months> <amount>\"   - Create a deposit to the blockchain.\n";
    menu_item += "\"deposit_info <id>\"           - Display full information for deposit <id>.\n";
    menu_item += "\"exit\"                        - Safely exits the wallet application.\n";
    menu_item += "\"export_keys\"                 - Displays backup keys.\n";
    menu_item += "\"list_deposits\"               - Show all known deposits.\n";
    menu_item += "\"list_transfers\"              - Show all known transfers, optionally from a certain height. | <block_height>\n";
    menu_item += "\"reset\"                       - Reset cached blockchain data and starts synchronizing from block 0.\n";
    menu_item += "\"transfer <address> <amount>\" - Transfers <amount> to <address>. | [-p<payment_id>] [<amount_2>]...[<amount_N>] [<address_2>]...[<address_n>]\n";
    menu_item += "\"save\"                        - Save wallet synchronized blockchain data.\n";
    menu_item += "\"save_keys\"                   - Saves wallet private keys to \"<wallet_name>_ekrone_backup.txt\".\n";
    menu_item += "\"withdraw <id>\"               - Withdraw a deposit from the blockchain.\n";
  }

  return menu_item;
}

/* This function shows the number of outputs in the wallet
  that are below the dust threshold */
bool ekrone_wallet::show_dust(const std::vector<std::string>& args) {
  logger(INFO, BRIGHT_WHITE) << "Dust outputs: " << m_wallet->dustBalance() << std::endl;
	return true;
}

//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::set_log(const std::vector<std::string> &args) {
  if (args.size() != 1) {
    fail_msg_writer() << "use: set_log <log_level_number_0-4>";
    return true;
  }

  uint16_t l = 0;
  if (!common::fromString(args[0], l)) {
    fail_msg_writer() << "wrong number format, use: set_log <log_level_number_0-4>";
    return true;
  }

  if (l > logging::TRACE) {
    fail_msg_writer() << "wrong number range, use: set_log <log_level_number_0-4>";
    return true;
  }

  logManager.setMaxLevel(static_cast<logging::Level>(l));
  return true;
}

bool key_import = true;

//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::init(const boost::program_options::variables_map& vm) {
  handle_command_line(vm);

  if (!m_daemon_address.empty() && (!m_daemon_host.empty() || 0 != m_daemon_port)) {
    fail_msg_writer() << "you can't specify daemon host or port several times";
    return false;
  }

  if (m_daemon_host.empty())
    m_daemon_host = "localhost";
  if (!m_daemon_port)
    m_daemon_port = RPC_DEFAULT_PORT;

  if (!m_daemon_address.empty()) 
  {
    if (!parseUrlAddress(m_daemon_address, m_daemon_host, m_daemon_port)) 
    {
      fail_msg_writer() << "failed to parse daemon address: " << m_daemon_address;
      return false;
    }
    m_remote_node_address = getFeeAddress();
    logger(INFO, BRIGHT_WHITE) << "Connected to remote node: " << m_daemon_host;
    if (!m_remote_node_address.empty()) 
    {
      logger(INFO, BRIGHT_WHITE) << "Fee address: " << m_remote_node_address;
    }    
  } 
  else 
  {
    if (!m_daemon_host.empty()) {
      m_remote_node_address = getFeeAddress();
    }
    m_daemon_address = std::string("http://") + m_daemon_host + ":" + std::to_string(m_daemon_port);
    logger(INFO, BRIGHT_WHITE) << "Connected to remote node: " << m_daemon_host;
    if (!m_remote_node_address.empty()) 
    {
      logger(INFO, BRIGHT_WHITE) << "Fee address: " << m_remote_node_address;
    }   
  }

  if (m_generate_new.empty() && m_wallet_file_arg.empty()) {
    std::cout << "  " << ENDL
    << "  " << ENDL
      << "  XXXXXXX       XXXXXXXUUUUUUUU     UUUUUUUUNNNNNNNN        NNNNNNNNIIIIIIIIII" << ENDL
      << "  X:::::X       X:::::XU::::::U     U::::::UN:::::::N       N::::::NI::::::::I" << ENDL
      << "  X:::::X       X:::::XU::::::U     U::::::UN::::::::N      N::::::NI::::::::I" << ENDL
      << "  X::::::X     X::::::XUU:::::U     U:::::UUN:::::::::N     N::::::NII::::::II" << ENDL
      << "  XXX:::::X   X:::::XXX U:::::U     U:::::U N::::::::::N    N::::::N  I::::I  " << ENDL
      << "     X:::::X X:::::X    U:::::D     D:::::U N:::::::::::N   N::::::N  I::::I  " << ENDL
      << "      X:::::X:::::X     U:::::D     D:::::U N:::::::N::::N  N::::::N  I::::I  " << ENDL
      << "       X:::::::::X      U:::::D     D:::::U N::::::N N::::N N::::::N  I::::I  " << ENDL
      << "       X:::::::::X      U:::::D     D:::::U N::::::N  N::::N:::::::N  I::::I  " << ENDL
      << "      X:::::X:::::X     U:::::D     D:::::U N::::::N   N:::::::::::N  I::::I  " << ENDL
      << "     X:::::X X:::::X    U:::::D     D:::::U N::::::N    N::::::::::N  I::::I  " << ENDL
      << "  XXX:::::X   X:::::XXX U::::::U   U::::::U N::::::N     N:::::::::N  I::::I  " << ENDL
      << "  X::::::X     X::::::X U:::::::UUU:::::::U N::::::N      N::::::::NII::::::II" << ENDL
      << "  X:::::X       X:::::X  UU:::::::::::::UU  N::::::N       N:::::::NI::::::::I" << ENDL
      << "  X:::::X       X:::::X    UU:::::::::UU    N::::::N        N::::::NI::::::::I" << ENDL
      << "  XXXXXXX       XXXXXXX      UUUUUUUUU      NNNNNNNN         NNNNNNNIIIIIIIIII" << ENDL
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    << "  " << ENDL
    << "  " << ENDL;
    std::cout << "How you would like to proceed?\n\n\t[O]pen an existing wallet\n\t[G]enerate a new wallet file\n\t[I]mport wallet from keys\n\t[M]nemonic seed import\n\t[E]xit.\n\n";
    char c;
    do {
      std::string answer;
      std::getline(std::cin, answer);
      c = answer[0];
      if (!(c == 'O' || c == 'G' || c == 'E' || c == 'I' || c == 'o' || c == 'g' || c == 'e' || c == 'i' || c == 'm' || c == 'M')) {
        std::cout << "Unknown command: " << c <<std::endl;
      } else {
        break;
      }
    } while (true);

    if (c == 'E' || c == 'e') {
      return false;
    }

    std::cout << "Specify wallet file name (e.g., name.wallet).\n";
    std::string userInput;
    do {
      std::cout << "Wallet file name: ";
      std::getline(std::cin, userInput);
      boost::algorithm::trim(userInput);
    } while (userInput.empty());
    if (c == 'i' || c == 'I'){
      key_import = true;
      m_import_new = userInput;
    } else if (c == 'm' || c == 'M') {
      key_import = false;
      m_import_new = userInput;
    } else if (c == 'g' || c == 'G') {
      m_generate_new = userInput;
    } else {
      m_wallet_file_arg = userInput;
    }
  }



  if (!m_generate_new.empty() && !m_wallet_file_arg.empty() && !m_import_new.empty()) {
    fail_msg_writer() << "you can't specify 'generate-new-wallet' and 'wallet-file' arguments simultaneously";
    return false;
  }

  std::string walletFileName;
  if (!m_generate_new.empty() || !m_import_new.empty()) {
    std::string ignoredString;
    if (!m_generate_new.empty()) {
      WalletHelper::prepareFileNames(m_generate_new, ignoredString, walletFileName);
    } else if (!m_import_new.empty()) {
      WalletHelper::prepareFileNames(m_import_new, ignoredString, walletFileName);
    }
    boost::system::error_code ignore;
    if (boost::filesystem::exists(walletFileName, ignore)) {
      fail_msg_writer() << walletFileName << " already exists";
      return false;
    }
  }



  tools::PasswordContainer pwd_container;
  if (command_line::has_arg(vm, arg_password)) {
    pwd_container.password(command_line::get_arg(vm, arg_password));
  } else if (!pwd_container.read_password()) {
    fail_msg_writer() << "failed to read wallet password";
    return false;
  }

  this->m_node.reset(new NodeRpcProxy(m_daemon_host, m_daemon_port));

  std::promise<std::error_code> errorPromise;
  std::future<std::error_code> f_error = errorPromise.get_future();
  auto callback = [&errorPromise](std::error_code e) {errorPromise.set_value(e); };

  m_node->addObserver(static_cast<INodeRpcProxyObserver*>(this));
  m_node->init(callback);
  auto error = f_error.get();
  if (error) {
    fail_msg_writer() << "failed to init NodeRPCProxy: " << error.message();
    return false;
  }

  if (!m_generate_new.empty()) {
    std::string walletAddressFile = prepareWalletAddressFilename(m_generate_new);
    boost::system::error_code ignore;
    if (boost::filesystem::exists(walletAddressFile, ignore)) {
      logger(ERROR, BRIGHT_RED) << "Address file already exists: " + walletAddressFile;
      return false;
    }

    if (!new_wallet(walletFileName, pwd_container.password())) {
      logger(ERROR, BRIGHT_RED) << "account creation failed";
      return false;
    }

    if (!writeAddressFile(walletAddressFile, m_wallet->getAddress())) {
      logger(WARNING, BRIGHT_RED) << "Couldn't write wallet address file: " + walletAddressFile;
    }
  } else if (!m_import_new.empty()) {
    std::string walletAddressFile = prepareWalletAddressFilename(m_import_new);
    boost::system::error_code ignore;
    if (boost::filesystem::exists(walletAddressFile, ignore)) {
      logger(ERROR, BRIGHT_RED) << "Address file already exists: " + walletAddressFile;
      return false;
    }

    std::string private_spend_key_string;
    std::string private_view_key_string;

crypto::SecretKey private_spend_key;
crypto::SecretKey private_view_key;

if (key_import) {
    do {
      std::cout << "Private Spend Key: ";
      std::getline(std::cin, private_spend_key_string);
      boost::algorithm::trim(private_spend_key_string);
    } while (private_spend_key_string.empty());
    do {
      std::cout << "Private View Key: ";
      std::getline(std::cin, private_view_key_string);
      boost::algorithm::trim(private_view_key_string);
    } while (private_view_key_string.empty());
} else {
  std::string mnemonic_phrase;

  do {
    std::cout << "Mnemonics Phrase (25 words): ";
    std::getline(std::cin, mnemonic_phrase);
    boost::algorithm::trim(mnemonic_phrase);
    boost::algorithm::to_lower(mnemonic_phrase);
  } while (!is_valid_mnemonic(mnemonic_phrase, private_spend_key));

  /* This is not used, but is needed to be passed to the function, not sure how we can avoid this */
  crypto::PublicKey unused_dummy_variable;

  AccountBase::generateViewFromSpend(private_spend_key, private_view_key, unused_dummy_variable);
}

/* We already have our keys if we import via mnemonic seed */
if (key_import) {
    crypto::Hash private_spend_key_hash;
    crypto::Hash private_view_key_hash;
    size_t size;
    if (!common::fromHex(private_spend_key_string, &private_spend_key_hash, sizeof(private_spend_key_hash), size) || size != sizeof(private_spend_key_hash)) {
      return false;
    }
    if (!common::fromHex(private_view_key_string, &private_view_key_hash, sizeof(private_view_key_hash), size) || size != sizeof(private_spend_key_hash)) {
      return false;
    }
      private_spend_key = *(struct crypto::SecretKey *) &private_spend_key_hash;
      private_view_key = *(struct crypto::SecretKey *) &private_view_key_hash;
    }

    if (!new_wallet(private_spend_key, private_view_key, walletFileName, pwd_container.password())) {
      logger(ERROR, BRIGHT_RED) << "account creation failed";
      return false;
    }

    if (!writeAddressFile(walletAddressFile, m_wallet->getAddress())) {
      logger(WARNING, BRIGHT_RED) << "Couldn't write wallet address file: " + walletAddressFile;
    }
  } else {
    m_wallet.reset(new WalletLegacy(m_currency, *m_node, logManager));

    try {
      m_wallet_file = tryToOpenWalletOrLoadKeysOrThrow(logger, m_wallet, m_wallet_file_arg, pwd_container.password());
    } catch (const std::exception& e) {
      fail_msg_writer() << "failed to load wallet: " << e.what();
      return false;
    }

    m_wallet->addObserver(this);
    m_node->addObserver(static_cast<INodeObserver*>(this));

    logger(INFO, BRIGHT_WHITE) << "Opened wallet: " << m_wallet->getAddress();

    success_msg_writer() <<
      "**********************************************************************\n" <<
      "Use \"help\" command to see the list of available commands.\n" <<
      "**********************************************************************";
  }

  return true;
}


//----------------------------------------------------------------------------------------------------
/* adding support for 25 word electrum seeds. however, we have to ensure that all old wallets that are
not deterministic, dont get a seed to avoid any loss of funds.
*/
std::string ekrone_wallet::generate_mnemonic(crypto::SecretKey &private_spend_key) {

  std::string mnemonic_str;

  if (!crypto::electrum_words::bytes_to_words(private_spend_key, mnemonic_str, "English")) {
      logger(ERROR, BRIGHT_RED) << "Cant create the mnemonic for the private spend key!";
  }

  return mnemonic_str;
}
//----------------------------------------------------------------------------------------------------
void ekrone_wallet::log_incorrect_words(std::vector<std::string> words) {
  seed_language::Base *language = seed_language::Singleton<seed_language::English>::instance();
  const std::vector<std::string> &dictionary = language->get_word_list();

  for (auto i : words) {
    if (std::find(dictionary.begin(), dictionary.end(), i) == dictionary.end()) {
      logger(ERROR, BRIGHT_RED) << i << " is not in the english word list!";
    }
  }
}
//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::is_valid_mnemonic(std::string &mnemonic_phrase, crypto::SecretKey &private_spend_key) {

  static std::string languages[] = {"English"};
  static const int num_of_languages = 1;
  static const int mnemonic_phrase_length = 25;

  std::vector<std::string> words;

  words = boost::split(words, mnemonic_phrase, ::isspace);

  if (words.size() != mnemonic_phrase_length) {
    logger(ERROR, BRIGHT_RED) << "Invalid mnemonic phrase!";
    logger(ERROR, BRIGHT_RED) << "Seed phrase is not 25 words! Please try again.";
    log_incorrect_words(words);
    return false;
  }

  for (int i = 0; i < num_of_languages; i++) {
    if (crypto::electrum_words::words_to_bytes(mnemonic_phrase, private_spend_key, languages[i])) {
      return true;
    }
  }

  logger(ERROR, BRIGHT_RED) << "Invalid mnemonic phrase!";
  log_incorrect_words(words);
  return false;
}
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::deinit() {
  m_wallet->removeObserver(this);
  m_node->removeObserver(static_cast<INodeObserver*>(this));
  m_node->removeObserver(static_cast<INodeRpcProxyObserver*>(this));

  if (!m_wallet.get())
    return true;

  return close_wallet();
}
//----------------------------------------------------------------------------------------------------
void ekrone_wallet::handle_command_line(const boost::program_options::variables_map& vm) {
  m_wallet_file_arg = command_line::get_arg(vm, arg_wallet_file);
  m_generate_new = command_line::get_arg(vm, arg_generate_new_wallet);
  m_daemon_address = command_line::get_arg(vm, arg_daemon_address);
  m_daemon_host = command_line::get_arg(vm, arg_daemon_host);
  m_daemon_port = command_line::get_arg(vm, arg_daemon_port);
}
//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::new_wallet(const std::string &wallet_file, const std::string& password) {
  m_wallet_file = wallet_file;

  m_wallet.reset(new WalletLegacy(m_currency, *m_node, logManager));
  m_node->addObserver(static_cast<INodeObserver*>(this));
  m_wallet->addObserver(this);
  try {
    m_initResultPromise.reset(new std::promise<std::error_code>());
    std::future<std::error_code> f_initError = m_initResultPromise->get_future();
    m_wallet->initAndGenerate(password);
    auto initError = f_initError.get();
    m_initResultPromise.reset(nullptr);
    if (initError) {
      fail_msg_writer() << "failed to generate new wallet: " << initError.message();
      return false;
    }

    try {
      cn::WalletHelper::storeWallet(*m_wallet, m_wallet_file);
    } catch (std::exception& e) {
      fail_msg_writer() << "failed to save new wallet: " << e.what();
      throw;
    }

    AccountKeys keys;
    m_wallet->getAccountKeys(keys);

    std::string secretKeysData = std::string(reinterpret_cast<char*>(&keys.spendSecretKey), sizeof(keys.spendSecretKey)) + std::string(reinterpret_cast<char*>(&keys.viewSecretKey), sizeof(keys.viewSecretKey));
    std::string guiKeys = tools::base_58::encode_addr(cn::parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX, secretKeysData);

    logger(INFO, BRIGHT_GREEN) << "EkroneIwallet is an open-source, client-side, free wallet which allow you to send and receive EKRONE instantly on the blockchain. You are  in control of your funds & your keys. When you generate a new wallet, login, send, receive or deposit $Ekr everything happens locally. Your seed is never transmitted, received or stored. That's why its imperative to write, print or save your seed somewhere safe. The backup of keys is your responsibility. If you lose your seed, your account can not be recovered. The Ekrone Team doesn't take any responsibility for lost funds due to nonexistent/missing/lost private keys." << std::endl << std::endl;

    std::cout << "Wallet Address: " << BrightMagentaMsg(m_wallet->getAddress()) << std::endl;
    std::cout << "Private spend key: " << BrightMagentaMsg(common::podToHex(keys.spendSecretKey)) << std::endl;
    std::cout << "Private view key: " << BrightMagentaMsg(common::podToHex(keys.viewSecretKey)) << std::endl;
    std::cout << "Mnemonic Seed: " << BrightMagentaMsg(generate_mnemonic(keys.spendSecretKey)) << std::endl;


  }
  catch (const std::exception& e) {
    fail_msg_writer() << "failed to generate new wallet: " << e.what();
    return false;
  }

  std::cout << "" << std::endl
            << BrightGreenMsg("Congratulations, your wallet has been created!") << std::endl
            << "" << std::endl
            << BrightYellowMsg("You should always use \"exit\" command when closing Ekrone-Infinity wallet to save") << std::endl
            << BrightYellowMsg("your current session's state.") << std::endl
            << BrightYellowMsg("Otherwise, you will possibly need to re-synchronize your chain.") << std::endl
            << "" << std::endl
            << YellowMsg("If you forget to use exit, your wallet is not at risk in anyway.") << std::endl;
            
  return true;
}
//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::new_wallet(crypto::SecretKey &secret_key, crypto::SecretKey &view_key, const std::string &wallet_file, const std::string& password) {
                m_wallet_file = wallet_file;

                m_wallet.reset(new WalletLegacy(m_currency, *m_node.get(), logManager));
                m_node->addObserver(static_cast<INodeObserver*>(this));
                m_wallet->addObserver(this);
                try {
                  m_initResultPromise.reset(new std::promise<std::error_code>());
                  std::future<std::error_code> f_initError = m_initResultPromise->get_future();

                  AccountKeys wallet_keys;
                  wallet_keys.spendSecretKey = secret_key;
                  wallet_keys.viewSecretKey = view_key;
                  crypto::secret_key_to_public_key(wallet_keys.spendSecretKey, wallet_keys.address.spendPublicKey);
                  crypto::secret_key_to_public_key(wallet_keys.viewSecretKey, wallet_keys.address.viewPublicKey);

                  m_wallet->initWithKeys(wallet_keys, password);
                  auto initError = f_initError.get();
                  m_initResultPromise.reset(nullptr);
                  if (initError) {
                    fail_msg_writer() << "failed to generate new wallet: " << initError.message();
                    return false;
                  }

                  try {
                    cn::WalletHelper::storeWallet(*m_wallet, m_wallet_file);
                  } catch (std::exception& e) {
                    fail_msg_writer() << "failed to save new wallet: " << e.what();
                    throw;
                  }

                  AccountKeys keys;
                  m_wallet->getAccountKeys(keys);

                  logger(INFO, BRIGHT_WHITE) <<
                    "Imported wallet: " << m_wallet->getAddress() << std::endl;
                }
                catch (const std::exception& e) {
                  fail_msg_writer() << "failed to import wallet: " << e.what();
                  return false;
                }

                success_msg_writer() <<
                  "**********************************************************************\n" <<
                  "Your wallet has been imported.\n" <<
                  "Use \"help\" command to see the list of available commands.\n" <<
                  "Always use \"exit\" command when closing ekronewallet to save\n" <<
                  "current session's state. Otherwise, you will possibly need to synchronize \n" <<
                  "your wallet again. Your wallet key is NOT under risk anyway.\n" <<
                  "**********************************************************************";
                return true;
                }

//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::close_wallet()
{
  try {
    cn::WalletHelper::storeWallet(*m_wallet, m_wallet_file);
  } catch (const std::exception& e) {
    fail_msg_writer() << e.what();
    return false;
  }

  m_wallet->removeObserver(this);
  m_wallet->shutdown();

  return true;
}

//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::save(const std::vector<std::string> &args)
{
  try {
    cn::WalletHelper::storeWallet(*m_wallet, m_wallet_file);
    success_msg_writer() << "Wallet data saved";
  } catch (const std::exception& e) {
    fail_msg_writer() << e.what();
  }

  return true;
}

bool ekrone_wallet::reset(const std::vector<std::string> &args) {
  {
    std::unique_lock<std::mutex> lock(m_walletSynchronizedMutex);
    m_walletSynchronized = false;
  }

  m_wallet->reset();
  success_msg_writer(true) << "Reset completed successfully.";

  std::unique_lock<std::mutex> lock(m_walletSynchronizedMutex);
  while (!m_walletSynchronized) {
    m_walletSynchronizedCV.wait(lock);
  }

  std::cout << std::endl;

  return true;
}

bool ekrone_wallet::start_mining(const std::vector<std::string>& args) {
  COMMAND_RPC_START_MINING::request req;
  req.miner_address = m_wallet->getAddress();

  bool ok = true;
  size_t max_mining_threads_count = (std::max)(std::thread::hardware_concurrency(), static_cast<unsigned>(2));
  if (0 == args.size()) {
    req.threads_count = 1;
  } else if (1 == args.size()) {
    uint16_t num = 1;
    ok = common::fromString(args[0], num);
    ok = ok && (1 <= num && num <= max_mining_threads_count);
    req.threads_count = num;
  } else {
    ok = false;
  }

  if (!ok) {
    fail_msg_writer() << "invalid arguments. Please use start_mining [<number_of_threads>], " <<
      "<number_of_threads> should be from 1 to " << max_mining_threads_count;
    return true;
  }


  COMMAND_RPC_START_MINING::response res;

  try {
    HttpClient httpClient(m_dispatcher, m_daemon_host, m_daemon_port);

    invokeJsonCommand(httpClient, "/start_mining", req, res);

    std::string err = interpret_rpc_response(true, res.status);
    if (err.empty())
      success_msg_writer() << "Mining started in daemon";
    else
      fail_msg_writer() << "mining has NOT been started: " << err;

  } catch (const ConnectException&) {
    printConnectionError();
  } catch (const std::exception& e) {
    fail_msg_writer() << "Failed to invoke rpc method: " << e.what();
  }

  return true;
}
//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::stop_mining(const std::vector<std::string>& args)
{
  COMMAND_RPC_STOP_MINING::request req;
  COMMAND_RPC_STOP_MINING::response res;

  try {
    HttpClient httpClient(m_dispatcher, m_daemon_host, m_daemon_port);

    invokeJsonCommand(httpClient, "/stop_mining", req, res);
    std::string err = interpret_rpc_response(true, res.status);
    if (err.empty())
      success_msg_writer() << "Mining stopped in daemon";
    else
      fail_msg_writer() << "mining has NOT been stopped: " << err;
  } catch (const ConnectException&) {
    printConnectionError();
  } catch (const std::exception& e) {
    fail_msg_writer() << "Failed to invoke rpc method: " << e.what();
  }

  return true;
}

bool ekrone_wallet::get_reserve_proof(const std::vector<std::string> &args)
{
	if (args.size() != 1 && args.size() != 2) {
		fail_msg_writer() << "Usage: get_reserve_proof (all|<amount>) [<message>]";
		return true;
	}


	uint64_t reserve = 0;
	if (args[0] != "all") {
		if (!m_currency.parseAmount(args[0], reserve)) {
			fail_msg_writer() << "amount is wrong: " << args[0];
			return true;
		}
	} else {
		reserve = m_wallet->actualBalance();
	}

	try {
		const std::string sig_str = m_wallet->getReserveProof(reserve, args.size() == 2 ? args[1] : "");
		
		//logger(INFO, BRIGHT_WHITE) << "\n\n" << sig_str << "\n\n" << std::endl;

		const std::string filename = "reserve_proof_" + args[0] + "_Ekr.txt";
		boost::system::error_code ec;
		if (boost::filesystem::exists(filename, ec)) {
			boost::filesystem::remove(filename, ec);
		}

		std::ofstream proofFile(filename, std::ios::out | std::ios::trunc | std::ios::binary);
		if (!proofFile.good()) {
			return false;
		}
		proofFile << sig_str;

		success_msg_writer() << "signature file saved to: " << filename;

	} catch (const std::exception &e) {
		fail_msg_writer() << e.what();
	}

	return true;
}


bool ekrone_wallet::get_tx_proof(const std::vector<std::string> &args)
{
  if(args.size() != 2 && args.size() != 3) {
    fail_msg_writer() << "Usage: get_tx_proof <txid> <dest_address> [<txkey>]";
    return true;
  }

  const std::string &str_hash = args[0];
  crypto::Hash txid;
  if (!parse_hash256(str_hash, txid)) {
    fail_msg_writer() << "Failed to parse txid";
    return true;
  }

  const std::string address_string = args[1];
  cn::AccountPublicAddress address;
  if (!m_currency.parseAccountAddressString(address_string, address)) {
     fail_msg_writer() << "Failed to parse address " << address_string;
     return true;
  }

  std::string sig_str;
  crypto::SecretKey tx_key, tx_key2;
  bool r = m_wallet->get_tx_key(txid, tx_key);

  if (args.size() == 3) {
    crypto::Hash tx_key_hash;
    size_t size;
    if (!common::fromHex(args[2], &tx_key_hash, sizeof(tx_key_hash), size) || size != sizeof(tx_key_hash)) {
      fail_msg_writer() << "failed to parse tx_key";
      return true;
    }
    tx_key2 = *(struct crypto::SecretKey *) &tx_key_hash;

    if (r) {
      if (args.size() == 3 && tx_key != tx_key2) {
        fail_msg_writer() << "Tx secret key was found for the given txid, but you've also provided another tx secret key which doesn't match the found one.";
        return true;
      }
    }
	tx_key = tx_key2;
  } else {
    if (!r) {
      fail_msg_writer() << "Tx secret key wasn't found in the wallet file. Provide it as the optional third parameter if you have it elsewhere.";
      return true;
    }
  }

  if (m_wallet->getTxProof(txid, address, tx_key, sig_str)) {
    success_msg_writer() << "Signature: " << sig_str << std::endl;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------
void ekrone_wallet::initCompleted(std::error_code result) {
  if (m_initResultPromise.get() != nullptr) {
    m_initResultPromise->set_value(result);
  }
}
//----------------------------------------------------------------------------------------------------
void ekrone_wallet::connectionStatusUpdated(bool connected) {
  if (connected) {
    logger(INFO, GREEN) << "Wallet connected to daemon.";
  } else {
    printConnectionError();
  }
}
//----------------------------------------------------------------------------------------------------
void ekrone_wallet::externalTransactionCreated(cn::TransactionId transactionId)  {
  WalletLegacyTransaction txInfo;
  m_wallet->getTransaction(transactionId, txInfo);

  std::stringstream logPrefix;
  if (txInfo.blockHeight == WALLET_LEGACY_UNCONFIRMED_TRANSACTION_HEIGHT) {
    logPrefix << "Unconfirmed";
  } else {
    logPrefix << "Height " << txInfo.blockHeight << ',';
  }

  if (txInfo.totalAmount >= 0) {
    logger(INFO, GREEN) <<
      logPrefix.str() << " transaction " << common::podToHex(txInfo.hash) <<
      ", received " << m_currency.formatAmount(txInfo.totalAmount);
  } else {
    logger(INFO, MAGENTA) <<
      logPrefix.str() << " transaction " << common::podToHex(txInfo.hash) <<
      ", spent " << m_currency.formatAmount(static_cast<uint64_t>(-txInfo.totalAmount));
  }

  if (txInfo.blockHeight == WALLET_LEGACY_UNCONFIRMED_TRANSACTION_HEIGHT) {
    m_refresh_progress_reporter.update(m_node->getLastLocalBlockHeight(), true);
  } else {
    m_refresh_progress_reporter.update(txInfo.blockHeight, true);
  }
}
//----------------------------------------------------------------------------------------------------
void ekrone_wallet::synchronizationCompleted(std::error_code result) {
  std::unique_lock<std::mutex> lock(m_walletSynchronizedMutex);
  m_walletSynchronized = true;
  m_walletSynchronizedCV.notify_one();
}

void ekrone_wallet::synchronizationProgressUpdated(uint32_t current, uint32_t total) {
  std::unique_lock<std::mutex> lock(m_walletSynchronizedMutex);
  if (!m_walletSynchronized) {
    m_refresh_progress_reporter.update(current, false);
  }
}

bool ekrone_wallet::show_balance(const std::vector<std::string>& args/* = std::vector<std::string>()*/) {
  success_msg_writer() << "available balance: " << m_currency.formatAmount(m_wallet->actualBalance()) <<
    ", locked amount: " << m_currency.formatAmount(m_wallet->pendingBalance()) <<
    ", total amount: " << m_currency.formatAmount(m_wallet->actualBalance() + m_wallet->pendingBalance());

  return true;
}

bool ekrone_wallet::sign_message(const std::vector<std::string>& args)
{
  if(args.size() < 1)
  {
    fail_msg_writer() << "Use: sign_message <message>";
    return true;
  }
    
  AccountKeys keys;
  m_wallet->getAccountKeys(keys);

  crypto::Hash message_hash;
  crypto::Signature sig;
  crypto::cn_fast_hash(args[0].data(), args[0].size(), message_hash);
  crypto::generate_signature(message_hash, keys.address.spendPublicKey, keys.spendSecretKey, sig);
  
  success_msg_writer() << "Sig" << tools::base_58::encode(std::string(reinterpret_cast<char*>(&sig)));

  return true;	
}

bool ekrone_wallet::verify_signature(const std::vector<std::string>& args)
{
  if (args.size() != 3)
  {
    fail_msg_writer() << "Use: verify_signature <message> <address> <signature>";
    return true;
  }
  
  std::string encodedSig = args[2];
  const size_t prefix_size = strlen("Sig");
  
  if(encodedSig.substr(0, prefix_size) != "Sig")
  {
    fail_msg_writer() << "Invalid signature prefix";
    return true;
  } 
  
  crypto::Hash message_hash;
  crypto::cn_fast_hash(args[0].data(), args[0].size(), message_hash);
  
  std::string decodedSig;
  crypto::Signature sig;
  tools::base_58::decode(encodedSig.substr(prefix_size), decodedSig);
  memcpy(&sig, decodedSig.data(), sizeof(sig));
  
  uint64_t prefix;
  cn::AccountPublicAddress addr;
  cn::parseAccountAddressString(prefix, addr, args[1]);
  
  if(crypto::check_signature(message_hash, addr.spendPublicKey, sig))
    success_msg_writer() << "Valid";
  else
    success_msg_writer() << "Invalid";
  return true;
}

/* ------------------------------------------------------------------------------------------- */

/* CREATE INTEGRATED ADDRESS */
/* take a payment Id as an argument and generate an integrated wallet address */

bool ekrone_wallet::create_integrated(const std::vector<std::string>& args/* = std::vector<std::string>()*/) 
{

  /* check if there is a payment id */
  if (args.empty()) 
  {

    fail_msg_writer() << "Please enter a payment ID";
    return true;
  }

  std::string paymentID = args[0];
  std::regex hexChars("^[0-9a-f]+$");
  if(paymentID.size() != 64 || !regex_match(paymentID, hexChars))
  {
    fail_msg_writer() << "Invalid payment ID";
    return true;
  }

  std::string address = m_wallet->getAddress();
  uint64_t prefix;
  cn::AccountPublicAddress addr;

  /* get the spend and view public keys from the address */
  if(!cn::parseAccountAddressString(prefix, addr, address))
  {
    logger(ERROR, BRIGHT_RED) << "Failed to parse account address from string";
    return true;
  }

  cn::BinaryArray ba;
  cn::toBinaryArray(addr, ba);
  std::string keys = common::asString(ba);

  /* create the integrated address the same way you make a public address */
  std::string integratedAddress = tools::base_58::encode_addr (cn::parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX,
                                                              paymentID + keys
  );

  std::cout << std::endl << "Integrated address: " << integratedAddress << std::endl << std::endl;

  return true;
}

/* ---------------------------------------------------------------------------------------- */


bool ekrone_wallet::export_keys(const std::vector<std::string>& args/* = std::vector<std::string>()*/) {
  AccountKeys keys;
  m_wallet->getAccountKeys(keys);

  std::string secretKeysData = std::string(reinterpret_cast<char*>(&keys.spendSecretKey), sizeof(keys.spendSecretKey)) + std::string(reinterpret_cast<char*>(&keys.viewSecretKey), sizeof(keys.viewSecretKey));
  std::string guiKeys = tools::base_58::encode_addr(cn::parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX, secretKeysData);

  logger(INFO, BRIGHT_GREEN) << std::endl << "EkroneIwallet is an open-source, client-side, free wallet which allow you to send and receive Ekr instantly on the blockchain. You are  in control of your funds & your keys. When you generate a new wallet, login, send, receive or deposit $Ekr everything happens locally. Your seed is never transmitted, received or stored. That's why its imperative to write, print or save your seed somewhere safe. The backup of keys is your responsibility. If you lose your seed, your account can not be recovered. The Ekrone Team doesn't take any responsibility for lost funds due to nonexistent/missing/lost private keys." << std::endl << std::endl;

  std::cout << "Private spend key: " << common::podToHex(keys.spendSecretKey) << std::endl;
  std::cout << "Private view key: " <<  common::podToHex(keys.viewSecretKey) << std::endl;

  crypto::PublicKey unused_dummy_variable;
  crypto::SecretKey deterministic_private_view_key;

  AccountBase::generateViewFromSpend(keys.spendSecretKey, deterministic_private_view_key, unused_dummy_variable);

  bool deterministic_private_keys = deterministic_private_view_key == keys.viewSecretKey;

/* dont show a mnemonic seed if it is an old non-deterministic wallet */
  if (deterministic_private_keys) {
    std::cout << "Mnemonic seed: " << generate_mnemonic(keys.spendSecretKey) << std::endl << std::endl;
  }
  return true;
}
//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::show_incoming_transfers(const std::vector<std::string>& args) {
  bool hasTransfers = false;
  size_t transactionsCount = m_wallet->getTransactionCount();
  for (size_t trantransactionNumber = 0; trantransactionNumber < transactionsCount; ++trantransactionNumber) {
    WalletLegacyTransaction txInfo;
    m_wallet->getTransaction(trantransactionNumber, txInfo);
    if (txInfo.totalAmount < 0) continue;
    hasTransfers = true;
    logger(INFO, YELLOW) << "        amount       \t                              tx id";
    logger(INFO, GREEN) <<
      std::setw(21) << m_currency.formatAmount(txInfo.totalAmount) << '\t' << common::podToHex(txInfo.hash);
  }

  if (!hasTransfers) success_msg_writer() << "No incoming transfers";
  return true;
}

//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::show_outgoing_transfers(const std::vector<std::string>& args) {
  bool hasTransfers = false;
  size_t transactionsCount = m_wallet->getTransactionCount();
  for (size_t trantransactionNumber = 0; trantransactionNumber < transactionsCount; ++trantransactionNumber) {
    WalletLegacyTransaction txInfo;
    m_wallet->getTransaction(trantransactionNumber, txInfo);
    if (txInfo.totalAmount > 0) continue;
    hasTransfers = true;
    logger(INFO) << "        amount       \t                              tx id";
    logger(INFO, MAGENTA) <<
      std::setw(21) << m_currency.formatAmount(txInfo.totalAmount) << '\t' << common::podToHex(txInfo.hash);
  }

  if (!hasTransfers) success_msg_writer() << "No outgoing transfers";
  return true;
}


bool ekrone_wallet::listTransfers(const std::vector<std::string>& args) {
  bool haveTransfers = false;
  bool haveBlockHeight = false;
  std::string blockHeightString = ""; 
  uint32_t blockHeight = 0;
  WalletLegacyTransaction txInfo;


  /* get block height from arguments */
  if (args.empty()) 
  {
    haveBlockHeight = false;
  } else {
    blockHeightString = args[0];
    haveBlockHeight = true;
    blockHeight = atoi(blockHeightString.c_str());
  }

  size_t transactionsCount = m_wallet->getTransactionCount();
  for (size_t trantransactionNumber = 0; trantransactionNumber < transactionsCount; ++trantransactionNumber) 
  {
    
    m_wallet->getTransaction(trantransactionNumber, txInfo);
    if (txInfo.state != WalletLegacyTransactionState::Active || txInfo.blockHeight == WALLET_LEGACY_UNCONFIRMED_TRANSACTION_HEIGHT) {
      continue;
    }

    if (!haveTransfers) {
      printListTransfersHeader(logger);
      haveTransfers = true;
    }

    if (haveBlockHeight == false) {
      printListTransfersItem(logger, txInfo, *m_wallet, m_currency);
    } else {
      if (txInfo.blockHeight >= blockHeight) {
        printListTransfersItem(logger, txInfo, *m_wallet, m_currency);

      }

    }
  }

  if (!haveTransfers) {
    success_msg_writer() << "No transfers";
  }

  return true;
}

bool ekrone_wallet::show_payments(const std::vector<std::string> &args) {
  if (args.empty()) {
    fail_msg_writer() << "expected at least one payment ID";
    return true;
  }

  try {
    auto hashes = args;
    std::sort(std::begin(hashes), std::end(hashes));
    hashes.erase(std::unique(std::begin(hashes), std::end(hashes)), std::end(hashes));
    std::vector<PaymentId> paymentIds;
    paymentIds.reserve(hashes.size());
    std::transform(std::begin(hashes), std::end(hashes), std::back_inserter(paymentIds), [](const std::string& arg) {
      PaymentId paymentId;
      if (!cn::parsePaymentId(arg, paymentId)) {
        throw std::runtime_error("payment ID has invalid format: \"" + arg + "\", expected 64-character string");
      }

      return paymentId;
    });

    logger(INFO, YELLOW) << "                            payment                             \t" <<
      "                          transaction                           \t" <<
      "  height\t       amount        ";

    auto payments = m_wallet->getTransactionsByPaymentIds(paymentIds);

    for (auto& payment : payments) {
      for (auto& transaction : payment.transactions) {
        success_msg_writer(true) <<
          common::podToHex(payment.paymentId) << '\t' <<
          common::podToHex(transaction.hash) << '\t' <<
          std::setw(8) << transaction.blockHeight << '\t' <<
          std::setw(21) << m_currency.formatAmount(transaction.totalAmount);
      }

      if (payment.transactions.empty()) {
        success_msg_writer() << "No payments with id " << common::podToHex(payment.paymentId);
      }
    }
  } catch (std::exception& e) {
    fail_msg_writer() << "show_payments exception: " << e.what();
  }

  return true;
}
//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::show_blockchain_height(const std::vector<std::string>& args) {
  try {
    uint64_t bc_height = m_node->getLastLocalBlockHeight();
    success_msg_writer() << bc_height;
  } catch (std::exception &e) {
    fail_msg_writer() << "failed to get blockchain height: " << e.what();
  }

  return true;
}
//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::show_num_unlocked_outputs(const std::vector<std::string>& args) {
  try {
    std::vector<TransactionOutputInformation> unlocked_outputs = m_wallet->getUnspentOutputs();
    success_msg_writer() << "Count: " << unlocked_outputs.size();
    for (const auto& out : unlocked_outputs) {
      success_msg_writer() << "Key: " << out.transactionPublicKey << " amount: " << m_currency.formatAmount(out.amount);
    }
  } catch (std::exception &e) {
    fail_msg_writer() << "failed to get outputs: " << e.what();
  }

  return true;
}
//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::optimize_outputs(const std::vector<std::string>& args) {
  try {
    cn::WalletHelper::SendCompleteResultObserver sent;
    WalletHelper::IWalletRemoveObserverGuard removeGuard(*m_wallet, sent);

    std::vector<cn::WalletLegacyTransfer> transfers;
    std::vector<cn::TransactionMessage> messages;
    std::string extraString;
    uint64_t fee = cn::parameters::MINIMUM_FEE;
    uint64_t mixIn = 0;
    uint64_t unlockTimestamp = 0;
    uint64_t ttl = 0;
    crypto::SecretKey transactionSK;
    cn::TransactionId tx = m_wallet->sendTransaction(transactionSK, transfers, fee, extraString, mixIn, unlockTimestamp, messages, ttl);
    if (tx == WALLET_LEGACY_INVALID_TRANSACTION_ID) {
      fail_msg_writer() << "Can't send money";
      return true;
    }

    std::error_code sendError = sent.wait(tx);
    removeGuard.removeObserver();

    if (sendError) {
      fail_msg_writer() << sendError.message();
      return true;
    }

    cn::WalletLegacyTransaction txInfo;
    m_wallet->getTransaction(tx, txInfo);
    success_msg_writer(true) << "Money successfully sent, transaction " << common::podToHex(txInfo.hash);
    success_msg_writer(true) << "Transaction secret key " << common::podToHex(transactionSK);

    try {
      cn::WalletHelper::storeWallet(*m_wallet, m_wallet_file);
    } catch (const std::exception& e) {
      fail_msg_writer() << e.what();
      return true;
    }
  } catch (const std::system_error& e) {
    fail_msg_writer() << e.what();
  } catch (const std::exception& e) {
    fail_msg_writer() << e.what();
  } catch (...) {
    fail_msg_writer() << "unknown error";
  }

  return true;
}

//----------------------------------------------------------------------------------------------------


bool ekrone_wallet::optimize_all_outputs(const std::vector<std::string>& args) {

  uint64_t num_unlocked_outputs = 0;

  try {
    num_unlocked_outputs = m_wallet->getNumUnlockedOutputs();
    success_msg_writer() << "Total outputs: " << num_unlocked_outputs;

  } catch (std::exception &e) {
    fail_msg_writer() << "failed to get outputs: " << e.what();
  }

  uint64_t remainder = num_unlocked_outputs % 100;
  uint64_t rounds = (num_unlocked_outputs - remainder) / 100;
  success_msg_writer() << "Total optimization rounds: " << rounds;
  for(uint64_t a = 1; a < rounds; a = a + 1 ) {
    
    try {
      cn::WalletHelper::SendCompleteResultObserver sent;
      WalletHelper::IWalletRemoveObserverGuard removeGuard(*m_wallet, sent);

      std::vector<cn::WalletLegacyTransfer> transfers;
      std::vector<cn::TransactionMessage> messages;
      std::string extraString;
      uint64_t fee = cn::parameters::MINIMUM_FEE;
      uint64_t mixIn = 0;
      uint64_t unlockTimestamp = 0;
      uint64_t ttl = 0;
      crypto::SecretKey transactionSK;
      cn::TransactionId tx = m_wallet->sendTransaction(transactionSK, transfers, fee, extraString, mixIn, unlockTimestamp, messages, ttl);
      if (tx == WALLET_LEGACY_INVALID_TRANSACTION_ID) {
        fail_msg_writer() << "Can't send money";
        return true;
      }

      std::error_code sendError = sent.wait(tx);
      removeGuard.removeObserver();

      if (sendError) {
        fail_msg_writer() << sendError.message();
        return true;
      }

      cn::WalletLegacyTransaction txInfo;
      m_wallet->getTransaction(tx, txInfo);
      success_msg_writer(true) << a << ". Optimization transaction successfully sent, transaction " << common::podToHex(txInfo.hash);

      try {
        cn::WalletHelper::storeWallet(*m_wallet, m_wallet_file);
      } catch (const std::exception& e) {
        fail_msg_writer() << e.what();
        return true;
      }
    } catch (const std::system_error& e) {
      fail_msg_writer() << e.what();
    } catch (const std::exception& e) {
      fail_msg_writer() << e.what();
    } catch (...) {
      fail_msg_writer() << "unknown error";
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------

std::string ekrone_wallet::resolveAlias(const std::string& aliasUrl) {
  std::string host;
	std::string uri;
	std::vector<std::string>records;
	std::string address;

	if (!common::fetch_dns_txt(aliasUrl, records)) {
		throw std::runtime_error("Failed to lookup DNS record");
	}

	for (const auto& record : records) {
		if (processServerAliasResponse(record, address)) {
			return address;
		}
	}
	throw std::runtime_error("Failed to parse server response");
}
//----------------------------------------------------------------------------------------------------

/* This extracts the fee address from the remote node */
std::string ekrone_wallet::getFeeAddress() {
  
  HttpClient httpClient(m_dispatcher, m_daemon_host, m_daemon_port);

  HttpRequest req;
  HttpResponse res;

  req.setUrl("/feeaddress");
  try {
	  httpClient.request(req, res);
  }
  catch (const std::exception& e) {
	  fail_msg_writer() << "Error connecting to the remote node: " << e.what();
  }

  if (res.getStatus() != HttpResponse::STATUS_200) {
	  fail_msg_writer() << "Remote node returned code " + std::to_string(res.getStatus());
  }

  std::string address;
  if (!processServerFeeAddressResponse(res.getBody(), address)) {
	  fail_msg_writer() << "Failed to parse remote node response";
  }

  return address;
}


bool ekrone_wallet::transfer(const std::vector<std::string> &args) {
  try {
    TransferCommand cmd(m_currency, m_remote_node_address);

    if (!cmd.parseArguments(logger, args))
      return true;

    for (auto& kv: cmd.aliases) {
      std::string address;

      try {
        address = resolveAlias(kv.first);

        AccountPublicAddress ignore;
        if (!m_currency.parseAccountAddressString(address, ignore)) {
          throw std::runtime_error("Address \"" + address + "\" is invalid");
        }
      } catch (std::exception& e) {
        fail_msg_writer() << "Couldn't resolve alias: " << e.what() << ", alias: " << kv.first;
        return true;
      }

      for (auto& transfer: kv.second) {
        transfer.address = address;
      }
    }

    if (!cmd.aliases.empty()) {
      if (!askAliasesTransfersConfirmation(cmd.aliases, m_currency)) {
        return true;
      }

      for (auto& kv: cmd.aliases) {
        std::copy(std::move_iterator<std::vector<WalletLegacyTransfer>::iterator>(kv.second.begin()),
                  std::move_iterator<std::vector<WalletLegacyTransfer>::iterator>(kv.second.end()),
                  std::back_inserter(cmd.dsts));
      }
    }

    std::vector<TransactionMessage> messages;
    for (auto dst : cmd.dsts) {
      for (auto msg : cmd.messages) {
        messages.emplace_back(TransactionMessage{ msg, dst.address });
      }
    }

    uint64_t ttl = 0;
    if (cmd.ttl != 0) {
      ttl = static_cast<uint64_t>(time(nullptr)) + cmd.ttl;
    }

    cn::WalletHelper::SendCompleteResultObserver sent;

    std::string extraString;
    std::copy(cmd.extra.begin(), cmd.extra.end(), std::back_inserter(extraString));

    WalletHelper::IWalletRemoveObserverGuard removeGuard(*m_wallet, sent);

  
    /* force minimum fee */
    if (cmd.fee < cn::parameters::MINIMUM_FEE) {
      cmd.fee = cn::parameters::MINIMUM_FEE;
    }

    crypto::SecretKey transactionSK;
    cn::TransactionId tx = m_wallet->sendTransaction(transactionSK, cmd.dsts, cmd.fee, extraString, cmd.fake_outs_count, 0, messages, ttl);
    if (tx == WALLET_LEGACY_INVALID_TRANSACTION_ID) {
      fail_msg_writer() << "Can't send money";
      return true;
    }

    std::error_code sendError = sent.wait(tx);
    removeGuard.removeObserver();

    if (sendError) {
      fail_msg_writer() << sendError.message();
      return true;
    }

    cn::WalletLegacyTransaction txInfo;
    m_wallet->getTransaction(tx, txInfo);
    success_msg_writer(true) << "Money successfully sent, transaction hash: " << common::podToHex(txInfo.hash);
    success_msg_writer(true) << "Transaction secret key " << common::podToHex(transactionSK); 

    try {
      cn::WalletHelper::storeWallet(*m_wallet, m_wallet_file);
    } catch (const std::exception& e) {
      fail_msg_writer() << e.what();
      return true;
    }
  } catch (const std::system_error& e) {
    fail_msg_writer() << e.what();
  } catch (const std::exception& e) {
    fail_msg_writer() << e.what();
  } catch (...) {
    fail_msg_writer() << "unknown error";
  }

  return true;
}
//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::run() {
  {
    std::unique_lock<std::mutex> lock(m_walletSynchronizedMutex);
    while (!m_walletSynchronized) {
      m_walletSynchronizedCV.wait(lock);
    }
  }

  std::cout << std::endl;

  std::string addr_start = m_wallet->getAddress().substr(0, 6);
  m_consoleHandler.start(false, "[wallet " + addr_start + "]: ", common::console::Color::BrightYellow);
  return true;
}
//----------------------------------------------------------------------------------------------------
void ekrone_wallet::stop() {
  m_consoleHandler.requestStop();
}
//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::print_address(const std::vector<std::string> &args/* = std::vector<std::string>()*/) {
  success_msg_writer() << m_wallet->getAddress();
  return true;
}
//----------------------------------------------------------------------------------------------------
bool ekrone_wallet::process_command(const std::vector<std::string> &args) {
  return m_consoleHandler.runCommand(args);
}

void ekrone_wallet::printConnectionError() const {
  fail_msg_writer() << "wallet failed to connect to daemon (" << m_daemon_address << ").";
}

bool ekrone_wallet::save_keys_to_file(const std::vector<std::string>& args)
{
  if (!args.empty())
  {
    logger(ERROR) <<  "Usage: \"export_keys\"";
    return true;
  }

  /* remove ".wallet" from the end of the string */
  std::string formatted_wal_str = m_wallet_file.erase(m_wallet_file.size() - 7);
  std::ofstream backup_file(formatted_wal_str + "_ekrone_backup.txt");

  AccountKeys keys;
  m_wallet->getAccountKeys(keys);

  std::string priv_key = "\t\tEkrone Keys Backup\n\n";
  priv_key += "Wallet file name: " + m_wallet_file + "\n";
  priv_key += "Private spend key: " + common::podToHex(keys.spendSecretKey) + "\n";
  priv_key += "Private view key: " +  common::podToHex(keys.viewSecretKey) + "\n";

  crypto::PublicKey unused_dummy_variable;
  crypto::SecretKey deterministic_private_view_key;

  AccountBase::generateViewFromSpend(keys.spendSecretKey, deterministic_private_view_key, unused_dummy_variable);
  bool deterministic_private_keys = deterministic_private_view_key == keys.viewSecretKey;

  /* dont show a mnemonic seed if it is an old non-deterministic wallet */
  if (deterministic_private_keys)
    priv_key += "Mnemonic seed: " + generate_mnemonic(keys.spendSecretKey) + "\n";

  backup_file << priv_key;

  logger(INFO, BRIGHT_GREEN) << "Wallet keys have been saved to the current folder where \"ekronewallet\" is located as \""
    << formatted_wal_str << "_ekrone_backup.txt\".";

  return true;
}

bool ekrone_wallet::list_deposits(const std::vector<std::string> &args)
{
  bool haveDeposits = m_wallet->getDepositCount() > 0;

  if (!haveDeposits)
  {
    success_msg_writer() << "No deposits";
    return true;
  }

  printListDepositsHeader(logger);

  /* go through deposits ids for the amount of deposits in wallet */
  for (DepositId id = 0; id < m_wallet->getDepositCount(); ++id)
  {
    /* get deposit info from id and store it to deposit */
    Deposit deposit = m_wallet->get_deposit(id);
    cn::WalletLegacyTransaction txInfo;
    m_wallet->getTransaction(deposit.creatingTransactionId, txInfo);

    logger(INFO) << m_chelper.get_deposit_info(deposit, id, m_currency, txInfo);
  }

  return true;
}

bool ekrone_wallet::deposit(const std::vector<std::string> &args)
{
  if (args.size() != 2)
  {
    logger(ERROR) << "Usage: deposit <months> <amount>";
    return true;
  }

  try
  {
    /**
     * Change arg to uint64_t using boost then
     * multiply by min_term so user can type in months
    **/
    uint64_t deposit_term = boost::lexical_cast<uint64_t>(args[0]) * 22000;

    /* Now validate the deposit term and the amount */
    if (deposit_term < cn::parameters::DEPOSIT_MIN_TERM)
    {
      logger(ERROR, BRIGHT_RED) << "Deposit term is too small, min=22000, given=" << deposit_term;
      return true;
    }

    if (deposit_term > cn::parameters::DEPOSIT_MAX_TERM)
    {
      logger(ERROR, BRIGHT_RED) << "Deposit term is too big, min=" << cn::parameters::DEPOSIT_MAX_TERM
        << ", given=" << deposit_term;
      return true;
    }

    uint64_t deposit_amount = boost::lexical_cast<uint64_t>(args[1]);
    bool ok = m_currency.parseAmount(args[1], deposit_amount);

    if (!ok || 0 == deposit_amount)
    {
      logger(ERROR, BRIGHT_RED) << "amount is wrong: " << deposit_amount <<
        ", expected number from 1 to " << m_currency.formatAmount(cn::parameters::MONEY_SUPPLY);
      return true;
    }

    if (deposit_amount < cn::parameters::DEPOSIT_MIN_AMOUNT)
    {
      logger(ERROR, BRIGHT_RED) << "Deposit amount is too small, min=" << cn::parameters::DEPOSIT_MIN_AMOUNT
        << ", given=" << m_currency.formatAmount(deposit_amount);
      return true;
    }

    if (!confirm_deposit(deposit_term, deposit_amount))
    {
      logger(ERROR) << "Deposit is not being created.";
      return true;
    }

    logger(INFO) << "Creating deposit...";

    /* Use defaults for fee + mix in */
    uint64_t deposit_fee = cn::parameters::MINIMUM_FEE;
    uint64_t deposit_mix_in = cn::parameters::MINIMUM_MIXIN;

    cn::WalletHelper::SendCompleteResultObserver sent;
    WalletHelper::IWalletRemoveObserverGuard removeGuard(*m_wallet, sent);

    cn::TransactionId tx = m_wallet->deposit(deposit_term, deposit_amount, deposit_fee, deposit_mix_in);

    if (tx == WALLET_LEGACY_INVALID_DEPOSIT_ID)
    {
      fail_msg_writer() << "Can't deposit money";
      return true;
    }

    std::error_code sendError = sent.wait(tx);
    removeGuard.removeObserver();

    if (sendError)
    {
      fail_msg_writer() << sendError.message();
      return true;
    }

    cn::WalletLegacyTransaction d_info;
    m_wallet->getTransaction(tx, d_info);
    success_msg_writer(true) << "Money successfully sent, transaction hash: " << common::podToHex(d_info.hash)
      << "\n\tID: " << d_info.firstDepositId;

    try
    {
      cn::WalletHelper::storeWallet(*m_wallet, m_wallet_file);
    }
    catch (const std::exception& e)
    {
      fail_msg_writer() << e.what();
      return true;
    }
  }
  catch (const std::system_error& e)
  {
    fail_msg_writer() << e.what();
  }
  catch (const std::exception& e)
  {
    fail_msg_writer() << e.what();
  }
  catch (...)
  {
    fail_msg_writer() << "unknown error";
  }

  return true;
}

bool ekrone_wallet::withdraw(const std::vector<std::string> &args)
{
  if (args.size() != 1)
  {
    logger(ERROR) << "Usage: withdraw <id>";
    return true;
  }

  try
  {
    if (m_wallet->getDepositCount() == 0)
    {
      logger(ERROR) << "No deposits have been made in this wallet.";
      return true;
    }

    uint64_t deposit_id = boost::lexical_cast<uint64_t>(args[0]);
    uint64_t deposit_fee = cn::parameters::MINIMUM_FEE;
    
    cn::WalletHelper::SendCompleteResultObserver sent;
    WalletHelper::IWalletRemoveObserverGuard removeGuard(*m_wallet, sent);

    cn::TransactionId tx = m_wallet->withdrawDeposit(deposit_id, deposit_fee);
  
    if (tx == WALLET_LEGACY_INVALID_DEPOSIT_ID)
    {
      fail_msg_writer() << "Can't withdraw money";
      return true;
    }

    std::error_code sendError = sent.wait(tx);
    removeGuard.removeObserver();

    if (sendError)
    {
      fail_msg_writer() << sendError.message();
      return true;
    }

    cn::WalletLegacyTransaction d_info;
    m_wallet->getTransaction(tx, d_info);
    success_msg_writer(true) << "Money successfully sent, transaction hash: " << common::podToHex(d_info.hash);

    try
    {
      cn::WalletHelper::storeWallet(*m_wallet, m_wallet_file);
    }
    catch (const std::exception& e)
    {
      fail_msg_writer() << e.what();
      return true;
    }
  }
  catch (std::exception &e)
  {
    fail_msg_writer() << "failed to withdraw deposit: " << e.what();
  }

  return true;
}

bool ekrone_wallet::deposit_info(const std::vector<std::string> &args)
{
  if (args.size() != 1)
  {
    logger(ERROR) << "Usage: withdraw <id>";
    return true;
  }

  uint64_t deposit_id = boost::lexical_cast<uint64_t>(args[0]);
  cn::Deposit deposit = m_wallet->get_deposit(deposit_id);
  cn::WalletLegacyTransaction txInfo;
  m_wallet->getTransaction(deposit.creatingTransactionId, txInfo);

  logger(INFO) << m_chelper.get_full_deposit_info(deposit, deposit_id, m_currency, txInfo);

  return true;
}

bool ekrone_wallet::confirm_deposit(uint64_t term, uint64_t amount)
{
  uint64_t interest = m_currency.calculateInterest(amount, term);

  logger(INFO) << "Confirm deposit details:\n"
    << "\tAmount: " << m_currency.formatAmount(amount) << "\n"
    << "\tMonths: " << term / 22000 << "\n"
    << "\tInterest: " << m_currency.formatAmount(interest) << "\n";

  logger(INFO) << "Is this correct? (Y/N): \n";

  char c;
  std::cin >> c;
  c = std::tolower(c);

  if (c == 'y')
  {
    return true;
  }
  else if (c == 'n')
  {
    return false;
  }
  else
  {
    logger(ERROR) << "Bad input, please enter either Y or N.";
  }

  return false;
}
