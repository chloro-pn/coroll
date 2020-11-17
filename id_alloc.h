#ifndef ID_ALLOC_H
#define ID_ALLOC_H

#include <cstdint>

class IdAlloc {
public:
    uint64_t GetId() {
        ++current_id;
        if(current_id == 0) {
            ++current_id;
        }
        return current_id;
    }

    static IdAlloc& instance() {
        static IdAlloc obj;
        return obj;
    }

private:
    uint64_t current_id;
};

#endif // ID_ALLOC_H
