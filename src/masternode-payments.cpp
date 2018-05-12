// Copyright (c) 2014-2017 The Curium Core developers

// Distributed under the MIT/X11 software license, see the accompanying

// file COPYING or http://www.opensource.org/licenses/mit-license.php.



#include "activemasternode.h"

#include "darksend.h"

#include "governance-classes.h"

#include "masternode-payments.h"
<<<<<<< HEAD
#include "masternode-budget.h"
#include "masternode-sync.h"
=======

#include "masternode-sync.h"

>>>>>>> dev-1.12.1.0
#include "masternodeman.h"

#include "netfulfilledman.h"

#include "spork.h"

#include "util.h"



#include <boost/lexical_cast.hpp>
<<<<<<< HEAD
#include <boost/filesystem.hpp>

/** Object for who's going to get paid on which blocks */
CMasternodePayments masternodePayments;

CCriticalSection cs_vecPayments;
CCriticalSection cs_mapMasternodeBlocks;
CCriticalSection cs_mapMasternodePayeeVotes;

//
// CMasternodePaymentDB
//

CMasternodePaymentDB::CMasternodePaymentDB()
{
    pathDB = GetDataDir() / "mnpayments.dat";
    strMagicMessage = "MasternodePayments";
}

bool CMasternodePaymentDB::Write(const CMasternodePayments& objToSave)
{
    int64_t nStart = GetTimeMillis();

    // serialize, checksum data up to that point, then append checksum
    CDataStream ssObj(SER_DISK, CLIENT_VERSION);
    ssObj << strMagicMessage; // masternode cache file specific magic message
    ssObj << FLATDATA(Params().MessageStart()); // network specific magic number
    ssObj << objToSave;
    uint256 hash = Hash(ssObj.begin(), ssObj.end());
    ssObj << hash;

    // open output file, and associate with CAutoFile
    FILE *file = fopen(pathDB.string().c_str(), "wb");
    CAutoFile fileout(file, SER_DISK, CLIENT_VERSION);
    if (fileout.IsNull())
        return error("%s : Failed to open file %s", __func__, pathDB.string());

    // Write and commit header, data
    try {
        fileout << ssObj;
    }
    catch (std::exception &e) {
        return error("%s : Serialize or I/O error - %s", __func__, e.what());
    }
    fileout.fclose();

    LogPrintf("Written info to mnpayments.dat  %dms\n", GetTimeMillis() - nStart);

    return true;
}

CMasternodePaymentDB::ReadResult CMasternodePaymentDB::Read(CMasternodePayments& objToLoad, bool fDryRun)
{

    int64_t nStart = GetTimeMillis();
    // open input file, and associate with CAutoFile
    FILE *file = fopen(pathDB.string().c_str(), "rb");
    CAutoFile filein(file, SER_DISK, CLIENT_VERSION);
    if (filein.IsNull())
    {
        error("%s : Failed to open file %s", __func__, pathDB.string());
        return FileError;
    }

    // use file size to size memory buffer
    int fileSize = boost::filesystem::file_size(pathDB);
    int dataSize = fileSize - sizeof(uint256);
    // Don't try to resize to a negative number if file is small
    if (dataSize < 0)
        dataSize = 0;
    vector<unsigned char> vchData;
    vchData.resize(dataSize);
    uint256 hashIn;

    // read data and checksum from file
    try {
        filein.read((char *)&vchData[0], dataSize);
        filein >> hashIn;
    }
    catch (std::exception &e) {
        error("%s : Deserialize or I/O error - %s", __func__, e.what());
        return HashReadError;
    }
    filein.fclose();

    CDataStream ssObj(vchData, SER_DISK, CLIENT_VERSION);

    // verify stored checksum matches input data
    uint256 hashTmp = Hash(ssObj.begin(), ssObj.end());
    if (hashIn != hashTmp)
    {
        error("%s : Checksum mismatch, data corrupted", __func__);
        return IncorrectHash;
    }


    unsigned char pchMsgTmp[4];
    std::string strMagicMessageTmp;
    try {
        // de-serialize file header (masternode cache file specific magic message) and ..
        ssObj >> strMagicMessageTmp;

        // ... verify the message matches predefined one
        if (strMagicMessage != strMagicMessageTmp)
        {
            error("%s : Invalid masternode payement cache magic message", __func__);
            return IncorrectMagicMessage;
        }


        // de-serialize file header (network specific magic number) and ..
        ssObj >> FLATDATA(pchMsgTmp);

        // ... verify the network matches ours
        if (memcmp(pchMsgTmp, Params().MessageStart(), sizeof(pchMsgTmp)))
        {
            error("%s : Invalid network magic number", __func__);
            return IncorrectMagicNumber;
        }

        // de-serialize data into CMasternodePayments object
        ssObj >> objToLoad;
    }
    catch (std::exception &e) {
        objToLoad.Clear();
        error("%s : Deserialize or I/O error - %s", __func__, e.what());
        return IncorrectFormat;
    }

    LogPrintf("Loaded info from mnpayments.dat  %dms\n", GetTimeMillis() - nStart);
    LogPrintf("  %s\n", objToLoad.ToString());
    if(!fDryRun) {
        LogPrintf("Masternode payments manager - cleaning....\n");
        objToLoad.CleanPaymentList();
        LogPrintf("Masternode payments manager - result:\n");
        LogPrintf("  %s\n", objToLoad.ToString());
    }

    return Ok;
}

void DumpMasternodePayments()
{
    int64_t nStart = GetTimeMillis();

    CMasternodePaymentDB paymentdb;
    CMasternodePayments tempPayments;

    LogPrintf("Verifying mnpayments.dat format...\n");
    CMasternodePaymentDB::ReadResult readResult = paymentdb.Read(tempPayments, true);
    // there was an error and it was not an error on file opening => do not proceed
    if (readResult == CMasternodePaymentDB::FileError)
        LogPrintf("Missing budgets file - mnpayments.dat, will try to recreate\n");
    else if (readResult != CMasternodePaymentDB::Ok)
    {
        LogPrintf("Error reading mnpayments.dat: ");
        if(readResult == CMasternodePaymentDB::IncorrectFormat)
            LogPrintf("magic is ok but data has invalid format, will try to recreate\n");
        else
        {
            LogPrintf("file format is unknown or invalid, please fix it manually\n");
            return;
        }
    }
    LogPrintf("Writting info to mnpayments.dat...\n");
    paymentdb.Write(masternodePayments);

    LogPrintf("Budget dump finished  %dms\n", GetTimeMillis() - nStart);
}

bool IsBlockValueValid(const CBlock& block, int64_t nExpectedValue){
    CBlockIndex* pindexPrev = chainActive.Tip();
    if(pindexPrev == NULL) return true;

    int nHeight = 0;
    if(pindexPrev->GetBlockHash() == block.hashPrevBlock)
    {
        nHeight = pindexPrev->nHeight+1;
    } else { //out of order
        BlockMap::iterator mi = mapBlockIndex.find(block.hashPrevBlock);
        if (mi != mapBlockIndex.end() && (*mi).second)
            nHeight = (*mi).second->nHeight+1;
    }

    if(nHeight == 0){
        LogPrintf("IsBlockValueValid() : WARNING: Couldn't find previous block");
    }

    if(!masternodeSync.IsSynced()) { //there is no budget data to use to check anything
        //super blocks will always be on these blocks, max 100 per budgeting
        if(nHeight % GetBudgetPaymentCycleBlocks() < 100){
            return true;
        } else {
            if(block.vtx[0].GetValueOut() > nExpectedValue) return false;
        }
    } else { // we're synced and have data so check the budget schedule

        //are these blocks even enabled
        if(!IsSporkActive(SPORK_13_ENABLE_SUPERBLOCKS)){
            return block.vtx[0].GetValueOut() <= nExpectedValue;
        }
        
        if(budget.IsBudgetPaymentBlock(nHeight)){
            //the value of the block is evaluated in CheckBlock
            return true;
        } else {
            if(block.vtx[0].GetValueOut() > nExpectedValue) return false;
=======



/** Object for who's going to get paid on which blocks */

CMasternodePayments mnpayments;



CCriticalSection cs_vecPayees;

CCriticalSection cs_mapMasternodeBlocks;

CCriticalSection cs_mapMasternodePaymentVotes;



/**

* IsBlockValueValid

*

*   Determine if coinbase outgoing created money is the correct value

*

*   Why is this needed?

*   - In Curium some blocks are superblocks, which output much higher amounts of coins

*   - Otherblocks are 10% lower in outgoing value, so in total, no extra coins are created

*   - When non-superblocks are detected, the normal schedule should be maintained

*/



bool IsBlockValueValid(const CBlock& block, int nBlockHeight, CAmount blockReward, std::string &strErrorRet)

{

    strErrorRet = "";



    bool isBlockRewardValueMet = (block.vtx[0].GetValueOut() <= blockReward);

    if(fDebug) LogPrintf("block.vtx[0].GetValueOut() %lld <= blockReward %lld\n", block.vtx[0].GetValueOut(), blockReward);



    // we are still using budgets, but we have no data about them anymore,

    // all we know is predefined budget cycle and window



    const Consensus::Params& consensusParams = Params().GetConsensus();



    if(nBlockHeight < consensusParams.nSuperblockStartBlock) {

        int nOffset = nBlockHeight % consensusParams.nBudgetPaymentsCycleBlocks;

        if(nBlockHeight >= consensusParams.nBudgetPaymentsStartBlock &&

            nOffset < consensusParams.nBudgetPaymentsWindowBlocks) {

            // NOTE: make sure SPORK_13_OLD_SUPERBLOCK_FLAG is disabled when 12.1 starts to go live

            if(masternodeSync.IsSynced() && !sporkManager.IsSporkActive(SPORK_13_OLD_SUPERBLOCK_FLAG)) {

                // no budget blocks should be accepted here, if SPORK_13_OLD_SUPERBLOCK_FLAG is disabled

                LogPrint("gobject", "IsBlockValueValid -- Client synced but budget spork is disabled, checking block value against block reward\n");

                if(!isBlockRewardValueMet) {

                    strErrorRet = strprintf("coinbase pays too much at height %d (actual=%d vs limit=%d), exceeded block reward, budgets are disabled",

                                            nBlockHeight, block.vtx[0].GetValueOut(), blockReward);

                }

                return isBlockRewardValueMet;

            }

            LogPrint("gobject", "IsBlockValueValid -- WARNING: Skipping budget block value checks, accepting block\n");

            // TODO: reprocess blocks to make sure they are legit?

            return true;

        }

        // LogPrint("gobject", "IsBlockValueValid -- Block is not in budget cycle window, checking block value against block reward\n");

        if(!isBlockRewardValueMet) {

            strErrorRet = strprintf("coinbase pays too much at height %d (actual=%d vs limit=%d), exceeded block reward, block is not in budget cycle window",

                                    nBlockHeight, block.vtx[0].GetValueOut(), blockReward);

        }

        return isBlockRewardValueMet;

    }



    // superblocks started



    CAmount nSuperblockMaxValue =  blockReward + CSuperblock::GetPaymentsLimit(nBlockHeight);

    bool isSuperblockMaxValueMet = (block.vtx[0].GetValueOut() <= nSuperblockMaxValue);



    LogPrint("gobject", "block.vtx[0].GetValueOut() %lld <= nSuperblockMaxValue %lld\n", block.vtx[0].GetValueOut(), nSuperblockMaxValue);



    if(!masternodeSync.IsSynced()) {

        // not enough data but at least it must NOT exceed superblock max value

        if(CSuperblock::IsValidBlockHeight(nBlockHeight)) {

            if(fDebug) LogPrintf("IsBlockPayeeValid -- WARNING: Client not synced, checking superblock max bounds only\n");

            if(!isSuperblockMaxValueMet) {

                strErrorRet = strprintf("coinbase pays too much at height %d (actual=%d vs limit=%d), exceeded superblock max value",

                                        nBlockHeight, block.vtx[0].GetValueOut(), nSuperblockMaxValue);

            }

            return isSuperblockMaxValueMet;

        }

        if(!isBlockRewardValueMet) {

            strErrorRet = strprintf("coinbase pays too much at height %d (actual=%d vs limit=%d), exceeded block reward, only regular blocks are allowed at this height",

                                    nBlockHeight, block.vtx[0].GetValueOut(), blockReward);

        }

        // it MUST be a regular block otherwise

        return isBlockRewardValueMet;

    }



    // we are synced, let's try to check as much data as we can



    if(sporkManager.IsSporkActive(SPORK_9_SUPERBLOCKS_ENABLED)) {

        if(CSuperblockManager::IsSuperblockTriggered(nBlockHeight)) {

            if(CSuperblockManager::IsValid(block.vtx[0], nBlockHeight, blockReward)) {

                LogPrint("gobject", "IsBlockValueValid -- Valid superblock at height %d: %s", nBlockHeight, block.vtx[0].ToString());

                // all checks are done in CSuperblock::IsValid, nothing to do here

                return true;

            }



            // triggered but invalid? that's weird

            LogPrintf("IsBlockValueValid -- ERROR: Invalid superblock detected at height %d: %s", nBlockHeight, block.vtx[0].ToString());

            // should NOT allow invalid superblocks, when superblocks are enabled

            strErrorRet = strprintf("invalid superblock detected at height %d", nBlockHeight);

            return false;

        }

        LogPrint("gobject", "IsBlockValueValid -- No triggered superblock detected at height %d\n", nBlockHeight);

        if(!isBlockRewardValueMet) {

            strErrorRet = strprintf("coinbase pays too much at height %d (actual=%d vs limit=%d), exceeded block reward, no triggered superblock detected",

                                    nBlockHeight, block.vtx[0].GetValueOut(), blockReward);

        }

    } else {

        // should NOT allow superblocks at all, when superblocks are disabled

        LogPrint("gobject", "IsBlockValueValid -- Superblocks are disabled, no superblocks allowed\n");

        if(!isBlockRewardValueMet) {

            strErrorRet = strprintf("coinbase pays too much at height %d (actual=%d vs limit=%d), exceeded block reward, superblocks are disabled",

                                    nBlockHeight, block.vtx[0].GetValueOut(), blockReward);

        }

    }



    // it MUST be a regular block

    return isBlockRewardValueMet;

}



bool IsBlockPayeeValid(const CTransaction& txNew, int nBlockHeight, CAmount blockReward)

{

    if(!masternodeSync.IsSynced()) {

        //there is no budget data to use to check anything, let's just accept the longest chain

        if(fDebug) LogPrintf("IsBlockPayeeValid -- WARNING: Client not synced, skipping block payee checks\n");

        return true;

    }



    // we are still using budgets, but we have no data about them anymore,

    // we can only check masternode payments



    const Consensus::Params& consensusParams = Params().GetConsensus();



    if(nBlockHeight < consensusParams.nSuperblockStartBlock) {

        if(mnpayments.IsTransactionValid(txNew, nBlockHeight)) {

            LogPrint("mnpayments", "IsBlockPayeeValid -- Valid masternode payment at height %d: %s", nBlockHeight, txNew.ToString());

            return true;

        }



        int nOffset = nBlockHeight % consensusParams.nBudgetPaymentsCycleBlocks;

        if(nBlockHeight >= consensusParams.nBudgetPaymentsStartBlock &&

            nOffset < consensusParams.nBudgetPaymentsWindowBlocks) {

            if(!sporkManager.IsSporkActive(SPORK_13_OLD_SUPERBLOCK_FLAG)) {

                // no budget blocks should be accepted here, if SPORK_13_OLD_SUPERBLOCK_FLAG is disabled

                LogPrint("gobject", "IsBlockPayeeValid -- ERROR: Client synced but budget spork is disabled and masternode payment is invalid\n");

                return false;

            }

            // NOTE: this should never happen in real, SPORK_13_OLD_SUPERBLOCK_FLAG MUST be disabled when 12.1 starts to go live

            LogPrint("gobject", "IsBlockPayeeValid -- WARNING: Probably valid budget block, have no data, accepting\n");

            // TODO: reprocess blocks to make sure they are legit?

            return true;

        }



        if(sporkManager.IsSporkActive(SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT)) {

            LogPrintf("IsBlockPayeeValid -- ERROR: Invalid masternode payment detected at height %d: %s", nBlockHeight, txNew.ToString());

            return false;

>>>>>>> dev-1.12.1.0
        }



        LogPrintf("IsBlockPayeeValid -- WARNING: Masternode payment enforcement is disabled, accepting any payee\n");

        return true;

    }


<<<<<<< HEAD
bool IsBlockPayeeValid(const CTransaction& txNew, int nBlockHeight)
{
    if(!masternodeSync.IsSynced()) { //there is no budget data to use to check anything -- find the longest chain
        LogPrint("mnpayments", "Client not synced, skipping block payee checks\n");
        return true;
    }

    //check if it's a budget block
    if(IsSporkActive(SPORK_13_ENABLE_SUPERBLOCKS)){
        if(budget.IsBudgetPaymentBlock(nBlockHeight)){
            if(budget.IsTransactionValid(txNew, nBlockHeight)){
                return true;
            } else {
                LogPrintf("Invalid budget payment detected %s\n", txNew.ToString().c_str());
                if(IsSporkActive(SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT)){
                    return false;
                } else {
                    LogPrintf("Budget enforcement is disabled, accepting block\n");
                    return true;
                }
            }
        }
    }

    //check for masternode payee
    if(masternodePayments.IsTransactionValid(txNew, nBlockHeight))
    {
        return true;
    } else {
        LogPrintf("Invalid mn payment detected %s\n", txNew.ToString().c_str());
        if(IsSporkActive(SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT)){
            return false;
        } else {
            LogPrintf("Masternode payment enforcement is disabled, accepting block\n");
            return true;
=======

    // superblocks started

    // SEE IF THIS IS A VALID SUPERBLOCK



    if(sporkManager.IsSporkActive(SPORK_9_SUPERBLOCKS_ENABLED)) {

        if(CSuperblockManager::IsSuperblockTriggered(nBlockHeight)) {

            if(CSuperblockManager::IsValid(txNew, nBlockHeight, blockReward)) {

                LogPrint("gobject", "IsBlockPayeeValid -- Valid superblock at height %d: %s", nBlockHeight, txNew.ToString());

                return true;

            }



            LogPrintf("IsBlockPayeeValid -- ERROR: Invalid superblock detected at height %d: %s", nBlockHeight, txNew.ToString());

            // should NOT allow such superblocks, when superblocks are enabled

            return false;

        }

        // continue validation, should pay MN

        LogPrint("gobject", "IsBlockPayeeValid -- No triggered superblock detected at height %d\n", nBlockHeight);

    } else {

        // should NOT allow superblocks at all, when superblocks are disabled

        LogPrint("gobject", "IsBlockPayeeValid -- Superblocks are disabled, no superblocks allowed\n");

    }



    // IF THIS ISN'T A SUPERBLOCK OR SUPERBLOCK IS INVALID, IT SHOULD PAY A MASTERNODE DIRECTLY

    if(mnpayments.IsTransactionValid(txNew, nBlockHeight)) {

        LogPrint("mnpayments", "IsBlockPayeeValid -- Valid masternode payment at height %d: %s", nBlockHeight, txNew.ToString());

        return true;

    }



    if(sporkManager.IsSporkActive(SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT)) {

        LogPrintf("IsBlockPayeeValid -- ERROR: Invalid masternode payment detected at height %d: %s", nBlockHeight, txNew.ToString());

        return false;

    }



    LogPrintf("IsBlockPayeeValid -- WARNING: Masternode payment enforcement is disabled, accepting any payee\n");

    return true;

}



void FillBlockPayments(CMutableTransaction& txNew, int nBlockHeight, CAmount blockReward, CTxOut& txoutMasternodeRet, std::vector<CTxOut>& voutSuperblockRet)

{

    // only create superblocks if spork is enabled AND if superblock is actually triggered

    // (height should be validated inside)

    if(sporkManager.IsSporkActive(SPORK_9_SUPERBLOCKS_ENABLED) &&

        CSuperblockManager::IsSuperblockTriggered(nBlockHeight)) {

            LogPrint("gobject", "FillBlockPayments -- triggered superblock creation at height %d\n", nBlockHeight);

            CSuperblockManager::CreateSuperblock(txNew, nBlockHeight, voutSuperblockRet);

            return;

    }



    // FILL BLOCK PAYEE WITH MASTERNODE PAYMENT OTHERWISE

    mnpayments.FillBlockPayee(txNew, nBlockHeight, blockReward, txoutMasternodeRet);

    LogPrint("mnpayments", "FillBlockPayments -- nBlockHeight %d blockReward %lld txoutMasternodeRet %s txNew %s",

                            nBlockHeight, blockReward, txoutMasternodeRet.ToString(), txNew.ToString());

}



std::string GetRequiredPaymentsString(int nBlockHeight)

{

    // IF WE HAVE A ACTIVATED TRIGGER FOR THIS HEIGHT - IT IS A SUPERBLOCK, GET THE REQUIRED PAYEES

    if(CSuperblockManager::IsSuperblockTriggered(nBlockHeight)) {

        return CSuperblockManager::GetRequiredPaymentsString(nBlockHeight);

    }



    // OTHERWISE, PAY MASTERNODE

    return mnpayments.GetRequiredPaymentsString(nBlockHeight);

}



void CMasternodePayments::Clear()

{

    LOCK2(cs_mapMasternodeBlocks, cs_mapMasternodePaymentVotes);

    mapMasternodeBlocks.clear();

    mapMasternodePaymentVotes.clear();

}



bool CMasternodePayments::CanVote(COutPoint outMasternode, int nBlockHeight)

{

    LOCK(cs_mapMasternodePaymentVotes);



    if (mapMasternodesLastVote.count(outMasternode) && mapMasternodesLastVote[outMasternode] == nBlockHeight) {

        return false;

    }



    //record this masternode voted

    mapMasternodesLastVote[outMasternode] = nBlockHeight;

    return true;

}



/**

*   FillBlockPayee

*

*   Fill Masternode ONLY payment block

*/



void CMasternodePayments::FillBlockPayee(CMutableTransaction& txNew, int nBlockHeight, CAmount blockReward, CTxOut& txoutMasternodeRet)

{

    // make sure it's not filled yet

    txoutMasternodeRet = CTxOut();



    CScript payee;



    if(!mnpayments.GetBlockPayee(nBlockHeight, payee)) {

        // no masternode detected...

        int nCount = 0;

        CMasternode *winningNode = mnodeman.GetNextMasternodeInQueueForPayment(nBlockHeight, true, nCount);

        if(!winningNode) {

            // ...and we can't calculate it on our own

            LogPrintf("CMasternodePayments::FillBlockPayee -- Failed to detect masternode to pay\n");

            return;

        }

        // fill payee with locally calculated winner and hope for the best

        payee = GetScriptForDestination(winningNode->pubKeyCollateralAddress.GetID());

    }



    // GET MASTERNODE PAYMENT VARIABLES SETUP

    CAmount masternodePayment = GetMasternodePayment(nBlockHeight, blockReward);



    // split reward between miner ...

    txNew.vout[0].nValue -= masternodePayment;

    // ... and masternode

    txoutMasternodeRet = CTxOut(masternodePayment, payee);

    txNew.vout.push_back(txoutMasternodeRet);



    CTxDestination address1;

    ExtractDestination(payee, address1);

    CBitcoinAddress address2(address1);



    LogPrintf("CMasternodePayments::FillBlockPayee -- Masternode payment %lld to %s\n", masternodePayment, address2.ToString());

}



int CMasternodePayments::GetMinMasternodePaymentsProto() {

    return sporkManager.IsSporkActive(SPORK_10_MASTERNODE_PAY_UPDATED_NODES)

            ? MIN_MASTERNODE_PAYMENT_PROTO_VERSION_2

            : MIN_MASTERNODE_PAYMENT_PROTO_VERSION_1;

}



void CMasternodePayments::ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)

{

    // Ignore any payments messages until masternode list is synced

    if(!masternodeSync.IsMasternodeListSynced()) return;



    if(fLiteMode) return; // disable all Curium specific functionality



    if (strCommand == NetMsgType::MASTERNODEPAYMENTSYNC) { //Masternode Payments Request Sync



        // Ignore such requests until we are fully synced.

        // We could start processing this after masternode list is synced

        // but this is a heavy one so it's better to finish sync first.

        if (!masternodeSync.IsSynced()) return;



        int nCountNeeded;

        vRecv >> nCountNeeded;



        if(netfulfilledman.HasFulfilledRequest(pfrom->addr, NetMsgType::MASTERNODEPAYMENTSYNC)) {

            // Asking for the payments list multiple times in a short period of time is no good

            LogPrintf("MASTERNODEPAYMENTSYNC -- peer already asked me for the list, peer=%d\n", pfrom->id);

            Misbehaving(pfrom->GetId(), 20);

            return;

        }

        netfulfilledman.AddFulfilledRequest(pfrom->addr, NetMsgType::MASTERNODEPAYMENTSYNC);



        Sync(pfrom, nCountNeeded);

        LogPrintf("MASTERNODEPAYMENTSYNC -- Sent Masternode payment votes to peer %d\n", pfrom->id);



    } else if (strCommand == NetMsgType::MASTERNODEPAYMENTVOTE) { // Masternode Payments Vote for the Winner



        CMasternodePaymentVote vote;

        vRecv >> vote;



        if(pfrom->nVersion < GetMinMasternodePaymentsProto()) return;



        if(!pCurrentBlockIndex) return;



        uint256 nHash = vote.GetHash();



        pfrom->setAskFor.erase(nHash);



        {

            LOCK(cs_mapMasternodePaymentVotes);

            if(mapMasternodePaymentVotes.count(nHash)) {

                LogPrint("mnpayments", "MASTERNODEPAYMENTVOTE -- hash=%s, nHeight=%d seen\n", nHash.ToString(), pCurrentBlockIndex->nHeight);

                return;

            }



            // Avoid processing same vote multiple times

            mapMasternodePaymentVotes[nHash] = vote;

            // but first mark vote as non-verified,

            // AddPaymentVote() below should take care of it if vote is actually ok

            mapMasternodePaymentVotes[nHash].MarkAsNotVerified();

        }



        int nFirstBlock = pCurrentBlockIndex->nHeight - GetStorageLimit();

        if(vote.nBlockHeight < nFirstBlock || vote.nBlockHeight > pCurrentBlockIndex->nHeight+20) {

            LogPrint("mnpayments", "MASTERNODEPAYMENTVOTE -- vote out of range: nFirstBlock=%d, nBlockHeight=%d, nHeight=%d\n", nFirstBlock, vote.nBlockHeight, pCurrentBlockIndex->nHeight);

            return;

        }



        std::string strError = "";

        if(!vote.IsValid(pfrom, pCurrentBlockIndex->nHeight, strError)) {

            LogPrint("mnpayments", "MASTERNODEPAYMENTVOTE -- invalid message, error: %s\n", strError);

            return;

        }



        if(!CanVote(vote.vinMasternode.prevout, vote.nBlockHeight)) {

            LogPrintf("MASTERNODEPAYMENTVOTE -- masternode already voted, masternode=%s\n", vote.vinMasternode.prevout.ToStringShort());

            return;

        }



        masternode_info_t mnInfo = mnodeman.GetMasternodeInfo(vote.vinMasternode);

        if(!mnInfo.fInfoValid) {

            // mn was not found, so we can't check vote, some info is probably missing

            LogPrintf("MASTERNODEPAYMENTVOTE -- masternode is missing %s\n", vote.vinMasternode.prevout.ToStringShort());

            mnodeman.AskForMN(pfrom, vote.vinMasternode);

            return;

        }



        int nDos = 0;

        if(!vote.CheckSignature(mnInfo.pubKeyMasternode, pCurrentBlockIndex->nHeight, nDos)) {

            if(nDos) {

                LogPrintf("MASTERNODEPAYMENTVOTE -- ERROR: invalid signature\n");

                Misbehaving(pfrom->GetId(), nDos);

            } else {

                // only warn about anything non-critical (i.e. nDos == 0) in debug mode

                LogPrint("mnpayments", "MASTERNODEPAYMENTVOTE -- WARNING: invalid signature\n");

            }

            // Either our info or vote info could be outdated.

            // In case our info is outdated, ask for an update,

            mnodeman.AskForMN(pfrom, vote.vinMasternode);

            // but there is nothing we can do if vote info itself is outdated

            // (i.e. it was signed by a mn which changed its key),

            // so just quit here.

            return;

        }



        CTxDestination address1;

        ExtractDestination(vote.payee, address1);

        CBitcoinAddress address2(address1);



        LogPrint("mnpayments", "MASTERNODEPAYMENTVOTE -- vote: address=%s, nBlockHeight=%d, nHeight=%d, prevout=%s\n", address2.ToString(), vote.nBlockHeight, pCurrentBlockIndex->nHeight, vote.vinMasternode.prevout.ToStringShort());



        if(AddPaymentVote(vote)){

            vote.Relay();

            masternodeSync.AddedPaymentVote();

        }

    }

}



bool CMasternodePaymentVote::Sign()

{

    std::string strError;

    std::string strMessage = vinMasternode.prevout.ToStringShort() +

                boost::lexical_cast<std::string>(nBlockHeight) +

                ScriptToAsmStr(payee);



    if(!darkSendSigner.SignMessage(strMessage, vchSig, activeMasternode.keyMasternode)) {

        LogPrintf("CMasternodePaymentVote::Sign -- SignMessage() failed\n");

        return false;

    }



    if(!darkSendSigner.VerifyMessage(activeMasternode.pubKeyMasternode, vchSig, strMessage, strError)) {

        LogPrintf("CMasternodePaymentVote::Sign -- VerifyMessage() failed, error: %s\n", strError);

        return false;

    }



    return true;

}



bool CMasternodePayments::GetBlockPayee(int nBlockHeight, CScript& payee)

{

    if(mapMasternodeBlocks.count(nBlockHeight)){

        return mapMasternodeBlocks[nBlockHeight].GetBestPayee(payee);

    }



    return false;

}



// Is this masternode scheduled to get paid soon?

// -- Only look ahead up to 8 blocks to allow for propagation of the latest 2 blocks of votes

bool CMasternodePayments::IsScheduled(CMasternode& mn, int nNotBlockHeight)

{

    LOCK(cs_mapMasternodeBlocks);



    if(!pCurrentBlockIndex) return false;



    CScript mnpayee;

    mnpayee = GetScriptForDestination(mn.pubKeyCollateralAddress.GetID());



    CScript payee;

    for(int64_t h = pCurrentBlockIndex->nHeight; h <= pCurrentBlockIndex->nHeight + 8; h++){

        if(h == nNotBlockHeight) continue;

        if(mapMasternodeBlocks.count(h) && mapMasternodeBlocks[h].GetBestPayee(payee) && mnpayee == payee) {

            return true;

        }

    }



    return false;

}



bool CMasternodePayments::AddPaymentVote(const CMasternodePaymentVote& vote)

{

    uint256 blockHash = uint256();

    if(!GetBlockHash(blockHash, vote.nBlockHeight - 101)) return false;



    if(HasVerifiedPaymentVote(vote.GetHash())) return false;



    LOCK2(cs_mapMasternodeBlocks, cs_mapMasternodePaymentVotes);



    mapMasternodePaymentVotes[vote.GetHash()] = vote;



    if(!mapMasternodeBlocks.count(vote.nBlockHeight)) {

       CMasternodeBlockPayees blockPayees(vote.nBlockHeight);

       mapMasternodeBlocks[vote.nBlockHeight] = blockPayees;

    }



    mapMasternodeBlocks[vote.nBlockHeight].AddPayee(vote);



    return true;

}



bool CMasternodePayments::HasVerifiedPaymentVote(uint256 hashIn)

{

    LOCK(cs_mapMasternodePaymentVotes);

    std::map<uint256, CMasternodePaymentVote>::iterator it = mapMasternodePaymentVotes.find(hashIn);

    return it != mapMasternodePaymentVotes.end() && it->second.IsVerified();

}



void CMasternodeBlockPayees::AddPayee(const CMasternodePaymentVote& vote)

{

    LOCK(cs_vecPayees);



    BOOST_FOREACH(CMasternodePayee& payee, vecPayees) {

        if (payee.GetPayee() == vote.payee) {

            payee.AddVoteHash(vote.GetHash());

            return;

        }

    }

    CMasternodePayee payeeNew(vote.payee, vote.GetHash());

    vecPayees.push_back(payeeNew);

}



bool CMasternodeBlockPayees::GetBestPayee(CScript& payeeRet)

{

    LOCK(cs_vecPayees);



    if(!vecPayees.size()) {

        LogPrint("mnpayments", "CMasternodeBlockPayees::GetBestPayee -- ERROR: couldn't find any payee\n");

        return false;

    }



    int nVotes = -1;

    BOOST_FOREACH(CMasternodePayee& payee, vecPayees) {

        if (payee.GetVoteCount() > nVotes) {

            payeeRet = payee.GetPayee();

            nVotes = payee.GetVoteCount();

        }

    }



    return (nVotes > -1);

}



bool CMasternodeBlockPayees::HasPayeeWithVotes(CScript payeeIn, int nVotesReq)

{

    LOCK(cs_vecPayees);



    BOOST_FOREACH(CMasternodePayee& payee, vecPayees) {

        if (payee.GetVoteCount() >= nVotesReq && payee.GetPayee() == payeeIn) {

            return true;

        }

    }



    LogPrint("mnpayments", "CMasternodeBlockPayees::HasPayeeWithVotes -- ERROR: couldn't find any payee with %d+ votes\n", nVotesReq);

    return false;

}



bool CMasternodeBlockPayees::IsTransactionValid(const CTransaction& txNew)

{

    LOCK(cs_vecPayees);



    int nMaxSignatures = 0;

    std::string strPayeesPossible = "";



    CAmount nMasternodePayment = GetMasternodePayment(nBlockHeight, txNew.GetValueOut());



    //require at least MNPAYMENTS_SIGNATURES_REQUIRED signatures



    BOOST_FOREACH(CMasternodePayee& payee, vecPayees) {

        if (payee.GetVoteCount() >= nMaxSignatures) {

            nMaxSignatures = payee.GetVoteCount();

        }

    }



    // if we don't have at least MNPAYMENTS_SIGNATURES_REQUIRED signatures on a payee, approve whichever is the longest chain

    if(nMaxSignatures < MNPAYMENTS_SIGNATURES_REQUIRED) return true;



    BOOST_FOREACH(CMasternodePayee& payee, vecPayees) {

        if (payee.GetVoteCount() >= MNPAYMENTS_SIGNATURES_REQUIRED) {

            BOOST_FOREACH(CTxOut txout, txNew.vout) {

                if (payee.GetPayee() == txout.scriptPubKey && nMasternodePayment == txout.nValue) {

                    LogPrint("mnpayments", "CMasternodeBlockPayees::IsTransactionValid -- Found required payment\n");

                    return true;

                }

            }



            CTxDestination address1;

            ExtractDestination(payee.GetPayee(), address1);

            CBitcoinAddress address2(address1);



            if(strPayeesPossible == "") {

                strPayeesPossible = address2.ToString();

            } else {

                strPayeesPossible += "," + address2.ToString();

            }

        }

    }



    LogPrintf("CMasternodeBlockPayees::IsTransactionValid -- ERROR: Missing required payment, possible payees: '%s', amount: %f CURIUM\n", strPayeesPossible, (float)nMasternodePayment/COIN);

    return false;

}



std::string CMasternodeBlockPayees::GetRequiredPaymentsString()

{

    LOCK(cs_vecPayees);



    std::string strRequiredPayments = "Unknown";



    BOOST_FOREACH(CMasternodePayee& payee, vecPayees)

    {

        CTxDestination address1;

        ExtractDestination(payee.GetPayee(), address1);

        CBitcoinAddress address2(address1);



        if (strRequiredPayments != "Unknown") {

            strRequiredPayments += ", " + address2.ToString() + ":" + boost::lexical_cast<std::string>(payee.GetVoteCount());

        } else {

            strRequiredPayments = address2.ToString() + ":" + boost::lexical_cast<std::string>(payee.GetVoteCount());

        }

    }



    return strRequiredPayments;

}



std::string CMasternodePayments::GetRequiredPaymentsString(int nBlockHeight)

{

    LOCK(cs_mapMasternodeBlocks);



    if(mapMasternodeBlocks.count(nBlockHeight)){

        return mapMasternodeBlocks[nBlockHeight].GetRequiredPaymentsString();

    }



    return "Unknown";

}



bool CMasternodePayments::IsTransactionValid(const CTransaction& txNew, int nBlockHeight)

{

    LOCK(cs_mapMasternodeBlocks);



    if(mapMasternodeBlocks.count(nBlockHeight)){

        return mapMasternodeBlocks[nBlockHeight].IsTransactionValid(txNew);

    }



    return true;

}



void CMasternodePayments::CheckAndRemove()

{

    if(!pCurrentBlockIndex) return;



    LOCK2(cs_mapMasternodeBlocks, cs_mapMasternodePaymentVotes);



    int nLimit = GetStorageLimit();



    std::map<uint256, CMasternodePaymentVote>::iterator it = mapMasternodePaymentVotes.begin();

    while(it != mapMasternodePaymentVotes.end()) {

        CMasternodePaymentVote vote = (*it).second;



        if(pCurrentBlockIndex->nHeight - vote.nBlockHeight > nLimit) {

            LogPrint("mnpayments", "CMasternodePayments::CheckAndRemove -- Removing old Masternode payment: nBlockHeight=%d\n", vote.nBlockHeight);

            mapMasternodePaymentVotes.erase(it++);

            mapMasternodeBlocks.erase(vote.nBlockHeight);

        } else {

            ++it;

        }

    }

    LogPrintf("CMasternodePayments::CheckAndRemove -- %s\n", ToString());

}



bool CMasternodePaymentVote::IsValid(CNode* pnode, int nValidationHeight, std::string& strError)

{

    CMasternode* pmn = mnodeman.Find(vinMasternode);



    if(!pmn) {

        strError = strprintf("Unknown Masternode: prevout=%s", vinMasternode.prevout.ToStringShort());

        // Only ask if we are already synced and still have no idea about that Masternode

        if(masternodeSync.IsSynced()) {

            mnodeman.AskForMN(pnode, vinMasternode);

        }



        return false;

    }



    int nMinRequiredProtocol;

    if(nBlockHeight >= nValidationHeight) {

        // new votes must comply SPORK_10_MASTERNODE_PAY_UPDATED_NODES rules

        nMinRequiredProtocol = mnpayments.GetMinMasternodePaymentsProto();

    } else {

        // allow non-updated masternodes for old blocks

        nMinRequiredProtocol = MIN_MASTERNODE_PAYMENT_PROTO_VERSION_1;

    }



    if(pmn->nProtocolVersion < nMinRequiredProtocol) {

        strError = strprintf("Masternode protocol is too old: nProtocolVersion=%d, nMinRequiredProtocol=%d", pmn->nProtocolVersion, nMinRequiredProtocol);

        return false;

    }



    // Only masternodes should try to check masternode rank for old votes - they need to pick the right winner for future blocks.

    // Regular clients (miners included) need to verify masternode rank for future block votes only.

    if(!fMasterNode && nBlockHeight < nValidationHeight) return true;



    int nRank = mnodeman.GetMasternodeRank(vinMasternode, nBlockHeight - 101, nMinRequiredProtocol, false);



    if(nRank > MNPAYMENTS_SIGNATURES_TOTAL) {

        // It's common to have masternodes mistakenly think they are in the top 10

        // We don't want to print all of these messages in normal mode, debug mode should print though

        strError = strprintf("Masternode is not in the top %d (%d)", MNPAYMENTS_SIGNATURES_TOTAL, nRank);

        // Only ban for new mnw which is out of bounds, for old mnw MN list itself might be way too much off

        if(nRank > MNPAYMENTS_SIGNATURES_TOTAL*2 && nBlockHeight > nValidationHeight) {

            strError = strprintf("Masternode is not in the top %d (%d)", MNPAYMENTS_SIGNATURES_TOTAL*2, nRank);

            LogPrintf("CMasternodePaymentVote::IsValid -- Error: %s\n", strError);

            Misbehaving(pnode->GetId(), 20);

        }

        // Still invalid however

        return false;

    }



    return true;

}



bool CMasternodePayments::ProcessBlock(int nBlockHeight)

{

    // DETERMINE IF WE SHOULD BE VOTING FOR THE NEXT PAYEE



    if(fLiteMode || !fMasterNode) return false;



    // We have little chances to pick the right winner if winners list is out of sync

    // but we have no choice, so we'll try. However it doesn't make sense to even try to do so

    // if we have not enough data about masternodes.

    if(!masternodeSync.IsMasternodeListSynced()) return false;



    int nRank = mnodeman.GetMasternodeRank(activeMasternode.vin, nBlockHeight - 101, GetMinMasternodePaymentsProto(), false);



    if (nRank == -1) {

        LogPrint("mnpayments", "CMasternodePayments::ProcessBlock -- Unknown Masternode\n");

        return false;

    }



    if (nRank > MNPAYMENTS_SIGNATURES_TOTAL) {

        LogPrint("mnpayments", "CMasternodePayments::ProcessBlock -- Masternode not in the top %d (%d)\n", MNPAYMENTS_SIGNATURES_TOTAL, nRank);

        return false;

    }





    // LOCATE THE NEXT MASTERNODE WHICH SHOULD BE PAID



    LogPrintf("CMasternodePayments::ProcessBlock -- Start: nBlockHeight=%d, masternode=%s\n", nBlockHeight, activeMasternode.vin.prevout.ToStringShort());



    // pay to the oldest MN that still had no payment but its input is old enough and it was active long enough

    int nCount = 0;

    CMasternode *pmn = mnodeman.GetNextMasternodeInQueueForPayment(nBlockHeight, true, nCount);



    if (pmn == NULL) {

        LogPrintf("CMasternodePayments::ProcessBlock -- ERROR: Failed to find masternode to pay\n");

        return false;

    }



    LogPrintf("CMasternodePayments::ProcessBlock -- Masternode found by GetNextMasternodeInQueueForPayment(): %s\n", pmn->vin.prevout.ToStringShort());





    CScript payee = GetScriptForDestination(pmn->pubKeyCollateralAddress.GetID());



    CMasternodePaymentVote voteNew(activeMasternode.vin, nBlockHeight, payee);



    CTxDestination address1;

    ExtractDestination(payee, address1);

    CBitcoinAddress address2(address1);



    LogPrintf("CMasternodePayments::ProcessBlock -- vote: payee=%s, nBlockHeight=%d\n", address2.ToString(), nBlockHeight);



    // SIGN MESSAGE TO NETWORK WITH OUR MASTERNODE KEYS



    LogPrintf("CMasternodePayments::ProcessBlock -- Signing vote\n");

    if (voteNew.Sign()) {

        LogPrintf("CMasternodePayments::ProcessBlock -- AddPaymentVote()\n");



        if (AddPaymentVote(voteNew)) {

            voteNew.Relay();

            return true;

        }

    }



    return false;

}



void CMasternodePaymentVote::Relay()

{

    // do not relay until synced

    if (!masternodeSync.IsSynced()) return;

    CInv inv(MSG_MASTERNODE_PAYMENT_VOTE, GetHash());

    RelayInv(inv);

}



bool CMasternodePaymentVote::CheckSignature(const CPubKey& pubKeyMasternode, int nValidationHeight, int &nDos)

{

    // do not ban by default

    nDos = 0;



    std::string strMessage = vinMasternode.prevout.ToStringShort() +

                boost::lexical_cast<std::string>(nBlockHeight) +

                ScriptToAsmStr(payee);



    std::string strError = "";

    if (!darkSendSigner.VerifyMessage(pubKeyMasternode, vchSig, strMessage, strError)) {

        // Only ban for future block vote when we are already synced.

        // Otherwise it could be the case when MN which signed this vote is using another key now

        // and we have no idea about the old one.

        if(masternodeSync.IsSynced() && nBlockHeight > nValidationHeight) {

            nDos = 20;

>>>>>>> dev-1.12.1.0
        }

        return error("CMasternodePaymentVote::CheckSignature -- Got bad Masternode payment signature, masternode=%s, error: %s", vinMasternode.prevout.ToStringShort().c_str(), strError);

    }



<<<<<<< HEAD
void FillBlockPayee(CMutableTransaction& txNew, int64_t nFees)
{
    CBlockIndex* pindexPrev = chainActive.Tip();
    if(!pindexPrev) return;

    if(IsSporkActive(SPORK_13_ENABLE_SUPERBLOCKS) && budget.IsBudgetPaymentBlock(pindexPrev->nHeight+1)){
        budget.FillBlockPayee(txNew, nFees);
    } else {
        masternodePayments.FillBlockPayee(txNew, nFees);
    }
}

std::string GetRequiredPaymentsString(int nBlockHeight)
{
    if(IsSporkActive(SPORK_13_ENABLE_SUPERBLOCKS) && budget.IsBudgetPaymentBlock(nBlockHeight)){
        return budget.GetRequiredPaymentsString(nBlockHeight);
    } else {
        return masternodePayments.GetRequiredPaymentsString(nBlockHeight);
    }
=======
    return true;

>>>>>>> dev-1.12.1.0
}



std::string CMasternodePaymentVote::ToString() const

{

<<<<<<< HEAD
    bool hasPayment = true;
    CScript payee;

    //spork
    if(!masternodePayments.GetBlockPayee(pindexPrev->nHeight+1, payee)){
        //no masternode detected
        CMasternode* winningNode = mnodeman.GetCurrentMasterNode(1);
        if(winningNode){
            payee = GetScriptForDestination(winningNode->pubkey.GetID());
        } else {
            LogPrintf("CreateNewBlock: Failed to detect masternode to pay\n");
            hasPayment = false;
        }
    }

    CAmount blockValue = GetBlockValue(pindexPrev->nBits, pindexPrev->nHeight, nFees);
    CAmount masternodePayment = GetMasternodePayment(pindexPrev->nHeight+1, blockValue);

    txNew.vout[0].nValue = blockValue;
=======
    std::ostringstream info;

>>>>>>> dev-1.12.1.0


    info << vinMasternode.prevout.ToStringShort() <<

            ", " << nBlockHeight <<

            ", " << ScriptToAsmStr(payee) <<

            ", " << (int)vchSig.size();



    return info.str();

}

<<<<<<< HEAD
int CMasternodePayments::GetMinMasternodePaymentsProto() {
    return IsSporkActive(SPORK_10_MASTERNODE_PAY_UPDATED_NODES)
            ? MIN_MASTERNODE_PAYMENT_PROTO_VERSION_2
            : MIN_MASTERNODE_PAYMENT_PROTO_VERSION_1;
}

void CMasternodePayments::ProcessMessageMasternodePayments(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    if(!masternodeSync.IsBlockchainSynced()) return;

    if(fLiteMode) return; //disable all Darksend/Masternode related functionality

=======


// Send all votes up to nCountNeeded blocks (but not more than GetStorageLimit)

void CMasternodePayments::Sync(CNode* pnode, int nCountNeeded)

{
>>>>>>> dev-1.12.1.0

    LOCK(cs_mapMasternodeBlocks);

<<<<<<< HEAD
        int nCountNeeded;
        vRecv >> nCountNeeded;

        if(Params().NetworkID() == CBaseChainParams::MAIN){
            if(pfrom->HasFulfilledRequest("mnget")) {
                LogPrintf("mnget - peer already asked me for the list\n");
                Misbehaving(pfrom->GetId(), 20);
                return;
            }
        }

        pfrom->FulfilledRequest("mnget");
        masternodePayments.Sync(pfrom, nCountNeeded);
        LogPrintf("mnget - Sent Masternode winners to %s\n", pfrom->addr.ToString().c_str());
    }
    else if (strCommand == "mnw") { //Masternode Payments Declare Winner
        //this is required in litemodef
        CMasternodePaymentWinner winner;
        vRecv >> winner;

        if(pfrom->nVersion < MIN_MNW_PEER_PROTO_VERSION) return;

        int nHeight;
        {
            TRY_LOCK(cs_main, locked);
            if(!locked || chainActive.Tip() == NULL) return;
            nHeight = chainActive.Tip()->nHeight;
        }

        if(masternodePayments.mapMasternodePayeeVotes.count(winner.GetHash())){
            LogPrint("mnpayments", "mnw - Already seen - %s bestHeight %d\n", winner.GetHash().ToString().c_str(), nHeight);
            masternodeSync.AddedMasternodeWinner(winner.GetHash());
            return;
        }

        int nFirstBlock = nHeight - (mnodeman.CountEnabled()*1.25);
        if(winner.nBlockHeight < nFirstBlock || winner.nBlockHeight > nHeight+20){
            LogPrint("mnpayments", "mnw - winner out of range - FirstBlock %d Height %d bestHeight %d\n", nFirstBlock, winner.nBlockHeight, nHeight);
            return;
        }

        std::string strError = "";
        if(!winner.IsValid(pfrom, strError)){
            if(strError != "") LogPrintf("mnw - invalid message - %s\n", strError);
            return;
        }

        if(!masternodePayments.CanVote(winner.vinMasternode.prevout, winner.nBlockHeight)){
            LogPrintf("mnw - masternode already voted - %s\n", winner.vinMasternode.prevout.ToStringShort());
            return;
        }

        if(!winner.SignatureValid()){
            LogPrintf("mnw - invalid signature\n");
            if(masternodeSync.IsSynced()) Misbehaving(pfrom->GetId(), 20);
            // it could just be a non-synced masternode
            mnodeman.AskForMN(pfrom, winner.vinMasternode);
            return;
        }

        CTxDestination address1;
        ExtractDestination(winner.payee, address1);
        CBitcoinAddress address2(address1);

        LogPrint("mnpayments", "mnw - winning vote - Addr %s Height %d bestHeight %d - %s\n", address2.ToString().c_str(), winner.nBlockHeight, nHeight, winner.vinMasternode.prevout.ToStringShort());

        if(masternodePayments.AddWinningMasternode(winner)){
            winner.Relay();
            masternodeSync.AddedMasternodeWinner(winner.GetHash());
        }
    }
}
=======


    if(!pCurrentBlockIndex) return;



    if(pnode->nVersion < 70202) {

        // Old nodes can only sync via heavy method

        int nLimit = GetStorageLimit();

        if(nCountNeeded > nLimit) nCountNeeded = nLimit;

    } else {

        // New nodes request missing payment blocks themselves, push only votes for future blocks to them
>>>>>>> dev-1.12.1.0

        nCountNeeded = 0;

<<<<<<< HEAD
    std::string strMessage =  vinMasternode.prevout.ToStringShort() +
                boost::lexical_cast<std::string>(nBlockHeight) +
                payee.ToString();

    if(!darkSendSigner.SignMessage(strMessage, errorMessage, vchSig, keyMasternode)) {
        LogPrintf("CMasternodePing::Sign() - Error: %s\n", errorMessage.c_str());
        return false;
=======
>>>>>>> dev-1.12.1.0
    }



    int nInvCount = 0;


<<<<<<< HEAD
// Is this masternode scheduled to get paid soon? 
// -- Only look ahead up to 8 blocks to allow for propagation of the latest 2 winners
bool CMasternodePayments::IsScheduled(CMasternode& mn, int nNotBlockHeight)
{
    LOCK(cs_mapMasternodeBlocks);

    int nHeight;
    {
        TRY_LOCK(cs_main, locked);
        if(!locked || chainActive.Tip() == NULL) return false;
        nHeight = chainActive.Tip()->nHeight;
    }
=======
>>>>>>> dev-1.12.1.0

    for(int h = pCurrentBlockIndex->nHeight - nCountNeeded; h < pCurrentBlockIndex->nHeight + 20; h++) {

        if(mapMasternodeBlocks.count(h)) {

            BOOST_FOREACH(CMasternodePayee& payee, mapMasternodeBlocks[h].vecPayees) {

                std::vector<uint256> vecVoteHashes = payee.GetVoteHashes();

                BOOST_FOREACH(uint256& hash, vecVoteHashes) {

                    if(!HasVerifiedPaymentVote(hash)) continue;

                    pnode->PushInventory(CInv(MSG_MASTERNODE_PAYMENT_VOTE, hash));

                    nInvCount++;

<<<<<<< HEAD
    CScript payee;
    for(int64_t h = nHeight; h <= nHeight+8; h++){
        if(h == nNotBlockHeight) continue;
        if(mapMasternodeBlocks.count(h)){
            if(mapMasternodeBlocks[h].GetPayee(payee)){
                if(mnpayee == payee) {
                    return true;
=======
>>>>>>> dev-1.12.1.0
                }

            }

        }

    }



    LogPrintf("CMasternodePayments::Sync -- Sent %d votes to peer %d\n", nInvCount, pnode->id);

    pnode->PushMessage(NetMsgType::SYNCSTATUSCOUNT, MASTERNODE_SYNC_MNW, nInvCount);

}



// Request low data/unknown payment blocks in batches directly from some node instead of/after preliminary Sync.

void CMasternodePayments::RequestLowDataPaymentBlocks(CNode* pnode)

{

<<<<<<< HEAD
    {
        LOCK2(cs_mapMasternodePayeeVotes, cs_mapMasternodeBlocks);
    
        if(mapMasternodePayeeVotes.count(winnerIn.GetHash())){
           return false;
        }

        mapMasternodePayeeVotes[winnerIn.GetHash()] = winnerIn;

        if(!mapMasternodeBlocks.count(winnerIn.nBlockHeight)){
           CMasternodeBlockPayees blockPayees(winnerIn.nBlockHeight);
           mapMasternodeBlocks[winnerIn.nBlockHeight] = blockPayees;
        }
    }

    int n = 1;
    if(IsReferenceNode(winnerIn.vinMasternode)) n = 100;
    mapMasternodeBlocks[winnerIn.nBlockHeight].AddPayee(winnerIn.payee, n);
=======
    // Old nodes can't process this

    if(pnode->nVersion < 70202) return;

    if(!pCurrentBlockIndex) return;

>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
bool CMasternodeBlockPayees::IsTransactionValid(const CTransaction& txNew)
{
    LOCK(cs_vecPayments);

    int nMaxSignatures = 0;
    std::string strPayeesPossible = "";

    CAmount masternodePayment = GetMasternodePayment(nBlockHeight, txNew.GetValueOut());

    //require at least 6 signatures
=======
    LOCK2(cs_main, cs_mapMasternodeBlocks);

>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
    // if we don't have at least 6 signatures on a payee, approve whichever is the longest chain
    if(nMaxSignatures < MNPAYMENTS_SIGNATURES_REQUIRED) return true;

    BOOST_FOREACH(CMasternodePayee& payee, vecPayments)
    {
        bool found = false;
        BOOST_FOREACH(CTxOut out, txNew.vout){
            if(payee.scriptPubKey == out.scriptPubKey && masternodePayment == out.nValue){
                found = true;
            }
        }

        if(payee.nVotes >= MNPAYMENTS_SIGNATURES_REQUIRED){
            if(found) return true;
=======
    std::vector<CInv> vToFetch;

    int nLimit = GetStorageLimit();
>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
            if(strPayeesPossible == ""){
                strPayeesPossible += address2.ToString();
            } else {
                strPayeesPossible += "," + address2.ToString();
            }
        }
    }
=======
>>>>>>> dev-1.12.1.0

    const CBlockIndex *pindex = pCurrentBlockIndex;


<<<<<<< HEAD
std::string CMasternodeBlockPayees::GetRequiredPaymentsString()
{
    LOCK(cs_vecPayments);

    std::string ret = "Unknown";
=======
>>>>>>> dev-1.12.1.0

    while(pCurrentBlockIndex->nHeight - pindex->nHeight < nLimit) {

<<<<<<< HEAD
        if(ret != "Unknown"){
            ret += ", " + address2.ToString() + ":" + boost::lexical_cast<std::string>(payee.nVotes);
        } else {
            ret = address2.ToString() + ":" + boost::lexical_cast<std::string>(payee.nVotes);
        }
    }
=======
        if(!mapMasternodeBlocks.count(pindex->nHeight)) {
>>>>>>> dev-1.12.1.0

            // We have no idea about this block height, let's ask

<<<<<<< HEAD
std::string CMasternodePayments::GetRequiredPaymentsString(int nBlockHeight)
{
    LOCK(cs_mapMasternodeBlocks);

    if(mapMasternodeBlocks.count(nBlockHeight)){
        return mapMasternodeBlocks[nBlockHeight].GetRequiredPaymentsString();
    }
=======
            vToFetch.push_back(CInv(MSG_MASTERNODE_PAYMENT_BLOCK, pindex->GetBlockHash()));
>>>>>>> dev-1.12.1.0

            // We should not violate GETDATA rules

<<<<<<< HEAD
bool CMasternodePayments::IsTransactionValid(const CTransaction& txNew, int nBlockHeight)
{
    LOCK(cs_mapMasternodeBlocks);

    if(mapMasternodeBlocks.count(nBlockHeight)){
        return mapMasternodeBlocks[nBlockHeight].IsTransactionValid(txNew);
    }
=======
            if(vToFetch.size() == MAX_INV_SZ) {
>>>>>>> dev-1.12.1.0

                LogPrintf("CMasternodePayments::SyncLowDataPaymentBlocks -- asking peer %d for %d blocks\n", pnode->id, MAX_INV_SZ);

<<<<<<< HEAD
void CMasternodePayments::CleanPaymentList()
{
    LOCK2(cs_mapMasternodePayeeVotes, cs_mapMasternodeBlocks);

    int nHeight;
    {
        TRY_LOCK(cs_main, locked);
        if(!locked || chainActive.Tip() == NULL) return;
        nHeight = chainActive.Tip()->nHeight;
    }

    //keep up to five cycles for historical sake
    int nLimit = std::max(int(mnodeman.size()*1.25), 1000);

    std::map<uint256, CMasternodePaymentWinner>::iterator it = mapMasternodePayeeVotes.begin();
    while(it != mapMasternodePayeeVotes.end()) {
        CMasternodePaymentWinner winner = (*it).second;

        if(nHeight - winner.nBlockHeight > nLimit){
            LogPrint("mnpayments", "CMasternodePayments::CleanPaymentList - Removing old Masternode payment - block %d\n", winner.nBlockHeight);
            masternodeSync.mapSeenSyncMNW.erase((*it).first);
            mapMasternodePayeeVotes.erase(it++);
            mapMasternodeBlocks.erase(winner.nBlockHeight);
        } else {
            ++it;
=======
                pnode->PushMessage(NetMsgType::GETDATA, vToFetch);

                // Start filling new batch

                vToFetch.clear();

            }

>>>>>>> dev-1.12.1.0
        }

        if(!pindex->pprev) break;

        pindex = pindex->pprev;

    }

<<<<<<< HEAD
bool IsReferenceNode(CTxIn& vin)
{
    //reference node - hybrid mode
    if(vin.prevout.ToStringShort() == "099c01bea63abd1692f60806bb646fa1d288e2d049281225f17e499024084e28-0") return true; // mainnet
    if(vin.prevout.ToStringShort() == "fbc16ae5229d6d99181802fd76a4feee5e7640164dcebc7f8feb04a7bea026f8-0") return true; // testnet
    if(vin.prevout.ToStringShort() == "e466f5d8beb4c2d22a314310dc58e0ea89505c95409754d0d68fb874952608cc-1") return true; // regtest
=======
>>>>>>> dev-1.12.1.0


<<<<<<< HEAD
bool CMasternodePaymentWinner::IsValid(CNode* pnode, std::string& strError)
{
    if(IsReferenceNode(vinMasternode)) return true;

    CMasternode* pmn = mnodeman.Find(vinMasternode);

    if(!pmn)
    {
        strError = strprintf("Unknown Masternode %s", vinMasternode.prevout.ToStringShort());
        LogPrintf ("CMasternodePaymentWinner::IsValid - %s\n", strError);
        mnodeman.AskForMN(pnode, vinMasternode);
        return false;
    }

    if(pmn->protocolVersion < MIN_MNW_PEER_PROTO_VERSION)
    {
        strError = strprintf("Masternode protocol too old %d - req %d", pmn->protocolVersion, MIN_MNW_PEER_PROTO_VERSION);
        LogPrintf ("CMasternodePaymentWinner::IsValid - %s\n", strError);
        return false;
    }

    int n = mnodeman.GetMasternodeRank(vinMasternode, nBlockHeight-100, MIN_MNW_PEER_PROTO_VERSION);

    if(n > MNPAYMENTS_SIGNATURES_TOTAL)
    {    
        //It's common to have masternodes mistakenly think they are in the top 10
        // We don't want to print all of these messages, or punish them unless they're way off
        if(n > MNPAYMENTS_SIGNATURES_TOTAL*2)
        {
            strError = strprintf("Masternode not in the top %d (%d)", MNPAYMENTS_SIGNATURES_TOTAL, n);
            LogPrintf("CMasternodePaymentWinner::IsValid - %s\n", strError);
            if(masternodeSync.IsSynced()) Misbehaving(pnode->GetId(), 20);
        }
        return false;
    }
=======
    std::map<int, CMasternodeBlockPayees>::iterator it = mapMasternodeBlocks.begin();



    while(it != mapMasternodeBlocks.end()) {
>>>>>>> dev-1.12.1.0

        int nTotalVotes = 0;

<<<<<<< HEAD
bool CMasternodePayments::ProcessBlock(int nBlockHeight)
{
    if(!fMasterNode) return false;
=======
        bool fFound = false;

        BOOST_FOREACH(CMasternodePayee& payee, it->second.vecPayees) {
>>>>>>> dev-1.12.1.0

            if(payee.GetVoteCount() >= MNPAYMENTS_SIGNATURES_REQUIRED) {

<<<<<<< HEAD
    if(!IsReferenceNode(activeMasternode.vin)){
        int n = mnodeman.GetMasternodeRank(activeMasternode.vin, nBlockHeight-100, MIN_MNW_PEER_PROTO_VERSION);

        if(n == -1)
        {
            LogPrint("mnpayments", "CMasternodePayments::ProcessBlock - Unknown Masternode\n");
            return false;
        }

        if(n > MNPAYMENTS_SIGNATURES_TOTAL)
        {
            LogPrint("mnpayments", "CMasternodePayments::ProcessBlock - Masternode not in the top %d (%d)\n", MNPAYMENTS_SIGNATURES_TOTAL, n);
            return false;
=======
                fFound = true;

                break;

            }

            nTotalVotes += payee.GetVoteCount();

        }

        // A clear winner (MNPAYMENTS_SIGNATURES_REQUIRED+ votes) was found

        // or no clear winner was found but there are at least avg number of votes

        if(fFound || nTotalVotes >= (MNPAYMENTS_SIGNATURES_TOTAL + MNPAYMENTS_SIGNATURES_REQUIRED)/2) {

            // so just move to the next block

            ++it;

            continue;

>>>>>>> dev-1.12.1.0
        }

<<<<<<< HEAD
    if(nBlockHeight <= nLastBlockHeight) return false;
=======
        // DEBUG
>>>>>>> dev-1.12.1.0

        DBG (

<<<<<<< HEAD
    if(budget.IsBudgetPaymentBlock(nBlockHeight)){
        //is budget payment block -- handled by the budgeting software
    } else {
        LogPrintf("CMasternodePayments::ProcessBlock() Start nHeight %d - vin %s. \n", nBlockHeight, activeMasternode.vin.ToString().c_str());

        // pay to the oldest MN that still had no payment but its input is old enough and it was active long enough
        int nCount = 0;
        CMasternode *pmn = mnodeman.GetNextMasternodeInQueueForPayment(nBlockHeight, true, nCount);
        
        if(pmn != NULL)
        {
            LogPrintf("CMasternodePayments::ProcessBlock() Found by FindOldestNotInVec \n");
=======
            // Let's see why this failed

            BOOST_FOREACH(CMasternodePayee& payee, it->second.vecPayees) {

                CTxDestination address1;

                ExtractDestination(payee.GetPayee(), address1);
>>>>>>> dev-1.12.1.0

                CBitcoinAddress address2(address1);

                printf("payee %s votes %d\n", address2.ToString().c_str(), payee.GetVoteCount());

<<<<<<< HEAD
            CScript payee = GetScriptForDestination(pmn->pubkey.GetID());
            newWinner.AddPayee(payee);

            CTxDestination address1;
            ExtractDestination(payee, address1);
            CBitcoinAddress address2(address1);

            LogPrintf("CMasternodePayments::ProcessBlock() Winner payee %s nHeight %d. \n", address2.ToString().c_str(), newWinner.nBlockHeight);
        } else {
            LogPrintf("CMasternodePayments::ProcessBlock() Failed to find masternode to pay\n");
=======
            }

            printf("block %d votes total %d\n", it->first, nTotalVotes);

        )

        // END DEBUG

        // Low data block found, let's try to sync it

        uint256 hash;

        if(GetBlockHash(hash, it->first)) {

            vToFetch.push_back(CInv(MSG_MASTERNODE_PAYMENT_BLOCK, hash));

>>>>>>> dev-1.12.1.0
        }

        // We should not violate GETDATA rules

        if(vToFetch.size() == MAX_INV_SZ) {

            LogPrintf("CMasternodePayments::SyncLowDataPaymentBlocks -- asking peer %d for %d blocks\n", pnode->id, MAX_INV_SZ);

            pnode->PushMessage(NetMsgType::GETDATA, vToFetch);

            // Start filling new batch

            vToFetch.clear();

<<<<<<< HEAD
    LogPrintf("CMasternodePayments::ProcessBlock() - Signing Winner\n");
    if(newWinner.Sign(keyMasternode, pubKeyMasternode))
    {
        LogPrintf("CMasternodePayments::ProcessBlock() - AddWinningMasternode\n");

        if(AddWinningMasternode(newWinner))
        {
            newWinner.Relay();
            nLastBlockHeight = nBlockHeight;
            return true;
=======
>>>>>>> dev-1.12.1.0
        }

        ++it;

    }

    // Ask for the rest of it

<<<<<<< HEAD
void CMasternodePaymentWinner::Relay()
{
    CInv inv(MSG_MASTERNODE_WINNER, GetHash());
    RelayInv(inv);
=======
    if(!vToFetch.empty()) {

        LogPrintf("CMasternodePayments::SyncLowDataPaymentBlocks -- asking peer %d for %d blocks\n", pnode->id, vToFetch.size());

        pnode->PushMessage(NetMsgType::GETDATA, vToFetch);

    }

>>>>>>> dev-1.12.1.0
}



std::string CMasternodePayments::ToString() const

{

    std::ostringstream info;

<<<<<<< HEAD
    if(pmn != NULL)
    {
        std::string strMessage =  vinMasternode.prevout.ToStringShort() +
                    boost::lexical_cast<std::string>(nBlockHeight) +
                    payee.ToString();

        std::string errorMessage = "";
        if(!darkSendSigner.VerifyMessage(pmn->pubkey2, vchSig, strMessage, errorMessage)){
            return error("CMasternodePaymentWinner::SignatureValid() - Got bad Masternode address signature %s \n", vinMasternode.ToString().c_str());
        }
=======


    info << "Votes: " << (int)mapMasternodePaymentVotes.size() <<
>>>>>>> dev-1.12.1.0

            ", Blocks: " << (int)mapMasternodeBlocks.size();



    return info.str();

}

<<<<<<< HEAD
void CMasternodePayments::Sync(CNode* node, int nCountNeeded)
{
    LOCK(cs_mapMasternodePayeeVotes);

    int nHeight;
    {
        TRY_LOCK(cs_main, locked);
        if(!locked || chainActive.Tip() == NULL) return;
        nHeight = chainActive.Tip()->nHeight;
    }

    int nCount = (mnodeman.CountEnabled()*1.25);
    if(nCountNeeded > nCount) nCountNeeded = nCount;

    int nInvCount = 0;
    std::map<uint256, CMasternodePaymentWinner>::iterator it = mapMasternodePayeeVotes.begin();
    while(it != mapMasternodePayeeVotes.end()) {
        CMasternodePaymentWinner winner = (*it).second;
        if(winner.nBlockHeight >= nHeight-nCountNeeded && winner.nBlockHeight <= nHeight + 20) {
            node->PushInventory(CInv(MSG_MASTERNODE_WINNER, winner.GetHash()));
            nInvCount++;
        }
        ++it;
    }
    node->PushMessage("ssc", MASTERNODE_SYNC_MNW, nInvCount);
}

std::string CMasternodePayments::ToString() const
{
    std::ostringstream info;

    info << "Votes: " << (int)mapMasternodePayeeVotes.size() <<
            ", Blocks: " << (int)mapMasternodeBlocks.size();

    return info.str();
}



int CMasternodePayments::GetOldestBlock()
{
    LOCK(cs_mapMasternodeBlocks);

    int nOldestBlock = std::numeric_limits<int>::max();

    std::map<int, CMasternodeBlockPayees>::iterator it = mapMasternodeBlocks.begin();
    while(it != mapMasternodeBlocks.end()) {
        if((*it).first < nOldestBlock) {
            nOldestBlock = (*it).first;
        }
        it++;
    }

    return nOldestBlock;
}



int CMasternodePayments::GetNewestBlock()
{
    LOCK(cs_mapMasternodeBlocks);

    int nNewestBlock = 0;

    std::map<int, CMasternodeBlockPayees>::iterator it = mapMasternodeBlocks.begin();
    while(it != mapMasternodeBlocks.end()) {
        if((*it).first > nNewestBlock) {
            nNewestBlock = (*it).first;
        }
        it++;
    }

    return nNewestBlock;
=======


bool CMasternodePayments::IsEnoughData()

{

    float nAverageVotes = (MNPAYMENTS_SIGNATURES_TOTAL + MNPAYMENTS_SIGNATURES_REQUIRED) / 2;

    int nStorageLimit = GetStorageLimit();

    return GetBlockCount() > nStorageLimit && GetVoteCount() > nStorageLimit * nAverageVotes;

}



int CMasternodePayments::GetStorageLimit()

{

    return std::max(int(mnodeman.size() * nStorageCoeff), nMinBlocksToStore);

>>>>>>> dev-1.12.1.0
}



void CMasternodePayments::UpdatedBlockTip(const CBlockIndex *pindex)

{

    pCurrentBlockIndex = pindex;

    LogPrint("mnpayments", "CMasternodePayments::UpdatedBlockTip -- pCurrentBlockIndex->nHeight=%d\n", pCurrentBlockIndex->nHeight);



    ProcessBlock(pindex->nHeight + 10);

}