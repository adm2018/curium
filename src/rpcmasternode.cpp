// Copyright (c) 2014-2017 The Curium Core developers

// Distributed under the MIT/X11 software license, see the accompanying

// file COPYING or http://www.opensource.org/licenses/mit-license.php.



#include "activemasternode.h"

#include "darksend.h"

#include "init.h"

#include "main.h"

#include "masternode-payments.h"

#include "masternode-sync.h"

#include "masternodeconfig.h"

#include "masternodeman.h"

#include "rpcserver.h"

#include "util.h"

#include "utilmoneystr.h"

<<<<<<< HEAD
#include <fstream>
using namespace json_spirit;
=======


#include <fstream>
>>>>>>> dev-1.12.1.0

#include <iomanip>

#include <univalue.h>



void EnsureWalletIsUnlocked();



UniValue privatesend(const UniValue& params, bool fHelp)

{
<<<<<<< HEAD
    if (fHelp || params.size() == 0)
        throw runtime_error(
            "darksend <curiumaddress> <amount>\n"
            "curiumaddress, reset, or auto (AutoDenominate)"
            "<amount> is a real and will be rounded to the next 0.1"
=======

    if (fHelp || params.size() != 1)

        throw std::runtime_error(

            "privatesend \"command\"\n"

            "\nArguments:\n"

            "1. \"command\"        (string or set of strings, required) The command to execute\n"

            "\nAvailable commands:\n"

            "  start       - Start mixing\n"

            "  stop        - Stop mixing\n"

            "  reset       - Reset mixing\n"

>>>>>>> dev-1.12.1.0
            + HelpRequiringPassphrase());



    if(params[0].get_str() == "start") {

        if (pwalletMain->IsLocked(true))

            throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Please enter the wallet passphrase with walletpassphrase first.");



        if(fMasterNode)

<<<<<<< HEAD
        return "DoAutomaticDenominating " + (darkSendPool.DoAutomaticDenominating() ? "successful" : ("failed: " + darkSendPool.GetStatus()));
    }

    if(params[0].get_str() == "reset"){
        darkSendPool.Reset();
        return "successfully reset darksend";
    }

    if (params.size() != 2)
        throw runtime_error(
            "darksend <curiumaddress> <amount>\n"
            "curiumaddress, denominate, or auto (AutoDenominate)"
            "<amount> is a real and will be rounded to the next 0.1"
            + HelpRequiringPassphrase());
=======
            return "Mixing is not supported from masternodes";



        fEnablePrivateSend = true;

        bool result = darkSendPool.DoAutomaticDenominating();

        return "Mixing " + (result ? "started successfully" : ("start failed: " + darkSendPool.GetStatus() + ", will retry"));

    }



    if(params[0].get_str() == "stop") {

        fEnablePrivateSend = false;

        return "Mixing was stopped";

    }

>>>>>>> dev-1.12.1.0


    if(params[0].get_str() == "reset") {

        darkSendPool.ResetPool();

        return "Mixing was reset";

    }



    return "Unknown command, please see \"help privatesend\"";

}



UniValue getpoolinfo(const UniValue& params, bool fHelp)

{

    if (fHelp || params.size() != 0)

        throw std::runtime_error(

            "getpoolinfo\n"

            "Returns an object containing mixing pool related information.\n");



    UniValue obj(UniValue::VOBJ);

    obj.push_back(Pair("state",             darkSendPool.GetStateString()));

    obj.push_back(Pair("mixing_mode",       fPrivateSendMultiSession ? "multi-session" : "normal"));

    obj.push_back(Pair("queue",             darkSendPool.GetQueueSize()));

    obj.push_back(Pair("entries",           darkSendPool.GetEntriesCount()));

    obj.push_back(Pair("status",            darkSendPool.GetStatus()));



    if (darkSendPool.pSubmittedToMasternode) {

        obj.push_back(Pair("outpoint",      darkSendPool.pSubmittedToMasternode->vin.prevout.ToStringShort()));

        obj.push_back(Pair("addr",          darkSendPool.pSubmittedToMasternode->addr.ToString()));

    }



    if (pwalletMain) {

        obj.push_back(Pair("keys_left",     pwalletMain->nKeysLeftSinceAutoBackup));

        obj.push_back(Pair("warnings",      pwalletMain->nKeysLeftSinceAutoBackup < PRIVATESEND_KEYS_THRESHOLD_WARNING

                                                ? "WARNING: keypool is almost depleted!" : ""));

    }



    return obj;

}





UniValue masternode(const UniValue& params, bool fHelp)

{

    std::string strCommand;

    if (params.size() >= 1) {

        strCommand = params[0].get_str();

    }



    if (strCommand == "start-many")

        throw JSONRPCError(RPC_INVALID_PARAMETER, "DEPRECATED, please use start-all instead");



    if (fHelp  ||
<<<<<<< HEAD
        (strCommand != "start" && strCommand != "start-alias" && strCommand != "start-many" && strCommand != "start-all" && strCommand != "start-missing" &&
         strCommand != "start-disabled" && strCommand != "list" && strCommand != "list-conf" && strCommand != "count"  && strCommand != "enforce" &&
        strCommand != "debug" && strCommand != "current" && strCommand != "winners" && strCommand != "genkey" && strCommand != "connect" &&
        strCommand != "outputs" && strCommand != "status" && strCommand != "calcscore"))
        throw runtime_error(
=======

        (strCommand != "start" && strCommand != "start-alias" && strCommand != "start-all" && strCommand != "start-missing" &&

         strCommand != "start-disabled" && strCommand != "list" && strCommand != "list-conf" && strCommand != "count" &&

         strCommand != "debug" && strCommand != "current" && strCommand != "winner" && strCommand != "winners" && strCommand != "genkey" &&

         strCommand != "connect" && strCommand != "outputs" && strCommand != "status"))

            throw std::runtime_error(

>>>>>>> dev-1.12.1.0
                "masternode \"command\"... ( \"passphrase\" )\n"

                "Set of commands to execute masternode related actions\n"

                "\nArguments:\n"

                "1. \"command\"        (string or set of strings, required) The command to execute\n"

                "2. \"passphrase\"     (string, optional) The wallet passphrase\n"

                "\nAvailable commands:\n"
<<<<<<< HEAD
                "  count        - Print number of all known masternodes (optional: 'ds', 'enabled', 'all', 'qualify')\n"
                "  current      - Print info on current masternode winner\n"
=======

                "  count        - Print number of all known masternodes (optional: 'ps', 'enabled', 'all', 'qualify')\n"

                "  current      - Print info on current masternode winner to be paid the next block (calculated locally)\n"

>>>>>>> dev-1.12.1.0
                "  debug        - Print masternode status\n"

                "  genkey       - Generate new masternodeprivkey\n"

                "  outputs      - Print masternode compatible outputs\n"
<<<<<<< HEAD
                "  start        - Start masternode configured in curium.conf\n"
                "  start-alias  - Start single masternode by assigned alias configured in masternode.conf\n"
                "  start-<mode> - Start masternodes configured in masternode.conf (<mode>: 'all', 'missing', 'disabled')\n"
                "  status       - Print masternode status information\n"
=======

                "  start        - Start local Hot masternode configured in curium.conf\n"

                "  start-alias  - Start single remote masternode by assigned alias configured in masternode.conf\n"

                "  start-<mode> - Start remote masternodes configured in masternode.conf (<mode>: 'all', 'missing', 'disabled')\n"

                "  status       - Print masternode status information\n"

>>>>>>> dev-1.12.1.0
                "  list         - Print list of all known masternodes (see masternodelist for more info)\n"

                "  list-conf    - Print masternode.conf in JSON format\n"

                "  winner       - Print info on next masternode winner to vote for\n"

                "  winners      - Print list of masternode winners\n"

                );



    if (strCommand == "list")

    {

<<<<<<< HEAD
    if(strCommand == "connect")
    {
        std::string strAddress = "";
        if (params.size() == 2){
            strAddress = params[1].get_str();
        } else {
            throw runtime_error("Masternode address required\n");
        }

        CService addr = CService(strAddress);

        CNode *pnode = ConnectNode((CAddress)addr, NULL, false);
        if(pnode){
            pnode->Release();
            return "successfully connected";
        } else {
            throw runtime_error("error connecting\n");
        }
    }

    if (strCommand == "count")
    {
        if (params.size() > 2){
            throw runtime_error("too many parameters\n");
        }
        if (params.size() == 2)
        {
            int nCount = 0;

            if(chainActive.Tip())
                mnodeman.GetNextMasternodeInQueueForPayment(chainActive.Tip()->nHeight, true, nCount);

            if(params[1] == "ds") return mnodeman.CountEnabled(MIN_POOL_PEER_PROTO_VERSION);
            if(params[1] == "enabled") return mnodeman.CountEnabled();
            if(params[1] == "qualify") return nCount;
            if(params[1] == "all") return strprintf("Total: %d (DS Compatible: %d / Enabled: %d / Qualify: %d)",
                                                    mnodeman.size(),
                                                    mnodeman.CountEnabled(MIN_POOL_PEER_PROTO_VERSION),
                                                    mnodeman.CountEnabled(),
                                                    nCount);
=======
        UniValue newParams(UniValue::VARR);

        // forward params but skip "list"

        for (unsigned int i = 1; i < params.size(); i++) {

            newParams.push_back(params[i]);

>>>>>>> dev-1.12.1.0
        }

        return masternodelist(newParams, fHelp);

    }

<<<<<<< HEAD
    if (strCommand == "current")
    {
        CMasternode* winner = mnodeman.GetCurrentMasterNode(1);
        if(winner) {
            Object obj;

            obj.push_back(Pair("IP:port",       winner->addr.ToString()));
            obj.push_back(Pair("protocol",      (int64_t)winner->protocolVersion));
            obj.push_back(Pair("vin",           winner->vin.prevout.hash.ToString()));
            obj.push_back(Pair("pubkey",        CBitcoinAddress(winner->pubkey.GetID()).ToString()));
            obj.push_back(Pair("lastseen",      (winner->lastPing == CMasternodePing()) ? winner->sigTime :
                                                        (int64_t)winner->lastPing.sigTime));
            obj.push_back(Pair("activeseconds", (winner->lastPing == CMasternodePing()) ? 0 :
                                                        (int64_t)(winner->lastPing.sigTime - winner->sigTime)));
            return obj;
        }

        return "unknown";
    }

    if (strCommand == "debug")
    {
        if(activeMasternode.status != ACTIVE_MASTERNODE_INITIAL || !masternodeSync.IsSynced())
            return activeMasternode.GetStatus();

        CTxIn vin = CTxIn();
        CPubKey pubkey = CScript();
        CKey key;
        bool found = activeMasternode.GetMasterNodeVin(vin, pubkey, key);
        if(!found){
            throw runtime_error("Missing masternode input, please look at the documentation for instructions on masternode creation\n");
        } else {
            return activeMasternode.GetStatus();
        }
    }

    if(strCommand == "enforce")
    {
        return (uint64_t)enforceMasternodePaymentsTime;
    }

    if (strCommand == "start")
    {
        if(!fMasterNode) throw runtime_error("you must set masternode=1 in the configuration\n");
=======


    if(strCommand == "connect")

    {
>>>>>>> dev-1.12.1.0

        if (params.size() < 2)

<<<<<<< HEAD
            if (params.size() == 2){
                strWalletPass = params[1].get_str().c_str();
            } else {
                throw runtime_error("Your wallet is locked, passphrase is required\n");
            }

            if(!pwalletMain->Unlock(strWalletPass)){
                throw runtime_error("incorrect passphrase\n");
            }
        }

        if(activeMasternode.status != ACTIVE_MASTERNODE_STARTED){
            activeMasternode.status = ACTIVE_MASTERNODE_INITIAL; // TODO: consider better way
            activeMasternode.ManageStatus();
            pwalletMain->Lock();
        }

        return activeMasternode.GetStatus();
    }

    if (strCommand == "start-alias")
    {
        if (params.size() < 2){
            throw runtime_error("command needs at least 2 parameters\n");
        }

        std::string alias = params[1].get_str();

        if(pwalletMain->IsLocked()) {
            SecureString strWalletPass;
            strWalletPass.reserve(100);

            if (params.size() == 3){
                strWalletPass = params[2].get_str().c_str();
            } else {
                throw runtime_error("Your wallet is locked, passphrase is required\n");
            }

            if(!pwalletMain->Unlock(strWalletPass)){
                throw runtime_error("incorrect passphrase\n");
            }
        }

        bool found = false;

        Object statusObj;
        statusObj.push_back(Pair("alias", alias));

        BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
            if(mne.getAlias() == alias) {
                found = true;
                std::string errorMessage;
                CMasternodeBroadcast mnb;

                bool result = activeMasternode.CreateBroadcast(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), errorMessage, mnb);

                statusObj.push_back(Pair("result", result ? "successful" : "failed"));
                if(result) {
                    mnodeman.UpdateMasternodeList(mnb);
                    mnb.Relay();
                } else {
                    statusObj.push_back(Pair("errorMessage", errorMessage));
                }
                break;
            }
        }

        if(!found) {
            statusObj.push_back(Pair("result", "failed"));
            statusObj.push_back(Pair("errorMessage", "could not find alias in config. Verify with list-conf."));
        }

        pwalletMain->Lock();
        return statusObj;

    }

    if (strCommand == "start-many" || strCommand == "start-all" || strCommand == "start-missing" || strCommand == "start-disabled")
    {
        if(pwalletMain->IsLocked()) {
            SecureString strWalletPass;
            strWalletPass.reserve(100);

            if (params.size() == 2){
                strWalletPass = params[1].get_str().c_str();
            } else {
                throw runtime_error("Your wallet is locked, passphrase is required\n");
            }

            if(!pwalletMain->Unlock(strWalletPass)){
                throw runtime_error("incorrect passphrase\n");
            }
        }

        if((strCommand == "start-missing" || strCommand == "start-disabled") &&
         (masternodeSync.RequestedMasternodeAssets <= MASTERNODE_SYNC_LIST ||
          masternodeSync.RequestedMasternodeAssets == MASTERNODE_SYNC_FAILED)) {
            throw runtime_error("You can't use this command until masternode list is synced\n");
        }

        std::vector<CMasternodeConfig::CMasternodeEntry> mnEntries;
        mnEntries = masternodeConfig.getEntries();

        int successful = 0;
        int failed = 0;

        Object resultsObj;

        BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
            std::string errorMessage;

            CTxIn vin = CTxIn(uint256(mne.getTxHash()), uint32_t(atoi(mne.getOutputIndex().c_str())));
            CMasternode *pmn = mnodeman.Find(vin);
            CMasternodeBroadcast mnb;

            if(strCommand == "start-missing" && pmn) continue;
            if(strCommand == "start-disabled" && pmn && pmn->IsEnabled()) continue;

            bool result = activeMasternode.CreateBroadcast(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), errorMessage, mnb);

            Object statusObj;
            statusObj.push_back(Pair("alias", mne.getAlias()));
            statusObj.push_back(Pair("result", result ? "successful" : "failed"));

            if(result) {
                successful++;
                mnodeman.UpdateMasternodeList(mnb);
                mnb.Relay();
            } else {
                failed++;
                statusObj.push_back(Pair("errorMessage", errorMessage));
            }

            resultsObj.push_back(Pair("status", statusObj));
        }
        pwalletMain->Lock();

        Object returnObj;
        returnObj.push_back(Pair("overall", strprintf("Successfully started %d masternodes, failed to start %d, total %d", successful, failed, successful + failed)));
        returnObj.push_back(Pair("detail", resultsObj));

        return returnObj;
    }

    if (strCommand == "create")
    {

        throw runtime_error("Not implemented yet, please look at the documentation for instructions on masternode creation\n");
    }
=======
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Masternode address required");



        std::string strAddress = params[1].get_str();



        CService addr = CService(strAddress);



        CNode *pnode = ConnectNode((CAddress)addr, NULL);

        if(!pnode)

            throw JSONRPCError(RPC_INTERNAL_ERROR, strprintf("Couldn't connect to masternode %s", strAddress));



        return "successfully connected";

    }



    if (strCommand == "count")

    {

        if (params.size() > 2)

            throw JSONRPCError(RPC_INVALID_PARAMETER, "Too many parameters");



        if (params.size() == 1)

            return mnodeman.size();



        std::string strMode = params[1].get_str();



        if (strMode == "ps")

            return mnodeman.CountEnabled(MIN_PRIVATESEND_PEER_PROTO_VERSION);



        if (strMode == "enabled")

            return mnodeman.CountEnabled();


>>>>>>> dev-1.12.1.0

        int nCount;

        mnodeman.GetNextMasternodeInQueueForPayment(true, nCount);



        if (strMode == "qualify")

            return nCount;



        if (strMode == "all")

            return strprintf("Total: %d (PS Compatible: %d / Enabled: %d / Qualify: %d)",

                mnodeman.size(), mnodeman.CountEnabled(MIN_PRIVATESEND_PEER_PROTO_VERSION),

                mnodeman.CountEnabled(), nCount);

    }

<<<<<<< HEAD
    if(strCommand == "list-conf")
    {
        std::vector<CMasternodeConfig::CMasternodeEntry> mnEntries;
        mnEntries = masternodeConfig.getEntries();
=======


    if (strCommand == "current" || strCommand == "winner")

    {

        int nCount;

        int nHeight;

        CMasternode* winner = NULL;

        {

            LOCK(cs_main);

            nHeight = chainActive.Height() + (strCommand == "current" ? 1 : 10);

        }

        mnodeman.UpdateLastPaid();

        winner = mnodeman.GetNextMasternodeInQueueForPayment(nHeight, true, nCount);

        if(!winner) return "unknown";
>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
        BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
            CTxIn vin = CTxIn(uint256(mne.getTxHash()), uint32_t(atoi(mne.getOutputIndex().c_str())));
            CMasternode *pmn = mnodeman.Find(vin);

            std::string strStatus = pmn ? pmn->Status() : "MISSING";

            Object mnObj;
            mnObj.push_back(Pair("alias", mne.getAlias()));
            mnObj.push_back(Pair("address", mne.getIp()));
            mnObj.push_back(Pair("privateKey", mne.getPrivKey()));
            mnObj.push_back(Pair("txHash", mne.getTxHash()));
            mnObj.push_back(Pair("outputIndex", mne.getOutputIndex()));
            mnObj.push_back(Pair("status", strStatus));
            resultObj.push_back(Pair("masternode", mnObj));
        }
=======
>>>>>>> dev-1.12.1.0

        UniValue obj(UniValue::VOBJ);


<<<<<<< HEAD
        Object obj;
        BOOST_FOREACH(COutput& out, possibleCoins) {
            obj.push_back(Pair(out.tx->GetHash().ToString(), strprintf("%d", out.i)));
        }
=======

        obj.push_back(Pair("height",        nHeight));

        obj.push_back(Pair("IP:port",       winner->addr.ToString()));

        obj.push_back(Pair("protocol",      (int64_t)winner->nProtocolVersion));

        obj.push_back(Pair("vin",           winner->vin.prevout.ToStringShort()));

        obj.push_back(Pair("payee",         CBitcoinAddress(winner->pubKeyCollateralAddress.GetID()).ToString()));

        obj.push_back(Pair("lastseen",      (winner->lastPing == CMasternodePing()) ? winner->sigTime :

                                                    winner->lastPing.sigTime));

        obj.push_back(Pair("activeseconds", (winner->lastPing == CMasternodePing()) ? 0 :

                                                    (winner->lastPing.sigTime - winner->sigTime)));
>>>>>>> dev-1.12.1.0

        return obj;

    }

<<<<<<< HEAD
    if(strCommand == "status")
    {
        if(!fMasterNode) throw runtime_error("This is not a masternode\n");

        Object mnObj;
        CMasternode *pmn = mnodeman.Find(activeMasternode.vin);

        mnObj.push_back(Pair("vin", activeMasternode.vin.ToString()));
        mnObj.push_back(Pair("service", activeMasternode.service.ToString()));
        if (pmn) mnObj.push_back(Pair("pubkey", CBitcoinAddress(pmn->pubkey.GetID()).ToString()));
        mnObj.push_back(Pair("status", activeMasternode.GetStatus()));
        return mnObj;
    }

    if (strCommand == "winners")
    {
        int nLast = 10;

        if (params.size() >= 2){
            nLast = atoi(params[1].get_str());
        }

        Object obj;

        for(int nHeight = chainActive.Tip()->nHeight-nLast; nHeight < chainActive.Tip()->nHeight+20; nHeight++)
        {
            obj.push_back(Pair(strprintf("%d", nHeight), GetRequiredPaymentsString(nHeight)));
        }

        return obj;
    }

    /*
        Shows which masternode wins by score each block
    */
    if (strCommand == "calcscore")
    {

        int nLast = 10;

        if (params.size() >= 2){
            nLast = atoi(params[1].get_str());
        }
        Object obj;

        std::vector<CMasternode> vMasternodes = mnodeman.GetFullMasternodeVector();
        for(int nHeight = chainActive.Tip()->nHeight-nLast; nHeight < chainActive.Tip()->nHeight+20; nHeight++){
            uint256 nHigh = 0;
            CMasternode *pBestMasternode = NULL;
            BOOST_FOREACH(CMasternode& mn, vMasternodes) {
                uint256 n = mn.CalculateScore(1, nHeight-100);
                if(n > nHigh){
                    nHigh = n;
                    pBestMasternode = &mn;
                }
            }
            if(pBestMasternode)
                obj.push_back(Pair(strprintf("%d", nHeight), pBestMasternode->vin.prevout.ToStringShort().c_str()));
        }

        return obj;
    }

    return Value::null;
}
=======
>>>>>>> dev-1.12.1.0


    if (strCommand == "debug")

<<<<<<< HEAD
    if (fHelp ||
            (strMode != "status" && strMode != "vin" && strMode != "pubkey" && strMode != "lastseen" && strMode != "activeseconds" && strMode != "rank" && strMode != "addr"
                && strMode != "protocol" && strMode != "full" && strMode != "lastpaid"))
=======
>>>>>>> dev-1.12.1.0
    {

        if(activeMasternode.nState != ACTIVE_MASTERNODE_INITIAL || !masternodeSync.IsBlockchainSynced())

            return activeMasternode.GetStatus();



        CTxIn vin;

        CPubKey pubkey;

        CKey key;



        if(!pwalletMain || !pwalletMain->GetMasternodeVinAndKeys(vin, pubkey, key))

            throw JSONRPCError(RPC_INVALID_PARAMETER, "Missing masternode input, please look at the documentation for instructions on masternode creation");



        return activeMasternode.GetStatus();

    }



    if (strCommand == "start")

    {

        if(!fMasterNode)

            throw JSONRPCError(RPC_INTERNAL_ERROR, "You must set masternode=1 in the configuration");



        {

            LOCK(pwalletMain->cs_wallet);

            EnsureWalletIsUnlocked();

        }



        if(activeMasternode.nState != ACTIVE_MASTERNODE_STARTED){

            activeMasternode.nState = ACTIVE_MASTERNODE_INITIAL; // TODO: consider better way

            activeMasternode.ManageState();

        }



        return activeMasternode.GetStatus();

    }



    if (strCommand == "start-alias")

    {

        if (params.size() < 2)

            throw JSONRPCError(RPC_INVALID_PARAMETER, "Please specify an alias");



        {

            LOCK(pwalletMain->cs_wallet);

            EnsureWalletIsUnlocked();

        }



        std::string strAlias = params[1].get_str();



        bool fFound = false;



        UniValue statusObj(UniValue::VOBJ);

        statusObj.push_back(Pair("alias", strAlias));



        BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {

            if(mne.getAlias() == strAlias) {

                fFound = true;

                std::string strError;

                CMasternodeBroadcast mnb;



                bool fResult = CMasternodeBroadcast::Create(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), strError, mnb);



                statusObj.push_back(Pair("result", fResult ? "successful" : "failed"));

                if(fResult) {

                    mnodeman.UpdateMasternodeList(mnb);

                    mnb.Relay();

                } else {

                    statusObj.push_back(Pair("errorMessage", strError));

                }

                mnodeman.NotifyMasternodeUpdates();

                break;

            }

        }



        if(!fFound) {

            statusObj.push_back(Pair("result", "failed"));

            statusObj.push_back(Pair("errorMessage", "Could not find alias in config. Verify with list-conf."));

        }



        return statusObj;



    }



    if (strCommand == "start-all" || strCommand == "start-missing" || strCommand == "start-disabled")

    {

        {

            LOCK(pwalletMain->cs_wallet);

            EnsureWalletIsUnlocked();

        }



        if((strCommand == "start-missing" || strCommand == "start-disabled") && !masternodeSync.IsMasternodeListSynced()) {

            throw JSONRPCError(RPC_CLIENT_IN_INITIAL_DOWNLOAD, "You can't use this command until masternode list is synced");

        }



        int nSuccessful = 0;

        int nFailed = 0;



        UniValue resultsObj(UniValue::VOBJ);



        BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {

            std::string strError;



            CTxIn vin = CTxIn(uint256S(mne.getTxHash()), uint32_t(atoi(mne.getOutputIndex().c_str())));

            CMasternode *pmn = mnodeman.Find(vin);

            CMasternodeBroadcast mnb;



            if(strCommand == "start-missing" && pmn) continue;

            if(strCommand == "start-disabled" && pmn && pmn->IsEnabled()) continue;



            bool fResult = CMasternodeBroadcast::Create(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), strError, mnb);



            UniValue statusObj(UniValue::VOBJ);

            statusObj.push_back(Pair("alias", mne.getAlias()));

            statusObj.push_back(Pair("result", fResult ? "successful" : "failed"));



            if (fResult) {

                nSuccessful++;

                mnodeman.UpdateMasternodeList(mnb);

                mnb.Relay();

            } else {

                nFailed++;

                statusObj.push_back(Pair("errorMessage", strError));

            }



            resultsObj.push_back(Pair("status", statusObj));

        }

        mnodeman.NotifyMasternodeUpdates();



        UniValue returnObj(UniValue::VOBJ);

        returnObj.push_back(Pair("overall", strprintf("Successfully started %d masternodes, failed to start %d, total %d", nSuccessful, nFailed, nSuccessful + nFailed)));

        returnObj.push_back(Pair("detail", resultsObj));



        return returnObj;

    }



    if (strCommand == "genkey")

    {

        CKey secret;

        secret.MakeNewKey(false);



        return CBitcoinSecret(secret).ToString();

    }



    if (strCommand == "list-conf")

    {

        UniValue resultObj(UniValue::VOBJ);



        BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {

            CTxIn vin = CTxIn(uint256S(mne.getTxHash()), uint32_t(atoi(mne.getOutputIndex().c_str())));

            CMasternode *pmn = mnodeman.Find(vin);



            std::string strStatus = pmn ? pmn->GetStatus() : "MISSING";



            UniValue mnObj(UniValue::VOBJ);

            mnObj.push_back(Pair("alias", mne.getAlias()));

            mnObj.push_back(Pair("address", mne.getIp()));

            mnObj.push_back(Pair("privateKey", mne.getPrivKey()));

            mnObj.push_back(Pair("txHash", mne.getTxHash()));

            mnObj.push_back(Pair("outputIndex", mne.getOutputIndex()));

            mnObj.push_back(Pair("status", strStatus));

            resultObj.push_back(Pair("masternode", mnObj));

        }



        return resultObj;

    }



    if (strCommand == "outputs") {

        // Find possible candidates

        std::vector<COutput> vPossibleCoins;

        pwalletMain->AvailableCoins(vPossibleCoins, true, NULL, false, ONLY_10000);



        UniValue obj(UniValue::VOBJ);

        BOOST_FOREACH(COutput& out, vPossibleCoins) {

            obj.push_back(Pair(out.tx->GetHash().ToString(), strprintf("%d", out.i)));

        }



        return obj;



    }



    if (strCommand == "status")

    {

        if (!fMasterNode)

            throw JSONRPCError(RPC_INTERNAL_ERROR, "This is not a masternode");



        UniValue mnObj(UniValue::VOBJ);



        mnObj.push_back(Pair("vin", activeMasternode.vin.ToString()));

        mnObj.push_back(Pair("service", activeMasternode.service.ToString()));



        CMasternode mn;

        if(mnodeman.Get(activeMasternode.vin, mn)) {

            mnObj.push_back(Pair("payee", CBitcoinAddress(mn.pubKeyCollateralAddress.GetID()).ToString()));

        }



        mnObj.push_back(Pair("status", activeMasternode.GetStatus()));

        return mnObj;

    }



    if (strCommand == "winners")

    {

        int nHeight;

        {

            LOCK(cs_main);

            CBlockIndex* pindex = chainActive.Tip();

            if(!pindex) return NullUniValue;



            nHeight = pindex->nHeight;

        }



        int nLast = 10;

        std::string strFilter = "";



        if (params.size() >= 2) {

            nLast = atoi(params[1].get_str());

        }



        if (params.size() == 3) {

            strFilter = params[2].get_str();

        }



        if (params.size() > 3)

            throw JSONRPCError(RPC_INVALID_PARAMETER, "Correct usage is 'masternode winners ( \"count\" \"filter\" )'");



        UniValue obj(UniValue::VOBJ);



        for(int i = nHeight - nLast; i < nHeight + 20; i++) {

            std::string strPayment = GetRequiredPaymentsString(i);

            if (strFilter !="" && strPayment.find(strFilter) == std::string::npos) continue;

            obj.push_back(Pair(strprintf("%d", i), strPayment));

        }



        return obj;

    }



    return NullUniValue;

}



UniValue masternodelist(const UniValue& params, bool fHelp)

{

    std::string strMode = "status";

    std::string strFilter = "";



    if (params.size() >= 1) strMode = params[0].get_str();

    if (params.size() == 2) strFilter = params[1].get_str();



    if (fHelp || (

                strMode != "activeseconds" && strMode != "addr" && strMode != "full" &&

                strMode != "lastseen" && strMode != "lastpaidtime" && strMode != "lastpaidblock" &&

                strMode != "protocol" && strMode != "payee" && strMode != "rank" && strMode != "status"))

    {

        throw std::runtime_error(

                "masternodelist ( \"mode\" \"filter\" )\n"

                "Get a list of masternodes in different modes\n"

                "\nArguments:\n"

                "1. \"mode\"      (string, optional/required to use filter, defaults = status) The mode to run list in\n"

                "2. \"filter\"    (string, optional) Filter results. Partial match by outpoint by default in all modes,\n"

                "                                    additional matches in some modes are also available\n"

                "\nAvailable modes:\n"

                "  activeseconds  - Print number of seconds masternode recognized by the network as enabled\n"
<<<<<<< HEAD
                "                   (since latest issued \"masternode start/start-many/start-alias\")\n"
                "  addr           - Print ip address associated with a masternode (can be additionally filtered, partial match)\n"
                "  full           - Print info in format 'status protocol pubkey IP lastseen activeseconds lastpaid'\n"
=======

                "                   (since latest issued \"masternode start/start-many/start-alias\")\n"

                "  addr           - Print ip address associated with a masternode (can be additionally filtered, partial match)\n"

                "  full           - Print info in format 'status protocol payee lastseen activeseconds lastpaidtime lastpaidblock IP'\n"

>>>>>>> dev-1.12.1.0
                "                   (can be additionally filtered, partial match)\n"

                "  lastpaidblock  - Print the last block height a node was paid on the network\n"

                "  lastpaidtime   - Print the last time a node was paid on the network\n"

                "  lastseen       - Print timestamp of when a masternode was last seen on the network\n"
<<<<<<< HEAD
                "  lastpaid       - The last time a node was paid on the network\n"
                "  protocol       - Print protocol of a masternode (can be additionally filtered, exact match))\n"
                "  pubkey         - Print public key associated with a masternode (can be additionally filtered,\n"
=======

                "  payee          - Print Curium address associated with a masternode (can be additionally filtered,\n"

>>>>>>> dev-1.12.1.0
                "                   partial match)\n"

                "  protocol       - Print protocol of a masternode (can be additionally filtered, exact match))\n"

                "  rank           - Print rank of a masternode based on current block\n"
<<<<<<< HEAD
                "  status         - Print masternode status: ENABLED / EXPIRED / VIN_SPENT / REMOVE / POS_ERROR\n"
                "                   (can be additionally filtered, partial match)\n"
=======

                "  status         - Print masternode status: PRE_ENABLED / ENABLED / EXPIRED / WATCHDOG_EXPIRED / NEW_START_REQUIRED /\n"

                "                   UPDATE_REQUIRED / POSE_BAN / OUTPOINT_SPENT (can be additionally filtered, partial match)\n"

>>>>>>> dev-1.12.1.0
                );

    }



    if (strMode == "full" || strMode == "lastpaidtime" || strMode == "lastpaidblock") {

        mnodeman.UpdateLastPaid();

    }



    UniValue obj(UniValue::VOBJ);

    if (strMode == "rank") {

        std::vector<std::pair<int, CMasternode> > vMasternodeRanks = mnodeman.GetMasternodeRanks();

        BOOST_FOREACH(PAIRTYPE(int, CMasternode)& s, vMasternodeRanks) {

            std::string strOutpoint = s.second.vin.prevout.ToStringShort();

            if (strFilter !="" && strOutpoint.find(strFilter) == std::string::npos) continue;

            obj.push_back(Pair(strOutpoint, s.first));

        }

    } else {

        std::vector<CMasternode> vMasternodes = mnodeman.GetFullMasternodeVector();

        BOOST_FOREACH(CMasternode& mn, vMasternodes) {

            std::string strOutpoint = mn.vin.prevout.ToStringShort();

            if (strMode == "activeseconds") {

                if (strFilter !="" && strOutpoint.find(strFilter) == std::string::npos) continue;

                obj.push_back(Pair(strOutpoint, (int64_t)(mn.lastPing.sigTime - mn.sigTime)));

            } else if (strMode == "addr") {

                std::string strAddress = mn.addr.ToString();

                if (strFilter !="" && strAddress.find(strFilter) == std::string::npos &&

                    strOutpoint.find(strFilter) == std::string::npos) continue;

                obj.push_back(Pair(strOutpoint, strAddress));

            } else if (strMode == "full") {

                std::ostringstream streamFull;

                streamFull << std::setw(18) <<

                               mn.GetStatus() << " " <<

                               mn.nProtocolVersion << " " <<

                               CBitcoinAddress(mn.pubKeyCollateralAddress.GetID()).ToString() << " " <<

                               (int64_t)mn.lastPing.sigTime << " " << std::setw(8) <<

                               (int64_t)(mn.lastPing.sigTime - mn.sigTime) << " " << std::setw(10) <<

                               mn.GetLastPaidTime() << " "  << std::setw(6) <<

                               mn.GetLastPaidBlock() << " " <<

                               mn.addr.ToString();

                std::string strFull = streamFull.str();

                if (strFilter !="" && strFull.find(strFilter) == std::string::npos &&

                    strOutpoint.find(strFilter) == std::string::npos) continue;

                obj.push_back(Pair(strOutpoint, strFull));

            } else if (strMode == "lastpaidblock") {

                if (strFilter !="" && strOutpoint.find(strFilter) == std::string::npos) continue;

                obj.push_back(Pair(strOutpoint, mn.GetLastPaidBlock()));

            } else if (strMode == "lastpaidtime") {

                if (strFilter !="" && strOutpoint.find(strFilter) == std::string::npos) continue;

                obj.push_back(Pair(strOutpoint, mn.GetLastPaidTime()));

            } else if (strMode == "lastseen") {

                if (strFilter !="" && strOutpoint.find(strFilter) == std::string::npos) continue;

                obj.push_back(Pair(strOutpoint, (int64_t)mn.lastPing.sigTime));

            } else if (strMode == "payee") {

                CBitcoinAddress address(mn.pubKeyCollateralAddress.GetID());

                std::string strPayee = address.ToString();

                if (strFilter !="" && strPayee.find(strFilter) == std::string::npos &&

                    strOutpoint.find(strFilter) == std::string::npos) continue;

                obj.push_back(Pair(strOutpoint, strPayee));

            } else if (strMode == "protocol") {

                if (strFilter !="" && strFilter != strprintf("%d", mn.nProtocolVersion) &&

                    strOutpoint.find(strFilter) == std::string::npos) continue;

                obj.push_back(Pair(strOutpoint, (int64_t)mn.nProtocolVersion));

            } else if (strMode == "status") {

                std::string strStatus = mn.GetStatus();

                if (strFilter !="" && strStatus.find(strFilter) == std::string::npos &&

                    strOutpoint.find(strFilter) == std::string::npos) continue;

                obj.push_back(Pair(strOutpoint, strStatus));

            }

        }

    }

    return obj;

}



bool DecodeHexVecMnb(std::vector<CMasternodeBroadcast>& vecMnb, std::string strHexMnb) {



    if (!IsHex(strHexMnb))

        return false;



    std::vector<unsigned char> mnbData(ParseHex(strHexMnb));

    CDataStream ssData(mnbData, SER_NETWORK, PROTOCOL_VERSION);

    try {

        ssData >> vecMnb;

    }

    catch (const std::exception&) {

        return false;

    }



    return true;

}



UniValue masternodebroadcast(const UniValue& params, bool fHelp)

{

    std::string strCommand;

    if (params.size() >= 1)

        strCommand = params[0].get_str();



    if (fHelp  ||

        (strCommand != "create-alias" && strCommand != "create-all" && strCommand != "decode" && strCommand != "relay"))

        throw std::runtime_error(

                "masternodebroadcast \"command\"... ( \"passphrase\" )\n"

                "Set of commands to create and relay masternode broadcast messages\n"

                "\nArguments:\n"

                "1. \"command\"        (string or set of strings, required) The command to execute\n"

                "2. \"passphrase\"     (string, optional) The wallet passphrase\n"

                "\nAvailable commands:\n"

                "  create-alias  - Create single remote masternode broadcast message by assigned alias configured in masternode.conf\n"

                "  create-all    - Create remote masternode broadcast messages for all masternodes configured in masternode.conf\n"

                "  decode        - Decode masternode broadcast message\n"

                "  relay         - Relay masternode broadcast message to the network\n"

                + HelpRequiringPassphrase());



    if (strCommand == "create-alias")

    {

        // wait for reindex and/or import to finish

        if (fImporting || fReindex)

            throw JSONRPCError(RPC_INTERNAL_ERROR, "Wait for reindex and/or import to finish");



        if (params.size() < 2)

            throw JSONRPCError(RPC_INVALID_PARAMETER, "Please specify an alias");



        {

            LOCK(pwalletMain->cs_wallet);

            EnsureWalletIsUnlocked();

        }



        bool fFound = false;

        std::string strAlias = params[1].get_str();



        UniValue statusObj(UniValue::VOBJ);

        std::vector<CMasternodeBroadcast> vecMnb;



        statusObj.push_back(Pair("alias", strAlias));



        BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {

            if(mne.getAlias() == strAlias) {

                fFound = true;

                std::string strError;

                CMasternodeBroadcast mnb;



                bool fResult = CMasternodeBroadcast::Create(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), strError, mnb, true);



                statusObj.push_back(Pair("result", fResult ? "successful" : "failed"));

                if(fResult) {

                    vecMnb.push_back(mnb);

                    CDataStream ssVecMnb(SER_NETWORK, PROTOCOL_VERSION);

                    ssVecMnb << vecMnb;

                    statusObj.push_back(Pair("hex", HexStr(ssVecMnb.begin(), ssVecMnb.end())));

                } else {

                    statusObj.push_back(Pair("errorMessage", strError));

                }

                break;

            }

        }



        if(!fFound) {

            statusObj.push_back(Pair("result", "not found"));

            statusObj.push_back(Pair("errorMessage", "Could not find alias in config. Verify with list-conf."));

        }



        return statusObj;



    }



    if (strCommand == "create-all")

    {

        // wait for reindex and/or import to finish

        if (fImporting || fReindex)

            throw JSONRPCError(RPC_INTERNAL_ERROR, "Wait for reindex and/or import to finish");



        {

            LOCK(pwalletMain->cs_wallet);

            EnsureWalletIsUnlocked();

        }
<<<<<<< HEAD
    } else {
        std::vector<CMasternode> vMasternodes = mnodeman.GetFullMasternodeVector();
        BOOST_FOREACH(CMasternode& mn, vMasternodes) {
            std::string strVin = mn.vin.prevout.ToStringShort();
            if (strMode == "activeseconds") {
                if(strFilter !="" && strVin.find(strFilter) == string::npos) continue;
                obj.push_back(Pair(strVin,       (int64_t)(mn.lastPing.sigTime - mn.sigTime)));
            } else if (strMode == "addr") {
                if(strFilter !="" && mn.vin.prevout.hash.ToString().find(strFilter) == string::npos &&
                    strVin.find(strFilter) == string::npos) continue;
                obj.push_back(Pair(strVin,       mn.addr.ToString()));
            } else if (strMode == "full") {
                std::ostringstream addrStream;
                addrStream << setw(21) << strVin;

                std::ostringstream stringStream;
                stringStream << setw(9) <<
                               mn.Status() << " " <<
                               mn.protocolVersion << " " <<
                               CBitcoinAddress(mn.pubkey.GetID()).ToString() << " " << setw(21) <<
                               mn.addr.ToString() << " " <<
                               (int64_t)mn.lastPing.sigTime << " " << setw(8) <<
                               (int64_t)(mn.lastPing.sigTime - mn.sigTime) << " " <<
                               (int64_t)mn.GetLastPaid();
                std::string output = stringStream.str();
                stringStream << " " << strVin;
                if(strFilter !="" && stringStream.str().find(strFilter) == string::npos &&
                        strVin.find(strFilter) == string::npos) continue;
                obj.push_back(Pair(addrStream.str(), output));
            } else if (strMode == "lastseen") {
                if(strFilter !="" && strVin.find(strFilter) == string::npos) continue;
                obj.push_back(Pair(strVin,       (int64_t)mn.lastPing.sigTime));
            } else if (strMode == "lastpaid"){
                if(strFilter !="" && mn.vin.prevout.hash.ToString().find(strFilter) == string::npos &&
                    strVin.find(strFilter) == string::npos) continue;
                obj.push_back(Pair(strVin,      (int64_t)mn.GetLastPaid()));
            } else if (strMode == "protocol") {
                if(strFilter !="" && strFilter != strprintf("%d", mn.protocolVersion) &&
                    strVin.find(strFilter) == string::npos) continue;
                obj.push_back(Pair(strVin,       (int64_t)mn.protocolVersion));
            } else if (strMode == "pubkey") {
                CBitcoinAddress address(mn.pubkey.GetID());

                if(strFilter !="" && address.ToString().find(strFilter) == string::npos &&
                    strVin.find(strFilter) == string::npos) continue;
                obj.push_back(Pair(strVin,       address.ToString()));
            } else if(strMode == "status") {
                std::string strStatus = mn.Status();
                if(strFilter !="" && strVin.find(strFilter) == string::npos && strStatus.find(strFilter) == string::npos) continue;
                obj.push_back(Pair(strVin,       strStatus));
=======



        std::vector<CMasternodeConfig::CMasternodeEntry> mnEntries;

        mnEntries = masternodeConfig.getEntries();



        int nSuccessful = 0;

        int nFailed = 0;



        UniValue resultsObj(UniValue::VOBJ);

        std::vector<CMasternodeBroadcast> vecMnb;



        BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {

            std::string strError;

            CMasternodeBroadcast mnb;



            bool fResult = CMasternodeBroadcast::Create(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), strError, mnb, true);



            UniValue statusObj(UniValue::VOBJ);

            statusObj.push_back(Pair("alias", mne.getAlias()));

            statusObj.push_back(Pair("result", fResult ? "successful" : "failed"));



            if(fResult) {

                nSuccessful++;

                vecMnb.push_back(mnb);

            } else {

                nFailed++;

                statusObj.push_back(Pair("errorMessage", strError));

            }



            resultsObj.push_back(Pair("status", statusObj));

        }



        CDataStream ssVecMnb(SER_NETWORK, PROTOCOL_VERSION);

        ssVecMnb << vecMnb;

        UniValue returnObj(UniValue::VOBJ);

        returnObj.push_back(Pair("overall", strprintf("Successfully created broadcast messages for %d masternodes, failed to create %d, total %d", nSuccessful, nFailed, nSuccessful + nFailed)));

        returnObj.push_back(Pair("detail", resultsObj));

        returnObj.push_back(Pair("hex", HexStr(ssVecMnb.begin(), ssVecMnb.end())));



        return returnObj;

    }



    if (strCommand == "decode")

    {

        if (params.size() != 2)

            throw JSONRPCError(RPC_INVALID_PARAMETER, "Correct usage is 'masternodebroadcast decode \"hexstring\"'");



        std::vector<CMasternodeBroadcast> vecMnb;



        if (!DecodeHexVecMnb(vecMnb, params[1].get_str()))

            throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "Masternode broadcast message decode failed");



        int nSuccessful = 0;

        int nFailed = 0;

        int nDos = 0;

        UniValue returnObj(UniValue::VOBJ);



        BOOST_FOREACH(CMasternodeBroadcast& mnb, vecMnb) {

            UniValue resultObj(UniValue::VOBJ);



            if(mnb.CheckSignature(nDos)) {

                nSuccessful++;

                resultObj.push_back(Pair("vin", mnb.vin.ToString()));

                resultObj.push_back(Pair("addr", mnb.addr.ToString()));

                resultObj.push_back(Pair("pubKeyCollateralAddress", CBitcoinAddress(mnb.pubKeyCollateralAddress.GetID()).ToString()));

                resultObj.push_back(Pair("pubKeyMasternode", CBitcoinAddress(mnb.pubKeyMasternode.GetID()).ToString()));

                resultObj.push_back(Pair("vchSig", EncodeBase64(&mnb.vchSig[0], mnb.vchSig.size())));

                resultObj.push_back(Pair("sigTime", mnb.sigTime));

                resultObj.push_back(Pair("protocolVersion", mnb.nProtocolVersion));

                resultObj.push_back(Pair("nLastDsq", mnb.nLastDsq));



                UniValue lastPingObj(UniValue::VOBJ);

                lastPingObj.push_back(Pair("vin", mnb.lastPing.vin.ToString()));

                lastPingObj.push_back(Pair("blockHash", mnb.lastPing.blockHash.ToString()));

                lastPingObj.push_back(Pair("sigTime", mnb.lastPing.sigTime));

                lastPingObj.push_back(Pair("vchSig", EncodeBase64(&mnb.lastPing.vchSig[0], mnb.lastPing.vchSig.size())));



                resultObj.push_back(Pair("lastPing", lastPingObj));

            } else {

                nFailed++;

                resultObj.push_back(Pair("errorMessage", "Masternode broadcast signature verification failed"));

            }



            returnObj.push_back(Pair(mnb.GetHash().ToString(), resultObj));

        }



        returnObj.push_back(Pair("overall", strprintf("Successfully decoded broadcast messages for %d masternodes, failed to decode %d, total %d", nSuccessful, nFailed, nSuccessful + nFailed)));



        return returnObj;

    }



    if (strCommand == "relay")

    {

        if (params.size() < 2 || params.size() > 3)

            throw JSONRPCError(RPC_INVALID_PARAMETER,   "masternodebroadcast relay \"hexstring\" ( fast )\n"

                                                        "\nArguments:\n"

                                                        "1. \"hex\"      (string, required) Broadcast messages hex string\n"

                                                        "2. fast       (string, optional) If none, using safe method\n");



        std::vector<CMasternodeBroadcast> vecMnb;



        if (!DecodeHexVecMnb(vecMnb, params[1].get_str()))

            throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "Masternode broadcast message decode failed");



        int nSuccessful = 0;

        int nFailed = 0;

        bool fSafe = params.size() == 2;

        UniValue returnObj(UniValue::VOBJ);



        // verify all signatures first, bailout if any of them broken

        BOOST_FOREACH(CMasternodeBroadcast& mnb, vecMnb) {

            UniValue resultObj(UniValue::VOBJ);



            resultObj.push_back(Pair("vin", mnb.vin.ToString()));

            resultObj.push_back(Pair("addr", mnb.addr.ToString()));



            int nDos = 0;

            bool fResult;

            if (mnb.CheckSignature(nDos)) {

                if (fSafe) {

                    fResult = mnodeman.CheckMnbAndUpdateMasternodeList(NULL, mnb, nDos);

                } else {

                    mnodeman.UpdateMasternodeList(mnb);

                    mnb.Relay();

                    fResult = true;

                }

                mnodeman.NotifyMasternodeUpdates();

            } else fResult = false;



            if(fResult) {

                nSuccessful++;

                resultObj.push_back(Pair(mnb.GetHash().ToString(), "successful"));

            } else {

                nFailed++;

                resultObj.push_back(Pair("errorMessage", "Masternode broadcast signature verification failed"));

>>>>>>> dev-1.12.1.0
            }



            returnObj.push_back(Pair(mnb.GetHash().ToString(), resultObj));

        }



        returnObj.push_back(Pair("overall", strprintf("Successfully relayed broadcast messages for %d masternodes, failed to relay %d, total %d", nSuccessful, nFailed, nSuccessful + nFailed)));



        return returnObj;

    }

<<<<<<< HEAD
}

bool DecodeHexVecMnb(std::vector<CMasternodeBroadcast>& vecMnb, std::string strHexMnb) {

    if (!IsHex(strHexMnb))
        return false;

    vector<unsigned char> mnbData(ParseHex(strHexMnb));
    CDataStream ssData(mnbData, SER_NETWORK, PROTOCOL_VERSION);
    try {
        ssData >> vecMnb;
    }
    catch (const std::exception&) {
        return false;
    }

    return true;
}

Value masternodebroadcast(const Array& params, bool fHelp)
{
    string strCommand;
    if (params.size() >= 1)
        strCommand = params[0].get_str();

    if (fHelp  ||
        (strCommand != "create-alias" && strCommand != "create-all" && strCommand != "decode" && strCommand != "relay"))
        throw runtime_error(
                "masternodebroadcast \"command\"... ( \"passphrase\" )\n"
                "Set of commands to create and relay masternode broadcast messages\n"
                "\nArguments:\n"
                "1. \"command\"        (string or set of strings, required) The command to execute\n"
                "2. \"passphrase\"     (string, optional) The wallet passphrase\n"
                "\nAvailable commands:\n"
                "  create-alias  - Create single remote masternode broadcast message by assigned alias configured in masternode.conf\n"
                "  create-all    - Create remote masternode broadcast messages for all masternodes configured in masternode.conf\n"
                "  decode        - Decode masternode broadcast message\n"
                "  relay         - Relay masternode broadcast message to the network\n"
                + HelpRequiringPassphrase());

    if (strCommand == "create-alias")
    {
        // wait for reindex and/or import to finish
        if (fImporting || fReindex)
            throw JSONRPCError(RPC_INTERNAL_ERROR, "Wait for reindex and/or import to finish");

        if (params.size() < 2)
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Command needs at least 2 parameters");

        std::string alias = params[1].get_str();

        if(pwalletMain->IsLocked()) {
            SecureString strWalletPass;
            strWalletPass.reserve(100);

            if (params.size() == 3){
                strWalletPass = params[2].get_str().c_str();
            } else {
                throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Your wallet is locked, passphrase is required");
            }
 
            if(!pwalletMain->Unlock(strWalletPass)){
                throw JSONRPCError(RPC_WALLET_PASSPHRASE_INCORRECT, "The wallet passphrase entered was incorrect");
            }
        }

        bool found = false;

        Object statusObj;
        std::vector<CMasternodeBroadcast> vecMnb;

        statusObj.push_back(Pair("alias", alias));

        BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
            if(mne.getAlias() == alias) {
                found = true;
                std::string errorMessage;
                CMasternodeBroadcast mnb;

                bool result = activeMasternode.CreateBroadcast(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), errorMessage, mnb, true);

                statusObj.push_back(Pair("result", result ? "successful" : "failed"));
                if(result) {
                    vecMnb.push_back(mnb);
                    CDataStream ssVecMnb(SER_NETWORK, PROTOCOL_VERSION);
                    ssVecMnb << vecMnb;
                    statusObj.push_back(Pair("hex", HexStr(ssVecMnb.begin(), ssVecMnb.end())));
                } else {
                    statusObj.push_back(Pair("errorMessage", errorMessage));
                }
                break;
            }
        }

        if(!found) {
            statusObj.push_back(Pair("result", "not found"));
            statusObj.push_back(Pair("errorMessage", "Could not find alias in config. Verify with list-conf."));
        }

        pwalletMain->Lock();
        return statusObj;

    }

    if (strCommand == "create-all")
    {
        // wait for reindex and/or import to finish
        if (fImporting || fReindex)
            throw JSONRPCError(RPC_INTERNAL_ERROR, "Wait for reindex and/or import to finish");

        if(pwalletMain->IsLocked()) {
            SecureString strWalletPass;
            strWalletPass.reserve(100);

            if (params.size() == 2){
                strWalletPass = params[1].get_str().c_str();
            } else {
                throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Your wallet is locked, passphrase is required");
            }

            if(!pwalletMain->Unlock(strWalletPass)){
                throw JSONRPCError(RPC_WALLET_PASSPHRASE_INCORRECT, "The wallet passphrase entered was incorrect");
            }
        }

        std::vector<CMasternodeConfig::CMasternodeEntry> mnEntries;
        mnEntries = masternodeConfig.getEntries();

        int successful = 0;
        int failed = 0;

        Object resultsObj;
        std::vector<CMasternodeBroadcast> vecMnb;

        BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
            std::string errorMessage;

            CTxIn vin = CTxIn(uint256(mne.getTxHash()), uint32_t(atoi(mne.getOutputIndex().c_str())));
            CMasternodeBroadcast mnb;

            bool result = activeMasternode.CreateBroadcast(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), errorMessage, mnb, true);

            Object statusObj;
            statusObj.push_back(Pair("alias", mne.getAlias()));
            statusObj.push_back(Pair("result", result ? "successful" : "failed"));

            if(result) {
                successful++;
                vecMnb.push_back(mnb);
            } else {
                failed++;
                statusObj.push_back(Pair("errorMessage", errorMessage));
            }

            resultsObj.push_back(Pair("status", statusObj));
        }
        pwalletMain->Lock();

        CDataStream ssVecMnb(SER_NETWORK, PROTOCOL_VERSION);
        ssVecMnb << vecMnb;
        Object returnObj;
        returnObj.push_back(Pair("overall", strprintf("Successfully created broadcast messages for %d masternodes, failed to create %d, total %d", successful, failed, successful + failed)));
        returnObj.push_back(Pair("detail", resultsObj));
        returnObj.push_back(Pair("hex", HexStr(ssVecMnb.begin(), ssVecMnb.end())));

        return returnObj;
    }

    if (strCommand == "decode")
    {
        if (params.size() != 2)
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Correct usage is 'masternodebroadcast decode \"hexstring\"'");

        int successful = 0;
        int failed = 0;

        std::vector<CMasternodeBroadcast> vecMnb;
        Object returnObj;

        if (!DecodeHexVecMnb(vecMnb, params[1].get_str()))
            throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "Masternode broadcast message decode failed");

        BOOST_FOREACH(CMasternodeBroadcast& mnb, vecMnb) {
            Object resultObj;

            if(mnb.VerifySignature()) {
                successful++;
                resultObj.push_back(Pair("vin", mnb.vin.ToString()));
                resultObj.push_back(Pair("addr", mnb.addr.ToString()));
                resultObj.push_back(Pair("pubkey", CBitcoinAddress(mnb.pubkey.GetID()).ToString()));
                resultObj.push_back(Pair("pubkey2", CBitcoinAddress(mnb.pubkey2.GetID()).ToString()));
                resultObj.push_back(Pair("vchSig", EncodeBase64(&mnb.sig[0], mnb.sig.size())));
                resultObj.push_back(Pair("sigTime", mnb.sigTime));
                resultObj.push_back(Pair("protocolVersion", mnb.protocolVersion));
                resultObj.push_back(Pair("nLastDsq", mnb.nLastDsq));

                Object lastPingObj;
                lastPingObj.push_back(Pair("vin", mnb.lastPing.vin.ToString()));
                lastPingObj.push_back(Pair("blockHash", mnb.lastPing.blockHash.ToString()));
                lastPingObj.push_back(Pair("sigTime", mnb.lastPing.sigTime));
                lastPingObj.push_back(Pair("vchSig", EncodeBase64(&mnb.lastPing.vchSig[0], mnb.lastPing.vchSig.size())));

                resultObj.push_back(Pair("lastPing", lastPingObj));
            } else {
                failed++;
                resultObj.push_back(Pair("errorMessage", "Masternode broadcast signature verification failed"));
            }

            returnObj.push_back(Pair(mnb.GetHash().ToString(), resultObj));
        }

        returnObj.push_back(Pair("overall", strprintf("Successfully decoded broadcast messages for %d masternodes, failed to decode %d, total %d", successful, failed, successful + failed)));

        return returnObj;
    }

    if (strCommand == "relay")
    {
        if (params.size() < 2 || params.size() > 3)
            throw JSONRPCError(RPC_INVALID_PARAMETER,   "masternodebroadcast relay \"hexstring\" ( fast )\n"
                                                        "\nArguments:\n"
                                                        "1. \"hex\"      (string, required) Broadcast messages hex string\n"
                                                        "2. fast       (string, optional) If none, using safe method\n");

        int successful = 0;
        int failed = 0;
        bool fSafe = params.size() == 2;

        std::vector<CMasternodeBroadcast> vecMnb;
        Object returnObj;

        if (!DecodeHexVecMnb(vecMnb, params[1].get_str()))
            throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "Masternode broadcast message decode failed");

        // verify all signatures first, bailout if any of them broken
        BOOST_FOREACH(CMasternodeBroadcast& mnb, vecMnb) {
            Object resultObj;

            resultObj.push_back(Pair("vin", mnb.vin.ToString()));
            resultObj.push_back(Pair("addr", mnb.addr.ToString()));

            int nDos = 0;
            bool fResult;
            if (mnb.VerifySignature()) {
                if (fSafe) {
                    fResult = mnodeman.CheckMnbAndUpdateMasternodeList(mnb, nDos);
                } else {
                    mnodeman.UpdateMasternodeList(mnb);
                    mnb.Relay();
                    fResult = true;
                }
            } else fResult = false;

            if(fResult) {
                successful++;
                mnodeman.UpdateMasternodeList(mnb);
                mnb.Relay();
                resultObj.push_back(Pair(mnb.GetHash().ToString(), "successful"));
            } else {
                failed++;
                resultObj.push_back(Pair("errorMessage", "Masternode broadcast signature verification failed"));
            }

            returnObj.push_back(Pair(mnb.GetHash().ToString(), resultObj));
        }

        returnObj.push_back(Pair("overall", strprintf("Successfully relayed broadcast messages for %d masternodes, failed to relay %d, total %d", successful, failed, successful + failed)));

        return returnObj;
    }

    return Value::null;
}
=======


    return NullUniValue;

}
>>>>>>> dev-1.12.1.0
