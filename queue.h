/**
 * @file queue.h
 * @author Rikhard Honkanen (rikhard.honkanen@mail.com)
 * @brief In this assignment, by practicing CI you are supposed to use googletest and a Makefile to
 test and develop a template circular queue class implemented using a circular linked list according
 to the requirements below.

    There shall be a Makefile to build and run the test
        The generated files shall be stored in a directory named build
        There shall be a rule to clean the project.
        The default target shall build the test.
        There shall be a rule to run the test
    Make a private repository and follow the Github Flow strategy
    Make a workflow which is triggered on
        Push to any branch in order to build and run the test
        Pull request to main in order to build and run the test
    Create a status badge in the README.md file of your repo.
    The template circular queue class shall have a template type (T) parameter for type of data
 stored in a queue. Make the class uncopyable by deleteing the copy constructor and the copy
 assignment operator Make the class movable by implementing the move constructor and the move
 assignment operator Size of a queue is specified during runtime when the queue is created Size
 shall be greater than 2 Size is passed to the constructor of the class The nodes shall be created
 and linked in the constructor The class shall have a function to resize a queue No queue shall be
 created The new size shall also be greater than 2 In the case you need to delete nodes which hold
 data elements, nodes with the oldest data elements shall be deleted. The class shall have a
 function to make a queue empty No node shall be created or deleted The class shall have a function
 to read data from a queue No node shall be created or deleted The class shall have a function to
 write data to a queue If the queue is full then the oldest data shall be overwritten No node shall
 be created or deleted The class shall have a function to get size of a queue The class shall have a
 function to return number of data elements stored in a queue The class shall have a function to
 check if a queue is full or not If the template type(T) is an arithmetic type then the class shall
 have a function to return average of the data elements stored in a queue. The function shall not
 change the queue The return type of the function shall be double. The class shall be tested and
 developed using TDD and gtest Test the class for int, float and std::string types To get VG, using
 gmock ensure that the dynamic memory is managed properly and the class is tested fully.
 * @version 0.1
 * @date 2026-02-24
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef QUEUE_H
#define QUEUE_H

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <type_traits>

constexpr int SIZE_MIN{3};

struct IMemory
{
    virtual void *malloc(size_t size) = 0;
    virtual void free(void *ptr) = 0;
    virtual ~IMemory() = default;
};

template <typename T> class Queue
{
    struct node_t
    {
        T data;
        node_t *next;
    };

    IMemory &memory;
    size_t count{0};
    size_t size{0};
    node_t *head{nullptr};
    node_t *tail{nullptr};

    node_t *create_node(const T &data)
    {
        // malloc only allocates memory for a node_t. It does not construct a node_t;
        node_t *node = reinterpret_cast<node_t *>(memory.malloc(sizeof(node_t)));

        if (node != nullptr)
        {
            // The placement new is used to construct an object in an allocated block of memory.
            (void)new (node) node_t{data, nullptr};
        }

        return node;
    }

    void delete_node(node_t *node)
    {
        // The node destructor shall be called
        node->~node_t();

        memory.free(node);
    }

  public:
    Queue(const Queue &) = delete;
    Queue &operator=(const Queue &) = delete;

    Queue(IMemory &_memory, size_t _size) : memory{_memory}, size{_size}
    {
        if (size < SIZE_MIN)
        {
            throw std::invalid_argument{"Invalid size"};
        }

        node_t *previous = nullptr;

        for (size_t i = 0; i < size; ++i)
        {
            node_t *current = create_node(T{});

            if (!head)
                head = current;

            if (previous)
                previous->next = current;

            previous = current;
        }

        previous->next = head;
        tail = head;
    }

    Queue(Queue &&that) noexcept;

    Queue &operator=(Queue &&that) noexcept;

    bool enqueue(const T &item);

    bool dequeue(T &item);

    size_t available(void) { return count; }

    size_t capacity(void) { return size; }

    bool full(void) { return size == count; }

    void clear(void);

    bool queue_resize(int new_size)
    {
        bool status = false;

        if (new_size > 2)
        {
            status = true;

            if (new_size > size)
            {
                node_t *current = tail;

                while (current->next != head) // Go to last node (connecting to head)
                {
                    current = current->next;
                }
                for (size_t i = 0; i < new_size - size; i++)
                {
                    node_t *new_current = create_node(nullptr);
                    current->next = new_current;
                    current = new_current;

                    if (i == (new_size - size - 1)) // Last insert
                    {
                        current->next = head;
                    }
                }
                size = new_size;
            }
            else if (new_size < size)
            {
                int removed = 0;
                if (full())
                {
                    node_t *current = head;

                    for (size_t i = 0; i < size - new_size; i++)
                    {
                        node_t *next = current->next;
                        head = current->next;
                        delete_node(current);
                        current = head;
                    }

                    tail->next = current;
                    count = new_size;
                    size = new_size;
                }
                else
                {
                    node_t *current = tail;

                    for (size_t i = 0; i < size - new_size; i++)
                    {
                        if (current->next->data == NULL)
                        {
                            node_t *new_next = current->next->next;
                            delete_node(current->next);
                            current->next = new_next;
                        }
                        else // Data present, move head
                        {
                            head = head->next;
                            delete_node(current->next);
                            current->next = head;
                        }
                    }
                    size = new_size;
                }
            }
        }

        return status;
    }

    // template <typename U = T> std::enable_if_t<std::is_arithmetic_v<U>, double> average() const
    // {
    //     if (count == 0)
    //         return 0.0;

    //     double sum{0.0};
    //     std::size_t index = head;

    //     for (std::size_t i = 0; i < count; ++i)
    //     {
    //         sum += static_cast<double>(array[index]);
    //         index = (index + 1) % N;
    //     }

    //     return sum / static_cast<double>(count);
    // }

    ~Queue() { clear(); }
};

template <typename T>
Queue<T>::Queue(Queue<T> &&that) noexcept
    : memory{that.memory}, count{that.count}, head{that.head}, tail{that.tail}
{
    that.count = 0;
    that.head = nullptr;
    that.tail = nullptr;
}

template <typename T> Queue<T> &Queue<T>::operator=(Queue<T> &&that) noexcept
{
    if (this != &that)
    {
        clear();

        memory = that.memory;
        count = that.count;
        head = that.head;
        tail = that.tail;

        that.count = 0;
        that.head = nullptr;
        that.tail = nullptr;
    }

    return *this;
}

template <typename T> bool Queue<T>::enqueue(const T &item)
{
    tail->data = item;
    if (count == size) // Queue full
    {
        head = head->next;
    }
    else
    {
        (count)++;
    }
    tail = tail->next;

    return true;
}

template <typename T> bool Queue<T>::dequeue(T &item)
{
    bool status{false};

    if (count > 0)
    {
        status = true;
        item = head->data;
        head = head->next;
        count--;
    }

    return status;
}

template <typename T> void Queue<T>::clear(void)
{
    if (head)
    {
        node_t *current = head;

        for (size_t i = 0; i < size; ++i)
        {
            node_t *next = current->next;
            delete_node(current);
            current = next;
        }

        head = nullptr;
        tail = nullptr;
        count = 0;
    }
}

#endif
