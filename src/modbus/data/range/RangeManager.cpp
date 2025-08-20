#include "RangeManager.h"
#include "Logger.h"

namespace mb {
namespace data {

using namespace mb::log;

void RangeManager::addRange(const int slave_id, const FuncNumber func, Range& range) {
	std::vector<Range>& ranges = m_ranges[slave_id][func];
	bool is_merge = false;
	for (auto it = ranges.begin(); it != ranges.end(); it++) {
		is_merge = tryMergeRanges(*it, range);
		break;
   }
	if (!is_merge) ranges.push_back(range);
}

void RangeManager::addRange(const int slave_id, const FuncNumber func, int start, int end) {
	Range range(start, end);
	
   // Убедимся, что start меньше или равно end
   if (range.start > range.end) {
		int temp = range.start;
		range.start = range.end;
		range.end = temp;
   }

	addRange(slave_id, func, range);    
}

bool RangeManager::addRange(const int slave_id, const FuncNumber func, std::string& range_str) {
	bool result = false;
	bool is_find = false;
	Range range;
	
	range_str = trim(range_str);
	std::vector<std::string> tokens = split(range_str, '-');

	std::string start_str;
	std::string end_str;

	if (tokens.size() >= 2) {
		start_str = trim(tokens[0]);
		end_str = trim(tokens[1]);
	}
	else if (!range_str.empty() && isNumber(range_str)) {
		start_str = range_str;
		end_str = range_str;
	}

	// Проверяем, что обе подстроки являются числами
    if (isNumber(start_str) || isNumber(end_str)) {
    	// Преобразуем строки в целые числа
    	range.start = std::stoi(start_str);
    	range.end = std::stoi(end_str);

    	// Убедимся, что start меньше или равно end
    	if (range.start > range.end) {
			int temp = range.start;
			range.start = range.end;
			range.end = temp;
    	}

		addRange(slave_id, func, range);
		result = true;
    }

 	return result;
}

void RangeManager::normalizeRanges() {
	for (const auto& pair : m_ranges) {
		int slave_id = pair.first;
		const InnerMap& inner_map = pair.second;

		for (const auto& inner_pair : inner_map) {
			FuncNumber func_key = inner_pair.first;
			normalizeRanges(slave_id, func_key);
		}
	}
}

void RangeManager::normalizeRanges(const int slave_id, const FuncNumber func) {
	std::vector<Range>& ranges = m_ranges[slave_id][func];
	std::vector<Range> result;

	if (ranges.empty()) return;

	std::sort(ranges.begin(), ranges.end(), [](const Range& a, const Range& b) { 
		bool result;
		if (a.start == b.start) result = a.end < b.end;
		else result = a.start < b.start;
		return result; 
	});

	result.push_back(ranges.at(0));

	for (int i = 1; i < ranges.size(); i++) {
		Range& last = result.back();
		if (last.overlaps(ranges.at(i).start, ranges.at(i).end)) last.merge(ranges.at(i).start, ranges.at(i).end);
		else result.push_back(ranges.at(i));
	}

	m_ranges[slave_id][func] = std::move(result);
}

bool RangeManager::tryMergeRanges(Range& r1, const Range& r2) const {
	bool result = false;
	if (r1.overlaps(r2.start, r2.end)) { 
		r1.merge(r2.start, r2.end);
		result = true;
	}
	return result;
}

RangesMap& RangeManager::getRanges() { return m_ranges; }

void RangeManager::printInfo() {
	Logger::Instance()->rawLog("********************************* RANGES ********************************");

	for (const auto& out_pair : m_ranges) {
		int slave_id = out_pair.first;
		const InnerMap& inner_map = out_pair.second;

		Logger::Instance()->rawLog("SLAVE_ID %d", slave_id);

		for (const auto& inner_pair : inner_map) {
			FuncNumber func = inner_pair.first;
			const std::vector<Range>& ranges = inner_pair.second;

			Logger::Instance()->rawLog("FUNC %d", func);

			for (const Range& range : ranges) {
				Logger::Instance()->rawLog("[%d-%d]", range.start, range.end);
			}
		}
	}
	Logger::Instance()->rawLog("*************************************************************************");
}


} // data
} // mb