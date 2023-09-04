// Copyright (c) 2018-2022 Conceal Network & Conceal Devs
// Copyright (c) 2017-2022 Ekrone Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <fstream>
#include <boost/program_options.hpp>

#include "EkroneWallet.h"
#include "ClientHelper.h"
#include "Const.h"

#include "Common/CommandLine.h"
#include "Common/PathTools.h"
#include "Common/SignalHandler.h"
#include "Wallet/WalletRpcServer.h"

#include "version.h"

#include <Logging/LoggerManager.h>

#if defined(WIN32)
#include <crtdbg.h>
#endif

namespace po = boost::program_options;
using namespace logging;
using namespace cn;
using namespace std;

int main(int argc, char* argv[])
{
#ifdef _WIN32
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  po::options_description desc_general("General options");
  command_line::add_arg(desc_general, command_line::arg_help);
  command_line::add_arg(desc_general, command_line::arg_version);

  po::options_description desc_params("Wallet options");
  command_line::add_arg(desc_params, arg_wallet_file);
  command_line::add_arg(desc_params, arg_generate_new_wallet);
  command_line::add_arg(desc_params, arg_password);
  command_line::add_arg(desc_params, arg_daemon_address);
  command_line::add_arg(desc_params, arg_daemon_host);
  command_line::add_arg(desc_params, arg_daemon_port);
  command_line::add_arg(desc_params, arg_command);
  command_line::add_arg(desc_params, arg_log_level);
  command_line::add_arg(desc_params, arg_testnet);
  tools::wallet_rpc_server::init_options(desc_params);

  po::positional_options_description positional_options;
  positional_options.add(arg_command.name, -1);

  po::options_description desc_all;
  desc_all.add(desc_general).add(desc_params);

  logging::LoggerManager logManager;
  logging::LoggerRef logger(logManager, "ekronewallet");
  platform_system::Dispatcher dispatcher;

  po::variables_map vm;

  cn::client_helper m_chelper;

  bool r = command_line::handle_error_helper(desc_all, [&]() {
    po::store(command_line::parse_command_line(argc, argv, desc_general, true), vm);

    if (command_line::get_arg(vm, command_line::arg_help)) {
      cn::Currency tmp_currency = cn::CurrencyBuilder(logManager).currency();
      cn::ekrone_wallet tmp_wallet(dispatcher, tmp_currency, logManager);

      std::cout << "Ekrone Wallet v" << PROJECT_VERSION_LONG << std::endl;
      std::cout << "Usage: ekronewallet [--wallet-file=<file>|--generate-new-wallet=<file>] [--daemon-address=<host>:<port>] [<COMMAND>]";
      std::cout << desc_all << '\n' << tmp_wallet.get_commands_str(false);
      return false;
    } else if (command_line::get_arg(vm, command_line::arg_version))  {
      std::cout << "Ekrone Wallet v" << PROJECT_VERSION_LONG << std::endl;
      return false;
    }

    auto parser = po::command_line_parser(argc, argv).options(desc_params).positional(positional_options);
    po::store(parser.run(), vm);
    po::notify(vm);
    return true;
  });

  if (!r)
    return 1;

  //set up logging options
  Level logLevel = DEBUGGING;

  if (command_line::has_arg(vm, arg_log_level)) {
    logLevel = static_cast<Level>(command_line::get_arg(vm, arg_log_level));
  }

  logManager.configure(m_chelper.buildLoggerConfiguration(logLevel, common::ReplaceExtenstion(argv[0], ".log")));

  logger(INFO, BRIGHT_GREEN) << "Ekrone Wallet v" << PROJECT_VERSION_LONG;

  cn::Currency currency = cn::CurrencyBuilder(logManager).
    testnet(command_line::get_arg(vm, arg_testnet)).currency();

  if (command_line::has_arg(vm, tools::wallet_rpc_server::arg_rpc_bind_port)) {
    //runs wallet with rpc interface
    if (!command_line::has_arg(vm, arg_wallet_file)) {
      logger(ERROR, BRIGHT_RED) << "Wallet file not set.";
      return 1;
    }

    if (!command_line::has_arg(vm, arg_daemon_address)) {
      logger(ERROR, BRIGHT_RED) << "Daemon address not set.";
      return 1;
    }

    if (!command_line::has_arg(vm, arg_password)) {
      logger(ERROR, BRIGHT_RED) << "Wallet password not set.";
      return 1;
    }

    std::string wallet_file = command_line::get_arg(vm, arg_wallet_file);
    std::string wallet_password = command_line::get_arg(vm, arg_password);
    std::string daemon_address = command_line::get_arg(vm, arg_daemon_address);
    std::string daemon_host = command_line::get_arg(vm, arg_daemon_host);
    uint16_t daemon_port = command_line::get_arg(vm, arg_daemon_port);
    if (daemon_host.empty())
      daemon_host = "localhost";
    if (!daemon_port)
      daemon_port = RPC_DEFAULT_PORT;

    if (!daemon_address.empty()) {
      if (!m_chelper.parseUrlAddress(daemon_address, daemon_host, daemon_port)) {
        logger(ERROR, BRIGHT_RED) << "failed to parse daemon address: " << daemon_address;
        return 1;
      }
    }

    std::unique_ptr<INode> node(new NodeRpcProxy(daemon_host, daemon_port));

    std::promise<std::error_code> errorPromise;
    std::future<std::error_code> error = errorPromise.get_future();
    auto callback = [&errorPromise](std::error_code e) {errorPromise.set_value(e); };
    node->init(callback);
    if (error.get()) {
      logger(ERROR, BRIGHT_RED) << ("failed to init NodeRPCProxy");
      return 1;
    }

    std::unique_ptr<IWalletLegacy> wallet(new WalletLegacy(currency, *node.get(), logManager));

    std::string walletFileName;
    try  {
      walletFileName = m_chelper.tryToOpenWalletOrLoadKeysOrThrow(logger, wallet, wallet_file, wallet_password);

      logger(INFO) << "available balance: " << currency.formatAmount(wallet->actualBalance()) <<
      ", locked amount: " << currency.formatAmount(wallet->pendingBalance());

      logger(INFO, BRIGHT_GREEN) << "Loaded ok";
    } catch (const std::exception& e)  {
      logger(ERROR, BRIGHT_RED) << "Wallet initialize failed: " << e.what();
      return 1;
    }

    tools::wallet_rpc_server wrpc(dispatcher, logManager, *wallet, *node, currency, walletFileName);

    if (!wrpc.init(vm)) {
      logger(ERROR, BRIGHT_RED) << "Failed to initialize wallet rpc server";
      return 1;
    }

    tools::SignalHandler::install([&wrpc, &wallet] {
      wrpc.send_stop_signal();
    });

    logger(INFO, YELLOW) << "Starting wallet rpc server";
    wrpc.run();
    logger(INFO, YELLOW) << "Stopped wallet rpc server";

     logger(INFO) << "Starting wallet rpc server";
    wrpc.run();
    logger(INFO) << "Stopped wallet rpc server";

    logger(logging::INFO) << "Saving wallet...";
    m_chelper.save_wallet(*wallet, walletFileName, logger);
    logger(logging::INFO, logging::BRIGHT_GREEN) << "Saving successful";
  } 
  else 
  {
    //runs wallet with console interface
    cn::ekrone_wallet wal(dispatcher, currency, logManager);

    if (!wal.init(vm)) {
      logger(ERROR, BRIGHT_RED) << "Failed to initialize wallet";
      return 1;
    }

    std::vector<std::string> command = command_line::get_arg(vm, arg_command);
    if (!command.empty())
      wal.process_command(command);

    tools::SignalHandler::install([&wal] {
      wal.stop();
    });

    wal.run();

    if (!wal.deinit()) {
      logger(ERROR, BRIGHT_RED) << "Failed to close wallet";
    } else {
      logger(INFO, YELLOW) << "Wallet closed";
    }
  }
  return 1;
  //CATCH_ENTRY_L0("main", 1);
}