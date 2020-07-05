/*
 * @title rwp_tags.hh
 * Authors: KangSK
 * Written at 20200703
 */

/*
 * @file
 * Header file of Read Write Partitioning Replacement Policy
 * Using clean bits for each cache block, dynamically partition
 * Read and Write Blocks to maximize read hits.
 */

#ifndef __MEM_CACHE_TAGS_RWP_TAGS_HH__
#define __MEM_CACHE_TAGS_RWP_TAGS_HH__

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "base/bitfield.hh"
#include "base/intmath.hh"
#include "base/logging.hh"
#include "base/statistics.hh"
#include "base/types.hh"
#include "base/refcnt.hh"
#include "mem/cache/cache_blk.hh"
#include "mem/cache/tags/base_set_assoc.hh"
#include "mem/packet.hh"
#include "params/RWPTags.hh"


class RWPTags : public BaseSetAssoc
{
    // Data structure used globally by RWP
    
    // Data structure per-block
    int *aLastTouch;
    bool *aWriteOnly;
    
    // Total number (current) of WriteOnly blocks and ReadPossible blocks
    int iTotalWriteOnly;
    int iTotalReadPoss;
    
    // MRU Queue and counter array for each type
    int aSetQueueWriteOnly[32];
    int aSetQueueReadPoss[32];
    int aCounterWriteOnly[32];
    int aCounterReadPoss[32];
    
    // Estimated Best ratio of WriteOnly blocks. value < [0, 1]
    double dEstimBestRatio;
    
    // Constructors
    RWPTags(const Params *p);
    ~RWPTags() {}
    
    // Helper Functions
    void shiftQueue(int *aQueue, int *aCounter, bool toHead);
    void updateQueue(int *aQueue, int *aCounter, int iIndex, bool toHead);
    void calculateBestRatio();
    
    
    BlkType* accessBlock(Addr addr, boll is_secure, Cycles &lat, int context_src);
    BlkType* findVictim(Addr addr) const;
    void insertBlcok(packetPtr pkt, BlkType *blk);
    void invalidate(BlkType *blk);
    
};

#endif // __MEM_CACHE_TAGS_RWP_TAGS_HH__
