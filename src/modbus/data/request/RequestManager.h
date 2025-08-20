#ifndef MB_REQUEST_MANAGER_H
#define MB_REQUEST_MANAGER_H

#define DEFAULT_MAX_COUNT_REGS_READ 8

#include "ModbusRequest.h"
#include "ModbusEnums.h"
#include "MemoryChunk.h"

#include <vector>
#include <string>

namespace mb {
namespace data {

using namespace mb::types;
    
class RequestManager {    
public:
    RequestManager(int max_count_regs_read = DEFAULT_MAX_COUNT_REGS_READ) 
        : m_max_count_regs_read(max_count_regs_read) {}
    
    void addRequest(Request& req);
    void addRequest(int id, FuncNumber func, int addr, int quantity, MemoryChunk* mem_chunk, int offset);
    const int getMaxCountRegsRead();
    const std::vector<Request>& getReadRequests();
    
    void printInfo();

private:
    int m_max_count_regs_read;
    std::vector<Request> m_read_requests;
};

} // data
} // mb

#endif // MB_REQUEST_MANAGER_H