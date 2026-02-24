#include "queue.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;

constexpr int SIZE{5};

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

template <typename T> class QueueFixture : public ::testing::Test
{
    const std::tuple<std::vector<int>, std::vector<float>, std::vector<std::string>> allValues{
        {1, 2, 3, 4, 5}, {1.5f, 2.5f, 3.5f, 4.5f, 5.5f}, {"aaa", "bbb", "ccc", "ddd", "eee"}};

  protected:
    const std::vector<T> values{std::get<std::vector<T>>(allValues)};
    NiceMock<Memory> memory;
    Queue<T> queue{memory, SIZE};

    void SetUp(void) override
    {
        EXPECT_EQ(0, queue.available());

        ON_CALL(memory, malloc(_)).WillByDefault(Invoke(&memory, &Memory::allocate));

        ON_CALL(memory, free(_)).WillByDefault(Invoke(&memory, &Memory::release));

        for (size_t i = 1; i <= values.size(); i++)
        {
            EXPECT_TRUE(queue.enqueue(values[i - 1]));
            EXPECT_EQ(i, queue.available());
        }
    }

    void TearDown(void) override {}
};

using TestTypes = ::testing::Types<int, float, std::string>;
TYPED_TEST_SUITE(QueueFixture, TestTypes);

TYPED_TEST(QueueFixture, mallocFails)
{
    EXPECT_CALL(this->memory, malloc(_)).WillRepeatedly(Return(nullptr));
    EXPECT_FALSE(this->queue.enqueue(this->values[0]));
}

TYPED_TEST(QueueFixture, testClear)
{
    EXPECT_EQ(this->values.size(), this->queue.available());
    this->queue.clear();
    EXPECT_EQ(0, this->queue.available());
}
