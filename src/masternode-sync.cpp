<<<<<<< HEAD
// Copyright (c) 2014-2015 The Curium developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "main.h"
#include "activemasternode.h"
#include "masternode-sync.h"
#include "masternode-payments.h"
#include "masternode-budget.h"
#include "masternode.h"
#include "masternodeman.h"
#include "spork.h"
#include "util.h"
#include "addrman.h"

class CMasternodeSync;
CMasternodeSync masternodeSync;

CMasternodeSync::CMasternodeSync()
{
    Reset();
}

bool CMasternodeSync::IsSynced()
{
    return RequestedMasternodeAssets == MASTERNODE_SYNC_FINISHED;
}

bool CMasternodeSync::IsBlockchainSynced()
{
    static bool fBlockchainSynced = false;
    static int64_t lastProcess = GetTime();

    // if the last call to this function was more than 60 minutes ago (client was in sleep mode) reset the sync process
    if(GetTime() - lastProcess > 60*60) {
        Reset();
        fBlockchainSynced = false;
    }
    lastProcess = GetTime();

    if(fBlockchainSynced) return true;

    if (fImporting || fReindex) return false;

    TRY_LOCK(cs_main, lockMain);
    if(!lockMain) return false;

    CBlockIndex* pindex = chainActive.Tip();
    if(pindex == NULL) return false;


    if(pindex->nTime + 60*60 < GetTime())
        return false;

    fBlockchainSynced = true;

    return true;
}

void CMasternodeSync::Reset()
{   
    lastMasternodeList = 0;
    lastMasternodeWinner = 0;
    lastBudgetItem = 0;
    mapSeenSyncMNB.clear();
    mapSeenSyncMNW.clear();
    mapSeenSyncBudget.clear();
    lastFailure = 0;
    nCountFailures = 0;
    sumMasternodeList = 0;
    sumMasternodeWinner = 0;
    sumBudgetItemProp = 0;
    sumBudgetItemFin = 0;
    countMasternodeList = 0;
    countMasternodeWinner = 0;
    countBudgetItemProp = 0;
    countBudgetItemFin = 0;
    RequestedMasternodeAssets = MASTERNODE_SYNC_INITIAL;
    RequestedMasternodeAttempt = 0;
    nAssetSyncStarted = GetTime();
}

void CMasternodeSync::AddedMasternodeList(uint256 hash)
{
    if(mnodeman.mapSeenMasternodeBroadcast.count(hash)) {
        if(mapSeenSyncMNB[hash] < MASTERNODE_SYNC_THRESHOLD) {
            lastMasternodeList = GetTime();
            mapSeenSyncMNB[hash]++;
        }
    } else {
        lastMasternodeList = GetTime();
        mapSeenSyncMNB.insert(make_pair(hash, 1));
    }
}

void CMasternodeSync::AddedMasternodeWinner(uint256 hash)
{
    if(masternodePayments.mapMasternodePayeeVotes.count(hash)) {
        if(mapSeenSyncMNW[hash] < MASTERNODE_SYNC_THRESHOLD) {
            lastMasternodeWinner = GetTime();
            mapSeenSyncMNW[hash]++;
        }
    } else {
        lastMasternodeWinner = GetTime();
        mapSeenSyncMNW.insert(make_pair(hash, 1));
    }
}

void CMasternodeSync::AddedBudgetItem(uint256 hash)
{
    if(budget.mapSeenMasternodeBudgetProposals.count(hash) || budget.mapSeenMasternodeBudgetVotes.count(hash) ||
            budget.mapSeenFinalizedBudgets.count(hash) || budget.mapSeenFinalizedBudgetVotes.count(hash)) {
        if(mapSeenSyncBudget[hash] < MASTERNODE_SYNC_THRESHOLD) {
            lastBudgetItem = GetTime();
            mapSeenSyncBudget[hash]++;
        }
    } else {
        lastBudgetItem = GetTime();
        mapSeenSyncBudget.insert(make_pair(hash, 1));
    }
}

bool CMasternodeSync::IsBudgetPropEmpty()
{
    return sumBudgetItemProp==0 && countBudgetItemProp>0;
}

bool CMasternodeSync::IsBudgetFinEmpty()
{
    return sumBudgetItemFin==0 && countBudgetItemFin>0;
}

void CMasternodeSync::GetNextAsset()
{
    switch(RequestedMasternodeAssets)
    {
        case(MASTERNODE_SYNC_INITIAL):
        case(MASTERNODE_SYNC_FAILED): // should never be used here actually, use Reset() instead
            ClearFulfilledRequest();
            RequestedMasternodeAssets = MASTERNODE_SYNC_SPORKS;
            break;
        case(MASTERNODE_SYNC_SPORKS):
            RequestedMasternodeAssets = MASTERNODE_SYNC_LIST;
            break;
        case(MASTERNODE_SYNC_LIST):
            RequestedMasternodeAssets = MASTERNODE_SYNC_MNW;
            break;
        case(MASTERNODE_SYNC_MNW):
            RequestedMasternodeAssets = MASTERNODE_SYNC_BUDGET;
            break;
        case(MASTERNODE_SYNC_BUDGET):
            LogPrintf("CMasternodeSync::GetNextAsset - Sync has finished\n");
            RequestedMasternodeAssets = MASTERNODE_SYNC_FINISHED;
            break;
    }
    RequestedMasternodeAttempt = 0;
    nAssetSyncStarted = GetTime();
}

std::string CMasternodeSync::GetSyncStatus()
{
    switch (masternodeSync.RequestedMasternodeAssets) {
        case MASTERNODE_SYNC_INITIAL: return _("Synchronization pending...");
        case MASTERNODE_SYNC_SPORKS: return _("Synchronizing sporks...");
        case MASTERNODE_SYNC_LIST: return _("Synchronizing masternodes...");
        case MASTERNODE_SYNC_MNW: return _("Synchronizing masternode winners...");
        case MASTERNODE_SYNC_BUDGET: return _("Synchronizing budgets...");
        case MASTERNODE_SYNC_FAILED: return _("Synchronization failed");
        case MASTERNODE_SYNC_FINISHED: return _("Synchronization finished");
    }
    return "";
}

void CMasternodeSync::ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    if (strCommand == "ssc") { //Sync status count
        int nItemID;
        int nCount;
        vRecv >> nItemID >> nCount;

        if(RequestedMasternodeAssets >= MASTERNODE_SYNC_FINISHED) return;

        //this means we will receive no further communication
        switch(nItemID)
        {
            case(MASTERNODE_SYNC_LIST):
                if(nItemID != RequestedMasternodeAssets) return;
                sumMasternodeList += nCount;
                countMasternodeList++;
                break;
            case(MASTERNODE_SYNC_MNW):
                if(nItemID != RequestedMasternodeAssets) return;
                sumMasternodeWinner += nCount;
                countMasternodeWinner++;
                break;
            case(MASTERNODE_SYNC_BUDGET_PROP):
                if(RequestedMasternodeAssets != MASTERNODE_SYNC_BUDGET) return;
                sumBudgetItemProp += nCount;
                countBudgetItemProp++;
                break;
            case(MASTERNODE_SYNC_BUDGET_FIN):
                if(RequestedMasternodeAssets != MASTERNODE_SYNC_BUDGET) return;
                sumBudgetItemFin += nCount;
                countBudgetItemFin++;
                break;
        }
        
        LogPrintf("CMasternodeSync:ProcessMessage - ssc - got inventory count %d %d\n", nItemID, nCount);
    }
}

void CMasternodeSync::ClearFulfilledRequest()
{
    TRY_LOCK(cs_vNodes, lockRecv);
    if(!lockRecv) return;

    BOOST_FOREACH(CNode* pnode, vNodes)
    {
        pnode->ClearFulfilledRequest("getspork");
        pnode->ClearFulfilledRequest("mnsync");
        pnode->ClearFulfilledRequest("mnwsync");
        pnode->ClearFulfilledRequest("busync");
    }
}

void CMasternodeSync::Process()
{
    static int tick = 0;

    if(tick++ % MASTERNODE_SYNC_TIMEOUT != 0) return;

    if(IsSynced()) {
        /* 
            Resync if we lose all masternodes from sleep/wake or failure to sync originally
        */
        if(mnodeman.CountEnabled() == 0) {
            Reset();
        } else
            return;
    }

    //try syncing again
    if(RequestedMasternodeAssets == MASTERNODE_SYNC_FAILED && lastFailure + (1*60) < GetTime()) {
        Reset();
    } else if (RequestedMasternodeAssets == MASTERNODE_SYNC_FAILED) {
        return;
    }

    if(fDebug) LogPrintf("CMasternodeSync::Process() - tick %d RequestedMasternodeAssets %d\n", tick, RequestedMasternodeAssets);

    if(RequestedMasternodeAssets == MASTERNODE_SYNC_INITIAL) GetNextAsset();

    // sporks synced but blockchain is not, wait until we're almost at a recent block to continue
    if(Params().NetworkID() != CBaseChainParams::REGTEST &&
            !IsBlockchainSynced() && RequestedMasternodeAssets > MASTERNODE_SYNC_SPORKS) return;

    TRY_LOCK(cs_vNodes, lockRecv);
    if(!lockRecv) return;

    BOOST_FOREACH(CNode* pnode, vNodes)
    {
        if(Params().NetworkID() == CBaseChainParams::REGTEST){
            if(RequestedMasternodeAttempt <= 2) {
                pnode->PushMessage("getsporks"); //get current network sporks
            } else if(RequestedMasternodeAttempt < 4) {
                mnodeman.DsegUpdate(pnode); 
            } else if(RequestedMasternodeAttempt < 6) {
                int nMnCount = mnodeman.CountEnabled();
                pnode->PushMessage("mnget", nMnCount); //sync payees
                uint256 n = 0;
                pnode->PushMessage("mnvs", n); //sync masternode votes
            } else {
                RequestedMasternodeAssets = MASTERNODE_SYNC_FINISHED;
            }
            RequestedMasternodeAttempt++;
            return;
        }

        //set to synced
        if(RequestedMasternodeAssets == MASTERNODE_SYNC_SPORKS){
            if(pnode->HasFulfilledRequest("getspork")) continue;
            pnode->FulfilledRequest("getspork");

            pnode->PushMessage("getsporks"); //get current network sporks
            if(RequestedMasternodeAttempt >= 2) GetNextAsset();
            RequestedMasternodeAttempt++;
            
            return;
        }

        if (pnode->nVersion >= masternodePayments.GetMinMasternodePaymentsProto()) {

            if(RequestedMasternodeAssets == MASTERNODE_SYNC_LIST) {
                if(fDebug) LogPrintf("CMasternodeSync::Process() - lastMasternodeList %lld (GetTime() - MASTERNODE_SYNC_TIMEOUT) %lld\n", lastMasternodeList, GetTime() - MASTERNODE_SYNC_TIMEOUT);
                if(lastMasternodeList > 0 && lastMasternodeList < GetTime() - MASTERNODE_SYNC_TIMEOUT*2 && RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD){ //hasn't received a new item in the last five seconds, so we'll move to the
                    GetNextAsset();
                    return;
                }

                if(pnode->HasFulfilledRequest("mnsync")) continue;
                pnode->FulfilledRequest("mnsync");

                // timeout
                if(lastMasternodeList == 0 &&
                (RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD*3 || GetTime() - nAssetSyncStarted > MASTERNODE_SYNC_TIMEOUT*5)) {
                    if(IsSporkActive(SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT)) {
                        LogPrintf("CMasternodeSync::Process - ERROR - Sync has failed, will retry later\n");
                        RequestedMasternodeAssets = MASTERNODE_SYNC_FAILED;
                        RequestedMasternodeAttempt = 0;
                        lastFailure = GetTime();
                        nCountFailures++;
                    } else {
                        GetNextAsset();
                    }
                    return;
                }

                if(RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD*3) return;

                mnodeman.DsegUpdate(pnode);
                RequestedMasternodeAttempt++;
                return;
            }

            if(RequestedMasternodeAssets == MASTERNODE_SYNC_MNW) {
                if(lastMasternodeWinner > 0 && lastMasternodeWinner < GetTime() - MASTERNODE_SYNC_TIMEOUT*2 && RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD){ //hasn't received a new item in the last five seconds, so we'll move to the
                    GetNextAsset();
                    return;
                }

                if(pnode->HasFulfilledRequest("mnwsync")) continue;
                pnode->FulfilledRequest("mnwsync");

                // timeout
                if(lastMasternodeWinner == 0 &&
                (RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD*3 || GetTime() - nAssetSyncStarted > MASTERNODE_SYNC_TIMEOUT*5)) {
                    if(IsSporkActive(SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT)) {
                        LogPrintf("CMasternodeSync::Process - ERROR - Sync has failed, will retry later\n");
                        RequestedMasternodeAssets = MASTERNODE_SYNC_FAILED;
                        RequestedMasternodeAttempt = 0;
                        lastFailure = GetTime();
                        nCountFailures++;
                    } else {
                        GetNextAsset();
                    }
                    return;
                }

                if(RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD*3) return;

                CBlockIndex* pindexPrev = chainActive.Tip();
                if(pindexPrev == NULL) return;

                int nMnCount = mnodeman.CountEnabled();
                pnode->PushMessage("mnget", nMnCount); //sync payees
                RequestedMasternodeAttempt++;

                return;
            }
        }

        if (pnode->nVersion >= MIN_BUDGET_PEER_PROTO_VERSION) {

            if(RequestedMasternodeAssets == MASTERNODE_SYNC_BUDGET){
                //we'll start rejecting votes if we accidentally get set as synced too soon
                if(lastBudgetItem > 0 && lastBudgetItem < GetTime() - MASTERNODE_SYNC_TIMEOUT*2 && RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD){ //hasn't received a new item in the last five seconds, so we'll move to the
                    //LogPrintf("CMasternodeSync::Process - HasNextFinalizedBudget %d nCountFailures %d IsBudgetPropEmpty %d\n", budget.HasNextFinalizedBudget(), nCountFailures, IsBudgetPropEmpty());
                    //if(budget.HasNextFinalizedBudget() || nCountFailures >= 2 || IsBudgetPropEmpty()) {
                        GetNextAsset();

                        //try to activate our masternode if possible
                        activeMasternode.ManageStatus();
                    // } else { //we've failed to sync, this state will reject the next budget block
                    //     LogPrintf("CMasternodeSync::Process - ERROR - Sync has failed, will retry later\n");
                    //     RequestedMasternodeAssets = MASTERNODE_SYNC_FAILED;
                    //     RequestedMasternodeAttempt = 0;
                    //     lastFailure = GetTime();
                    //     nCountFailures++;
                    // }
                    return;
                }

                // timeout
                if(lastBudgetItem == 0 &&
                (RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD*3 || GetTime() - nAssetSyncStarted > MASTERNODE_SYNC_TIMEOUT*5)) {
                    // maybe there is no budgets at all, so just finish syncing
                    GetNextAsset();
                    activeMasternode.ManageStatus();
                    return;
                }

                if(pnode->HasFulfilledRequest("busync")) continue;
                pnode->FulfilledRequest("busync");

                if(RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD*3) return;

                uint256 n = 0;
                pnode->PushMessage("mnvs", n); //sync masternode votes
                RequestedMasternodeAttempt++;
                
                return;
            }

        }
    }
}
=======
// Copyright (c) 2014-2017 The Dash Core developers

// Distributed under the MIT/X11 software license, see the accompanying

// file COPYING or http://www.opensource.org/licenses/mit-license.php.



#include "activemasternode.h"

#include "checkpoints.h"

#include "governance.h"

#include "main.h"

#include "masternode.h"

#include "masternode-payments.h"

#include "masternode-sync.h"

#include "masternodeman.h"

#include "netfulfilledman.h"

#include "spork.h"

#include "util.h"



class CMasternodeSync;

CMasternodeSync masternodeSync;



void ReleaseNodes(const std::vector<CNode*> &vNodesCopy)

{

    LOCK(cs_vNodes);

    BOOST_FOREACH(CNode* pnode, vNodesCopy)

        pnode->Release();

}



bool CMasternodeSync::CheckNodeHeight(CNode* pnode, bool fDisconnectStuckNodes)

{

    CNodeStateStats stats;

    if(!GetNodeStateStats(pnode->id, stats) || stats.nCommonHeight == -1 || stats.nSyncHeight == -1) return false; // not enough info about this peer



    // Check blocks and headers, allow a small error margin of 1 block

    if(pCurrentBlockIndex->nHeight - 1 > stats.nCommonHeight) {

        // This peer probably stuck, don't sync any additional data from it

        if(fDisconnectStuckNodes) {

            // Disconnect to free this connection slot for another peer.

            pnode->fDisconnect = true;

            LogPrintf("CMasternodeSync::CheckNodeHeight -- disconnecting from stuck peer, nHeight=%d, nCommonHeight=%d, peer=%d\n",

                        pCurrentBlockIndex->nHeight, stats.nCommonHeight, pnode->id);

        } else {

            LogPrintf("CMasternodeSync::CheckNodeHeight -- skipping stuck peer, nHeight=%d, nCommonHeight=%d, peer=%d\n",

                        pCurrentBlockIndex->nHeight, stats.nCommonHeight, pnode->id);

        }

        return false;

    }

    else if(pCurrentBlockIndex->nHeight < stats.nSyncHeight - 1) {

        // This peer announced more headers than we have blocks currently

        LogPrintf("CMasternodeSync::CheckNodeHeight -- skipping peer, who announced more headers than we have blocks currently, nHeight=%d, nSyncHeight=%d, peer=%d\n",

                    pCurrentBlockIndex->nHeight, stats.nSyncHeight, pnode->id);

        return false;

    }



    return true;

}



bool CMasternodeSync::IsBlockchainSynced(bool fBlockAccepted)

{

    static bool fBlockchainSynced = false;

    static int64_t nTimeLastProcess = GetTime();

    static int nSkipped = 0;

    static bool fFirstBlockAccepted = false;



    // if the last call to this function was more than 60 minutes ago (client was in sleep mode) reset the sync process

    if(GetTime() - nTimeLastProcess > 60*60) {

        Reset();

        fBlockchainSynced = false;

    }



    if(!pCurrentBlockIndex || !pindexBestHeader || fImporting || fReindex) return false;



    if(fBlockAccepted) {

        // this should be only triggered while we are still syncing

        if(!IsSynced()) {

            // we are trying to download smth, reset blockchain sync status

            if(fDebug) LogPrintf("CMasternodeSync::IsBlockchainSynced -- reset\n");

            fFirstBlockAccepted = true;

            fBlockchainSynced = false;

            nTimeLastProcess = GetTime();

            return false;

        }

    } else {

        // skip if we already checked less than 1 tick ago

        if(GetTime() - nTimeLastProcess < MASTERNODE_SYNC_TICK_SECONDS) {

            nSkipped++;

            return fBlockchainSynced;

        }

    }



    if(fDebug) LogPrintf("CMasternodeSync::IsBlockchainSynced -- state before check: %ssynced, skipped %d times\n", fBlockchainSynced ? "" : "not ", nSkipped);



    nTimeLastProcess = GetTime();

    nSkipped = 0;



    if(fBlockchainSynced) return true;



    if(fCheckpointsEnabled && pCurrentBlockIndex->nHeight < Checkpoints::GetTotalBlocksEstimate(Params().Checkpoints()))

        return false;



    std::vector<CNode*> vNodesCopy;

    {

        LOCK(cs_vNodes);

        vNodesCopy = vNodes;

        BOOST_FOREACH(CNode* pnode, vNodesCopy)

            pnode->AddRef();

    }



    // We have enough peers and assume most of them are synced

    if(vNodes.size() >= MASTERNODE_SYNC_ENOUGH_PEERS) {

        // Check to see how many of our peers are (almost) at the same height as we are

        int nNodesAtSameHeight = 0;

        BOOST_FOREACH(CNode* pnode, vNodesCopy)

        {

            // Make sure this peer is presumably at the same height

            if(!CheckNodeHeight(pnode)) continue;

            nNodesAtSameHeight++;

            // if we have decent number of such peers, most likely we are synced now

            if(nNodesAtSameHeight >= MASTERNODE_SYNC_ENOUGH_PEERS) {

                LogPrintf("CMasternodeSync::IsBlockchainSynced -- found enough peers on the same height as we are, done\n");

                fBlockchainSynced = true;

                ReleaseNodes(vNodesCopy);

                return true;

            }

        }

    }

    ReleaseNodes(vNodesCopy);



    // wait for at least one new block to be accepted

    if(!fFirstBlockAccepted) return false;



    // same as !IsInitialBlockDownload() but no cs_main needed here

    int64_t nMaxBlockTime = std::max(pCurrentBlockIndex->GetBlockTime(), pindexBestHeader->GetBlockTime());

    fBlockchainSynced = pindexBestHeader->nHeight - pCurrentBlockIndex->nHeight < 24 * 6 &&

                        GetTime() - nMaxBlockTime < Params().MaxTipAge();



    return fBlockchainSynced;

}



void CMasternodeSync::Fail()

{

    nTimeLastFailure = GetTime();

    nRequestedMasternodeAssets = MASTERNODE_SYNC_FAILED;

}



void CMasternodeSync::Reset()

{

    nRequestedMasternodeAssets = MASTERNODE_SYNC_INITIAL;

    nRequestedMasternodeAttempt = 0;

    nTimeAssetSyncStarted = GetTime();

    nTimeLastMasternodeList = GetTime();

    nTimeLastPaymentVote = GetTime();

    nTimeLastGovernanceItem = GetTime();

    nTimeLastFailure = 0;

    nCountFailures = 0;

}



std::string CMasternodeSync::GetAssetName()

{

    switch(nRequestedMasternodeAssets)

    {

        case(MASTERNODE_SYNC_INITIAL):      return "MASTERNODE_SYNC_INITIAL";

        case(MASTERNODE_SYNC_SPORKS):       return "MASTERNODE_SYNC_SPORKS";

        case(MASTERNODE_SYNC_LIST):         return "MASTERNODE_SYNC_LIST";

        case(MASTERNODE_SYNC_MNW):          return "MASTERNODE_SYNC_MNW";

        case(MASTERNODE_SYNC_GOVERNANCE):   return "MASTERNODE_SYNC_GOVERNANCE";

        case(MASTERNODE_SYNC_FAILED):       return "MASTERNODE_SYNC_FAILED";

        case MASTERNODE_SYNC_FINISHED:      return "MASTERNODE_SYNC_FINISHED";

        default:                            return "UNKNOWN";

    }

}



void CMasternodeSync::SwitchToNextAsset()

{

    switch(nRequestedMasternodeAssets)

    {

        case(MASTERNODE_SYNC_FAILED):

            throw std::runtime_error("Can't switch to next asset from failed, should use Reset() first!");

            break;

        case(MASTERNODE_SYNC_INITIAL):

            ClearFulfilledRequests();

            nRequestedMasternodeAssets = MASTERNODE_SYNC_SPORKS;

            LogPrintf("CMasternodeSync::SwitchToNextAsset -- Starting %s\n", GetAssetName());

            break;

        case(MASTERNODE_SYNC_SPORKS):

            nTimeLastMasternodeList = GetTime();

            nRequestedMasternodeAssets = MASTERNODE_SYNC_LIST;

            LogPrintf("CMasternodeSync::SwitchToNextAsset -- Starting %s\n", GetAssetName());

            break;

        case(MASTERNODE_SYNC_LIST):

            nTimeLastPaymentVote = GetTime();

            nRequestedMasternodeAssets = MASTERNODE_SYNC_MNW;

            LogPrintf("CMasternodeSync::SwitchToNextAsset -- Starting %s\n", GetAssetName());

            break;

        case(MASTERNODE_SYNC_MNW):

            nTimeLastGovernanceItem = GetTime();

            nRequestedMasternodeAssets = MASTERNODE_SYNC_GOVERNANCE;

            LogPrintf("CMasternodeSync::SwitchToNextAsset -- Starting %s\n", GetAssetName());

            break;

        case(MASTERNODE_SYNC_GOVERNANCE):

            LogPrintf("CMasternodeSync::SwitchToNextAsset -- Sync has finished\n");

            nRequestedMasternodeAssets = MASTERNODE_SYNC_FINISHED;

            uiInterface.NotifyAdditionalDataSyncProgressChanged(1);

            //try to activate our masternode if possible

            activeMasternode.ManageState();



            TRY_LOCK(cs_vNodes, lockRecv);

            if(!lockRecv) return;



            BOOST_FOREACH(CNode* pnode, vNodes) {

                netfulfilledman.AddFulfilledRequest(pnode->addr, "full-sync");

            }



            break;

    }

    nRequestedMasternodeAttempt = 0;

    nTimeAssetSyncStarted = GetTime();

}



std::string CMasternodeSync::GetSyncStatus()

{

    switch (masternodeSync.nRequestedMasternodeAssets) {

        case MASTERNODE_SYNC_INITIAL:       return _("Synchronization pending...");

        case MASTERNODE_SYNC_SPORKS:        return _("Synchronizing sporks...");

        case MASTERNODE_SYNC_LIST:          return _("Synchronizing masternodes...");

        case MASTERNODE_SYNC_MNW:           return _("Synchronizing masternode payments...");

        case MASTERNODE_SYNC_GOVERNANCE:    return _("Synchronizing governance objects...");

        case MASTERNODE_SYNC_FAILED:        return _("Synchronization failed");

        case MASTERNODE_SYNC_FINISHED:      return _("Synchronization finished");

        default:                            return "";

    }

}



void CMasternodeSync::ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)

{

    if (strCommand == NetMsgType::SYNCSTATUSCOUNT) { //Sync status count



        //do not care about stats if sync process finished or failed

        if(IsSynced() || IsFailed()) return;



        int nItemID;

        int nCount;

        vRecv >> nItemID >> nCount;



        LogPrintf("SYNCSTATUSCOUNT -- got inventory count: nItemID=%d  nCount=%d  peer=%d\n", nItemID, nCount, pfrom->id);

    }

}



void CMasternodeSync::ClearFulfilledRequests()

{

    TRY_LOCK(cs_vNodes, lockRecv);

    if(!lockRecv) return;



    BOOST_FOREACH(CNode* pnode, vNodes)

    {

        netfulfilledman.RemoveFulfilledRequest(pnode->addr, "spork-sync");

        netfulfilledman.RemoveFulfilledRequest(pnode->addr, "masternode-list-sync");

        netfulfilledman.RemoveFulfilledRequest(pnode->addr, "masternode-payment-sync");

        netfulfilledman.RemoveFulfilledRequest(pnode->addr, "governance-sync");

        netfulfilledman.RemoveFulfilledRequest(pnode->addr, "full-sync");

    }

}



void CMasternodeSync::ProcessTick()

{

    static int nTick = 0;

    if(nTick++ % MASTERNODE_SYNC_TICK_SECONDS != 0) return;

    if(!pCurrentBlockIndex) return;



    //the actual count of masternodes we have currently

    int nMnCount = mnodeman.CountMasternodes();



    if(fDebug) LogPrintf("CMasternodeSync::ProcessTick -- nTick %d nMnCount %d\n", nTick, nMnCount);



    // RESET SYNCING INCASE OF FAILURE

    {

        if(IsSynced()) {

            /*

                Resync if we lose all masternodes from sleep/wake or failure to sync originally

            */

            if(nMnCount == 0) {

                LogPrintf("CMasternodeSync::ProcessTick -- WARNING: not enough data, restarting sync\n");

                Reset();

            } else {

                std::vector<CNode*> vNodesCopy;

                {

                    LOCK(cs_vNodes);

                    vNodesCopy = vNodes;

                    BOOST_FOREACH(CNode* pnode, vNodesCopy)

                        pnode->AddRef();

                }

                governance.RequestGovernanceObjectVotes(vNodesCopy);

                ReleaseNodes(vNodesCopy);

                return;

            }

        }



        //try syncing again

        if(IsFailed()) {

            if(nTimeLastFailure + (1*60) < GetTime()) { // 1 minute cooldown after failed sync

                Reset();

            }

            return;

        }

    }



    // INITIAL SYNC SETUP / LOG REPORTING

    double nSyncProgress = double(nRequestedMasternodeAttempt + (nRequestedMasternodeAssets - 1) * 8) / (8*4);

    LogPrintf("CMasternodeSync::ProcessTick -- nTick %d nRequestedMasternodeAssets %d nRequestedMasternodeAttempt %d nSyncProgress %f\n", nTick, nRequestedMasternodeAssets, nRequestedMasternodeAttempt, nSyncProgress);

    uiInterface.NotifyAdditionalDataSyncProgressChanged(nSyncProgress);



    // sporks synced but blockchain is not, wait until we're almost at a recent block to continue

    if(Params().NetworkIDString() != CBaseChainParams::REGTEST &&

            !IsBlockchainSynced() && nRequestedMasternodeAssets > MASTERNODE_SYNC_SPORKS)

    {

        LogPrintf("CMasternodeSync::ProcessTick -- nTick %d nRequestedMasternodeAssets %d nRequestedMasternodeAttempt %d -- blockchain is not synced yet\n", nTick, nRequestedMasternodeAssets, nRequestedMasternodeAttempt);

        nTimeLastMasternodeList = GetTime();

        nTimeLastPaymentVote = GetTime();

        nTimeLastGovernanceItem = GetTime();

        return;

    }



    if(nRequestedMasternodeAssets == MASTERNODE_SYNC_INITIAL ||

        (nRequestedMasternodeAssets == MASTERNODE_SYNC_SPORKS && IsBlockchainSynced()))

    {

        SwitchToNextAsset();

    }



    std::vector<CNode*> vNodesCopy;

    {

        LOCK(cs_vNodes);

        vNodesCopy = vNodes;

        BOOST_FOREACH(CNode* pnode, vNodesCopy)

            pnode->AddRef();

    }



    BOOST_FOREACH(CNode* pnode, vNodesCopy)

    {

        // QUICK MODE (REGTEST ONLY!)

        if(Params().NetworkIDString() == CBaseChainParams::REGTEST)

        {

            if(nRequestedMasternodeAttempt <= 2) {

                pnode->PushMessage(NetMsgType::GETSPORKS); //get current network sporks

            } else if(nRequestedMasternodeAttempt < 4) {

                mnodeman.DsegUpdate(pnode);

            } else if(nRequestedMasternodeAttempt < 6) {

                int nMnCount = mnodeman.CountMasternodes();

                pnode->PushMessage(NetMsgType::MASTERNODEPAYMENTSYNC, nMnCount); //sync payment votes

                SendGovernanceSyncRequest(pnode);

            } else {

                nRequestedMasternodeAssets = MASTERNODE_SYNC_FINISHED;

            }

            nRequestedMasternodeAttempt++;

            ReleaseNodes(vNodesCopy);

            return;

        }



        // NORMAL NETWORK MODE - TESTNET/MAINNET

        {

            if(netfulfilledman.HasFulfilledRequest(pnode->addr, "full-sync")) {

                // we already fully synced from this node recently,

                // disconnect to free this connection slot for a new node

                pnode->fDisconnect = true;

                LogPrintf("CMasternodeSync::ProcessTick -- disconnecting from recently synced peer %d\n", pnode->id);

                continue;

            }



            // Make sure this peer is presumably at the same height

            if(!CheckNodeHeight(pnode, true)) continue;



            // SPORK : ALWAYS ASK FOR SPORKS AS WE SYNC (we skip this mode now)



            if(!netfulfilledman.HasFulfilledRequest(pnode->addr, "spork-sync")) {

                // only request once from each peer

                netfulfilledman.AddFulfilledRequest(pnode->addr, "spork-sync");

                // get current network sporks

                pnode->PushMessage(NetMsgType::GETSPORKS);

                LogPrintf("CMasternodeSync::ProcessTick -- nTick %d nRequestedMasternodeAssets %d -- requesting sporks from peer %d\n", nTick, nRequestedMasternodeAssets, pnode->id);

                continue; // always get sporks first, switch to the next node without waiting for the next tick

            }



            // MNLIST : SYNC MASTERNODE LIST FROM OTHER CONNECTED CLIENTS



            if(nRequestedMasternodeAssets == MASTERNODE_SYNC_LIST) {

                LogPrint("masternode", "CMasternodeSync::ProcessTick -- nTick %d nRequestedMasternodeAssets %d nTimeLastMasternodeList %lld GetTime() %lld diff %lld\n", nTick, nRequestedMasternodeAssets, nTimeLastMasternodeList, GetTime(), GetTime() - nTimeLastMasternodeList);

                // check for timeout first

                if(nTimeLastMasternodeList < GetTime() - MASTERNODE_SYNC_TIMEOUT_SECONDS) {

                    LogPrintf("CMasternodeSync::ProcessTick -- nTick %d nRequestedMasternodeAssets %d -- timeout\n", nTick, nRequestedMasternodeAssets);

                    if (nRequestedMasternodeAttempt == 0) {

                        LogPrintf("CMasternodeSync::ProcessTick -- ERROR: failed to sync %s\n", GetAssetName());

                        // there is no way we can continue without masternode list, fail here and try later

                        Fail();

                        ReleaseNodes(vNodesCopy);

                        return;

                    }

                    SwitchToNextAsset();

                    ReleaseNodes(vNodesCopy);

                    return;

                }



                // only request once from each peer

                if(netfulfilledman.HasFulfilledRequest(pnode->addr, "masternode-list-sync")) continue;

                netfulfilledman.AddFulfilledRequest(pnode->addr, "masternode-list-sync");



                if (pnode->nVersion < mnpayments.GetMinMasternodePaymentsProto()) continue;

                nRequestedMasternodeAttempt++;



                mnodeman.DsegUpdate(pnode);



                ReleaseNodes(vNodesCopy);

                return; //this will cause each peer to get one request each six seconds for the various assets we need

            }



            // MNW : SYNC MASTERNODE PAYMENT VOTES FROM OTHER CONNECTED CLIENTS



            if(nRequestedMasternodeAssets == MASTERNODE_SYNC_MNW) {

                LogPrint("mnpayments", "CMasternodeSync::ProcessTick -- nTick %d nRequestedMasternodeAssets %d nTimeLastPaymentVote %lld GetTime() %lld diff %lld\n", nTick, nRequestedMasternodeAssets, nTimeLastPaymentVote, GetTime(), GetTime() - nTimeLastPaymentVote);

                // check for timeout first

                // This might take a lot longer than MASTERNODE_SYNC_TIMEOUT_SECONDS minutes due to new blocks,

                // but that should be OK and it should timeout eventually.

                if(nTimeLastPaymentVote < GetTime() - MASTERNODE_SYNC_TIMEOUT_SECONDS) {

                    LogPrintf("CMasternodeSync::ProcessTick -- nTick %d nRequestedMasternodeAssets %d -- timeout\n", nTick, nRequestedMasternodeAssets);

                    if (nRequestedMasternodeAttempt == 0) {

                        LogPrintf("CMasternodeSync::ProcessTick -- ERROR: failed to sync %s\n", GetAssetName());

                        // probably not a good idea to proceed without winner list

                        Fail();

                        ReleaseNodes(vNodesCopy);

                        return;

                    }

                    SwitchToNextAsset();

                    ReleaseNodes(vNodesCopy);

                    return;

                }



                // check for data

                // if mnpayments already has enough blocks and votes, switch to the next asset

                // try to fetch data from at least two peers though

                if(nRequestedMasternodeAttempt > 1 && mnpayments.IsEnoughData()) {

                    LogPrintf("CMasternodeSync::ProcessTick -- nTick %d nRequestedMasternodeAssets %d -- found enough data\n", nTick, nRequestedMasternodeAssets);

                    SwitchToNextAsset();

                    ReleaseNodes(vNodesCopy);

                    return;

                }



                // only request once from each peer

                if(netfulfilledman.HasFulfilledRequest(pnode->addr, "masternode-payment-sync")) continue;

                netfulfilledman.AddFulfilledRequest(pnode->addr, "masternode-payment-sync");



                if(pnode->nVersion < mnpayments.GetMinMasternodePaymentsProto()) continue;

                nRequestedMasternodeAttempt++;



                // ask node for all payment votes it has (new nodes will only return votes for future payments)

                pnode->PushMessage(NetMsgType::MASTERNODEPAYMENTSYNC, mnpayments.GetStorageLimit());

                // ask node for missing pieces only (old nodes will not be asked)

                mnpayments.RequestLowDataPaymentBlocks(pnode);



                ReleaseNodes(vNodesCopy);

                return; //this will cause each peer to get one request each six seconds for the various assets we need

            }



            // GOVOBJ : SYNC GOVERNANCE ITEMS FROM OUR PEERS



            if(nRequestedMasternodeAssets == MASTERNODE_SYNC_GOVERNANCE) {

                LogPrint("mnpayments", "CMasternodeSync::ProcessTick -- nTick %d nRequestedMasternodeAssets %d nTimeLastPaymentVote %lld GetTime() %lld diff %lld\n", nTick, nRequestedMasternodeAssets, nTimeLastPaymentVote, GetTime(), GetTime() - nTimeLastPaymentVote);



                // check for timeout first

                if(GetTime() - nTimeLastGovernanceItem > MASTERNODE_SYNC_TIMEOUT_SECONDS) {

                    LogPrintf("CMasternodeSync::ProcessTick -- nTick %d nRequestedMasternodeAssets %d -- timeout\n", nTick, nRequestedMasternodeAssets);

                    if(nRequestedMasternodeAttempt == 0) {

                        LogPrintf("CMasternodeSync::ProcessTick -- WARNING: failed to sync %s\n", GetAssetName());

                        // it's kind of ok to skip this for now, hopefully we'll catch up later?

                    }

                    SwitchToNextAsset();

                    ReleaseNodes(vNodesCopy);

                    return;

                }



                // check for data

                // if(nCountBudgetItemProp > 0 && nCountBudgetItemFin)

                // {

                //     if(governance.CountProposalInventoryItems() >= (nSumBudgetItemProp / nCountBudgetItemProp)*0.9)

                //     {

                //         if(governance.CountFinalizedInventoryItems() >= (nSumBudgetItemFin / nCountBudgetItemFin)*0.9)

                //         {

                //             SwitchToNextAsset();

                //             return;

                //         }

                //     }

                // }



                // only request obj sync once from each peer, then request votes on per-obj basis

                if(netfulfilledman.HasFulfilledRequest(pnode->addr, "governance-sync")) {

                    governance.RequestGovernanceObjectVotes(pnode);

                    continue;

                }

                netfulfilledman.AddFulfilledRequest(pnode->addr, "governance-sync");



                if (pnode->nVersion < MIN_GOVERNANCE_PEER_PROTO_VERSION) continue;

                nRequestedMasternodeAttempt++;



                SendGovernanceSyncRequest(pnode);



                ReleaseNodes(vNodesCopy);

                return; //this will cause each peer to get one request each six seconds for the various assets we need

            }

        }

    }

    // looped through all nodes, release them

    ReleaseNodes(vNodesCopy);

}



void CMasternodeSync::SendGovernanceSyncRequest(CNode* pnode)

{

    if(pnode->nVersion >= GOVERNANCE_FILTER_PROTO_VERSION) {

        CBloomFilter filter;

        filter.clear();



        pnode->PushMessage(NetMsgType::MNGOVERNANCESYNC, uint256(), filter);

    }

    else {

        pnode->PushMessage(NetMsgType::MNGOVERNANCESYNC, uint256());

    }

}



void CMasternodeSync::UpdatedBlockTip(const CBlockIndex *pindex)

{

    pCurrentBlockIndex = pindex;

}
>>>>>>> dev-1.12.1.0
