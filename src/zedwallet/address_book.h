// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2019, The Ekrone Developers
//
// Please see the included LICENSE file for more information.

#include <zedwallet/types.h>

void addToAddressBook();

void sendFromAddressBook(std::shared_ptr<WalletInfo> walletInfo,
                         uint32_t height, std::string nodeAddress, uint32_t nodeFee);

void deleteFromAddressBook();

void listAddressBook();

const Maybe<std::string> getAddressBookPaymentID();

const Maybe<const std::string> getAddressBookAddress();

const Maybe<const AddressBookEntry> getAddressBookEntry(AddressBook addressBook);

const std::string getAddressBookName(AddressBook addressBook);

AddressBook getAddressBook();

bool saveAddressBook(AddressBook addressBook);

bool isAddressBookEmpty(AddressBook addressBook);