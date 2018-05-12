// Copyright (c) 2014-2017 The Dash Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ACTIVEMASTERNODE_H
#define ACTIVEMASTERNODE_H

#include "net.h"
#include "key.h"
#include "wallet/wallet.h"

class CActiveMasternode;

static const int ACTIVE_MASTERNODE_INITIAL          = 0; // initial state
static const int ACTIVE_MASTERNODE_SYNC_IN_PROCESS  = 1;
static const int ACTIVE_MASTERNODE_INPUT_TOO_NEW    = 2;
static const int ACTIVE_MASTERNODE_NOT_CAPABLE      = 3;
static const int ACTIVE_MASTERNODE_STARTED          = 4;

extern CActiveMasternode activeMasternode;

#define ACTIVE_MASTERNODE_INITIAL                     0 // initial state
#define ACTIVE_MASTERNODE_SYNC_IN_PROCESS             1
#define ACTIVE_MASTERNODE_INPUT_TOO_NEW               2
#define ACTIVE_MASTERNODE_NOT_CAPABLE                 3
#define ACTIVE_MASTERNODE_STARTED                     4

// Responsible for activating the Masternode and pinging the network
class CActiveMasternode
{
private:
    // critical section to protect the inner data structures
    mutable CCriticalSection cs;

    /// Ping Masternode
    bool SendMasternodePing(std::string& errorMessage);

    /// Create Masternode broadcast, needs to be relayed manually after that
    bool CreateBroadcast(CTxIn vin, CService service, CKey key, CPubKey pubKey, CKey keyMasternode, CPubKey pubKeyMasternode, std::string &errorMessage, CMasternodeBroadcast &mnb);

    /// Get 1000DRK input that can be used for the Masternode
    bool GetMasterNodeVin(CTxIn& vin, CPubKey& pubkey, CKey& secretKey, std::string strTxHash, std::string strOutputIndex);
    bool GetVinFromOutput(COutput out, CTxIn& vin, CPubKey& pubkey, CKey& secretKey);

public:
    enum masternode_type_enum_t {
        MASTERNODE_UNKNOWN = 0,
        MASTERNODE_REMOTE  = 1,
        MASTERNODE_LOCAL   = 2
    };

private:
    // critical section to protect the inner data structures
    mutable CCriticalSection cs;

    masternode_type_enum_t eType;

    bool fPingerEnabled;

    /// Ping Masternode
    bool SendMasternodePing();

public:
    // Keys for the active Masternode
    CPubKey pubKeyMasternode;
    CKey keyMasternode;

    // Initialized while registering Masternode
    CTxIn vin;
    CService service;

    int nState; // should be one of ACTIVE_MASTERNODE_XXXX
    std::string strNotCapableReason;

    CActiveMasternode()
<<<<<<< HEAD
    {        
        status = ACTIVE_MASTERNODE_INITIAL;
    }

    /// Manage status of main Masternode
    void ManageStatus(); 
    std::string GetStatus();

    /// Create Masternode broadcast, needs to be relayed manually after that
    bool CreateBroadcast(std::string strService, std::string strKey, std::string strTxHash, std::string strOutputIndex, std::string& errorMessage, CMasternodeBroadcast &mnb, bool fOffline = false);

    /// Get 1000DRK input that can be used for the Masternode
    bool GetMasterNodeVin(CTxIn& vin, CPubKey& pubkey, CKey& secretKey);
    vector<COutput> SelectCoinsMasternode();

    /// Enable cold wallet mode (run a Masternode with no funds)
    bool EnableHotColdMasterNode(CTxIn& vin, CService& addr);
=======
        : eType(MASTERNODE_UNKNOWN),
          fPingerEnabled(false),
          pubKeyMasternode(),
          keyMasternode(),
          vin(),
          service(),
          nState(ACTIVE_MASTERNODE_INITIAL)
    {}

    /// Manage state of active Masternode
    void ManageState();

    std::string GetStateString() const;
    std::string GetStatus() const;
    std::string GetTypeString() const;

private:
    void ManageStateInitial();
    void ManageStateRemote();
    void ManageStateLocal();
>>>>>>> dev-1.12.1.0
};

#endif
