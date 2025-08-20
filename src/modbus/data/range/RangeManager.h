#ifndef MB_RANGE_MANAGER_H
#define MB_RANGE_MANAGER_H

#include "ModbusEnums.h"
#include "ModbusRegister.h"

#include <vector>
#include <cstdint> 
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>

namespace mb {
namespace data {

using namespace mb::types;

struct Range;

using InnerMap = std::unordered_map<FuncNumber, std::vector<Range>>;
using RangesMap = std::unordered_map<int, InnerMap>;

struct Range {
    uint16_t start;
    uint16_t end;
    
    Range() : start(0), end(0) {}
    Range(uint16_t s, uint16_t e) : start(s), end(e) {}

    bool overlaps(const uint16_t& o_start, const uint16_t& o_end) const { 
        int diff_start = 0;
        if (start != 0) diff_start = 1;
        return start - diff_start <= o_end && end + 1 >= o_start; 
    }
    
    void merge(const uint16_t& o_start, const uint16_t& o_end) {
    	start = std::min(start, o_start);
    	end = std::max(end, o_end);
    }
};

class RangeManager {    
    public:
        RangeManager() {}
        ~RangeManager() {}

        RangesMap& getRanges();
        bool addRange(const int slave_id, const FuncNumber func, std::string& range_str);
        void addRange(const int slave_id, const FuncNumber func, int start, int end);
        void normalizeRanges();

        void printInfo();

    private:
        void addRange(const int slave_id, const FuncNumber func, Range& range);
        void normalizeRanges(const int slave_id, const FuncNumber func);
        bool tryMergeRanges(Range& r1, const Range& r2) const;

        RangesMap m_ranges;
};

} // data
} // mb

#endif // MB_RANGE_MANAGER_H