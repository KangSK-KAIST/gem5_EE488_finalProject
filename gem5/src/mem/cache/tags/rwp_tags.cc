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
    this->iFlushCounter = 0;
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
RWPTags::addToHistory(int iSet)
{
    int iFound = -1;
    for (int i=0; i<32; i++)
    {
        if (aHistoryBlock[i] == iSet)
        {
            iFound = i;
        }
    }
     
    if (iFound == -1)
    { // New block accessed
        for (int i=32; i>0; i--)
        {
            aHistoryBlock[i] = aHistoryBlock[i-1];
        }
        aHistoryBlock
    }
    else
    { // Block already in history
        
    }
    
}

int
RWPTags::updateQueue(int iSet)
{
    for (int i=0; i<32; i++)
    {
        if (aSetQueueWriteOnly[i] == iSet)
        {
            
        }
    }
}

double
RWPTags::calculateBestRatio()
{
    
}


BaseSetAssoc::BlkType*
RWPTags::accessBlock(Addr addr, bool is_secure, Cycles &lat, int master_id)
{
    // Accesses are based on parent class, no need to do anything special
    BlkType *blk = BaseSetAssoc::accessBlock(addr, is_secure, lat, master_id);
    
    // Update iFlushCounter
    ++iFlushCounter;
    
    
    
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
