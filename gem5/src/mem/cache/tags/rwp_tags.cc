/**
 * @file
 * Definitions of a RWP tag store.
 */

#include "mem/cache/tags/rwp_tags.hh"

#include "base/random.hh"
#include "debug/CacheRepl.hh"
#include "mem/cache/base_set_assoc.hh"

RWPTags::RWPTags(const Params *p)
    : BaseSetAssoc(p)
{
    this->aLastTouch = new int[numBlocks];
    this->aWriteOnly = new bool[numBlocks];
    this->iTotalWriteOnly = numBlocks;
    this->iTotalReadPoss = 0;
    this->aSetQueueWriteOnly = {0};
    this->aSetQueueReadPoss = {0};
    this->aCounterWriteOnly = {0};
    this->aCounterReadPoss = {0};
    this->dEstimBestRatio = 0.0;
}

RWPTags::~RWPTags()
{
    delete[] aLastTouch;
    delete[] aWriteOnly;
}


void
RWPTags::shiftQueue(int *aQueue, int *aCounter, bool toHead)
{
    if (toHead)
    {
        for (int i=31; i>0; --i)
        {
            aQueue[i] = aQueue[i-1];
            aCounter[i] = aCounter[i-1];
        }
        aQueue[0] = 0;
        aCounter[0] = 0;
    }
    else
    {
        for (int i=0; i<31; ++i)
        {
            aQueue[i] = aQueue[i+1];
            aCounter[i] = aCounter[i+1];
        }
        aQueue[32] = 0;
        aCounter[32] = 0;
    }
}


void
RWPTags::updateQueue(int *aQueue, int *aCounter, int iIndex, bool toHead)
{
    int iQueue = aQueue[iIndex];
    int iCounter = aCounter[iIndex];
    
    if (toHead)
    {
        for (int i=iIndex; i>0; --i)
        {
            aQueue[i] = aQueue[i-1];
            aCounter[i] = aCounter[i-1];
        }
        aQueue[0] = iQueue;
        aCounter[0] = iCounter;
    }
    else
    {
        for (int i=iIndex; i<31; ++i)
        {
            aQueue[i] = aQueue[i+1];
            aCounter[i] = aCounter[i+1];
        }
        aQueue[32] = iQueue;
        aCounter[32] = iCounter;
    }
}


void
RWPTags::calculateBestRatio()
{
    int iWO = 0;
    for (int i=0; i<32; ++i)
    {
        if (aCounterWriteOnly[i] > aCounterReadPoss[i])
        {
            ++iWO;
        }
    }
    
    dEstimBestRatio = 32.0;
    dEstimBestRatio /= (double)iWO;
}


BaseSetAssoc::BlkType*
RWPTags::accessBlock(Addr addr, bool is_secure, Cycles &lat, int master_id)
{
    // Accesses are based on parent class, no need to do anything special
    BlkType *blk = BaseSetAssoc::accessBlock(addr, is_secure, lat, master_id);

    for (int i=0; i<numBlocks; ++i)
    {
        if (aSetQueueWriteOnly[i] == blk.set)
        { // Originally WO
            ++aCounterWriteOnly[i];
            updateQueue(aSetQueueWriteOnly, aCounterWriteOnly, i, true);
            break;
        }
        if (aSetQueueReadPoss[i] == blk.set)
        { // Originally RP
            ++aCounterWriteOnly[i];
            updateQueue(aSetQueueWriteOnly, aCounterWriteOnly, i, true);
            break;
        }
        
    }
    
    
    return blk;
}

BaseSetAssoc::BlkType*
RWPTags::findVictim(Addr addr) const
{
    
    BlkType *blk = BaseSetAssoc::findVictim(addr);

    // if all blocks are valid, pick a replacement that is not MRU at random
    if (blk->isValid()) {
        // find a random index within the bounds of the set
        int idx = random_mt.random<int>(1, assoc - 1);
        assert(idx < assoc);
        assert(idx >= 0);
        blk = sets[extractSet(addr)].blks[idx];

        DPRINTF(CacheRepl, "set %x: selecting blk %x for replacement\n",
                blk->set, regenerateBlkAddr(blk->tag, blk->set));
    }
    
    ++iFlushCounter;
    return blk;
}

void
RWPTags::insertBlock(PacketPtr pkt, BlkType *blk)
{
    BaseSetAssoc::insertBlock(pkt, blk);

    int set = extractSet(pkt->getAddr());
    sets[set].moveToHead(blk);
    
    if (blk->isDirty())
    { // New block is a write block
        if (
    }
    else
    { // New block is a read block
        
    }
    ++iFlushCounter;
}

void
RWPTags::invalidate(BlkType *blk)
{
    BaseSetAssoc::invalidate(blk);

    // should be evicted before valid blocks
    int set = blk->set;
    sets[set].moveToTail(blk);
    ++iFlushCounter;
}

RWPTags*
RWPTagsParams::create()
{
    return new RWPTags(this);
}
