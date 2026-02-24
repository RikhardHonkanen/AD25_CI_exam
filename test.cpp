#include "queue.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;

class Memory : public IMemory
{
    size_t pos{0};
    uint8_t array[8192]; // Increase size of array if required
    std::vector<void *> vec;

  public:
    MOCK_METHOD(void *, malloc, (size_t size), (override));
    MOCK_METHOD(void, free, (void *ptr), (override));

    void *allocate(size_t size)
    {
        void *ptr{nullptr};

        if ((pos + size) < sizeof(array))
        {
            ptr = &array[pos];
            vec.push_back(ptr);
            pos += size;
        }

        return ptr;
    }

    void release(void *ptr) { vec.erase(std::remove(vec.begin(), vec.end(), ptr), vec.end()); }

    ~Memory() { EXPECT_EQ(0, vec.size()); }
};