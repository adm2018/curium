// Copyright (c) 2014-2017 The Dash Core developers

// Distributed under the MIT/X11 software license, see the accompanying

// file COPYING or http://www.opensource.org/licenses/mit-license.php.



#include "activemasternode.h"

#include "coincontrol.h"

#include "consensus/validation.h"

#include "darksend.h"

#include "init.h"

#include "instantx.h"

#include "masternode-payments.h"

#include "masternode-sync.h"

#include "masternodeman.h"

#include "script/sign.h"

#include "txmempool.h"

#include "util.h"

#include "utilmoneystr.h"



#include <boost/lexical_cast.hpp>



<<<<<<< HEAD
// The main object for accessing Darksend
CDarksendPool darkSendPool;
// A helper object for signing messages from Masternodes
CDarkSendSigner darkSendSigner;
// The current Darksends in progress on the network
std::vector<CDarksendQueue> vecDarksendQueue;
// Keep track of the used Masternodes
std::vector<CTxIn> vecMasternodesUsed;
// Keep track of the scanning errors I've seen
map<uint256, CDarksendBroadcastTx> mapDarksendBroadcastTxes;
// Keep track of the active Masternode
CActiveMasternode activeMasternode;

/* *** BEGIN DARKSEND MAGIC - CURIUM **********
    Copyright (c) 2014-2015, Curium Developers
        ecruield - evan@curiumpay.io
        udjinm6   - udjinm6@curiumpay.io
*/

void CDarksendPool::ProcessMessageDarksend(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    if(fLiteMode) return; //disable all Darksend/Masternode related functionality
    if(!masternodeSync.IsBlockchainSynced()) return;
=======
int nPrivateSendRounds = DEFAULT_PRIVATESEND_ROUNDS;

int nPrivateSendAmount = DEFAULT_PRIVATESEND_AMOUNT;

int nLiquidityProvider = DEFAULT_PRIVATESEND_LIQUIDITY;

bool fEnablePrivateSend = false;
>>>>>>> dev-1.12.1.0

bool fPrivateSendMultiSession = DEFAULT_PRIVATESEND_MULTISESSION;



CDarksendPool darkSendPool;

CDarkSendSigner darkSendSigner;

std::map<uint256, CDarksendBroadcastTx> mapDarksendBroadcastTxes;

std::vector<CAmount> vecPrivateSendDenominations;


<<<<<<< HEAD
        if(sessionUsers == 0) {
            if(pmn->nLastDsq != 0 &&
                pmn->nLastDsq + mnodeman.CountEnabled(MIN_POOL_PEER_PROTO_VERSION)/5 > mnodeman.nDsqCount){
                LogPrintf("dsa -- last dsq too recent, must wait. %s \n", pfrom->addr.ToString());
                errorID = ERR_RECENT;
                pfrom->PushMessage("dssu", sessionID, GetState(), GetEntriesCount(), MASTERNODE_REJECTED, errorID);
                return;
            }
        }
=======
>>>>>>> dev-1.12.1.0

void CDarksendPool::ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)

{

    if(fLiteMode) return; // ignore all Curium related functionality

    if(!masternodeSync.IsBlockchainSynced()) return;



    if(strCommand == NetMsgType::DSACCEPT) {

<<<<<<< HEAD
        // if the queue is ready, submit if we can
        if(dsq.ready) {
            if(!pSubmittedToMasternode) return;
            if((CNetAddr)pSubmittedToMasternode->addr != (CNetAddr)addr){
                LogPrintf("dsq - message doesn't match current Masternode - %s != %s\n", pSubmittedToMasternode->addr.ToString(), addr.ToString());
                return;
            }

            if(state == POOL_STATUS_QUEUE){
                LogPrint("darksend", "Darksend queue is ready - %s\n", addr.ToString());
                PrepareDarksendDenominate();
            }
        } else {
            BOOST_FOREACH(CDarksendQueue q, vecDarksendQueue){
                if(q.vin == dsq.vin) return;
            }

            LogPrint("darksend", "dsq last %d last2 %d count %d\n", pmn->nLastDsq, pmn->nLastDsq + mnodeman.size()/5, mnodeman.nDsqCount);
            //don't allow a few nodes to dominate the queuing process
            if(pmn->nLastDsq != 0 &&
                pmn->nLastDsq + mnodeman.CountEnabled(MIN_POOL_PEER_PROTO_VERSION)/5 > mnodeman.nDsqCount){
                LogPrint("darksend", "dsq -- Masternode sending too many dsq messages. %s \n", pmn->addr.ToString());
                return;
            }
            mnodeman.nDsqCount++;
            pmn->nLastDsq = mnodeman.nDsqCount;
            pmn->allowFreeTx = true;

            LogPrint("darksend", "dsq - new Darksend queue object - %s\n", addr.ToString());
            vecDarksendQueue.push_back(dsq);
            dsq.Relay();
            dsq.time = GetTime();
        }

    } else if (strCommand == "dsi") { //DarkSend vIn
        int errorID;

        if (pfrom->nVersion < MIN_POOL_PEER_PROTO_VERSION) {
            LogPrintf("dsi -- incompatible version! \n");
            errorID = ERR_VERSION;
            pfrom->PushMessage("dssu", sessionID, GetState(), GetEntriesCount(), MASTERNODE_REJECTED, errorID);
=======


        if(pfrom->nVersion < MIN_PRIVATESEND_PEER_PROTO_VERSION) {

            LogPrintf("DSACCEPT -- incompatible version! nVersion: %d\n", pfrom->nVersion);

            PushStatus(pfrom, STATUS_REJECTED, ERR_VERSION);
>>>>>>> dev-1.12.1.0

            return;

        }



        if(!fMasterNode) {

            LogPrintf("DSACCEPT -- not a Masternode!\n");

            PushStatus(pfrom, STATUS_REJECTED, ERR_NOT_A_MN);

            return;

        }



        if(IsSessionReady()) {

            // too many users in this session already, reject new ones

            LogPrintf("DSACCEPT -- queue is already full!\n");

            PushStatus(pfrom, STATUS_ACCEPTED, ERR_QUEUE_FULL);

            return;

        }



        int nDenom;

<<<<<<< HEAD
                if(o.scriptPubKey.size() != 25){
                    LogPrintf("dsi - non-standard pubkey detected! %s\n", o.scriptPubKey.ToString());
                    errorID = ERR_NON_STANDARD_PUBKEY;
                    pfrom->PushMessage("dssu", sessionID, GetState(), GetEntriesCount(), MASTERNODE_REJECTED, errorID);
                    return;
                }
                if(!o.scriptPubKey.IsNormalPaymentScript()){
                    LogPrintf("dsi - invalid script! %s\n", o.scriptPubKey.ToString());
                    errorID = ERR_INVALID_SCRIPT;
                    pfrom->PushMessage("dssu", sessionID, GetState(), GetEntriesCount(), MASTERNODE_REJECTED, errorID);
                    return;
                }
            }
=======
        CTransaction txCollateral;
>>>>>>> dev-1.12.1.0

        vRecv >> nDenom >> txCollateral;

<<<<<<< HEAD
                LogPrint("darksend", "dsi -- tx in %s\n", i.ToString());
=======
>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
            if (nValueIn > DARKSEND_POOL_MAX) {
                LogPrintf("dsi -- more than Darksend pool max! %s\n", tx.ToString());
                errorID = ERR_MAXIMUM;
                pfrom->PushMessage("dssu", sessionID, GetState(), GetEntriesCount(), MASTERNODE_REJECTED, errorID);
                return;
            }

            if(!missingTx){
                if (nValueIn-nValueOut > nValueIn*.01) {
                    LogPrintf("dsi -- fees are too high! %s\n", tx.ToString());
                    errorID = ERR_FEES;
                    pfrom->PushMessage("dssu", sessionID, GetState(), GetEntriesCount(), MASTERNODE_REJECTED, errorID);
                    return;
                }
            } else {
                LogPrintf("dsi -- missing input tx! %s\n", tx.ToString());
                errorID = ERR_MISSING_TX;
                pfrom->PushMessage("dssu", sessionID, GetState(), GetEntriesCount(), MASTERNODE_REJECTED, errorID);
                return;
            }

            {
                LOCK(cs_main);
                if(!AcceptableInputs(mempool, state, CTransaction(tx), false, NULL, false, true)) {
                    LogPrintf("dsi -- transaction not valid! \n");
                    errorID = ERR_INVALID_TX;
                    pfrom->PushMessage("dssu", sessionID, GetState(), GetEntriesCount(), MASTERNODE_REJECTED, errorID);
                    return;
                }
            }
        }
=======
        LogPrint("privatesend", "DSACCEPT -- nDenom %d (%s)  txCollateral %s", nDenom, GetDenominationsToString(nDenom), txCollateral.ToString());


>>>>>>> dev-1.12.1.0

        CMasternode* pmn = mnodeman.Find(activeMasternode.vin);

        if(pmn == NULL) {

            PushStatus(pfrom, STATUS_REJECTED, ERR_MN_LIST);

<<<<<<< HEAD
        if(!pSubmittedToMasternode) return;
        if((CNetAddr)pSubmittedToMasternode->addr != (CNetAddr)pfrom->addr){
            //LogPrintf("dssu - message doesn't match current Masternode - %s != %s\n", pSubmittedToMasternode->addr.ToString(), pfrom->addr.ToString());
=======
>>>>>>> dev-1.12.1.0
            return;

        }


<<<<<<< HEAD
        LogPrint("darksend", "dssu - state: %i entriesCount: %i accepted: %i error: %s \n", state, entriesCount, accepted, GetMessageByID(errorID));
=======
>>>>>>> dev-1.12.1.0

        if(vecSessionCollaterals.size() == 0 && pmn->nLastDsq != 0 &&

            pmn->nLastDsq + mnodeman.CountEnabled(MIN_PRIVATESEND_PEER_PROTO_VERSION)/5 > mnodeman.nDsqCount)

        {

            LogPrintf("DSACCEPT -- last dsq too recent, must wait: addr=%s\n", pfrom->addr.ToString());

            PushStatus(pfrom, STATUS_REJECTED, ERR_RECENT);

            return;

        }



<<<<<<< HEAD
        BOOST_FOREACH(const CTxIn item, sigs)
        {
            if(AddScriptSig(item)) success = true;
            LogPrint("darksend", " -- sigs count %d %d\n", (int)sigs.size(), count);
            count++;
        }
=======
        PoolMessage nMessageID = MSG_NOERR;
>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
        if(!pSubmittedToMasternode) return;
        if((CNetAddr)pSubmittedToMasternode->addr != (CNetAddr)pfrom->addr){
            //LogPrintf("dsc - message doesn't match current Masternode - %s != %s\n", pSubmittedToMasternode->addr.ToString(), pfrom->addr.ToString());
            return;
        }
=======
>>>>>>> dev-1.12.1.0

        bool fResult = nSessionID == 0  ? CreateNewSession(nDenom, txCollateral, nMessageID)

<<<<<<< HEAD
        if(sessionID != sessionIDMessage){
            LogPrint("darksend", "dsf - message doesn't match current Darksend session %d %d\n", sessionID, sessionIDMessage);
            return;
        }
=======
                                        : AddUserToExistingSession(nDenom, txCollateral, nMessageID);
>>>>>>> dev-1.12.1.0

        if(fResult) {

            LogPrintf("DSACCEPT -- is compatible, please submit!\n");

            PushStatus(pfrom, STATUS_ACCEPTED, nMessageID);

<<<<<<< HEAD
        if(!pSubmittedToMasternode) return;
        if((CNetAddr)pSubmittedToMasternode->addr != (CNetAddr)pfrom->addr){
            //LogPrintf("dsc - message doesn't match current Masternode - %s != %s\n", pSubmittedToMasternode->addr.ToString(), pfrom->addr.ToString());
=======
>>>>>>> dev-1.12.1.0
            return;

<<<<<<< HEAD
        int sessionIDMessage;
        bool error;
        int errorID;
        vRecv >> sessionIDMessage >> error >> errorID;

        if(sessionID != sessionIDMessage){
            LogPrint("darksend", "dsc - message doesn't match current Darksend session %d %d\n", darkSendPool.sessionID, sessionIDMessage);
=======
        } else {

            LogPrintf("DSACCEPT -- not compatible with existing transactions!\n");

            PushStatus(pfrom, STATUS_REJECTED, nMessageID);

>>>>>>> dev-1.12.1.0
            return;

        }

<<<<<<< HEAD
        darkSendPool.CompletedTransaction(error, errorID);
    }
=======
>>>>>>> dev-1.12.1.0


    } else if(strCommand == NetMsgType::DSQUEUE) {

<<<<<<< HEAD
void CDarksendPool::Reset(){
    cachedLastSuccess = 0;
    lastNewBlock = 0;
    txCollateral = CMutableTransaction();
    vecMasternodesUsed.clear();
    UnlockCoins();
    SetNull();
}

void CDarksendPool::SetNull(){

    // MN side
    sessionUsers = 0;
    vecSessionCollateral.clear();

    // Client side
    entriesCount = 0;
    lastEntryAccepted = 0;
    countEntriesAccepted = 0;
    sessionFoundMasternode = false;

    // Both sides
    state = POOL_STATUS_IDLE;
    sessionID = 0;
    sessionDenom = 0;
    entries.clear();
    finalTransaction.vin.clear();
    finalTransaction.vout.clear();
    lastTimeChanged = GetTimeMillis();
=======
        TRY_LOCK(cs_darksend, lockRecv);

        if(!lockRecv) return;



        if(pfrom->nVersion < MIN_PRIVATESEND_PEER_PROTO_VERSION) {

            LogPrint("privatesend", "DSQUEUE -- incompatible version! nVersion: %d\n", pfrom->nVersion);

            return;

        }



        CDarksendQueue dsq;

        vRecv >> dsq;



        // process every dsq only once

        BOOST_FOREACH(CDarksendQueue q, vecDarksendQueue) {

            if(q == dsq) {

                // LogPrint("privatesend", "DSQUEUE -- %s seen\n", dsq.ToString());

                return;
>>>>>>> dev-1.12.1.0

            }

        }

<<<<<<< HEAD
//
// Unlock coins after Darksend fails or succeeds
//
void CDarksendPool::UnlockCoins(){
    while(true) {
        TRY_LOCK(pwalletMain->cs_wallet, lockWallet);
        if(!lockWallet) {MilliSleep(50); continue;}
        BOOST_FOREACH(CTxIn v, lockedCoins)
                pwalletMain->UnlockCoin(v.prevout);
        break;
    }
=======
>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
std::string CDarksendPool::GetStatus()
{
    static int showingDarkSendMessage = 0;
    showingDarkSendMessage += 10;
    std::string suffix = "";

    if(chainActive.Tip()->nHeight - cachedLastSuccess < minBlockSpacing || !masternodeSync.IsBlockchainSynced()) {
        return strAutoDenomResult;
    }
    switch(state) {
        case POOL_STATUS_IDLE:
            return _("Darksend is idle.");
        case POOL_STATUS_ACCEPTING_ENTRIES:
            if(entriesCount == 0) {
                showingDarkSendMessage = 0;
                return strAutoDenomResult;
            } else if (lastEntryAccepted == 1) {
                if(showingDarkSendMessage % 10 > 8) {
                    lastEntryAccepted = 0;
                    showingDarkSendMessage = 0;
                }
                return _("Darksend request complete:") + " " + _("Your transaction was accepted into the pool!");
            } else {
                std::string suffix = "";
                if(     showingDarkSendMessage % 70 <= 40) return strprintf(_("Submitted following entries to masternode: %u / %d"), entriesCount, GetMaxPoolTransactions());
                else if(showingDarkSendMessage % 70 <= 50) suffix = ".";
                else if(showingDarkSendMessage % 70 <= 60) suffix = "..";
                else if(showingDarkSendMessage % 70 <= 70) suffix = "...";
                return strprintf(_("Submitted to masternode, waiting for more entries ( %u / %d ) %s"), entriesCount, GetMaxPoolTransactions(), suffix);
            }
        case POOL_STATUS_SIGNING:
            if(     showingDarkSendMessage % 70 <= 40) return _("Found enough users, signing ...");
            else if(showingDarkSendMessage % 70 <= 50) suffix = ".";
            else if(showingDarkSendMessage % 70 <= 60) suffix = "..";
            else if(showingDarkSendMessage % 70 <= 70) suffix = "...";
            return strprintf(_("Found enough users, signing ( waiting %s )"), suffix);
        case POOL_STATUS_TRANSMISSION:
            return _("Transmitting final transaction.");
        case POOL_STATUS_FINALIZE_TRANSACTION:
            return _("Finalizing transaction.");
        case POOL_STATUS_ERROR:
            return _("Darksend request incomplete:") + " " + lastMessage + " " + _("Will retry...");
        case POOL_STATUS_SUCCESS:
            return _("Darksend request complete:") + " " + lastMessage;
        case POOL_STATUS_QUEUE:
            if(     showingDarkSendMessage % 70 <= 30) suffix = ".";
            else if(showingDarkSendMessage % 70 <= 50) suffix = "..";
            else if(showingDarkSendMessage % 70 <= 70) suffix = "...";
            return strprintf(_("Submitted to masternode, waiting in queue %s"), suffix);;
       default:
            return strprintf(_("Unknown state: id = %u"), state);
    }
}

//
// Check the Darksend progress and send client updates if a Masternode
//
void CDarksendPool::Check()
{
    if(fMasterNode) LogPrint("darksend", "CDarksendPool::Check() - entries count %lu\n", entries.size());
    //printf("CDarksendPool::Check() %d - %d - %d\n", state, anonTx.CountEntries(), GetTimeMillis()-lastTimeChanged);

    if(fMasterNode) {
        LogPrint("darksend", "CDarksendPool::Check() - entries count %lu\n", entries.size());

        // If entries is full, then move on to the next phase
        if(state == POOL_STATUS_ACCEPTING_ENTRIES && (int)entries.size() >= GetMaxPoolTransactions())
        {
            LogPrint("darksend", "CDarksendPool::Check() -- TRYING TRANSACTION \n");
            UpdateState(POOL_STATUS_FINALIZE_TRANSACTION);
        }
    }

    // create the finalized transaction for distribution to the clients
    if(state == POOL_STATUS_FINALIZE_TRANSACTION) {
        LogPrint("darksend", "CDarksendPool::Check() -- FINALIZE TRANSACTIONS\n");
        UpdateState(POOL_STATUS_SIGNING);
=======
        LogPrint("privatesend", "DSQUEUE -- %s new\n", dsq.ToString());



        if(dsq.IsExpired() || dsq.nTime > GetTime() + PRIVATESEND_QUEUE_TIMEOUT) return;
>>>>>>> dev-1.12.1.0



        CMasternode* pmn = mnodeman.Find(dsq.vin);

        if(pmn == NULL) return;



        if(!dsq.CheckSignature(pmn->pubKeyMasternode)) {

            // we probably have outdated info

            mnodeman.AskForMN(pfrom, dsq.vin);

<<<<<<< HEAD
            LogPrint("darksend", "Transaction 1: %s\n", txNew.ToString());
            finalTransaction = txNew;
=======
            return;
>>>>>>> dev-1.12.1.0

        }

<<<<<<< HEAD
    // If we have all of the signatures, try to compile the transaction
    if(fMasterNode && state == POOL_STATUS_SIGNING && SignaturesComplete()) {
        LogPrint("darksend", "CDarksendPool::Check() -- SIGNING\n");
        UpdateState(POOL_STATUS_TRANSMISSION);
=======
>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
    // reset if we're here for 10 seconds
    if((state == POOL_STATUS_ERROR || state == POOL_STATUS_SUCCESS) && GetTimeMillis()-lastTimeChanged >= 10000) {
        LogPrint("darksend", "CDarksendPool::Check() -- timeout, RESETTING\n");
        UnlockCoins();
        SetNull();
        if(fMasterNode) RelayStatus(sessionID, GetState(), GetEntriesCount(), MASTERNODE_RESET);
    }
}

void CDarksendPool::CheckFinalTransaction()
{
    if (!fMasterNode) return; // check and relay final tx only on masternode

    CWalletTx txNew = CWalletTx(pwalletMain, finalTransaction);

    LOCK2(cs_main, pwalletMain->cs_wallet);
    {
        LogPrint("darksend", "Transaction 2: %s\n", txNew.ToString());

        // See if the transaction is valid
        if (!txNew.AcceptToMemoryPool(false, true, true))
        {
            LogPrintf("CDarksendPool::Check() - CommitTransaction : Error: Transaction not valid\n");
            SetNull();

            // not much we can do in this case
            UpdateState(POOL_STATUS_ACCEPTING_ENTRIES);
            RelayCompletedTransaction(sessionID, true, ERR_INVALID_TX);
            return;
        }

        LogPrintf("CDarksendPool::Check() -- IS MASTER -- TRANSMITTING DARKSEND\n");

        // sign a message

        int64_t sigTime = GetAdjustedTime();
        std::string strMessage = txNew.GetHash().ToString() + boost::lexical_cast<std::string>(sigTime);
        std::string strError = "";
        std::vector<unsigned char> vchSig;
        CKey key2;
        CPubKey pubkey2;

        if(!darkSendSigner.SetKey(strMasterNodePrivKey, strError, key2, pubkey2))
        {
            LogPrintf("CDarksendPool::Check() - ERROR: Invalid Masternodeprivkey: '%s'\n", strError);
            return;
        }

        if(!darkSendSigner.SignMessage(strMessage, strError, vchSig, key2)) {
            LogPrintf("CDarksendPool::Check() - Sign message failed\n");
            return;
        }

        if(!darkSendSigner.VerifyMessage(pubkey2, vchSig, strMessage, strError)) {
            LogPrintf("CDarksendPool::Check() - Verify message failed\n");
            return;
        }

        if(!mapDarksendBroadcastTxes.count(txNew.GetHash())){
            CDarksendBroadcastTx dstx;
            dstx.tx = txNew;
            dstx.vin = activeMasternode.vin;
            dstx.vchSig = vchSig;
            dstx.sigTime = sigTime;

            mapDarksendBroadcastTxes.insert(make_pair(txNew.GetHash(), dstx));
        }

        CInv inv(MSG_DSTX, txNew.GetHash());
        RelayInv(inv);

        // Tell the clients it was successful
        RelayCompletedTransaction(sessionID, false, MSG_SUCCESS);

        // Randomly charge clients
        ChargeRandomFees();

        // Reset
        LogPrint("darksend", "CDarksendPool::Check() -- COMPLETED -- RESETTING\n");
        SetNull();
        RelayStatus(sessionID, GetState(), GetEntriesCount(), MASTERNODE_RESET);
    }
}

//
// Charge clients a fee if they're abusive
//
// Why bother? Darksend uses collateral to ensure abuse to the process is kept to a minimum.
// The submission and signing stages in Darksend are completely separate. In the cases where
// a client submits a transaction then refused to sign, there must be a cost. Otherwise they
// would be able to do this over and over again and bring the mixing to a hault.
//
// How does this work? Messages to Masternodes come in via "dsi", these require a valid collateral
// transaction for the client to be able to enter the pool. This transaction is kept by the Masternode
// until the transaction is either complete or fails.
//
void CDarksendPool::ChargeFees(){
    if(!fMasterNode) return;

    //we don't need to charge collateral for every offence.
    int offences = 0;
    int r = rand()%100;
    if(r > 33) return;

    if(state == POOL_STATUS_ACCEPTING_ENTRIES){
        BOOST_FOREACH(const CTransaction& txCollateral, vecSessionCollateral) {
            bool found = false;
            BOOST_FOREACH(const CDarkSendEntry& v, entries) {
                if(v.collateral == txCollateral) {
                    found = true;
                }
            }

            // This queue entry didn't send us the promised transaction
            if(!found){
                LogPrintf("CDarksendPool::ChargeFees -- found uncooperative node (didn't send transaction). Found offence.\n");
                offences++;
            }
        }
    }

    if(state == POOL_STATUS_SIGNING) {
        // who didn't sign?
        BOOST_FOREACH(const CDarkSendEntry v, entries) {
            BOOST_FOREACH(const CTxDSIn s, v.sev) {
                if(!s.fHasSig){
                    LogPrintf("CDarksendPool::ChargeFees -- found uncooperative node (didn't sign). Found offence\n");
                    offences++;
                }
            }
        }
    }

    r = rand()%100;
    int target = 0;

    //mostly offending?
    if(offences >= Params().PoolMaxTransactions()-1 && r > 33) return;

    //everyone is an offender? That's not right
    if(offences >= Params().PoolMaxTransactions()) return;

    //charge one of the offenders randomly
    if(offences > 1) target = 50;

    //pick random client to charge
    r = rand()%100;

    if(state == POOL_STATUS_ACCEPTING_ENTRIES){
        BOOST_FOREACH(const CTransaction& txCollateral, vecSessionCollateral) {
            bool found = false;
            BOOST_FOREACH(const CDarkSendEntry& v, entries) {
                if(v.collateral == txCollateral) {
                    found = true;
                }
            }

            // This queue entry didn't send us the promised transaction
            if(!found && r > target){
                LogPrintf("CDarksendPool::ChargeFees -- found uncooperative node (didn't send transaction). charging fees.\n");

                CWalletTx wtxCollateral = CWalletTx(pwalletMain, txCollateral);

                // Broadcast
                if (!wtxCollateral.AcceptToMemoryPool(true))
                {
                    // This must not fail. The transaction has already been signed and recorded.
                    LogPrintf("CDarksendPool::ChargeFees() : Error: Transaction not valid");
                }
                wtxCollateral.RelayWalletTransaction();
                return;
            }
        }
    }

    if(state == POOL_STATUS_SIGNING) {
        // who didn't sign?
        BOOST_FOREACH(const CDarkSendEntry v, entries) {
            BOOST_FOREACH(const CTxDSIn s, v.sev) {
                if(!s.fHasSig && r > target){
                    LogPrintf("CDarksendPool::ChargeFees -- found uncooperative node (didn't sign). charging fees.\n");

                    CWalletTx wtxCollateral = CWalletTx(pwalletMain, v.collateral);

                    // Broadcast
                    if (!wtxCollateral.AcceptToMemoryPool(false))
                    {
                        // This must not fail. The transaction has already been signed and recorded.
                        LogPrintf("CDarksendPool::ChargeFees() : Error: Transaction not valid");
                    }
                    wtxCollateral.RelayWalletTransaction();
                    return;
=======
        // if the queue is ready, submit if we can

        if(dsq.fReady) {

            if(!pSubmittedToMasternode) return;

            if((CNetAddr)pSubmittedToMasternode->addr != (CNetAddr)pmn->addr) {

                LogPrintf("DSQUEUE -- message doesn't match current Masternode: pSubmittedToMasternode=%s, addr=%s\n", pSubmittedToMasternode->addr.ToString(), pmn->addr.ToString());

                return;

            }



            if(nState == POOL_STATE_QUEUE) {

                LogPrint("privatesend", "DSQUEUE -- PrivateSend queue (%s) is ready on masternode %s\n", dsq.ToString(), pmn->addr.ToString());

                SubmitDenominate();

            }

        } else {

            BOOST_FOREACH(CDarksendQueue q, vecDarksendQueue) {

                if(q.vin == dsq.vin) {

                    // no way same mn can send another "not yet ready" dsq this soon

                    LogPrint("privatesend", "DSQUEUE -- Masternode %s is sending WAY too many dsq messages\n", pmn->addr.ToString());

                    return;

                }

            }



            int nThreshold = pmn->nLastDsq + mnodeman.CountEnabled(MIN_PRIVATESEND_PEER_PROTO_VERSION)/5;

            LogPrint("privatesend", "DSQUEUE -- nLastDsq: %d  threshold: %d  nDsqCount: %d\n", pmn->nLastDsq, nThreshold, mnodeman.nDsqCount);

            //don't allow a few nodes to dominate the queuing process

            if(pmn->nLastDsq != 0 && nThreshold > mnodeman.nDsqCount) {

                LogPrint("privatesend", "DSQUEUE -- Masternode %s is sending too many dsq messages\n", pmn->addr.ToString());

                return;

            }

            mnodeman.nDsqCount++;

            pmn->nLastDsq = mnodeman.nDsqCount;

            pmn->fAllowMixingTx = true;



            LogPrint("privatesend", "DSQUEUE -- new PrivateSend queue (%s) from masternode %s\n", dsq.ToString(), pmn->addr.ToString());

            if(pSubmittedToMasternode && pSubmittedToMasternode->vin.prevout == dsq.vin.prevout) {

                dsq.fTried = true;

            }

            vecDarksendQueue.push_back(dsq);

            dsq.Relay();

        }



    } else if(strCommand == NetMsgType::DSVIN) {



        if(pfrom->nVersion < MIN_PRIVATESEND_PEER_PROTO_VERSION) {

            LogPrintf("DSVIN -- incompatible version! nVersion: %d\n", pfrom->nVersion);

            PushStatus(pfrom, STATUS_REJECTED, ERR_VERSION);

            return;

        }



        if(!fMasterNode) {

            LogPrintf("DSVIN -- not a Masternode!\n");

            PushStatus(pfrom, STATUS_REJECTED, ERR_NOT_A_MN);

            return;

        }



        //do we have enough users in the current session?

        if(!IsSessionReady()) {

            LogPrintf("DSVIN -- session not complete!\n");

            PushStatus(pfrom, STATUS_REJECTED, ERR_SESSION);

            return;

        }



        CDarkSendEntry entry;

        vRecv >> entry;



        LogPrint("privatesend", "DSVIN -- txCollateral %s", entry.txCollateral.ToString());



        //do we have the same denominations as the current session?

        if(!IsOutputsCompatibleWithSessionDenom(entry.vecTxDSOut)) {

            LogPrintf("DSVIN -- not compatible with existing transactions!\n");

            PushStatus(pfrom, STATUS_REJECTED, ERR_EXISTING_TX);

            return;

        }



        //check it like a transaction

        {

            CAmount nValueIn = 0;

            CAmount nValueOut = 0;



            CMutableTransaction tx;



            BOOST_FOREACH(const CTxOut txout, entry.vecTxDSOut) {

                nValueOut += txout.nValue;

                tx.vout.push_back(txout);



                if(txout.scriptPubKey.size() != 25) {

                    LogPrintf("DSVIN -- non-standard pubkey detected! scriptPubKey=%s\n", ScriptToAsmStr(txout.scriptPubKey));

                    PushStatus(pfrom, STATUS_REJECTED, ERR_NON_STANDARD_PUBKEY);

                    return;

                }

                if(!txout.scriptPubKey.IsNormalPaymentScript()) {

                    LogPrintf("DSVIN -- invalid script! scriptPubKey=%s\n", ScriptToAsmStr(txout.scriptPubKey));

                    PushStatus(pfrom, STATUS_REJECTED, ERR_INVALID_SCRIPT);

                    return;

                }

            }



            BOOST_FOREACH(const CTxIn txin, entry.vecTxDSIn) {

                tx.vin.push_back(txin);



                LogPrint("privatesend", "DSVIN -- txin=%s\n", txin.ToString());



                CTransaction txPrev;

                uint256 hash;

                if(GetTransaction(txin.prevout.hash, txPrev, Params().GetConsensus(), hash, true)) {

                    if(txPrev.vout.size() > txin.prevout.n)

                        nValueIn += txPrev.vout[txin.prevout.n].nValue;

                } else {

                    LogPrintf("DSVIN -- missing input! tx=%s", tx.ToString());

                    PushStatus(pfrom, STATUS_REJECTED, ERR_MISSING_TX);

                    return;

                }

            }



            if(nValueIn > PRIVATESEND_POOL_MAX) {

                LogPrintf("DSVIN -- more than PrivateSend pool max! nValueIn: %lld, tx=%s", nValueIn, tx.ToString());

                PushStatus(pfrom, STATUS_REJECTED, ERR_MAXIMUM);

                return;

            }



            // Allow lowest denom (at max) as a a fee. Normally shouldn't happen though.

            // TODO: Or do not allow fees at all?

            if(nValueIn - nValueOut > vecPrivateSendDenominations.back()) {

                LogPrintf("DSVIN -- fees are too high! fees: %lld, tx=%s", nValueIn - nValueOut, tx.ToString());

                PushStatus(pfrom, STATUS_REJECTED, ERR_FEES);

                return;

            }



            {

                LOCK(cs_main);

                CValidationState validationState;

                mempool.PrioritiseTransaction(tx.GetHash(), tx.GetHash().ToString(), 1000, 0.1*COIN);

                if(!AcceptToMemoryPool(mempool, validationState, CTransaction(tx), false, NULL, false, true, true)) {

                    LogPrintf("DSVIN -- transaction not valid! tx=%s", tx.ToString());

                    PushStatus(pfrom, STATUS_REJECTED, ERR_INVALID_TX);

                    return;

>>>>>>> dev-1.12.1.0
                }

            }

        }



        PoolMessage nMessageID = MSG_NOERR;



        if(AddEntry(entry, nMessageID)) {

            PushStatus(pfrom, STATUS_ACCEPTED, nMessageID);

            CheckPool();

            RelayStatus(STATUS_ACCEPTED);

        } else {

            PushStatus(pfrom, STATUS_REJECTED, nMessageID);

            SetNull();

        }



    } else if(strCommand == NetMsgType::DSSTATUSUPDATE) {



        if(pfrom->nVersion < MIN_PRIVATESEND_PEER_PROTO_VERSION) {

            LogPrintf("DSSTATUSUPDATE -- incompatible version! nVersion: %d\n", pfrom->nVersion);

            return;

        }



        if(fMasterNode) {

            // LogPrintf("DSSTATUSUPDATE -- Can't run on a Masternode!\n");

            return;

        }



        if(!pSubmittedToMasternode) return;

        if((CNetAddr)pSubmittedToMasternode->addr != (CNetAddr)pfrom->addr) {

            //LogPrintf("DSSTATUSUPDATE -- message doesn't match current Masternode: pSubmittedToMasternode %s addr %s\n", pSubmittedToMasternode->addr.ToString(), pfrom->addr.ToString());

            return;

        }



        int nMsgSessionID;

        int nMsgState;

        int nMsgEntriesCount;

        int nMsgStatusUpdate;

        int nMsgMessageID;

        vRecv >> nMsgSessionID >> nMsgState >> nMsgEntriesCount >> nMsgStatusUpdate >> nMsgMessageID;



        LogPrint("privatesend", "DSSTATUSUPDATE -- nMsgSessionID %d  nMsgState: %d  nEntriesCount: %d  nMsgStatusUpdate: %d  nMsgMessageID %d\n",

                nMsgSessionID, nMsgState, nEntriesCount, nMsgStatusUpdate, nMsgMessageID);



        if(nMsgState < POOL_STATE_MIN || nMsgState > POOL_STATE_MAX) {

            LogPrint("privatesend", "DSSTATUSUPDATE -- nMsgState is out of bounds: %d\n", nMsgState);

            return;

        }



        if(nMsgStatusUpdate < STATUS_REJECTED || nMsgStatusUpdate > STATUS_ACCEPTED) {

            LogPrint("privatesend", "DSSTATUSUPDATE -- nMsgStatusUpdate is out of bounds: %d\n", nMsgStatusUpdate);

            return;

        }



        if(nMsgMessageID < MSG_POOL_MIN || nMsgMessageID > MSG_POOL_MAX) {

            LogPrint("privatesend", "DSSTATUSUPDATE -- nMsgMessageID is out of bounds: %d\n", nMsgMessageID);

            if(pfrom->nVersion < 70203) nMsgMessageID = MSG_NOERR;

            return;

        }



        LogPrint("privatesend", "DSSTATUSUPDATE -- GetMessageByID: %s\n", GetMessageByID(PoolMessage(nMsgMessageID)));



        if(!CheckPoolStateUpdate(PoolState(nMsgState), nMsgEntriesCount, PoolStatusUpdate(nMsgStatusUpdate), PoolMessage(nMsgMessageID), nMsgSessionID)) {

            LogPrint("privatesend", "DSSTATUSUPDATE -- CheckPoolStateUpdate failed\n");

        }



    } else if(strCommand == NetMsgType::DSSIGNFINALTX) {



        if(pfrom->nVersion < MIN_PRIVATESEND_PEER_PROTO_VERSION) {

            LogPrintf("DSSIGNFINALTX -- incompatible version! nVersion: %d\n", pfrom->nVersion);

            return;

        }



        if(!fMasterNode) {

            LogPrintf("DSSIGNFINALTX -- not a Masternode!\n");

            return;

        }



        std::vector<CTxIn> vecTxIn;

        vRecv >> vecTxIn;



        LogPrint("privatesend", "DSSIGNFINALTX -- vecTxIn.size() %s\n", vecTxIn.size());



        int nTxInIndex = 0;

        int nTxInsCount = (int)vecTxIn.size();



        BOOST_FOREACH(const CTxIn txin, vecTxIn) {

            nTxInIndex++;

            if(!AddScriptSig(txin)) {

                LogPrint("privatesend", "DSSIGNFINALTX -- AddScriptSig() failed at %d/%d, session: %d\n", nTxInIndex, nTxInsCount, nSessionID);

                RelayStatus(STATUS_REJECTED);

                return;

            }

            LogPrint("privatesend", "DSSIGNFINALTX -- AddScriptSig() %d/%d success\n", nTxInIndex, nTxInsCount);

        }

        // all is good

        CheckPool();



    } else if(strCommand == NetMsgType::DSFINALTX) {



        if(pfrom->nVersion < MIN_PRIVATESEND_PEER_PROTO_VERSION) {

            LogPrintf("DSFINALTX -- incompatible version! nVersion: %d\n", pfrom->nVersion);

            return;

        }



        if(fMasterNode) {

            // LogPrintf("DSFINALTX -- Can't run on a Masternode!\n");

            return;

        }



        if(!pSubmittedToMasternode) return;

        if((CNetAddr)pSubmittedToMasternode->addr != (CNetAddr)pfrom->addr) {

            //LogPrintf("DSFINALTX -- message doesn't match current Masternode: pSubmittedToMasternode %s addr %s\n", pSubmittedToMasternode->addr.ToString(), pfrom->addr.ToString());

            return;

        }



        int nMsgSessionID;

        CTransaction txNew;

        vRecv >> nMsgSessionID >> txNew;



        if(nSessionID != nMsgSessionID) {

            LogPrint("privatesend", "DSFINALTX -- message doesn't match current PrivateSend session: nSessionID: %d  nMsgSessionID: %d\n", nSessionID, nMsgSessionID);

            return;

        }



        LogPrint("privatesend", "DSFINALTX -- txNew %s", txNew.ToString());



        //check to see if input is spent already? (and probably not confirmed)

        SignFinalTransaction(txNew, pfrom);



    } else if(strCommand == NetMsgType::DSCOMPLETE) {



        if(pfrom->nVersion < MIN_PRIVATESEND_PEER_PROTO_VERSION) {

            LogPrintf("DSCOMPLETE -- incompatible version! nVersion: %d\n", pfrom->nVersion);

            return;

        }



        if(fMasterNode) {

            // LogPrintf("DSCOMPLETE -- Can't run on a Masternode!\n");

            return;

        }



        if(!pSubmittedToMasternode) return;

        if((CNetAddr)pSubmittedToMasternode->addr != (CNetAddr)pfrom->addr) {

            LogPrint("privatesend", "DSCOMPLETE -- message doesn't match current Masternode: pSubmittedToMasternode=%s  addr=%s\n", pSubmittedToMasternode->addr.ToString(), pfrom->addr.ToString());

            return;

        }



        int nMsgSessionID;

        int nMsgMessageID;

        vRecv >> nMsgSessionID >> nMsgMessageID;



        if(nMsgMessageID < MSG_POOL_MIN || nMsgMessageID > MSG_POOL_MAX) {

            LogPrint("privatesend", "DSCOMPLETE -- nMsgMessageID is out of bounds: %d\n", nMsgMessageID);

            return;

        }



        if(nSessionID != nMsgSessionID) {

            LogPrint("privatesend", "DSCOMPLETE -- message doesn't match current PrivateSend session: nSessionID: %d  nMsgSessionID: %d\n", darkSendPool.nSessionID, nMsgSessionID);

            return;

        }



        LogPrint("privatesend", "DSCOMPLETE -- nMsgSessionID %d  nMsgMessageID %d (%s)\n", nMsgSessionID, nMsgMessageID, GetMessageByID(PoolMessage(nMsgMessageID)));



        CompletedTransaction(PoolMessage(nMsgMessageID));

    }

}



void CDarksendPool::InitDenominations()

{

    vecPrivateSendDenominations.clear();

    /* Denominations



        A note about convertability. Within mixing pools, each denomination

        is convertable to another.



        For example:

        1CRU+1000 == (.1CRU+100)*10

        10CRU+10000 == (1CRU+1000)*10

    */

    /* Disabled

    vecPrivateSendDenominations.push_back( (100      * COIN)+100000 );

    */

    vecPrivateSendDenominations.push_back( (10       * COIN)+10000 );

    vecPrivateSendDenominations.push_back( (1        * COIN)+1000 );

    vecPrivateSendDenominations.push_back( (.1       * COIN)+100 );

    vecPrivateSendDenominations.push_back( (.01      * COIN)+10 );

    /* Disabled till we need them

    vecPrivateSendDenominations.push_back( (.001     * COIN)+1 );

    */

}



void CDarksendPool::ResetPool()

{

    nCachedLastSuccessBlock = 0;

    txMyCollateral = CMutableTransaction();

    vecMasternodesUsed.clear();

    UnlockCoins();

    SetNull();

}



void CDarksendPool::SetNull()

{

    // MN side

    vecSessionCollaterals.clear();



    // Client side

    nEntriesCount = 0;

    fLastEntryAccepted = false;

    pSubmittedToMasternode = NULL;



    // Both sides

    nState = POOL_STATE_IDLE;

    nSessionID = 0;

    nSessionDenom = 0;

    vecEntries.clear();

    finalMutableTransaction.vin.clear();

    finalMutableTransaction.vout.clear();

    nTimeLastSuccessfulStep = GetTimeMillis();

}



//

// Unlock coins after mixing fails or succeeds

//

void CDarksendPool::UnlockCoins()

{

    while(true) {

        TRY_LOCK(pwalletMain->cs_wallet, lockWallet);

        if(!lockWallet) {MilliSleep(50); continue;}

        BOOST_FOREACH(COutPoint outpoint, vecOutPointLocked)

            pwalletMain->UnlockCoin(outpoint);

        break;

    }



    vecOutPointLocked.clear();

}



std::string CDarksendPool::GetStateString() const

{

    switch(nState) {

        case POOL_STATE_IDLE:                   return "IDLE";

        case POOL_STATE_QUEUE:                  return "QUEUE";

        case POOL_STATE_ACCEPTING_ENTRIES:      return "ACCEPTING_ENTRIES";

        case POOL_STATE_SIGNING:                return "SIGNING";

        case POOL_STATE_ERROR:                  return "ERROR";

        case POOL_STATE_SUCCESS:                return "SUCCESS";

        default:                                return "UNKNOWN";

    }

}



std::string CDarksendPool::GetStatus()

{

    static int nStatusMessageProgress = 0;

    nStatusMessageProgress += 10;

    std::string strSuffix = "";



    if((pCurrentBlockIndex && pCurrentBlockIndex->nHeight - nCachedLastSuccessBlock < nMinBlockSpacing) || !masternodeSync.IsBlockchainSynced())

        return strAutoDenomResult;



    switch(nState) {

        case POOL_STATE_IDLE:

            return _("PrivateSend is idle.");

        case POOL_STATE_QUEUE:

            if(     nStatusMessageProgress % 70 <= 30) strSuffix = ".";

            else if(nStatusMessageProgress % 70 <= 50) strSuffix = "..";

            else if(nStatusMessageProgress % 70 <= 70) strSuffix = "...";

            return strprintf(_("Submitted to masternode, waiting in queue %s"), strSuffix);;

        case POOL_STATE_ACCEPTING_ENTRIES:

            if(nEntriesCount == 0) {

                nStatusMessageProgress = 0;

                return strAutoDenomResult;

            } else if(fLastEntryAccepted) {

                if(nStatusMessageProgress % 10 > 8) {

                    fLastEntryAccepted = false;

                    nStatusMessageProgress = 0;

                }

                return _("PrivateSend request complete:") + " " + _("Your transaction was accepted into the pool!");

            } else {

                if(     nStatusMessageProgress % 70 <= 40) return strprintf(_("Submitted following entries to masternode: %u / %d"), nEntriesCount, GetMaxPoolTransactions());

                else if(nStatusMessageProgress % 70 <= 50) strSuffix = ".";

                else if(nStatusMessageProgress % 70 <= 60) strSuffix = "..";

                else if(nStatusMessageProgress % 70 <= 70) strSuffix = "...";

                return strprintf(_("Submitted to masternode, waiting for more entries ( %u / %d ) %s"), nEntriesCount, GetMaxPoolTransactions(), strSuffix);

            }

        case POOL_STATE_SIGNING:

            if(     nStatusMessageProgress % 70 <= 40) return _("Found enough users, signing ...");

            else if(nStatusMessageProgress % 70 <= 50) strSuffix = ".";

            else if(nStatusMessageProgress % 70 <= 60) strSuffix = "..";

            else if(nStatusMessageProgress % 70 <= 70) strSuffix = "...";

            return strprintf(_("Found enough users, signing ( waiting %s )"), strSuffix);

        case POOL_STATE_ERROR:

            return _("PrivateSend request incomplete:") + " " + strLastMessage + " " + _("Will retry...");

        case POOL_STATE_SUCCESS:

            return _("PrivateSend request complete:") + " " + strLastMessage;

       default:

            return strprintf(_("Unknown state: id = %u"), nState);

    }

}



//

// Check the mixing progress and send client updates if a Masternode

//

void CDarksendPool::CheckPool()

{

    if(fMasterNode) {

        LogPrint("privatesend", "CDarksendPool::CheckPool -- entries count %lu\n", GetEntriesCount());



        // If entries are full, create finalized transaction

        if(nState == POOL_STATE_ACCEPTING_ENTRIES && GetEntriesCount() >= GetMaxPoolTransactions()) {

            LogPrint("privatesend", "CDarksendPool::CheckPool -- FINALIZE TRANSACTIONS\n");

            CreateFinalTransaction();

            return;

        }



        // If we have all of the signatures, try to compile the transaction

        if(nState == POOL_STATE_SIGNING && IsSignaturesComplete()) {

            LogPrint("privatesend", "CDarksendPool::CheckPool -- SIGNING\n");

            CommitFinalTransaction();

            return;

        }

    }



    // reset if we're here for 10 seconds

    if((nState == POOL_STATE_ERROR || nState == POOL_STATE_SUCCESS) && GetTimeMillis() - nTimeLastSuccessfulStep >= 10000) {

        LogPrint("privatesend", "CDarksendPool::CheckPool -- timeout, RESETTING\n");

        UnlockCoins();

        SetNull();

    }

}



void CDarksendPool::CreateFinalTransaction()

{

    LogPrint("privatesend", "CDarksendPool::CreateFinalTransaction -- FINALIZE TRANSACTIONS\n");



    CMutableTransaction txNew;



    // make our new transaction

    for(int i = 0; i < GetEntriesCount(); i++) {

        BOOST_FOREACH(const CTxDSOut& txdsout, vecEntries[i].vecTxDSOut)

            txNew.vout.push_back(txdsout);



        BOOST_FOREACH(const CTxDSIn& txdsin, vecEntries[i].vecTxDSIn)

            txNew.vin.push_back(txdsin);

    }



    // BIP69 https://github.com/kristovatlas/bips/blob/master/bip-0069.mediawiki

    sort(txNew.vin.begin(), txNew.vin.end());

    sort(txNew.vout.begin(), txNew.vout.end());



    finalMutableTransaction = txNew;

    LogPrint("privatesend", "CDarksendPool::CreateFinalTransaction -- finalMutableTransaction=%s", txNew.ToString());



    // request signatures from clients

    RelayFinalTransaction(finalMutableTransaction);

    SetState(POOL_STATE_SIGNING);

}



void CDarksendPool::CommitFinalTransaction()

{

    if(!fMasterNode) return; // check and relay final tx only on masternode



    CTransaction finalTransaction = CTransaction(finalMutableTransaction);

    uint256 hashTx = finalTransaction.GetHash();



    LogPrint("privatesend", "CDarksendPool::CommitFinalTransaction -- finalTransaction=%s", finalTransaction.ToString());



    {

        // See if the transaction is valid

        TRY_LOCK(cs_main, lockMain);

        CValidationState validationState;

        mempool.PrioritiseTransaction(hashTx, hashTx.ToString(), 1000, 0.1*COIN);

        if(!lockMain || !AcceptToMemoryPool(mempool, validationState, finalTransaction, false, NULL, false, true, true))

        {

            LogPrintf("CDarksendPool::CommitFinalTransaction -- AcceptToMemoryPool() error: Transaction not valid\n");

            SetNull();

            // not much we can do in this case, just notify clients

            RelayCompletedTransaction(ERR_INVALID_TX);

            return;

        }

    }



    LogPrintf("CDarksendPool::CommitFinalTransaction -- CREATING DSTX\n");



    // create and sign masternode dstx transaction

    if(!mapDarksendBroadcastTxes.count(hashTx)) {

        CDarksendBroadcastTx dstx(finalTransaction, activeMasternode.vin, GetAdjustedTime());

        dstx.Sign();

        mapDarksendBroadcastTxes.insert(std::make_pair(hashTx, dstx));

    }



    LogPrintf("CDarksendPool::CommitFinalTransaction -- TRANSMITTING DSTX\n");



    CInv inv(MSG_DSTX, hashTx);

    RelayInv(inv);



    // Tell the clients it was successful

    RelayCompletedTransaction(MSG_SUCCESS);



    // Randomly charge clients

    ChargeRandomFees();



    // Reset

    LogPrint("privatesend", "CDarksendPool::CommitFinalTransaction -- COMPLETED -- RESETTING\n");

    SetNull();

}



//

// Charge clients a fee if they're abusive

//

// Why bother? PrivateSend uses collateral to ensure abuse to the process is kept to a minimum.

// The submission and signing stages are completely separate. In the cases where

// a client submits a transaction then refused to sign, there must be a cost. Otherwise they

// would be able to do this over and over again and bring the mixing to a hault.

//

// How does this work? Messages to Masternodes come in via NetMsgType::DSVIN, these require a valid collateral

// transaction for the client to be able to enter the pool. This transaction is kept by the Masternode

// until the transaction is either complete or fails.

//

void CDarksendPool::ChargeFees()

{

    if(!fMasterNode) return;



    //we don't need to charge collateral for every offence.

    if(GetRandInt(100) > 33) return;



    std::vector<CTransaction> vecOffendersCollaterals;



    if(nState == POOL_STATE_ACCEPTING_ENTRIES) {

        BOOST_FOREACH(const CTransaction& txCollateral, vecSessionCollaterals) {

            bool fFound = false;

            BOOST_FOREACH(const CDarkSendEntry& entry, vecEntries)

                if(entry.txCollateral == txCollateral)

                    fFound = true;



            // This queue entry didn't send us the promised transaction

            if(!fFound) {

                LogPrintf("CDarksendPool::ChargeFees -- found uncooperative node (didn't send transaction), found offence\n");

                vecOffendersCollaterals.push_back(txCollateral);

            }

        }

    }



    if(nState == POOL_STATE_SIGNING) {

        // who didn't sign?

        BOOST_FOREACH(const CDarkSendEntry entry, vecEntries) {

            BOOST_FOREACH(const CTxDSIn txdsin, entry.vecTxDSIn) {

                if(!txdsin.fHasSig) {

                    LogPrintf("CDarksendPool::ChargeFees -- found uncooperative node (didn't sign), found offence\n");

                    vecOffendersCollaterals.push_back(entry.txCollateral);

                }

            }

        }

    }



    // no offences found

    if(vecOffendersCollaterals.empty()) return;



    //mostly offending? Charge sometimes

    if((int)vecOffendersCollaterals.size() >= Params().PoolMaxTransactions() - 1 && GetRandInt(100) > 33) return;



    //everyone is an offender? That's not right

    if((int)vecOffendersCollaterals.size() >= Params().PoolMaxTransactions()) return;



    //charge one of the offenders randomly

    std::random_shuffle(vecOffendersCollaterals.begin(), vecOffendersCollaterals.end());



    if(nState == POOL_STATE_ACCEPTING_ENTRIES || nState == POOL_STATE_SIGNING) {

        LogPrintf("CDarksendPool::ChargeFees -- found uncooperative node (didn't %s transaction), charging fees: %s\n",

                (nState == POOL_STATE_SIGNING) ? "sign" : "send", vecOffendersCollaterals[0].ToString());



        LOCK(cs_main);



        CValidationState state;

        bool fMissingInputs;

        if(!AcceptToMemoryPool(mempool, state, vecOffendersCollaterals[0], false, &fMissingInputs, false, true)) {

            // should never really happen

            LogPrintf("CDarksendPool::ChargeFees -- ERROR: AcceptToMemoryPool failed!\n");

        } else {

            RelayTransaction(vecOffendersCollaterals[0]);

        }

    }

}



/*

    Charge the collateral randomly.

    Mixing is completely free, to pay miners we randomly pay the collateral of users.



    Collateral Fee Charges:



    Being that mixing has "no fees" we need to have some kind of cost associated

    with using it to stop abuse. Otherwise it could serve as an attack vector and

    allow endless transaction that would bloat Curium and make it unusable. To

    stop these kinds of attacks 1 in 10 successful transactions are charged. This

    adds up to a cost of 0.001CRU per transaction on average.

*/

void CDarksendPool::ChargeRandomFees()

{

    if(!fMasterNode) return;



    LOCK(cs_main);



    BOOST_FOREACH(const CTransaction& txCollateral, vecSessionCollaterals) {



        if(GetRandInt(100) > 10) return;



        LogPrintf("CDarksendPool::ChargeRandomFees -- charging random fees, txCollateral=%s", txCollateral.ToString());



        CValidationState state;

        bool fMissingInputs;

        if(!AcceptToMemoryPool(mempool, state, txCollateral, false, &fMissingInputs, false, true)) {

            // should never really happen

            LogPrintf("CDarksendPool::ChargeRandomFees -- ERROR: AcceptToMemoryPool failed!\n");

        } else {

            RelayTransaction(txCollateral);

        }

    }

}



//

// Check for various timeouts (queue objects, mixing, etc)

//

void CDarksendPool::CheckTimeout()

{

    {

        TRY_LOCK(cs_darksend, lockDS);

        if(!lockDS) return; // it's ok to fail here, we run this quite frequently



        // check mixing queue objects for timeouts

        std::vector<CDarksendQueue>::iterator it = vecDarksendQueue.begin();

        while(it != vecDarksendQueue.end()) {

            if((*it).IsExpired()) {

                LogPrint("privatesend", "CDarksendPool::CheckTimeout -- Removing expired queue (%s)\n", (*it).ToString());

                it = vecDarksendQueue.erase(it);

            } else ++it;

        }

    }



    if(!fEnablePrivateSend && !fMasterNode) return;



    // catching hanging sessions

    if(!fMasterNode) {

        switch(nState) {

            case POOL_STATE_ERROR:

                LogPrint("privatesend", "CDarksendPool::CheckTimeout -- Pool error -- Running CheckPool\n");

                CheckPool();

                break;

            case POOL_STATE_SUCCESS:

                LogPrint("privatesend", "CDarksendPool::CheckTimeout -- Pool success -- Running CheckPool\n");

                CheckPool();

                break;

            default:

                break;

        }

    }



    int nLagTime = fMasterNode ? 0 : 10000; // if we're the client, give the server a few extra seconds before resetting.

    int nTimeout = (nState == POOL_STATE_SIGNING) ? PRIVATESEND_SIGNING_TIMEOUT : PRIVATESEND_QUEUE_TIMEOUT;

    bool fTimeout = GetTimeMillis() - nTimeLastSuccessfulStep >= nTimeout*1000 + nLagTime;



    if(nState != POOL_STATE_IDLE && fTimeout) {

        LogPrint("privatesend", "CDarksendPool::CheckTimeout -- %s timed out (%ds) -- restting\n",

                (nState == POOL_STATE_SIGNING) ? "Signing" : "Session", nTimeout);

        ChargeFees();

        UnlockCoins();

        SetNull();

        SetState(POOL_STATE_ERROR);

        strLastMessage = _("Session timed out.");

    }

}



/*

    Check to see if we're ready for submissions from clients

    After receiving multiple dsa messages, the queue will switch to "accepting entries"

    which is the active state right before merging the transaction

*/

void CDarksendPool::CheckForCompleteQueue()

{

    if(!fEnablePrivateSend && !fMasterNode) return;



    if(nState == POOL_STATE_QUEUE && IsSessionReady()) {

        SetState(POOL_STATE_ACCEPTING_ENTRIES);



        CDarksendQueue dsq(nSessionDenom, activeMasternode.vin, GetTime(), true);

        LogPrint("privatesend", "CDarksendPool::CheckForCompleteQueue -- queue is ready, signing and relaying (%s)\n", dsq.ToString());

        dsq.Sign();

        dsq.Relay();

    }

}



// Check to make sure a given input matches an input in the pool and its scriptSig is valid

bool CDarksendPool::IsInputScriptSigValid(const CTxIn& txin)

{

    CMutableTransaction txNew;

    txNew.vin.clear();

    txNew.vout.clear();



    int i = 0;

    int nTxInIndex = -1;

    CScript sigPubKey = CScript();



    BOOST_FOREACH(CDarkSendEntry& entry, vecEntries) {



        BOOST_FOREACH(const CTxDSOut& txdsout, entry.vecTxDSOut)

            txNew.vout.push_back(txdsout);



        BOOST_FOREACH(const CTxDSIn& txdsin, entry.vecTxDSIn) {

            txNew.vin.push_back(txdsin);



            if(txdsin.prevout == txin.prevout) {

                nTxInIndex = i;

                sigPubKey = txdsin.prevPubKey;

            }

            i++;

        }

    }



    if(nTxInIndex >= 0) { //might have to do this one input at a time?

        txNew.vin[nTxInIndex].scriptSig = txin.scriptSig;

        LogPrint("privatesend", "CDarksendPool::IsInputScriptSigValid -- verifying scriptSig %s\n", ScriptToAsmStr(txin.scriptSig).substr(0,24));

        if(!VerifyScript(txNew.vin[nTxInIndex].scriptSig, sigPubKey, SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_STRICTENC, MutableTransactionSignatureChecker(&txNew, nTxInIndex))) {

            LogPrint("privatesend", "CDarksendPool::IsInputScriptSigValid -- VerifyScript() failed on input %d\n", nTxInIndex);

            return false;

        }

    } else {

        LogPrint("privatesend", "CDarksendPool::IsInputScriptSigValid -- Failed to find matching input in pool, %s\n", txin.ToString());

        return false;

    }



    LogPrint("privatesend", "CDarksendPool::IsInputScriptSigValid -- Successfully validated input and scriptSig\n");

    return true;

}



// check to make sure the collateral provided by the client is valid

bool CDarksendPool::IsCollateralValid(const CTransaction& txCollateral)

{

    if(txCollateral.vout.empty()) return false;

    if(txCollateral.nLockTime != 0) return false;



    CAmount nValueIn = 0;

    CAmount nValueOut = 0;

    bool fMissingTx = false;



    BOOST_FOREACH(const CTxOut txout, txCollateral.vout) {

        nValueOut += txout.nValue;



        if(!txout.scriptPubKey.IsNormalPaymentScript()) {

            LogPrintf ("CDarksendPool::IsCollateralValid -- Invalid Script, txCollateral=%s", txCollateral.ToString());

            return false;

        }

    }



    BOOST_FOREACH(const CTxIn txin, txCollateral.vin) {

        CTransaction txPrev;

        uint256 hash;

        if(GetTransaction(txin.prevout.hash, txPrev, Params().GetConsensus(), hash, true)) {

            if(txPrev.vout.size() > txin.prevout.n)

                nValueIn += txPrev.vout[txin.prevout.n].nValue;

        } else {

            fMissingTx = true;

        }

    }



    if(fMissingTx) {

        LogPrint("privatesend", "CDarksendPool::IsCollateralValid -- Unknown inputs in collateral transaction, txCollateral=%s", txCollateral.ToString());

        return false;

    }



    //collateral transactions are required to pay out PRIVATESEND_COLLATERAL as a fee to the miners

    if(nValueIn - nValueOut < PRIVATESEND_COLLATERAL) {

        LogPrint("privatesend", "CDarksendPool::IsCollateralValid -- did not include enough fees in transaction: fees: %d, txCollateral=%s", nValueOut - nValueIn, txCollateral.ToString());

        return false;

    }



    LogPrint("privatesend", "CDarksendPool::IsCollateralValid -- %s", txCollateral.ToString());



    {

        LOCK(cs_main);

        CValidationState validationState;

        if(!AcceptToMemoryPool(mempool, validationState, txCollateral, false, NULL, false, true, true)) {

            LogPrint("privatesend", "CDarksendPool::IsCollateralValid -- didn't pass AcceptToMemoryPool()\n");

            return false;

        }

    }



    return true;

}





//

// Add a clients transaction to the pool

//

bool CDarksendPool::AddEntry(const CDarkSendEntry& entryNew, PoolMessage& nMessageIDRet)

{

    if(!fMasterNode) return false;



    BOOST_FOREACH(CTxIn txin, entryNew.vecTxDSIn) {

        if(txin.prevout.IsNull()) {

            LogPrint("privatesend", "CDarksendPool::AddEntry -- input not valid!\n");

            nMessageIDRet = ERR_INVALID_INPUT;

            return false;

        }

    }



    if(!IsCollateralValid(entryNew.txCollateral)) {

        LogPrint("privatesend", "CDarksendPool::AddEntry -- collateral not valid!\n");

        nMessageIDRet = ERR_INVALID_COLLATERAL;

        return false;

    }



    if(GetEntriesCount() >= GetMaxPoolTransactions()) {

        LogPrint("privatesend", "CDarksendPool::AddEntry -- entries is full!\n");

        nMessageIDRet = ERR_ENTRIES_FULL;

        return false;

    }



    BOOST_FOREACH(CTxIn txin, entryNew.vecTxDSIn) {

        LogPrint("privatesend", "looking for txin -- %s\n", txin.ToString());

        BOOST_FOREACH(const CDarkSendEntry& entry, vecEntries) {

            BOOST_FOREACH(const CTxDSIn& txdsin, entry.vecTxDSIn) {

                if(txdsin.prevout == txin.prevout) {

                    LogPrint("privatesend", "CDarksendPool::AddEntry -- found in txin\n");

                    nMessageIDRet = ERR_ALREADY_HAVE;

                    return false;

                }

            }

        }

    }



    vecEntries.push_back(entryNew);



    LogPrint("privatesend", "CDarksendPool::AddEntry -- adding entry\n");

    nMessageIDRet = MSG_ENTRIES_ADDED;

    nTimeLastSuccessfulStep = GetTimeMillis();



    return true;

}



bool CDarksendPool::AddScriptSig(const CTxIn& txinNew)

{

    LogPrint("privatesend", "CDarksendPool::AddScriptSig -- scriptSig=%s\n", ScriptToAsmStr(txinNew.scriptSig).substr(0,24));



    BOOST_FOREACH(const CDarkSendEntry& entry, vecEntries) {

        BOOST_FOREACH(const CTxDSIn& txdsin, entry.vecTxDSIn) {

            if(txdsin.scriptSig == txinNew.scriptSig) {

                LogPrint("privatesend", "CDarksendPool::AddScriptSig -- already exists\n");

                return false;

            }

        }

    }



    if(!IsInputScriptSigValid(txinNew)) {

        LogPrint("privatesend", "CDarksendPool::AddScriptSig -- Invalid scriptSig\n");

        return false;

    }



    LogPrint("privatesend", "CDarksendPool::AddScriptSig -- scriptSig=%s new\n", ScriptToAsmStr(txinNew.scriptSig).substr(0,24));



    BOOST_FOREACH(CTxIn& txin, finalMutableTransaction.vin) {

        if(txinNew.prevout == txin.prevout && txin.nSequence == txinNew.nSequence) {

            txin.scriptSig = txinNew.scriptSig;

            txin.prevPubKey = txinNew.prevPubKey;

            LogPrint("privatesend", "CDarksendPool::AddScriptSig -- adding to finalMutableTransaction, scriptSig=%s\n", ScriptToAsmStr(txinNew.scriptSig).substr(0,24));

        }

    }

    for(int i = 0; i < GetEntriesCount(); i++) {

        if(vecEntries[i].AddScriptSig(txinNew)) {

            LogPrint("privatesend", "CDarksendPool::AddScriptSig -- adding to entries, scriptSig=%s\n", ScriptToAsmStr(txinNew.scriptSig).substr(0,24));

            return true;

        }

    }



    LogPrintf("CDarksendPool::AddScriptSig -- Couldn't set sig!\n" );

    return false;

}



// Check to make sure everything is signed

bool CDarksendPool::IsSignaturesComplete()

{

    BOOST_FOREACH(const CDarkSendEntry& entry, vecEntries)

        BOOST_FOREACH(const CTxDSIn& txdsin, entry.vecTxDSIn)

            if(!txdsin.fHasSig) return false;



    return true;

}



//

// Execute a mixing denomination via a Masternode.

// This is only ran from clients

//

bool CDarksendPool::SendDenominate(const std::vector<CTxIn>& vecTxIn, const std::vector<CTxOut>& vecTxOut)

{

    if(fMasterNode) {

        LogPrintf("CDarksendPool::SendDenominate -- PrivateSend from a Masternode is not supported currently.\n");

        return false;

    }



    if(txMyCollateral == CMutableTransaction()) {

        LogPrintf("CDarksendPool:SendDenominate -- PrivateSend collateral not set\n");

        return false;

    }



    // lock the funds we're going to use

    BOOST_FOREACH(CTxIn txin, txMyCollateral.vin)

        vecOutPointLocked.push_back(txin.prevout);



    BOOST_FOREACH(CTxIn txin, vecTxIn)

        vecOutPointLocked.push_back(txin.prevout);



    // we should already be connected to a Masternode

    if(!nSessionID) {

        LogPrintf("CDarksendPool::SendDenominate -- No Masternode has been selected yet.\n");

        UnlockCoins();

        SetNull();

        return false;

    }



    if(!CheckDiskSpace()) {

        UnlockCoins();

        SetNull();

        fEnablePrivateSend = false;

        LogPrintf("CDarksendPool::SendDenominate -- Not enough disk space, disabling PrivateSend.\n");

        return false;

    }



    SetState(POOL_STATE_ACCEPTING_ENTRIES);

    strLastMessage = "";



    LogPrintf("CDarksendPool::SendDenominate -- Added transaction to pool.\n");



    //check it against the memory pool to make sure it's valid

    {

        CValidationState validationState;

        CMutableTransaction tx;



        BOOST_FOREACH(const CTxIn& txin, vecTxIn) {

            LogPrint("privatesend", "CDarksendPool::SendDenominate -- txin=%s\n", txin.ToString());

            tx.vin.push_back(txin);

        }



        BOOST_FOREACH(const CTxOut& txout, vecTxOut) {

            LogPrint("privatesend", "CDarksendPool::SendDenominate -- txout=%s\n", txout.ToString());

            tx.vout.push_back(txout);

        }



        LogPrintf("CDarksendPool::SendDenominate -- Submitting partial tx %s", tx.ToString());



        mempool.PrioritiseTransaction(tx.GetHash(), tx.GetHash().ToString(), 1000, 0.1*COIN);

        TRY_LOCK(cs_main, lockMain);

        if(!lockMain || !AcceptToMemoryPool(mempool, validationState, CTransaction(tx), false, NULL, false, true, true)) {

            LogPrintf("CDarksendPool::SendDenominate -- AcceptToMemoryPool() failed! tx=%s", tx.ToString());

            UnlockCoins();

            SetNull();

            return false;

        }

    }



    // store our entry for later use

    CDarkSendEntry entry(vecTxIn, vecTxOut, txMyCollateral);

    vecEntries.push_back(entry);

    RelayIn(entry);

    nTimeLastSuccessfulStep = GetTimeMillis();



    return true;

}



// Incoming message from Masternode updating the progress of mixing

bool CDarksendPool::CheckPoolStateUpdate(PoolState nStateNew, int nEntriesCountNew, PoolStatusUpdate nStatusUpdate, PoolMessage nMessageID, int nSessionIDNew)

{

    if(fMasterNode) return false;



    // do not update state when mixing client state is one of these

    if(nState == POOL_STATE_IDLE || nState == POOL_STATE_ERROR || nState == POOL_STATE_SUCCESS) return false;



    strAutoDenomResult = _("Masternode:") + " " + GetMessageByID(nMessageID);



    // if rejected at any state

    if(nStatusUpdate == STATUS_REJECTED) {

        LogPrintf("CDarksendPool::CheckPoolStateUpdate -- entry is rejected by Masternode\n");

        UnlockCoins();

        SetNull();

        SetState(POOL_STATE_ERROR);

        strLastMessage = GetMessageByID(nMessageID);

        return true;

    }



    if(nStatusUpdate == STATUS_ACCEPTED && nState == nStateNew) {

        if(nStateNew == POOL_STATE_QUEUE && nSessionID == 0 && nSessionIDNew != 0) {

            // new session id should be set only in POOL_STATE_QUEUE state

            nSessionID = nSessionIDNew;

            nTimeLastSuccessfulStep = GetTimeMillis();

            LogPrintf("CDarksendPool::CheckPoolStateUpdate -- set nSessionID to %d\n", nSessionID);

            return true;

        }

        else if(nStateNew == POOL_STATE_ACCEPTING_ENTRIES && nEntriesCount != nEntriesCountNew) {

            nEntriesCount = nEntriesCountNew;

            nTimeLastSuccessfulStep = GetTimeMillis();

            fLastEntryAccepted = true;

            LogPrintf("CDarksendPool::CheckPoolStateUpdate -- new entry accepted!\n");

            return true;

        }

    }



    // only situations above are allowed, fail in any other case

    return false;

}



//

// After we receive the finalized transaction from the Masternode, we must

// check it to make sure it's what we want, then sign it if we agree.

// If we refuse to sign, it's possible we'll be charged collateral

//

bool CDarksendPool::SignFinalTransaction(const CTransaction& finalTransactionNew, CNode* pnode)

{

    if(fMasterNode || pnode == NULL) return false;



    finalMutableTransaction = finalTransactionNew;

    LogPrintf("CDarksendPool::SignFinalTransaction -- finalMutableTransaction=%s", finalMutableTransaction.ToString());



    std::vector<CTxIn> sigs;



    //make sure my inputs/outputs are present, otherwise refuse to sign

    BOOST_FOREACH(const CDarkSendEntry entry, vecEntries) {

        BOOST_FOREACH(const CTxDSIn txdsin, entry.vecTxDSIn) {

            /* Sign my transaction and all outputs */

            int nMyInputIndex = -1;

            CScript prevPubKey = CScript();

            CTxIn txin = CTxIn();



            for(unsigned int i = 0; i < finalMutableTransaction.vin.size(); i++) {

                if(finalMutableTransaction.vin[i] == txdsin) {

                    nMyInputIndex = i;

                    prevPubKey = txdsin.prevPubKey;

                    txin = txdsin;

                }

            }



            if(nMyInputIndex >= 0) { //might have to do this one input at a time?

                int nFoundOutputsCount = 0;

                CAmount nValue1 = 0;

                CAmount nValue2 = 0;



                for(unsigned int i = 0; i < finalMutableTransaction.vout.size(); i++) {

                    BOOST_FOREACH(const CTxOut& txout, entry.vecTxDSOut) {

                        if(finalMutableTransaction.vout[i] == txout) {

                            nFoundOutputsCount++;

                            nValue1 += finalMutableTransaction.vout[i].nValue;

                        }

                    }

                }



                BOOST_FOREACH(const CTxOut txout, entry.vecTxDSOut)

                    nValue2 += txout.nValue;



                int nTargetOuputsCount = entry.vecTxDSOut.size();

                if(nFoundOutputsCount < nTargetOuputsCount || nValue1 != nValue2) {

                    // in this case, something went wrong and we'll refuse to sign. It's possible we'll be charged collateral. But that's

                    // better then signing if the transaction doesn't look like what we wanted.

                    LogPrintf("CDarksendPool::SignFinalTransaction -- My entries are not correct! Refusing to sign: nFoundOutputsCount: %d, nTargetOuputsCount: %d\n", nFoundOutputsCount, nTargetOuputsCount);

                    UnlockCoins();

                    SetNull();



                    return false;

                }



                const CKeyStore& keystore = *pwalletMain;



                LogPrint("privatesend", "CDarksendPool::SignFinalTransaction -- Signing my input %i\n", nMyInputIndex);

                if(!SignSignature(keystore, prevPubKey, finalMutableTransaction, nMyInputIndex, int(SIGHASH_ALL|SIGHASH_ANYONECANPAY))) { // changes scriptSig

                    LogPrint("privatesend", "CDarksendPool::SignFinalTransaction -- Unable to sign my own transaction!\n");

                    // not sure what to do here, it will timeout...?

                }



                sigs.push_back(finalMutableTransaction.vin[nMyInputIndex]);

                LogPrint("privatesend", "CDarksendPool::SignFinalTransaction -- nMyInputIndex: %d, sigs.size(): %d, scriptSig=%s\n", nMyInputIndex, (int)sigs.size(), ScriptToAsmStr(finalMutableTransaction.vin[nMyInputIndex].scriptSig));

            }

        }

    }



    if(sigs.empty()) {

        LogPrintf("CDarksendPool::SignFinalTransaction -- can't sign anything!\n");

        UnlockCoins();

        SetNull();



        return false;

    }



    // push all of our signatures to the Masternode

    LogPrintf("CDarksendPool::SignFinalTransaction -- pushing sigs to the masternode, finalMutableTransaction=%s", finalMutableTransaction.ToString());

    pnode->PushMessage(NetMsgType::DSSIGNFINALTX, sigs);

    SetState(POOL_STATE_SIGNING);

    nTimeLastSuccessfulStep = GetTimeMillis();



    return true;

}



void CDarksendPool::NewBlock()

{

    static int64_t nTimeNewBlockReceived = 0;



    //we we're processing lots of blocks, we'll just leave

    if(GetTime() - nTimeNewBlockReceived < 10) return;

    nTimeNewBlockReceived = GetTime();

    LogPrint("privatesend", "CDarksendPool::NewBlock\n");



    CheckTimeout();

}



// mixing transaction was completed (failed or successful)

void CDarksendPool::CompletedTransaction(PoolMessage nMessageID)

{

    if(fMasterNode) return;



    if(nMessageID == MSG_SUCCESS) {

        LogPrintf("CompletedTransaction -- success\n");

        nCachedLastSuccessBlock = pCurrentBlockIndex->nHeight;

    } else {

        LogPrintf("CompletedTransaction -- error\n");

    }

    UnlockCoins();

    SetNull();

    strLastMessage = GetMessageByID(nMessageID);

}



//

// Passively run mixing in the background to anonymize funds based on the given configuration.

//

bool CDarksendPool::DoAutomaticDenominating(bool fDryRun)

{

    if(!fEnablePrivateSend || fMasterNode || !pCurrentBlockIndex) return false;

    if(!pwalletMain || pwalletMain->IsLocked(true)) return false;

    if(nState != POOL_STATE_IDLE) return false;



    if(!masternodeSync.IsMasternodeListSynced()) {

        strAutoDenomResult = _("Can't mix while sync in progress.");

        return false;

    }



    switch(nWalletBackups) {

        case 0:

            LogPrint("privatesend", "CDarksendPool::DoAutomaticDenominating -- Automatic backups disabled, no mixing available.\n");

            strAutoDenomResult = _("Automatic backups disabled") + ", " + _("no mixing available.");

            fEnablePrivateSend = false; // stop mixing

            pwalletMain->nKeysLeftSinceAutoBackup = 0; // no backup, no "keys since last backup"

            return false;

        case -1:

            // Automatic backup failed, nothing else we can do until user fixes the issue manually.

            // There is no way to bring user attention in daemon mode so we just update status and

            // keep spaming if debug is on.

            LogPrint("privatesend", "CDarksendPool::DoAutomaticDenominating -- ERROR! Failed to create automatic backup.\n");

            strAutoDenomResult = _("ERROR! Failed to create automatic backup") + ", " + _("see debug.log for details.");

            return false;

        case -2:

            // We were able to create automatic backup but keypool was not replenished because wallet is locked.

            // There is no way to bring user attention in daemon mode so we just update status and

            // keep spaming if debug is on.

            LogPrint("privatesend", "CDarksendPool::DoAutomaticDenominating -- WARNING! Failed to create replenish keypool, please unlock your wallet to do so.\n");

            strAutoDenomResult = _("WARNING! Failed to replenish keypool, please unlock your wallet to do so.") + ", " + _("see debug.log for details.");

            return false;

    }



    if(pwalletMain->nKeysLeftSinceAutoBackup < PRIVATESEND_KEYS_THRESHOLD_STOP) {

        // We should never get here via mixing itself but probably smth else is still actively using keypool

        LogPrint("privatesend", "CDarksendPool::DoAutomaticDenominating -- Very low number of keys left: %d, no mixing available.\n", pwalletMain->nKeysLeftSinceAutoBackup);

        strAutoDenomResult = strprintf(_("Very low number of keys left: %d") + ", " + _("no mixing available."), pwalletMain->nKeysLeftSinceAutoBackup);

        // It's getting really dangerous, stop mixing

        fEnablePrivateSend = false;

        return false;

    } else if(pwalletMain->nKeysLeftSinceAutoBackup < PRIVATESEND_KEYS_THRESHOLD_WARNING) {

        // Low number of keys left but it's still more or less safe to continue

        LogPrint("privatesend", "CDarksendPool::DoAutomaticDenominating -- Very low number of keys left: %d\n", pwalletMain->nKeysLeftSinceAutoBackup);

        strAutoDenomResult = strprintf(_("Very low number of keys left: %d"), pwalletMain->nKeysLeftSinceAutoBackup);



        if(fCreateAutoBackups) {

            LogPrint("privatesend", "CDarksendPool::DoAutomaticDenominating -- Trying to create new backup.\n");

            std::string warningString;

            std::string errorString;



            if(!AutoBackupWallet(pwalletMain, "", warningString, errorString)) {

                if(!warningString.empty()) {

                    // There were some issues saving backup but yet more or less safe to continue

                    LogPrintf("CDarksendPool::DoAutomaticDenominating -- WARNING! Something went wrong on automatic backup: %s\n", warningString);

                }

                if(!errorString.empty()) {

                    // Things are really broken

                    LogPrintf("CDarksendPool::DoAutomaticDenominating -- ERROR! Failed to create automatic backup: %s\n", errorString);

                    strAutoDenomResult = strprintf(_("ERROR! Failed to create automatic backup") + ": %s", errorString);

                    return false;

                }

            }

        } else {

            // Wait for someone else (e.g. GUI action) to create automatic backup for us

            return false;

        }

    }



    LogPrint("privatesend", "CDarksendPool::DoAutomaticDenominating -- Keys left since latest backup: %d\n", pwalletMain->nKeysLeftSinceAutoBackup);



    if(GetEntriesCount() > 0) {

        strAutoDenomResult = _("Mixing in progress...");

        return false;

    }



    TRY_LOCK(cs_darksend, lockDS);

    if(!lockDS) {

        strAutoDenomResult = _("Lock is already in place.");

        return false;

    }



    if(!fDryRun && pwalletMain->IsLocked(true)) {

        strAutoDenomResult = _("Wallet is locked.");

        return false;

    }



    if(!fPrivateSendMultiSession && pCurrentBlockIndex->nHeight - nCachedLastSuccessBlock < nMinBlockSpacing) {

        LogPrintf("CDarksendPool::DoAutomaticDenominating -- Last successful PrivateSend action was too recent\n");

        strAutoDenomResult = _("Last successful PrivateSend action was too recent.");

        return false;

    }



    if(mnodeman.size() == 0) {

        LogPrint("privatesend", "CDarksendPool::DoAutomaticDenominating -- No Masternodes detected\n");

        strAutoDenomResult = _("No Masternodes detected.");

        return false;

    }



    // ** find the coins we'll use

    std::vector<CTxIn> vecTxIn;

    CAmount nValueMin = CENT;

    CAmount nValueIn = 0;



    CAmount nOnlyDenominatedBalance;

    CAmount nBalanceNeedsDenominated;



    CAmount nLowestDenom = vecPrivateSendDenominations.back();

    // if there are no confirmed DS collateral inputs yet

    if(!pwalletMain->HasCollateralInputs()) {

        // should have some additional amount for them

        nLowestDenom += PRIVATESEND_COLLATERAL*4;

    }



    CAmount nBalanceNeedsAnonymized = pwalletMain->GetNeedsToBeAnonymizedBalance(nLowestDenom);



    // anonymizable balance is way too small

    if(nBalanceNeedsAnonymized < nLowestDenom) {

        LogPrintf("CDarksendPool::DoAutomaticDenominating -- Not enough funds to anonymize\n");

        strAutoDenomResult = _("Not enough funds to anonymize.");

        return false;

    }



    LogPrint("privatesend", "CDarksendPool::DoAutomaticDenominating -- nLowestDenom: %f, nBalanceNeedsAnonymized: %f\n", (float)nLowestDenom/COIN, (float)nBalanceNeedsAnonymized/COIN);



    // select coins that should be given to the pool

    if(!pwalletMain->SelectCoinsDark(nValueMin, nBalanceNeedsAnonymized, vecTxIn, nValueIn, 0, nPrivateSendRounds))

    {

        if(pwalletMain->SelectCoinsDark(nValueMin, 9999999*COIN, vecTxIn, nValueIn, -2, 0))

        {

            nOnlyDenominatedBalance = pwalletMain->GetDenominatedBalance(true) + pwalletMain->GetDenominatedBalance() - pwalletMain->GetAnonymizedBalance();

            nBalanceNeedsDenominated = nBalanceNeedsAnonymized - nOnlyDenominatedBalance;



            if(nBalanceNeedsDenominated > nValueIn) nBalanceNeedsDenominated = nValueIn;



            LogPrint("privatesend", "CDarksendPool::DoAutomaticDenominating -- `SelectCoinsDark` (%f - (%f + %f - %f = %f) ) = %f\n",

                            (float)nBalanceNeedsAnonymized/COIN,

                            (float)pwalletMain->GetDenominatedBalance(true)/COIN,

                            (float)pwalletMain->GetDenominatedBalance()/COIN,

                            (float)pwalletMain->GetAnonymizedBalance()/COIN,

                            (float)nOnlyDenominatedBalance/COIN,

                            (float)nBalanceNeedsDenominated/COIN);



            if(nBalanceNeedsDenominated < nLowestDenom) { // most likely we are just waiting for denoms to confirm

                LogPrintf("CDarksendPool::DoAutomaticDenominating -- No funds detected in need of denominating\n");

                strAutoDenomResult = _("No funds detected in need of denominating.");

                return false;

            }

            if(!fDryRun) return CreateDenominated();



            return true;

        } else {

            LogPrintf("CDarksendPool::DoAutomaticDenominating -- Can't denominate (no compatible inputs left)\n");

            strAutoDenomResult = _("Can't denominate: no compatible inputs left.");

            return false;

        }

    }



    if(fDryRun) return true;



    nOnlyDenominatedBalance = pwalletMain->GetDenominatedBalance(true) + pwalletMain->GetDenominatedBalance() - pwalletMain->GetAnonymizedBalance();

    nBalanceNeedsDenominated = nBalanceNeedsAnonymized - nOnlyDenominatedBalance;

    LogPrint("privatesend", "CDarksendPool::DoAutomaticDenominating -- 'nBalanceNeedsDenominated > 0' (%f - (%f + %f - %f = %f) ) = %f\n",

                    (float)nBalanceNeedsAnonymized/COIN,

                    (float)pwalletMain->GetDenominatedBalance(true)/COIN,

                    (float)pwalletMain->GetDenominatedBalance()/COIN,

                    (float)pwalletMain->GetAnonymizedBalance()/COIN,

                    (float)nOnlyDenominatedBalance/COIN,

                    (float)nBalanceNeedsDenominated/COIN);



    //check if we have should create more denominated inputs

    if(nBalanceNeedsDenominated > 0) return CreateDenominated();



    //check if we have the collateral sized inputs

    if(!pwalletMain->HasCollateralInputs())

        return !pwalletMain->HasCollateralInputs(false) && MakeCollateralAmounts();



    if(nSessionID) {

        strAutoDenomResult = _("Mixing in progress...");

        return false;

    }



    // Initial phase, find a Masternode

    // Clean if there is anything left from previous session

    UnlockCoins();

    SetNull();



    if(!fPrivateSendMultiSession && pwalletMain->GetDenominatedBalance(true) > 0) { //get denominated unconfirmed inputs

        LogPrintf("CDarksendPool::DoAutomaticDenominating -- Found unconfirmed denominated outputs, will wait till they confirm to continue.\n");

        strAutoDenomResult = _("Found unconfirmed denominated outputs, will wait till they confirm to continue.");

        return false;

    }



    //check our collateral and create new if needed

    std::string strReason;

    if(txMyCollateral == CMutableTransaction()) {

        if(!pwalletMain->CreateCollateralTransaction(txMyCollateral, strReason)) {

            LogPrintf("CDarksendPool::DoAutomaticDenominating -- create collateral error:%s\n", strReason);

            return false;

        }

    } else {

        if(!IsCollateralValid(txMyCollateral)) {

            LogPrintf("CDarksendPool::DoAutomaticDenominating -- invalid collateral, recreating...\n");

            if(!pwalletMain->CreateCollateralTransaction(txMyCollateral, strReason)) {

                LogPrintf("CDarksendPool::DoAutomaticDenominating -- create collateral error: %s\n", strReason);

                return false;

            }

        }

    }



    int nMnCountEnabled = mnodeman.CountEnabled(MIN_PRIVATESEND_PEER_PROTO_VERSION);



    // If we've used 90% of the Masternode list then drop the oldest first ~30%

    int nThreshold_high = nMnCountEnabled * 0.9;

    int nThreshold_low = nThreshold_high * 0.7;

    LogPrint("privatesend", "Checking vecMasternodesUsed: size: %d, threshold: %d\n", (int)vecMasternodesUsed.size(), nThreshold_high);



    if((int)vecMasternodesUsed.size() > nThreshold_high) {

        vecMasternodesUsed.erase(vecMasternodesUsed.begin(), vecMasternodesUsed.begin() + vecMasternodesUsed.size() - nThreshold_low);

        LogPrint("privatesend", "  vecMasternodesUsed: new size: %d, threshold: %d\n", (int)vecMasternodesUsed.size(), nThreshold_high);

    }



    bool fUseQueue = GetRandInt(100) > 33;

    // don't use the queues all of the time for mixing unless we are a liquidity provider

    if(nLiquidityProvider || fUseQueue) {



        // Look through the queues and see if anything matches

        BOOST_FOREACH(CDarksendQueue& dsq, vecDarksendQueue) {

            // only try each queue once

            if(dsq.fTried) continue;

            dsq.fTried = true;



            if(dsq.IsExpired()) continue;



            CMasternode* pmn = mnodeman.Find(dsq.vin);

            if(pmn == NULL) {

                LogPrintf("CDarksendPool::DoAutomaticDenominating -- dsq masternode is not in masternode list, masternode=%s\n", dsq.vin.prevout.ToStringShort());

                continue;

            }



            if(pmn->nProtocolVersion < MIN_PRIVATESEND_PEER_PROTO_VERSION) continue;



            // incompatible denom

            if(dsq.nDenom >= (1 << vecPrivateSendDenominations.size())) continue;



            // mixing rate limit i.e. nLastDsq check should already pass in DSQUEUE ProcessMessage

            // in order for dsq to get into vecDarksendQueue, so we should be safe to mix already,

            // no need for additional verification here



            LogPrint("privatesend", "CDarksendPool::DoAutomaticDenominating -- found valid queue: %s\n", dsq.ToString());



            std::vector<CTxIn> vecTxInTmp;

            std::vector<COutput> vCoinsTmp;

            // Try to match their denominations if possible

            if(!pwalletMain->SelectCoinsByDenominations(dsq.nDenom, nValueMin, nBalanceNeedsAnonymized, vecTxInTmp, vCoinsTmp, nValueIn, 0, nPrivateSendRounds)) {

                LogPrintf("CDarksendPool::DoAutomaticDenominating -- Couldn't match denominations %d (%s)\n", dsq.nDenom, GetDenominationsToString(dsq.nDenom));

                continue;

            }



            vecMasternodesUsed.push_back(dsq.vin);



            LogPrintf("CDarksendPool::DoAutomaticDenominating -- attempt to connect to masternode from queue, addr=%s\n", pmn->addr.ToString());

            // connect to Masternode and submit the queue request

            CNode* pnode = ConnectNode((CAddress)pmn->addr, NULL, true);

            if(pnode) {

                pSubmittedToMasternode = pmn;

                nSessionDenom = dsq.nDenom;



                pnode->PushMessage(NetMsgType::DSACCEPT, nSessionDenom, txMyCollateral);

                LogPrintf("CDarksendPool::DoAutomaticDenominating -- connected (from queue), sending DSACCEPT: nSessionDenom: %d (%s), addr=%s\n",

                        nSessionDenom, GetDenominationsToString(nSessionDenom), pnode->addr.ToString());

                strAutoDenomResult = _("Mixing in progress...");

                SetState(POOL_STATE_QUEUE);

                nTimeLastSuccessfulStep = GetTimeMillis();

                return true;

            } else {

                LogPrintf("CDarksendPool::DoAutomaticDenominating -- can't connect, addr=%s\n", pmn->addr.ToString());

                strAutoDenomResult = _("Error connecting to Masternode.");

                continue;

            }

        }

    }



    // do not initiate queue if we are a liquidity provider to avoid useless inter-mixing

    if(nLiquidityProvider) return false;



    int nTries = 0;



    // otherwise, try one randomly

    while(nTries < 10) {

        CMasternode* pmn = mnodeman.FindRandomNotInVec(vecMasternodesUsed, MIN_PRIVATESEND_PEER_PROTO_VERSION);

        if(pmn == NULL) {

            LogPrintf("CDarksendPool::DoAutomaticDenominating -- Can't find random masternode!\n");

            strAutoDenomResult = _("Can't find random Masternode.");

            return false;

        }

        vecMasternodesUsed.push_back(pmn->vin);



        if(pmn->nLastDsq != 0 && pmn->nLastDsq + nMnCountEnabled/5 > mnodeman.nDsqCount) {

            LogPrintf("CDarksendPool::DoAutomaticDenominating -- Too early to mix on this masternode!"

                        " masternode=%s  addr=%s  nLastDsq=%d  CountEnabled/5=%d  nDsqCount=%d\n",

                        pmn->vin.prevout.ToStringShort(), pmn->addr.ToString(), pmn->nLastDsq,

                        nMnCountEnabled/5, mnodeman.nDsqCount);

            nTries++;

            continue;

        }



        LogPrintf("CDarksendPool::DoAutomaticDenominating -- attempt %d connection to Masternode %s\n", nTries, pmn->addr.ToString());

        CNode* pnode = ConnectNode((CAddress)pmn->addr, NULL, true);

        if(pnode) {

            LogPrintf("CDarksendPool::DoAutomaticDenominating -- connected, addr=%s\n", pmn->addr.ToString());

            pSubmittedToMasternode = pmn;



            std::vector<CAmount> vecAmounts;

            pwalletMain->ConvertList(vecTxIn, vecAmounts);

            // try to get a single random denom out of vecAmounts

            while(nSessionDenom == 0) {

                nSessionDenom = GetDenominationsByAmounts(vecAmounts);

            }



            pnode->PushMessage(NetMsgType::DSACCEPT, nSessionDenom, txMyCollateral);

            LogPrintf("CDarksendPool::DoAutomaticDenominating -- connected, sending DSACCEPT, nSessionDenom: %d (%s)\n",

                    nSessionDenom, GetDenominationsToString(nSessionDenom));

            strAutoDenomResult = _("Mixing in progress...");

            SetState(POOL_STATE_QUEUE);

            nTimeLastSuccessfulStep = GetTimeMillis();

            return true;

        } else {

            LogPrintf("CDarksendPool::DoAutomaticDenominating -- can't connect, addr=%s\n", pmn->addr.ToString());

            nTries++;

            continue;

        }

    }



    strAutoDenomResult = _("No compatible Masternode found.");

    return false;

}



bool CDarksendPool::SubmitDenominate()

{

    std::string strError;

    std::vector<CTxIn> vecTxInRet;

    std::vector<CTxOut> vecTxOutRet;



    // Submit transaction to the pool if we get here

    // Try to use only inputs with the same number of rounds starting from lowest number of rounds possible

    for(int i = 0; i < nPrivateSendRounds; i++) {

        if(PrepareDenominate(i, i+1, strError, vecTxInRet, vecTxOutRet)) {

            LogPrintf("CDarksendPool::SubmitDenominate -- Running PrivateSend denominate for %d rounds, success\n", i);

            return SendDenominate(vecTxInRet, vecTxOutRet);

        }

        LogPrintf("CDarksendPool::SubmitDenominate -- Running PrivateSend denominate for %d rounds, error: %s\n", i, strError);

    }



    // We failed? That's strange but let's just make final attempt and try to mix everything

    if(PrepareDenominate(0, nPrivateSendRounds, strError, vecTxInRet, vecTxOutRet)) {

        LogPrintf("CDarksendPool::SubmitDenominate -- Running PrivateSend denominate for all rounds, success\n");

        return SendDenominate(vecTxInRet, vecTxOutRet);

    }



    // Should never actually get here but just in case

    LogPrintf("CDarksendPool::SubmitDenominate -- Running PrivateSend denominate for all rounds, error: %s\n", strError);

    strAutoDenomResult = strError;

    return false;

}



bool CDarksendPool::PrepareDenominate(int nMinRounds, int nMaxRounds, std::string& strErrorRet, std::vector<CTxIn>& vecTxInRet, std::vector<CTxOut>& vecTxOutRet)

{

    if (pwalletMain->IsLocked(true)) {

        strErrorRet = "Wallet locked, unable to create transaction!";

        return false;

    }



    if (GetEntriesCount() > 0) {

        strErrorRet = "Already have pending entries in the PrivateSend pool";

        return false;

    }



    // make sure returning vectors are empty before filling them up

    vecTxInRet.clear();

    vecTxOutRet.clear();



    // ** find the coins we'll use

    std::vector<CTxIn> vecTxIn;

    std::vector<COutput> vCoins;

    CAmount nValueIn = 0;

    CReserveKey reservekey(pwalletMain);



    /*

        Select the coins we'll use



        if nMinRounds >= 0 it means only denominated inputs are going in and coming out

    */

    bool fSelected = pwalletMain->SelectCoinsByDenominations(nSessionDenom, vecPrivateSendDenominations.back(), PRIVATESEND_POOL_MAX, vecTxIn, vCoins, nValueIn, nMinRounds, nMaxRounds);

    if (nMinRounds >= 0 && !fSelected) {

        strErrorRet = "Can't select current denominated inputs";

        return false;

    }



    LogPrintf("CDarksendPool::PrepareDenominate -- max value: %f\n", (double)nValueIn/COIN);



    {

        LOCK(pwalletMain->cs_wallet);

        BOOST_FOREACH(CTxIn txin, vecTxIn) {

            pwalletMain->LockCoin(txin.prevout);

        }

    }



    CAmount nValueLeft = nValueIn;



    // Try to add every needed denomination, repeat up to 5-9 times.

    // NOTE: No need to randomize order of inputs because they were

    // initially shuffled in CWallet::SelectCoinsByDenominations already.

    int nStep = 0;

    int nStepsMax = 5 + GetRandInt(5);

    std::vector<int> vecBits;

    if (!GetDenominationsBits(nSessionDenom, vecBits)) {

        strErrorRet = "Incorrect session denom";

        return false;

    }



    while (nStep < nStepsMax) {

        BOOST_FOREACH(int nBit, vecBits) {

            CAmount nValueDenom = vecPrivateSendDenominations[nBit];

            if (nValueLeft - nValueDenom < 0) continue;



            // Note: this relies on a fact that both vectors MUST have same size

            std::vector<CTxIn>::iterator it = vecTxIn.begin();

            std::vector<COutput>::iterator it2 = vCoins.begin();

            while (it2 != vCoins.end()) {

                // we have matching inputs

                if ((*it2).tx->vout[(*it2).i].nValue == nValueDenom) {

                    // add new input in resulting vector

                    vecTxInRet.push_back(*it);

                    // remove corresponting items from initial vectors

                    vecTxIn.erase(it);

                    vCoins.erase(it2);



                    CScript scriptChange;

                    CPubKey vchPubKey;

                    // use a unique change address

                    assert(reservekey.GetReservedKey(vchPubKey)); // should never fail, as we just unlocked

                    scriptChange = GetScriptForDestination(vchPubKey.GetID());

                    reservekey.KeepKey();



                    // add new output

                    CTxOut txout(nValueDenom, scriptChange);

                    vecTxOutRet.push_back(txout);



                    // subtract denomination amount

                    nValueLeft -= nValueDenom;



                    // step is complete

                    break;

                }

                ++it;

                ++it2;

            }

        }

        if(nValueLeft == 0) break;

        nStep++;

    }



    {

        // unlock unused coins

        LOCK(pwalletMain->cs_wallet);

        BOOST_FOREACH(CTxIn txin, vecTxIn) {

            pwalletMain->UnlockCoin(txin.prevout);

        }

    }



    if (GetDenominations(vecTxOutRet) != nSessionDenom) {

        // unlock used coins on failure

        LOCK(pwalletMain->cs_wallet);

        BOOST_FOREACH(CTxIn txin, vecTxInRet) {

            pwalletMain->UnlockCoin(txin.prevout);

        }

        strErrorRet = "Can't make current denominated outputs";

        return false;

    }



    // We also do not care about full amount as long as we have right denominations

    return true;

}



// Create collaterals by looping through inputs grouped by addresses

bool CDarksendPool::MakeCollateralAmounts()

{

    std::vector<CompactTallyItem> vecTally;

    if(!pwalletMain->SelectCoinsGrouppedByAddresses(vecTally, false)) {

        LogPrint("privatesend", "CDarksendPool::MakeCollateralAmounts -- SelectCoinsGrouppedByAddresses can't find any inputs!\n");

        return false;

    }



    BOOST_FOREACH(CompactTallyItem& item, vecTally) {

        if(!MakeCollateralAmounts(item)) continue;

        return true;

    }



    LogPrintf("CDarksendPool::MakeCollateralAmounts -- failed!\n");

    return false;

}



// Split up large inputs or create fee sized inputs

bool CDarksendPool::MakeCollateralAmounts(const CompactTallyItem& tallyItem)

{

    CWalletTx wtx;

    CAmount nFeeRet = 0;

    int nChangePosRet = -1;

    std::string strFail = "";

    std::vector<CRecipient> vecSend;



    // make our collateral address

    CReserveKey reservekeyCollateral(pwalletMain);

    // make our change address

    CReserveKey reservekeyChange(pwalletMain);



    CScript scriptCollateral;

    CPubKey vchPubKey;

    assert(reservekeyCollateral.GetReservedKey(vchPubKey)); // should never fail, as we just unlocked

    scriptCollateral = GetScriptForDestination(vchPubKey.GetID());



    vecSend.push_back((CRecipient){scriptCollateral, PRIVATESEND_COLLATERAL*4, false});



    // try to use non-denominated and not mn-like funds first, select them explicitly

    CCoinControl coinControl;

    coinControl.fAllowOtherInputs = false;

    coinControl.fAllowWatchOnly = false;

    // send change to the same address so that we were able create more denoms out of it later

    coinControl.destChange = tallyItem.address.Get();

    BOOST_FOREACH(const CTxIn& txin, tallyItem.vecTxIn)

        coinControl.Select(txin.prevout);



    bool fSuccess = pwalletMain->CreateTransaction(vecSend, wtx, reservekeyChange,

            nFeeRet, nChangePosRet, strFail, &coinControl, true, ONLY_NONDENOMINATED_NOT10000IFMN);

    if(!fSuccess) {

        // if we failed (most likeky not enough funds), try to use all coins instead -

        // MN-like funds should not be touched in any case and we can't mix denominated without collaterals anyway

        LogPrintf("CDarksendPool::MakeCollateralAmounts -- ONLY_NONDENOMINATED_NOT10000IFMN Error: %s\n", strFail);

        CCoinControl *coinControlNull = NULL;

        fSuccess = pwalletMain->CreateTransaction(vecSend, wtx, reservekeyChange,

                nFeeRet, nChangePosRet, strFail, coinControlNull, true, ONLY_NOT10000IFMN);

        if(!fSuccess) {

            LogPrintf("CDarksendPool::MakeCollateralAmounts -- ONLY_NOT10000IFMN Error: %s\n", strFail);

            reservekeyCollateral.ReturnKey();

            return false;

        }

    }



    reservekeyCollateral.KeepKey();



    LogPrintf("CDarksendPool::MakeCollateralAmounts -- txid=%s\n", wtx.GetHash().GetHex());



    // use the same nCachedLastSuccessBlock as for DS mixinx to prevent race

    if(!pwalletMain->CommitTransaction(wtx, reservekeyChange)) {

        LogPrintf("CDarksendPool::MakeCollateralAmounts -- CommitTransaction failed!\n");

        return false;

    }



    nCachedLastSuccessBlock = pCurrentBlockIndex->nHeight;



    return true;

}



// Create denominations by looping through inputs grouped by addresses

bool CDarksendPool::CreateDenominated()

{

    std::vector<CompactTallyItem> vecTally;

    if(!pwalletMain->SelectCoinsGrouppedByAddresses(vecTally)) {

        LogPrint("privatesend", "CDarksendPool::CreateDenominated -- SelectCoinsGrouppedByAddresses can't find any inputs!\n");

        return false;

    }



    bool fCreateMixingCollaterals = !pwalletMain->HasCollateralInputs();



    BOOST_FOREACH(CompactTallyItem& item, vecTally) {

        if(!CreateDenominated(item, fCreateMixingCollaterals)) continue;

        return true;

    }



    LogPrintf("CDarksendPool::CreateDenominated -- failed!\n");

    return false;

}



// Create denominations

bool CDarksendPool::CreateDenominated(const CompactTallyItem& tallyItem, bool fCreateMixingCollaterals)

{

    std::vector<CRecipient> vecSend;

    CAmount nValueLeft = tallyItem.nAmount;

    nValueLeft -= PRIVATESEND_COLLATERAL; // leave some room for fees



    LogPrintf("CreateDenominated0 nValueLeft: %f\n", (float)nValueLeft/COIN);

    // make our collateral address

    CReserveKey reservekeyCollateral(pwalletMain);



    CScript scriptCollateral;

    CPubKey vchPubKey;

    assert(reservekeyCollateral.GetReservedKey(vchPubKey)); // should never fail, as we just unlocked

    scriptCollateral = GetScriptForDestination(vchPubKey.GetID());



    // ****** Add collateral outputs ************ /



    if(fCreateMixingCollaterals) {

        vecSend.push_back((CRecipient){scriptCollateral, PRIVATESEND_COLLATERAL*4, false});

        nValueLeft -= PRIVATESEND_COLLATERAL*4;

    }



    // ****** Add denoms ************ /



    // make our denom addresses

    CReserveKey reservekeyDenom(pwalletMain);



    // try few times - skipping smallest denoms first if there are too much already, if failed - use them

    int nOutputsTotal = 0;

    bool fSkip = true;

    do {



        BOOST_REVERSE_FOREACH(CAmount nDenomValue, vecPrivateSendDenominations) {



            if(fSkip) {

                // Note: denoms are skipped if there are already DENOMS_COUNT_MAX of them

                // and there are still larger denoms which can be used for mixing



                // check skipped denoms

                if(IsDenomSkipped(nDenomValue)) continue;



                // find new denoms to skip if any (ignore the largest one)

                if(nDenomValue != vecPrivateSendDenominations[0] && pwalletMain->CountInputsWithAmount(nDenomValue) > DENOMS_COUNT_MAX) {

                    strAutoDenomResult = strprintf(_("Too many %f denominations, removing."), (float)nDenomValue/COIN);

                    LogPrintf("CDarksendPool::CreateDenominated -- %s\n", strAutoDenomResult);

                    vecDenominationsSkipped.push_back(nDenomValue);

                    continue;

                }

            }



            int nOutputs = 0;



            // add each output up to 10 times until it can't be added again

            while(nValueLeft - nDenomValue >= 0 && nOutputs <= 10) {

                CScript scriptDenom;

                CPubKey vchPubKey;

                //use a unique change address

                assert(reservekeyDenom.GetReservedKey(vchPubKey)); // should never fail, as we just unlocked

                scriptDenom = GetScriptForDestination(vchPubKey.GetID());

                // TODO: do not keep reservekeyDenom here

                reservekeyDenom.KeepKey();



                vecSend.push_back((CRecipient){ scriptDenom, nDenomValue, false });



                //increment outputs and subtract denomination amount

                nOutputs++;

                nValueLeft -= nDenomValue;

                LogPrintf("CreateDenominated1: nOutputsTotal: %d, nOutputs: %d, nValueLeft: %f\n", nOutputsTotal, nOutputs, (float)nValueLeft/COIN);

            }



            nOutputsTotal += nOutputs;

            if(nValueLeft == 0) break;

        }

        LogPrintf("CreateDenominated2: nOutputsTotal: %d, nValueLeft: %f\n", nOutputsTotal, (float)nValueLeft/COIN);

        // if there were no outputs added, start over without skipping

        fSkip = !fSkip;

    } while (nOutputsTotal == 0 && !fSkip);

    LogPrintf("CreateDenominated3: nOutputsTotal: %d, nValueLeft: %f\n", nOutputsTotal, (float)nValueLeft/COIN);



    // if we have anything left over, it will be automatically send back as change - there is no need to send it manually



    CCoinControl coinControl;

    coinControl.fAllowOtherInputs = false;

    coinControl.fAllowWatchOnly = false;

    // send change to the same address so that we were able create more denoms out of it later

    coinControl.destChange = tallyItem.address.Get();

    BOOST_FOREACH(const CTxIn& txin, tallyItem.vecTxIn)

        coinControl.Select(txin.prevout);



    CWalletTx wtx;

    CAmount nFeeRet = 0;

    int nChangePosRet = -1;

    std::string strFail = "";

    // make our change address

    CReserveKey reservekeyChange(pwalletMain);



    bool fSuccess = pwalletMain->CreateTransaction(vecSend, wtx, reservekeyChange,

            nFeeRet, nChangePosRet, strFail, &coinControl, true, ONLY_NONDENOMINATED_NOT10000IFMN);

    if(!fSuccess) {

        LogPrintf("CDarksendPool::CreateDenominated -- Error: %s\n", strFail);

        // TODO: return reservekeyDenom here

        reservekeyCollateral.ReturnKey();

        return false;

    }



    // TODO: keep reservekeyDenom here

    reservekeyCollateral.KeepKey();



    if(!pwalletMain->CommitTransaction(wtx, reservekeyChange)) {

        LogPrintf("CDarksendPool::CreateDenominated -- CommitTransaction failed!\n");

        return false;

    }



    // use the same nCachedLastSuccessBlock as for DS mixing to prevent race

    nCachedLastSuccessBlock = pCurrentBlockIndex->nHeight;

    LogPrintf("CDarksendPool::CreateDenominated -- txid=%s\n", wtx.GetHash().GetHex());



    return true;

}



bool CDarksendPool::IsOutputsCompatibleWithSessionDenom(const std::vector<CTxDSOut>& vecTxDSOut)

{

    if(GetDenominations(vecTxDSOut) == 0) return false;



    BOOST_FOREACH(const CDarkSendEntry entry, vecEntries) {

        LogPrintf("CDarksendPool::IsOutputsCompatibleWithSessionDenom -- vecTxDSOut denom %d, entry.vecTxDSOut denom %d\n", GetDenominations(vecTxDSOut), GetDenominations(entry.vecTxDSOut));

        if(GetDenominations(vecTxDSOut) != GetDenominations(entry.vecTxDSOut)) return false;

    }



    return true;

}



bool CDarksendPool::IsAcceptableDenomAndCollateral(int nDenom, CTransaction txCollateral, PoolMessage& nMessageIDRet)

{

    if(!fMasterNode) return false;



    // is denom even smth legit?

    std::vector<int> vecBits;

    if(!GetDenominationsBits(nDenom, vecBits)) {

        LogPrint("privatesend", "CDarksendPool::IsAcceptableDenomAndCollateral -- denom not valid!\n");

        nMessageIDRet = ERR_DENOM;

        return false;

    }



    // check collateral

    if(!fUnitTest && !IsCollateralValid(txCollateral)) {

        LogPrint("privatesend", "CDarksendPool::IsAcceptableDenomAndCollateral -- collateral not valid!\n");

        nMessageIDRet = ERR_INVALID_COLLATERAL;

        return false;

    }



    return true;

}



bool CDarksendPool::CreateNewSession(int nDenom, CTransaction txCollateral, PoolMessage& nMessageIDRet)

{

    if(!fMasterNode || nSessionID != 0) return false;



    // new session can only be started in idle mode

    if(nState != POOL_STATE_IDLE) {

        nMessageIDRet = ERR_MODE;

        LogPrintf("CDarksendPool::CreateNewSession -- incompatible mode: nState=%d\n", nState);

        return false;

    }



    if(!IsAcceptableDenomAndCollateral(nDenom, txCollateral, nMessageIDRet)) {

        return false;

    }



    // start new session

    nMessageIDRet = MSG_NOERR;

    nSessionID = GetRandInt(999999)+1;

    nSessionDenom = nDenom;



    SetState(POOL_STATE_QUEUE);

    nTimeLastSuccessfulStep = GetTimeMillis();



    if(!fUnitTest) {

        //broadcast that I'm accepting entries, only if it's the first entry through

        CDarksendQueue dsq(nDenom, activeMasternode.vin, GetTime(), false);

        LogPrint("privatesend", "CDarksendPool::CreateNewSession -- signing and relaying new queue: %s\n", dsq.ToString());

        dsq.Sign();

        dsq.Relay();

        vecDarksendQueue.push_back(dsq);

    }



    vecSessionCollaterals.push_back(txCollateral);

    LogPrintf("CDarksendPool::CreateNewSession -- new session created, nSessionID: %d  nSessionDenom: %d (%s)  vecSessionCollaterals.size(): %d\n",

            nSessionID, nSessionDenom, GetDenominationsToString(nSessionDenom), vecSessionCollaterals.size());



    return true;

}



bool CDarksendPool::AddUserToExistingSession(int nDenom, CTransaction txCollateral, PoolMessage& nMessageIDRet)

{

    if(!fMasterNode || nSessionID == 0 || IsSessionReady()) return false;



    if(!IsAcceptableDenomAndCollateral(nDenom, txCollateral, nMessageIDRet)) {

        return false;

    }



    // we only add new users to an existing session when we are in queue mode

    if(nState != POOL_STATE_QUEUE) {

        nMessageIDRet = ERR_MODE;

        LogPrintf("CDarksendPool::AddUserToExistingSession -- incompatible mode: nState=%d\n", nState);

        return false;

    }



    if(nDenom != nSessionDenom) {

        LogPrintf("CDarksendPool::AddUserToExistingSession -- incompatible denom %d (%s) != nSessionDenom %d (%s)\n",

                    nDenom, GetDenominationsToString(nDenom), nSessionDenom, GetDenominationsToString(nSessionDenom));

        nMessageIDRet = ERR_DENOM;

        return false;

    }



    // count new user as accepted to an existing session



    nMessageIDRet = MSG_NOERR;

    nTimeLastSuccessfulStep = GetTimeMillis();

    vecSessionCollaterals.push_back(txCollateral);



    LogPrintf("CDarksendPool::AddUserToExistingSession -- new user accepted, nSessionID: %d  nSessionDenom: %d (%s)  vecSessionCollaterals.size(): %d\n",

            nSessionID, nSessionDenom, GetDenominationsToString(nSessionDenom), vecSessionCollaterals.size());



    return true;

}



/*  Create a nice string to show the denominations

    Function returns as follows (for 4 denominations):

        ( bit on if present )

        bit 0           - 100

        bit 1           - 10

        bit 2           - 1

        bit 3           - .1

        bit 4 and so on - out-of-bounds

        none of above   - non-denom

*/

std::string CDarksendPool::GetDenominationsToString(int nDenom)

{

    std::string strDenom = "";

    int nMaxDenoms = vecPrivateSendDenominations.size();



    if(nDenom >= (1 << nMaxDenoms)) {

        return "out-of-bounds";

    }



    for (int i = 0; i < nMaxDenoms; ++i) {

        if(nDenom & (1 << i)) {

            strDenom += (strDenom.empty() ? "" : "+") + FormatMoney(vecPrivateSendDenominations[i]);

        }

    }



    if(strDenom.empty()) {

        return "non-denom";

    }



    return strDenom;

}



int CDarksendPool::GetDenominations(const std::vector<CTxDSOut>& vecTxDSOut)

{

    std::vector<CTxOut> vecTxOut;



    BOOST_FOREACH(CTxDSOut out, vecTxDSOut)

        vecTxOut.push_back(out);



    return GetDenominations(vecTxOut);

}



/*  Return a bitshifted integer representing the denominations in this list

    Function returns as follows (for 4 denominations):

        ( bit on if present )

        100       - bit 0

        10        - bit 1

        1         - bit 2

        .1        - bit 3

        non-denom - 0, all bits off

*/

int CDarksendPool::GetDenominations(const std::vector<CTxOut>& vecTxOut, bool fSingleRandomDenom)

{

    std::vector<std::pair<CAmount, int> > vecDenomUsed;



    // make a list of denominations, with zero uses

    BOOST_FOREACH(CAmount nDenomValue, vecPrivateSendDenominations)

        vecDenomUsed.push_back(std::make_pair(nDenomValue, 0));



    // look for denominations and update uses to 1

    BOOST_FOREACH(CTxOut txout, vecTxOut) {

        bool found = false;

        BOOST_FOREACH (PAIRTYPE(CAmount, int)& s, vecDenomUsed) {

            if(txout.nValue == s.first) {

                s.second = 1;

                found = true;

            }

        }

        if(!found) return 0;

    }



    int nDenom = 0;

    int c = 0;

    // if the denomination is used, shift the bit on

    BOOST_FOREACH (PAIRTYPE(CAmount, int)& s, vecDenomUsed) {

        int bit = (fSingleRandomDenom ? GetRandInt(2) : 1) & s.second;

        nDenom |= bit << c++;

        if(fSingleRandomDenom && bit) break; // use just one random denomination

    }



    return nDenom;

}



bool CDarksendPool::GetDenominationsBits(int nDenom, std::vector<int> &vecBitsRet)

{

    // ( bit on if present, 4 denominations example )

    // bit 0 - 100CURIUM+1

    // bit 1 - 10CURIUM+1

    // bit 2 - 1CURIUM+1

    // bit 3 - .1CURIUM+1



    int nMaxDenoms = vecPrivateSendDenominations.size();



    if(nDenom >= (1 << nMaxDenoms)) return false;



    vecBitsRet.clear();



    for (int i = 0; i < nMaxDenoms; ++i) {

        if(nDenom & (1 << i)) {

            vecBitsRet.push_back(i);

        }

    }



    return !vecBitsRet.empty();

}



int CDarksendPool::GetDenominationsByAmounts(const std::vector<CAmount>& vecAmount)

{

    CScript scriptTmp = CScript();

    std::vector<CTxOut> vecTxOut;



    BOOST_REVERSE_FOREACH(CAmount nAmount, vecAmount) {

        CTxOut txout(nAmount, scriptTmp);

        vecTxOut.push_back(txout);

    }



    return GetDenominations(vecTxOut, true);

}



std::string CDarksendPool::GetMessageByID(PoolMessage nMessageID)

{

    switch (nMessageID) {

        case ERR_ALREADY_HAVE:          return _("Already have that input.");

        case ERR_DENOM:                 return _("No matching denominations found for mixing.");

        case ERR_ENTRIES_FULL:          return _("Entries are full.");

        case ERR_EXISTING_TX:           return _("Not compatible with existing transactions.");

        case ERR_FEES:                  return _("Transaction fees are too high.");

        case ERR_INVALID_COLLATERAL:    return _("Collateral not valid.");

        case ERR_INVALID_INPUT:         return _("Input is not valid.");

        case ERR_INVALID_SCRIPT:        return _("Invalid script detected.");

        case ERR_INVALID_TX:            return _("Transaction not valid.");

        case ERR_MAXIMUM:               return _("Value more than PrivateSend pool maximum allows.");

        case ERR_MN_LIST:               return _("Not in the Masternode list.");

        case ERR_MODE:                  return _("Incompatible mode.");

        case ERR_NON_STANDARD_PUBKEY:   return _("Non-standard public key detected.");

        case ERR_NOT_A_MN:              return _("This is not a Masternode.");

        case ERR_QUEUE_FULL:            return _("Masternode queue is full.");

        case ERR_RECENT:                return _("Last PrivateSend was too recent.");

        case ERR_SESSION:               return _("Session not complete!");

        case ERR_MISSING_TX:            return _("Missing input transaction information.");

        case ERR_VERSION:               return _("Incompatible version.");

        case MSG_NOERR:                 return _("No errors detected.");

        case MSG_SUCCESS:               return _("Transaction created successfully.");

        case MSG_ENTRIES_ADDED:         return _("Your entries added successfully.");

        default:                        return _("Unknown response.");

<<<<<<< HEAD
    // catching hanging sessions
    if(!fMasterNode) {
        switch(state) {
            case POOL_STATUS_TRANSMISSION:
                LogPrint("darksend", "CDarksendPool::CheckTimeout() -- Session complete -- Running Check()\n");
                Check();
                break;
            case POOL_STATUS_ERROR:
                LogPrint("darksend", "CDarksendPool::CheckTimeout() -- Pool error -- Running Check()\n");
                Check();
                break;
            case POOL_STATUS_SUCCESS:
                LogPrint("darksend", "CDarksendPool::CheckTimeout() -- Pool success -- Running Check()\n");
                Check();
                break;
        }
    }

    // check Darksend queue objects for timeouts
    int c = 0;
    vector<CDarksendQueue>::iterator it = vecDarksendQueue.begin();
    while(it != vecDarksendQueue.end()){
        if((*it).IsExpired()){
            LogPrint("darksend", "CDarksendPool::CheckTimeout() : Removing expired queue entry - %d\n", c);
            it = vecDarksendQueue.erase(it);
        } else ++it;
        c++;
    }

    int addLagTime = 0;
    if(!fMasterNode) addLagTime = 10000; //if we're the client, give the server a few extra seconds before resetting.

    if(state == POOL_STATUS_ACCEPTING_ENTRIES || state == POOL_STATUS_QUEUE){
        c = 0;

        // check for a timeout and reset if needed
        vector<CDarkSendEntry>::iterator it2 = entries.begin();
        while(it2 != entries.end()){
            if((*it2).IsExpired()){
                LogPrint("darksend", "CDarksendPool::CheckTimeout() : Removing expired entry - %d\n", c);
                it2 = entries.erase(it2);
                if(entries.size() == 0){
                    UnlockCoins();
                    SetNull();
                }
                if(fMasterNode){
                    RelayStatus(sessionID, GetState(), GetEntriesCount(), MASTERNODE_RESET);
                }
            } else ++it2;
            c++;
        }

        if(GetTimeMillis()-lastTimeChanged >= (DARKSEND_QUEUE_TIMEOUT*1000)+addLagTime){
            UnlockCoins();
            SetNull();
        }
    } else if(GetTimeMillis()-lastTimeChanged >= (DARKSEND_QUEUE_TIMEOUT*1000)+addLagTime){
        LogPrint("darksend", "CDarksendPool::CheckTimeout() -- Session timed out (%ds) -- resetting\n", DARKSEND_QUEUE_TIMEOUT);
        UnlockCoins();
        SetNull();

        UpdateState(POOL_STATUS_ERROR);
        lastMessage = _("Session timed out.");
    }

    if(state == POOL_STATUS_SIGNING && GetTimeMillis()-lastTimeChanged >= (DARKSEND_SIGNING_TIMEOUT*1000)+addLagTime ) {
            LogPrint("darksend", "CDarksendPool::CheckTimeout() -- Session timed out (%ds) -- restting\n", DARKSEND_SIGNING_TIMEOUT);
            ChargeFees();
            UnlockCoins();
            SetNull();

            UpdateState(POOL_STATUS_ERROR);
            lastMessage = _("Signing timed out.");
    }
}
=======
    }

}



bool CDarkSendSigner::IsVinAssociatedWithPubkey(const CTxIn& txin, const CPubKey& pubkey)

{
>>>>>>> dev-1.12.1.0

    CScript payee;

    payee = GetScriptForDestination(pubkey.GetID());



    CTransaction tx;

    uint256 hash;

    if(GetTransaction(txin.prevout.hash, tx, Params().GetConsensus(), hash, true)) {

        BOOST_FOREACH(CTxOut out, tx.vout)

            if(out.nValue == 10000*COIN && out.scriptPubKey == payee) return true;

<<<<<<< HEAD
    if(found >= 0){ //might have to do this one input at a time?
        int n = found;
        txNew.vin[n].scriptSig = newSig;
        LogPrint("darksend", "CDarksendPool::SignatureValid() - Sign with sig %s\n", newSig.ToString().substr(0,24));
        if (!VerifyScript(txNew.vin[n].scriptSig, sigPubKey, SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_STRICTENC, MutableTransactionSignatureChecker(&txNew, n))){
            LogPrint("darksend", "CDarksendPool::SignatureValid() - Signing - Error signing input %u\n", n);
            return false;
        }
    }

    LogPrint("darksend", "CDarksendPool::SignatureValid() - Signing - Successfully validated input\n");
    return true;
}
=======
    }

>>>>>>> dev-1.12.1.0


    return false;

}

<<<<<<< HEAD
        if(!o.scriptPubKey.IsNormalPaymentScript()){
            LogPrintf ("CDarksendPool::IsCollateralValid - Invalid Script %s\n", txCollateral.ToString());
            return false;
        }
    }
=======
>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
    if(missingTx){
        LogPrint("darksend", "CDarksendPool::IsCollateralValid - Unknown inputs in collateral transaction - %s\n", txCollateral.ToString());
        return false;
    }

    //collateral transactions are required to pay out DARKSEND_COLLATERAL as a fee to the miners
    if(nValueIn - nValueOut < DARKSEND_COLLATERAL) {
        LogPrint("darksend", "CDarksendPool::IsCollateralValid - did not include enough fees in transaction %d\n%s\n", nValueOut-nValueIn, txCollateral.ToString());
        return false;
    }

    LogPrint("darksend", "CDarksendPool::IsCollateralValid %s\n", txCollateral.ToString());

    {
        LOCK(cs_main);
        CValidationState state;
        if(!AcceptableInputs(mempool, state, txCollateral, true, NULL)){
            if(fDebug) LogPrintf ("CDarksendPool::IsCollateralValid - didn't pass IsAcceptable\n");
            return false;
        }
    }
=======
bool CDarkSendSigner::GetKeysFromSecret(std::string strSecret, CKey& keyRet, CPubKey& pubkeyRet)

{

    CBitcoinSecret vchSecret;

>>>>>>> dev-1.12.1.0


    if(!vchSecret.SetString(strSecret)) return false;

<<<<<<< HEAD
//
// Add a clients transaction to the pool
//
bool CDarksendPool::AddEntry(const std::vector<CTxIn>& newInput, const int64_t& nAmount, const CTransaction& txCollateral, const std::vector<CTxOut>& newOutput, int& errorID){
    if (!fMasterNode) return false;

    BOOST_FOREACH(CTxIn in, newInput) {
        if (in.prevout.IsNull() || nAmount < 0) {
            LogPrint("darksend", "CDarksendPool::AddEntry - input not valid!\n");
            errorID = ERR_INVALID_INPUT;
            sessionUsers--;
            return false;
        }
    }

    if (!IsCollateralValid(txCollateral)){
        LogPrint("darksend", "CDarksendPool::AddEntry - collateral not valid!\n");
        errorID = ERR_INVALID_COLLATERAL;
        sessionUsers--;
        return false;
    }

    if((int)entries.size() >= GetMaxPoolTransactions()){
        LogPrint("darksend", "CDarksendPool::AddEntry - entries is full!\n");
        errorID = ERR_ENTRIES_FULL;
        sessionUsers--;
        return false;
    }

    BOOST_FOREACH(CTxIn in, newInput) {
        LogPrint("darksend", "looking for vin -- %s\n", in.ToString());
        BOOST_FOREACH(const CDarkSendEntry& v, entries) {
            BOOST_FOREACH(const CTxDSIn& s, v.sev){
                if((CTxIn)s == in) {
                    LogPrint("darksend", "CDarksendPool::AddEntry - found in vin\n");
                    errorID = ERR_ALREADY_HAVE;
                    sessionUsers--;
                    return false;
                }
            }
        }
    }
=======


    keyRet = vchSecret.GetKey();

    pubkeyRet = keyRet.GetPubKey();
>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
    LogPrint("darksend", "CDarksendPool::AddEntry -- adding %s\n", newInput[0].ToString());
    errorID = MSG_ENTRIES_ADDED;
=======
>>>>>>> dev-1.12.1.0

    return true;

<<<<<<< HEAD
bool CDarksendPool::AddScriptSig(const CTxIn& newVin){
    LogPrint("darksend", "CDarksendPool::AddScriptSig -- new sig  %s\n", newVin.scriptSig.ToString().substr(0,24));


    BOOST_FOREACH(const CDarkSendEntry& v, entries) {
        BOOST_FOREACH(const CTxDSIn& s, v.sev){
            if(s.scriptSig == newVin.scriptSig) {
                LogPrint("darksend", "CDarksendPool::AddScriptSig - already exists\n");
                return false;
            }
        }
    }

    if(!SignatureValid(newVin.scriptSig, newVin)){
        LogPrint("darksend", "CDarksendPool::AddScriptSig - Invalid Sig\n");
        return false;
    }

    LogPrint("darksend", "CDarksendPool::AddScriptSig -- sig %s\n", newVin.ToString());

    BOOST_FOREACH(CTxIn& vin, finalTransaction.vin){
        if(newVin.prevout == vin.prevout && vin.nSequence == newVin.nSequence){
            vin.scriptSig = newVin.scriptSig;
            vin.prevPubKey = newVin.prevPubKey;
            LogPrint("darksend", "CDarkSendPool::AddScriptSig -- adding to finalTransaction  %s\n", newVin.scriptSig.ToString().substr(0,24));
        }
    }
    for(unsigned int i = 0; i < entries.size(); i++){
        if(entries[i].AddSig(newVin)){
            LogPrint("darksend", "CDarkSendPool::AddScriptSig -- adding  %s\n", newVin.scriptSig.ToString().substr(0,24));
            return true;
        }
    }
=======
}



bool CDarkSendSigner::SignMessage(std::string strMessage, std::vector<unsigned char>& vchSigRet, CKey key)

{

    CHashWriter ss(SER_GETHASH, 0);
>>>>>>> dev-1.12.1.0

    ss << strMessageMagic;

    ss << strMessage;

<<<<<<< HEAD
//
// Execute a Darksend denomination via a Masternode.
// This is only ran from clients
//
void CDarksendPool::SendDarksendDenominate(std::vector<CTxIn>& vin, std::vector<CTxOut>& vout, int64_t amount){

    if(fMasterNode) {
        LogPrintf("CDarksendPool::SendDarksendDenominate() - Darksend from a Masternode is not supported currently.\n");
        return;
    }

    if(txCollateral == CMutableTransaction()){
        LogPrintf ("CDarksendPool:SendDarksendDenominate() - Darksend collateral not set");
        return;
    }
=======
>>>>>>> dev-1.12.1.0


    return key.SignCompact(ss.GetHash(), vchSigRet);

<<<<<<< HEAD
    //BOOST_FOREACH(CTxOut o, vout)
    //    LogPrintf(" vout - %s\n", o.ToString());


    // we should already be connected to a Masternode
    if(!sessionFoundMasternode){
        LogPrintf("CDarksendPool::SendDarksendDenominate() - No Masternode has been selected yet.\n");
        UnlockCoins();
        SetNull();
        return;
    }

    if (!CheckDiskSpace()) {
        UnlockCoins();
        SetNull();
        fEnableDarksend = false;
        LogPrintf("CDarksendPool::SendDarksendDenominate() - Not enough disk space, disabling Darksend.\n");
        return;
    }
=======
}



bool CDarkSendSigner::VerifyMessage(CPubKey pubkey, const std::vector<unsigned char>& vchSig, std::string strMessage, std::string& strErrorRet)

{
>>>>>>> dev-1.12.1.0

    CHashWriter ss(SER_GETHASH, 0);

    ss << strMessageMagic;

    ss << strMessage;



    CPubKey pubkeyFromSig;

    if(!pubkeyFromSig.RecoverCompact(ss.GetHash(), vchSig)) {

<<<<<<< HEAD
            LogPrint("darksend", "dsi -- tx in %s\n", i.ToString());
        }

        LogPrintf("Submitting tx %s\n", tx.ToString());

        while(true){
            TRY_LOCK(cs_main, lockMain);
            if(!lockMain) { MilliSleep(50); continue;}
            if(!AcceptableInputs(mempool, state, CTransaction(tx), false, NULL, false, true)){
                LogPrintf("dsi -- transaction not valid! %s \n", tx.ToString());
                UnlockCoins();
                SetNull();
                return;
            }
            break;
        }
    }

    // store our entry for later use
    CDarkSendEntry e;
    e.Add(vin, amount, txCollateral, vout);
    entries.push_back(e);

    RelayIn(entries[0].sev, entries[0].amount, txCollateral, entries[0].vout);
    Check();
}
=======
        strErrorRet = "Error recovering public key.";

        return false;

    }


>>>>>>> dev-1.12.1.0

    if(pubkeyFromSig.GetID() != pubkey.GetID()) {

        strErrorRet = strprintf("Keys don't match: pubkey=%s, pubkeyFromSig=%s, strMessage=%s, vchSig=%s",

                    pubkey.GetID().ToString(), pubkeyFromSig.GetID().ToString(), strMessage,

                    EncodeBase64(&vchSig[0], vchSig.size()));

        return false;

    }



    return true;

}


<<<<<<< HEAD
    finalTransaction = finalTransactionNew;
    LogPrintf("CDarksendPool::SignFinalTransaction %s", finalTransaction.ToString());
=======
>>>>>>> dev-1.12.1.0

bool CDarkSendEntry::AddScriptSig(const CTxIn& txin)

<<<<<<< HEAD
    //make sure my inputs/outputs are present, otherwise refuse to sign
    BOOST_FOREACH(const CDarkSendEntry e, entries) {
        BOOST_FOREACH(const CTxDSIn s, e.sev) {
            /* Sign my transaction and all outputs */
            int mine = -1;
            CScript prevPubKey = CScript();
            CTxIn vin = CTxIn();
=======
{
>>>>>>> dev-1.12.1.0

    BOOST_FOREACH(CTxDSIn& txdsin, vecTxDSIn) {

        if(txdsin.prevout == txin.prevout && txdsin.nSequence == txin.nSequence) {

            if(txdsin.fHasSig) return false;


<<<<<<< HEAD
                int targetOuputs = e.vout.size();
                if(foundOutputs < targetOuputs || nValue1 != nValue2) {
                    // in this case, something went wrong and we'll refuse to sign. It's possible we'll be charged collateral. But that's
                    // better then signing if the transaction doesn't look like what we wanted.
                    LogPrintf("CDarksendPool::Sign - My entries are not correct! Refusing to sign. %d entries %d target. \n", foundOutputs, targetOuputs);
                    UnlockCoins();
                    SetNull();
=======
>>>>>>> dev-1.12.1.0

            txdsin.scriptSig = txin.scriptSig;

            txdsin.prevPubKey = txin.prevPubKey;

<<<<<<< HEAD
                LogPrint("darksend", "CDarksendPool::Sign - Signing my input %i\n", mine);
                if(!SignSignature(keystore, prevPubKey, finalTransaction, mine, int(SIGHASH_ALL|SIGHASH_ANYONECANPAY))) { // changes scriptSig
                    LogPrint("darksend", "CDarksendPool::Sign - Unable to sign my own transaction! \n");
                    // not sure what to do here, it will timeout...?
                }

                sigs.push_back(finalTransaction.vin[mine]);
                LogPrint("darksend", " -- dss %d %d %s\n", mine, (int)sigs.size(), finalTransaction.vin[mine].scriptSig.ToString());
            }

        }

        LogPrint("darksend", "CDarksendPool::Sign - txNew:\n%s", finalTransaction.ToString());
=======
            txdsin.fHasSig = true;



            return true;

        }

>>>>>>> dev-1.12.1.0
    }



    return false;

}



bool CDarksendQueue::Sign()

{
<<<<<<< HEAD
    LogPrint("darksend", "CDarksendPool::NewBlock \n");
=======
>>>>>>> dev-1.12.1.0

    if(!fMasterNode) return false;

<<<<<<< HEAD
    darkSendPool.CheckTimeout();
}

// Darksend transaction was completed (failed or successful)
void CDarksendPool::CompletedTransaction(bool error, int errorID)
{
    if(fMasterNode) return;
=======


    std::string strMessage = vin.ToString() + boost::lexical_cast<std::string>(nDenom) + boost::lexical_cast<std::string>(nTime) + boost::lexical_cast<std::string>(fReady);

>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
        Check();
        UnlockCoins();
        SetNull();
    } else {
        LogPrintf("CompletedTransaction -- success \n");
        UpdateState(POOL_STATUS_SUCCESS);

        UnlockCoins();
        SetNull();
=======
    if(!darkSendSigner.SignMessage(strMessage, vchSig, activeMasternode.keyMasternode)) {

        LogPrintf("CDarksendQueue::Sign -- SignMessage() failed, %s\n", ToString());

        return false;
>>>>>>> dev-1.12.1.0

    }
<<<<<<< HEAD
    lastMessage = GetMessageByID(errorID);
}
=======
>>>>>>> dev-1.12.1.0



    return CheckSignature(activeMasternode.pubKeyMasternode);

}

<<<<<<< HEAD
//
// Passively run Darksend in the background to anonymize funds based on the given configuration.
//
// This does NOT run by default for daemons, only for QT.
//
bool CDarksendPool::DoAutomaticDenominating(bool fDryRun)
{
    if(!fEnableDarksend) return false;
    if(fMasterNode) return false;
    if(state == POOL_STATUS_ERROR || state == POOL_STATUS_SUCCESS) return false;
    if(GetEntriesCount() > 0) {
        strAutoDenomResult = _("Mixing in progress...");
        return false;
    }

    TRY_LOCK(cs_darksend, lockDS);
    if(!lockDS) {
        strAutoDenomResult = _("Lock is already in place.");
        return false;
    }

    if(!masternodeSync.IsBlockchainSynced()) {
        strAutoDenomResult = _("Can't mix while sync in progress.");
        return false;
    }
=======


bool CDarksendQueue::CheckSignature(const CPubKey& pubKeyMasternode)

{

    std::string strMessage = vin.ToString() + boost::lexical_cast<std::string>(nDenom) + boost::lexical_cast<std::string>(nTime) + boost::lexical_cast<std::string>(fReady);

    std::string strError = "";

>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
    if(chainActive.Tip()->nHeight - cachedLastSuccess < minBlockSpacing) {
        LogPrintf("CDarksendPool::DoAutomaticDenominating - Last successful Darksend action was too recent\n");
        strAutoDenomResult = _("Last successful Darksend action was too recent.");
        return false;
    }

    if(mnodeman.size() == 0){
        LogPrint("darksend", "CDarksendPool::DoAutomaticDenominating - No Masternodes detected\n");
        strAutoDenomResult = _("No Masternodes detected.");
=======
    if(!darkSendSigner.VerifyMessage(pubKeyMasternode, vchSig, strMessage, strError)) {

        LogPrintf("CDarksendQueue::CheckSignature -- Got bad Masternode queue signature: %s; error: %s\n", ToString(), strError);

>>>>>>> dev-1.12.1.0
        return false;

    }



    return true;

}



bool CDarksendQueue::Relay()

{

<<<<<<< HEAD
    LogPrint("darksend", "DoAutomaticDenominating : nLowestDenom=%d, nBalanceNeedsAnonymized=%d\n", nLowestDenom, nBalanceNeedsAnonymized);
=======
    std::vector<CNode*> vNodesCopy;
>>>>>>> dev-1.12.1.0

    {

<<<<<<< HEAD
        if (pwalletMain->SelectCoinsDark(nValueMin, 9999999*COIN, vCoins, nValueIn, -2, 0))
        {
            nOnlyDenominatedBalance = pwalletMain->GetDenominatedBalance(true) + pwalletMain->GetDenominatedBalance() - pwalletMain->GetAnonymizedBalance();
            nBalanceNeedsDenominated = nBalanceNeedsAnonymized - nOnlyDenominatedBalance;
=======
        LOCK(cs_vNodes);
>>>>>>> dev-1.12.1.0

        vNodesCopy = vNodes;

<<<<<<< HEAD
            if(nBalanceNeedsDenominated < nLowestDenom) return false; // most likely we just waiting for denoms to confirm
            if(!fDryRun) return CreateDenominated(nBalanceNeedsDenominated);
=======
        BOOST_FOREACH(CNode* pnode, vNodesCopy)
>>>>>>> dev-1.12.1.0

            pnode->AddRef();

    }

<<<<<<< HEAD
    if(fDryRun) return true;

    nOnlyDenominatedBalance = pwalletMain->GetDenominatedBalance(true) + pwalletMain->GetDenominatedBalance() - pwalletMain->GetAnonymizedBalance();
    nBalanceNeedsDenominated = nBalanceNeedsAnonymized - nOnlyDenominatedBalance;

    //check if we have should create more denominated inputs
    if(nBalanceNeedsDenominated > nOnlyDenominatedBalance) return CreateDenominated(nBalanceNeedsDenominated);

    //check if we have the collateral sized inputs
    if(!pwalletMain->HasCollateralInputs()) return !pwalletMain->HasCollateralInputs(false) && MakeCollateralAmounts();
=======
    BOOST_FOREACH(CNode* pnode, vNodesCopy)

        if(pnode->nVersion >= MIN_PRIVATESEND_PEER_PROTO_VERSION)

            pnode->PushMessage(NetMsgType::DSQUEUE, (*this));
>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
    // initial phase, find a Masternode
    if(!sessionFoundMasternode){
        // Clean if there is anything left from previous session
        UnlockCoins();
        SetNull();
=======
>>>>>>> dev-1.12.1.0

    {

<<<<<<< HEAD
        if(pwalletMain->GetDenominatedBalance(true) > 0){ //get denominated unconfirmed inputs
            LogPrintf("DoAutomaticDenominating -- Found unconfirmed denominated outputs, will wait till they confirm to continue.\n");
            strAutoDenomResult = _("Found unconfirmed denominated outputs, will wait till they confirm to continue.");
            return false;
        }

        //check our collateral nad create new if needed
        std::string strReason;
        CValidationState state;
        if(txCollateral == CMutableTransaction()){
            if(!pwalletMain->CreateCollateralTransaction(txCollateral, strReason)){
                LogPrintf("% -- create collateral error:%s\n", __func__, strReason);
                return false;
            }
        } else {
            if(!IsCollateralValid(txCollateral)) {
                LogPrintf("%s -- invalid collateral, recreating...\n", __func__);
                if(!pwalletMain->CreateCollateralTransaction(txCollateral, strReason)){
                    LogPrintf("%s -- create collateral error: %s\n", __func__, strReason);
                    return false;
                }
            }
        }

        //if we've used 90% of the Masternode list then drop all the oldest first
        int nThreshold = (int)(mnodeman.CountEnabled(MIN_POOL_PEER_PROTO_VERSION) * 0.9);
        LogPrint("darksend", "Checking vecMasternodesUsed size %d threshold %d\n", (int)vecMasternodesUsed.size(), nThreshold);
        while((int)vecMasternodesUsed.size() > nThreshold){
            vecMasternodesUsed.erase(vecMasternodesUsed.begin());
            LogPrint("darksend", "  vecMasternodesUsed size %d threshold %d\n", (int)vecMasternodesUsed.size(), nThreshold);
        }

        //don't use the queues all of the time for mixing
        if(nUseQueue > 33){
=======
        LOCK(cs_vNodes);

        BOOST_FOREACH(CNode* pnode, vNodesCopy)

            pnode->Release();
>>>>>>> dev-1.12.1.0

    }

    return true;

}


<<<<<<< HEAD
                bool fUsed = false;
                //don't reuse Masternodes
                BOOST_FOREACH(CTxIn usedVin, vecMasternodesUsed){
                    if(dsq.vin == usedVin) {
                        fUsed = true;
                        break;
                    }
                }
                if(fUsed) continue;

                std::vector<CTxIn> vTempCoins;
                std::vector<COutput> vTempCoins2;
                // Try to match their denominations if possible
                if (!pwalletMain->SelectCoinsByDenominations(dsq.nDenom, nValueMin, nBalanceNeedsAnonymized, vTempCoins, vTempCoins2, nValueIn, 0, nDarksendRounds)){
                    LogPrintf("DoAutomaticDenominating --- Couldn't match denominations %d\n", dsq.nDenom);
                    continue;
                }

                CMasternode* pmn = mnodeman.Find(dsq.vin);
                if(pmn == NULL)
                {
                    LogPrintf("DoAutomaticDenominating --- dsq vin %s is not in masternode list!", dsq.vin.ToString());
                    continue;
                }

                LogPrintf("DoAutomaticDenominating --- attempt to connect to masternode from queue %s\n", pmn->addr.ToString());
                lastTimeChanged = GetTimeMillis();
                // connect to Masternode and submit the queue request
                CNode* pnode = ConnectNode((CAddress)addr, NULL, true);
                if(pnode != NULL)
                {
                    pSubmittedToMasternode = pmn;
                    vecMasternodesUsed.push_back(dsq.vin);
                    sessionDenom = dsq.nDenom;

                    pnode->PushMessage("dsa", sessionDenom, txCollateral);
                    LogPrintf("DoAutomaticDenominating --- connected (from queue), sending dsa for %d - %s\n", sessionDenom, pnode->addr.ToString());
                    strAutoDenomResult = _("Mixing in progress...");
                    dsq.time = 0; //remove node
                    return true;
                } else {
                    LogPrintf("DoAutomaticDenominating --- error connecting \n");
                    strAutoDenomResult = _("Error connecting to Masternode.");
                    dsq.time = 0; //remove node
                    continue;
                }
            }
        }

        // do not initiate queue if we are a liquidity proveder to avoid useless inter-mixing
        if(nLiquidityProvider) return false;

        int i = 0;

        // otherwise, try one randomly
        while(i < 10)
        {
            CMasternode* pmn = mnodeman.FindRandomNotInVec(vecMasternodesUsed, MIN_POOL_PEER_PROTO_VERSION);
            if(pmn == NULL)
            {
                LogPrintf("DoAutomaticDenominating --- Can't find random masternode!\n");
                strAutoDenomResult = _("Can't find random Masternode.");
                return false;
            }

            if(pmn->nLastDsq != 0 &&
                pmn->nLastDsq + mnodeman.CountEnabled(MIN_POOL_PEER_PROTO_VERSION)/5 > mnodeman.nDsqCount){
                i++;
                continue;
            }

            lastTimeChanged = GetTimeMillis();
            LogPrintf("DoAutomaticDenominating --- attempt %d connection to Masternode %s\n", i, pmn->addr.ToString());
            CNode* pnode = ConnectNode((CAddress)pmn->addr, NULL, true);
            if(pnode != NULL){
                pSubmittedToMasternode = pmn;
                vecMasternodesUsed.push_back(pmn->vin);

                std::vector<CAmount> vecAmounts;
                pwalletMain->ConvertList(vCoins, vecAmounts);
                // try to get a single random denom out of vecAmounts
                while(sessionDenom == 0)
                    sessionDenom = GetDenominationsByAmounts(vecAmounts);

                pnode->PushMessage("dsa", sessionDenom, txCollateral);
                LogPrintf("DoAutomaticDenominating --- connected, sending dsa for %d\n", sessionDenom);
                strAutoDenomResult = _("Mixing in progress...");
                return true;
            } else {
                vecMasternodesUsed.push_back(pmn->vin); // postpone MN we wasn't able to connect to
                i++;
                continue;
            }
        }
=======

bool CDarksendBroadcastTx::Sign()

{

    if(!fMasterNode) return false;



    std::string strMessage = tx.GetHash().ToString() + boost::lexical_cast<std::string>(sigTime);



    if(!darkSendSigner.SignMessage(strMessage, vchSig, activeMasternode.keyMasternode)) {

        LogPrintf("CDarksendBroadcastTx::Sign -- SignMessage() failed\n");
>>>>>>> dev-1.12.1.0

        return false;

    }

<<<<<<< HEAD
    strAutoDenomResult = _("Mixing in progress...");
    return false;
}
=======


    return CheckSignature(activeMasternode.pubKeyMasternode);
>>>>>>> dev-1.12.1.0

}

<<<<<<< HEAD
bool CDarksendPool::PrepareDarksendDenominate()
{
    std::string strError = "";
    // Submit transaction to the pool if we get here
    // Try to use only inputs with the same number of rounds starting from lowest number of rounds possible
    for(int i = 0; i < nDarksendRounds; i++) {
        strError = pwalletMain->PrepareDarksendDenominate(i, i+1);
        LogPrintf("DoAutomaticDenominating : Running Darksend denominate for %d rounds. Return '%s'\n", i, strError);
        if(strError == "") return true;
    }

    // We failed? That's strange but let's just make final attempt and try to mix everything
    strError = pwalletMain->PrepareDarksendDenominate(0, nDarksendRounds);
    LogPrintf("DoAutomaticDenominating : Running Darksend denominate for all rounds. Return '%s'\n", strError);
    if(strError == "") return true;

    // Should never actually get here but just in case
    strAutoDenomResult = strError;
    LogPrintf("DoAutomaticDenominating : Error running denominate, %s\n", strError);
    return false;
}
=======


bool CDarksendBroadcastTx::CheckSignature(const CPubKey& pubKeyMasternode)
>>>>>>> dev-1.12.1.0

{

    std::string strMessage = tx.GetHash().ToString() + boost::lexical_cast<std::string>(sigTime);

    std::string strError = "";



    if(!darkSendSigner.VerifyMessage(pubKeyMasternode, vchSig, strMessage, strError)) {

        LogPrintf("CDarksendBroadcastTx::CheckSignature -- Got bad dstx signature, error: %s\n", strError);

<<<<<<< HEAD
    CCoinControl *coinControl=NULL;
    bool success = pwalletMain->CreateTransaction(vecSend, wtx, reservekey, nFeeRet, strFail, coinControl, ONLY_DENOMINATED);
    if(!success){
        LogPrintf("SendRandomPaymentToSelf: Error - %s\n", strFail);
=======
>>>>>>> dev-1.12.1.0
        return false;

    }


<<<<<<< HEAD
    LogPrintf("SendRandomPaymentToSelf Success: tx %s\n", wtx.GetHash().GetHex());
=======
>>>>>>> dev-1.12.1.0

    return true;

}

<<<<<<< HEAD
// Split up large inputs or create fee sized inputs
bool CDarksendPool::MakeCollateralAmounts()
{
    CWalletTx wtx;
    int64_t nFeeRet = 0;
    std::string strFail = "";
    vector< pair<CScript, int64_t> > vecSend;
    CCoinControl *coinControl = NULL;

    // make our collateral address
    CReserveKey reservekeyCollateral(pwalletMain);
    // make our change address
    CReserveKey reservekeyChange(pwalletMain);

    CScript scriptCollateral;
    CPubKey vchPubKey;
    assert(reservekeyCollateral.GetReservedKey(vchPubKey)); // should never fail, as we just unlocked
    scriptCollateral = GetScriptForDestination(vchPubKey.GetID());

    vecSend.push_back(make_pair(scriptCollateral, DARKSEND_COLLATERAL*4));

    // try to use non-denominated and not mn-like funds
    bool success = pwalletMain->CreateTransaction(vecSend, wtx, reservekeyChange,
            nFeeRet, strFail, coinControl, ONLY_NONDENOMINATED_NOT1000IFMN);
    if(!success){
        // if we failed (most likeky not enough funds), try to use all coins instead -
        // MN-like funds should not be touched in any case and we can't mix denominated without collaterals anyway
        LogPrintf("MakeCollateralAmounts: ONLY_NONDENOMINATED_NOT1000IFMN Error - %s\n", strFail);
        success = pwalletMain->CreateTransaction(vecSend, wtx, reservekeyChange,
                nFeeRet, strFail, coinControl, ONLY_NOT1000IFMN);
        if(!success){
            LogPrintf("MakeCollateralAmounts: ONLY_NOT1000IFMN Error - %s\n", strFail);
            reservekeyCollateral.ReturnKey();
            return false;
        }
    }

    reservekeyCollateral.KeepKey();

    LogPrintf("MakeCollateralAmounts: tx %s\n", wtx.GetHash().GetHex());

    // use the same cachedLastSuccess as for DS mixinx to prevent race
    if(!pwalletMain->CommitTransaction(wtx, reservekeyChange)) {
        LogPrintf("MakeCollateralAmounts: CommitTransaction failed!\n");
        return false;
    }

    cachedLastSuccess = chainActive.Tip()->nHeight;
=======


void CDarksendPool::RelayFinalTransaction(const CTransaction& txFinal)

{

    LOCK(cs_vNodes);

    BOOST_FOREACH(CNode* pnode, vNodes)

        if(pnode->nVersion >= MIN_PRIVATESEND_PEER_PROTO_VERSION)

            pnode->PushMessage(NetMsgType::DSFINALTX, nSessionID, txFinal);
>>>>>>> dev-1.12.1.0

}

<<<<<<< HEAD
// Create denominations
bool CDarksendPool::CreateDenominated(int64_t nTotalValue)
{
    CWalletTx wtx;
    int64_t nFeeRet = 0;
    std::string strFail = "";
    vector< pair<CScript, int64_t> > vecSend;
    int64_t nValueLeft = nTotalValue;

    // make our collateral address
    CReserveKey reservekeyCollateral(pwalletMain);
    // make our change address
    CReserveKey reservekeyChange(pwalletMain);
    // make our denom addresses
    CReserveKey reservekeyDenom(pwalletMain);

    CScript scriptCollateral;
    CPubKey vchPubKey;
    assert(reservekeyCollateral.GetReservedKey(vchPubKey)); // should never fail, as we just unlocked
    scriptCollateral = GetScriptForDestination(vchPubKey.GetID());

    // ****** Add collateral outputs ************ /
    if(!pwalletMain->HasCollateralInputs()) {
        vecSend.push_back(make_pair(scriptCollateral, DARKSEND_COLLATERAL*4));
        nValueLeft -= DARKSEND_COLLATERAL*4;
    }
=======


void CDarksendPool::RelayIn(const CDarkSendEntry& entry)

{
>>>>>>> dev-1.12.1.0

    if(!pSubmittedToMasternode) return;

<<<<<<< HEAD
        // add each output up to 10 times until it can't be added again
        while(nValueLeft - v >= DARKSEND_COLLATERAL && nOutputs <= 10) {
            CScript scriptDenom;
            CPubKey vchPubKey;
            //use a unique change address
            assert(reservekeyDenom.GetReservedKey(vchPubKey)); // should never fail, as we just unlocked
            scriptDenom = GetScriptForDestination(vchPubKey.GetID());
            // TODO: do not keep reservekeyDenom here
            reservekeyDenom.KeepKey();

            vecSend.push_back(make_pair(scriptDenom, v));
=======

>>>>>>> dev-1.12.1.0

    CNode* pnode = FindNode(pSubmittedToMasternode->addr);

    if(pnode != NULL) {

        LogPrintf("CDarksendPool::RelayIn -- found master, relaying message to %s\n", pnode->addr.ToString());

        pnode->PushMessage(NetMsgType::DSVIN, entry);

<<<<<<< HEAD
    CCoinControl *coinControl=NULL;
    bool success = pwalletMain->CreateTransaction(vecSend, wtx, reservekeyChange,
            nFeeRet, strFail, coinControl, ONLY_NONDENOMINATED_NOT1000IFMN);
    if(!success){
        LogPrintf("CreateDenominated: Error - %s\n", strFail);
        // TODO: return reservekeyDenom here
        reservekeyCollateral.ReturnKey();
        return false;
    }

    // TODO: keep reservekeyDenom here
    reservekeyCollateral.KeepKey();

    // use the same cachedLastSuccess as for DS mixinx to prevent race
    if(pwalletMain->CommitTransaction(wtx, reservekeyChange))
        cachedLastSuccess = chainActive.Tip()->nHeight;
    else
        LogPrintf("CreateDenominated: CommitTransaction failed!\n");

    LogPrintf("CreateDenominated: tx %s\n", wtx.GetHash().GetHex());
=======
    }

}

>>>>>>> dev-1.12.1.0


void CDarksendPool::PushStatus(CNode* pnode, PoolStatusUpdate nStatusUpdate, PoolMessage nMessageID)

{

<<<<<<< HEAD
    BOOST_FOREACH(const CDarkSendEntry v, entries) {
        LogPrintf(" IsCompatibleWithEntries %d %d\n", GetDenominations(vout), GetDenominations(v.vout));
/*
        BOOST_FOREACH(CTxOut o1, vout)
            LogPrintf(" vout 1 - %s\n", o1.ToString());

        BOOST_FOREACH(CTxOut o2, v.vout)
            LogPrintf(" vout 2 - %s\n", o2.ToString());
*/
        if(GetDenominations(vout) != GetDenominations(v.vout)) return false;
    }
=======
    if(!pnode) return;

    pnode->PushMessage(NetMsgType::DSSTATUSUPDATE, nSessionID, (int)nState, (int)vecEntries.size(), (int)nStatusUpdate, (int)nMessageID);
>>>>>>> dev-1.12.1.0

}



void CDarksendPool::RelayStatus(PoolStatusUpdate nStatusUpdate, PoolMessage nMessageID)

{

    LOCK(cs_vNodes);

<<<<<<< HEAD
    if (!unitTest && !IsCollateralValid(txCollateral)){
        LogPrint("darksend", "CDarksendPool::IsCompatibleWithSession - collateral not valid!\n");
        errorID = ERR_INVALID_COLLATERAL;
        return false;
    }
=======
    BOOST_FOREACH(CNode* pnode, vNodes)
>>>>>>> dev-1.12.1.0

        if(pnode->nVersion >= MIN_PRIVATESEND_PEER_PROTO_VERSION)

<<<<<<< HEAD
    if(sessionUsers == 0) {
        sessionID = 1 + (rand() % 999999);
        sessionDenom = nDenom;
        sessionUsers++;
        lastTimeChanged = GetTimeMillis();
=======
            PushStatus(pnode, nStatusUpdate, nMessageID);
>>>>>>> dev-1.12.1.0

}



void CDarksendPool::RelayCompletedTransaction(PoolMessage nMessageID)

{

    LOCK(cs_vNodes);

    BOOST_FOREACH(CNode* pnode, vNodes)

        if(pnode->nVersion >= MIN_PRIVATESEND_PEER_PROTO_VERSION)

            pnode->PushMessage(NetMsgType::DSCOMPLETE, nSessionID, (int)nMessageID);

}



void CDarksendPool::SetState(PoolState nStateNew)

{

    if(fMasterNode && (nStateNew == POOL_STATE_ERROR || nStateNew == POOL_STATE_SUCCESS)) {

        LogPrint("privatesend", "CDarksendPool::SetState -- Can't set state to ERROR or SUCCESS as a Masternode. \n");

        return;

    }



    LogPrintf("CDarksendPool::SetState -- nState: %d, nStateNew: %d\n", nState, nStateNew);

    nState = nStateNew;

}

<<<<<<< HEAD
// return a bitshifted integer representing the denominations in this list
int CDarksendPool::GetDenominations(const std::vector<CTxOut>& vout, bool fSingleRandomDenom){
    std::vector<pair<int64_t, int> > denomUsed;
=======
>>>>>>> dev-1.12.1.0


void CDarksendPool::UpdatedBlockTip(const CBlockIndex *pindex)

<<<<<<< HEAD
    int denom = 0;
    int c = 0;
    // if the denomination is used, shift the bit on.
    // then move to the next
    BOOST_FOREACH (PAIRTYPE(int64_t, int)& s, denomUsed) {
        int bit = (fSingleRandomDenom ? rand()%2 : 1) * s.second;
        denom |= bit << c++;
        if(fSingleRandomDenom && bit) break; // use just one random denomination
    }

    // Function returns as follows:
    //
    // bit 0 - 100DRK+1 ( bit on if present )
    // bit 1 - 10DRK+1
    // bit 2 - 1DRK+1
    // bit 3 - .1DRK+1

    return denom;
}


int CDarksendPool::GetDenominationsByAmounts(std::vector<int64_t>& vecAmount){
    CScript e = CScript();
    std::vector<CTxOut> vout1;

    // Make outputs by looping through denominations, from small to large
    BOOST_REVERSE_FOREACH(int64_t v, vecAmount){
        CTxOut o(v, e);
        vout1.push_back(o);
    }

    return GetDenominations(vout1, true);
=======
{

    pCurrentBlockIndex = pindex;

    LogPrint("privatesend", "CDarksendPool::UpdatedBlockTip -- pCurrentBlockIndex->nHeight: %d\n", pCurrentBlockIndex->nHeight);



    if(!fLiteMode && masternodeSync.IsMasternodeListSynced()) {

        NewBlock();

    }

>>>>>>> dev-1.12.1.0
}



//TODO: Rename/move to core

void ThreadCheckDarkSendPool()

{

    if(fLiteMode) return; // disable all Curium specific functionality

<<<<<<< HEAD
std::string CDarksendPool::GetMessageByID(int messageID) {
    switch (messageID) {
    case ERR_ALREADY_HAVE: return _("Already have that input.");
    case ERR_DENOM: return _("No matching denominations found for mixing.");
    case ERR_ENTRIES_FULL: return _("Entries are full.");
    case ERR_EXISTING_TX: return _("Not compatible with existing transactions.");
    case ERR_FEES: return _("Transaction fees are too high.");
    case ERR_INVALID_COLLATERAL: return _("Collateral not valid.");
    case ERR_INVALID_INPUT: return _("Input is not valid.");
    case ERR_INVALID_SCRIPT: return _("Invalid script detected.");
    case ERR_INVALID_TX: return _("Transaction not valid.");
    case ERR_MAXIMUM: return _("Value more than Darksend pool maximum allows.");
    case ERR_MN_LIST: return _("Not in the Masternode list.");
    case ERR_MODE: return _("Incompatible mode.");
    case ERR_NON_STANDARD_PUBKEY: return _("Non-standard public key detected.");
    case ERR_NOT_A_MN: return _("This is not a Masternode.");
    case ERR_QUEUE_FULL: return _("Masternode queue is full.");
    case ERR_RECENT: return _("Last Darksend was too recent.");
    case ERR_SESSION: return _("Session not complete!");
    case ERR_MISSING_TX: return _("Missing input transaction information.");
    case ERR_VERSION: return _("Incompatible version.");
    case MSG_SUCCESS: return _("Transaction created successfully.");
    case MSG_ENTRIES_ADDED: return _("Your entries added successfully.");
    case MSG_NOERR:
    default:
        return "";
    }
}
=======
>>>>>>> dev-1.12.1.0


    static bool fOneThread;

    if(fOneThread) return;

    fOneThread = true;



    // Make this thread recognisable as the PrivateSend thread

    RenameThread("curium-privatesend");



    unsigned int nTick = 0;

    unsigned int nDoAutoNextRun = nTick + PRIVATESEND_AUTO_TIMEOUT_MIN;

<<<<<<< HEAD
    if (pubkey2.GetID() != pubkey.GetID()) {
        errorMessage = strprintf("keys don't match - input: %s, recovered: %s, message: %s, sig: %s\n",
                    pubkey.GetID().ToString(), pubkey2.GetID().ToString(), strMessage,
                    EncodeBase64(&vchSig[0], vchSig.size()));
        return false;
    }

    return true;
}
=======

>>>>>>> dev-1.12.1.0

    while (true)

    {

        MilliSleep(1000);

<<<<<<< HEAD
    if(!darkSendSigner.SetKey(strMasterNodePrivKey, errorMessage, key2, pubkey2))
    {
        LogPrintf("CDarksendQueue():Relay - ERROR: Invalid Masternodeprivkey: '%s'\n", errorMessage);
        return false;
    }
=======
>>>>>>> dev-1.12.1.0


        // try to sync from all available nodes, one step at a time

        masternodeSync.ProcessTick();



        if(masternodeSync.IsBlockchainSynced() && !ShutdownRequested()) {



            nTick++;



            // make sure to check all masternodes first

            mnodeman.Check();

<<<<<<< HEAD
void CDarksendPool::RelayIn(const std::vector<CTxDSIn>& vin, const int64_t& nAmount, const CTransaction& txCollateral, const std::vector<CTxDSOut>& vout)
{
    if(!pSubmittedToMasternode) return;
=======
>>>>>>> dev-1.12.1.0


            // check if we should activate or ping every few minutes,

            // slightly postpone first run to give net thread a chance to connect to some peers

<<<<<<< HEAD
    CNode* pnode = FindNode(pSubmittedToMasternode->addr);
    if(pnode != NULL) {
        LogPrintf("RelayIn - found master, relaying message - %s \n", pnode->addr.ToString());
        pnode->PushMessage("dsi", vin2, nAmount, txCollateral, vout2);
    }
}
=======
            if(nTick % MASTERNODE_MIN_MNP_SECONDS == 15)
>>>>>>> dev-1.12.1.0

                activeMasternode.ManageState();



            if(nTick % 60 == 0) {

<<<<<<< HEAD
    unsigned int c = 0;

    while (true)
    {
        MilliSleep(1000);
        //LogPrintf("ThreadCheckDarkSendPool::check timeout\n");

        // try to sync from all available nodes, one step at a time
        masternodeSync.Process();

        if(masternodeSync.IsBlockchainSynced()) {

            c++;

            // check if we should activate or ping every few minutes,
            // start right after sync is considered to be done
            if(c % MASTERNODE_PING_SECONDS == 1) activeMasternode.ManageStatus();

            if(c % 60 == 0)
            {
                mnodeman.CheckAndRemove();
                mnodeman.ProcessMasternodeConnections();
                masternodePayments.CleanPaymentList();
                CleanTransactionLocksList();
            }

            //if(c % MASTERNODES_DUMP_SECONDS == 0) DumpMasternodes();

            darkSendPool.CheckTimeout();
            darkSendPool.CheckForCompleteQueue();

            if(darkSendPool.GetState() == POOL_STATUS_IDLE && c % 15 == 0){
                darkSendPool.DoAutomaticDenominating();
            }
=======
                mnodeman.ProcessMasternodeConnections();

                mnodeman.CheckAndRemove();

                mnpayments.CheckAndRemove();

                instantsend.CheckAndRemove();

            }



            darkSendPool.CheckTimeout();

            darkSendPool.CheckForCompleteQueue();



            if(nDoAutoNextRun == nTick) {

                darkSendPool.DoAutomaticDenominating();

                nDoAutoNextRun = nTick + PRIVATESEND_AUTO_TIMEOUT_MIN + GetRandInt(PRIVATESEND_AUTO_TIMEOUT_MAX - PRIVATESEND_AUTO_TIMEOUT_MIN);

            }

>>>>>>> dev-1.12.1.0
        }

    }

}
