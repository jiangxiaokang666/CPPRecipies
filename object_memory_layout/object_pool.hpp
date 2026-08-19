#pragma once

#include <cstddef>
#include <utility>

template<typename T, std::size_t Capacity>
class ObjectPool
{
private:
    struct Slot
    {
        alignas(T) std::byte storage[sizeof(T)];
        Slot* next = nullptr;
        bool occupied = false;
    };

    Slot m_slots[Capacity];

    Slot* m_freeList = nullptr;

public:
    ObjectPool()
    {
        static_assert(Capacity > 0);
        
        for(std::size_t i = 0; i< Capacity - 1; ++i)
        {
            m_slots[i].next = &m_slots[i+1];
        }
        m_freeList = &m_slots[0];
    }

    ~ObjectPool()
    {
        for(std::size_t i = 0; i< Capacity; ++i)
        {
            Slot& slot = m_slots[i];
            if(slot.occupied)
            {
                T* object = reinterpret_cast<T*>(slot.storage);
                object->~T();
                slot.occupied =false;
            }
        }
    }

    template<typename... Args>
    T* Create(Args... args)
    {
        if(!m_freeList)
        {
            return nullptr;
        }
        Slot* slot = m_freeList;
        m_freeList = slot->next;
        T* object = new(slot->storage) T(std::forward<Args>(args)...);
        slot->occupied = true;
        return object;
    }

    void Destroy(T* object)
    {
        if(!object)
        {
            return;
        }
        for(auto& slot: m_slots)
        {
            if(reinterpret_cast<void*>(slot.storage) == reinterpret_cast<void*>(object))
            {
                object->~T();
                slot.occupied = false;
                slot.next = m_freeList;
                m_freeList = &slot;
                return;
            }
        }
    }

    ObjectPool(const ObjectPool& ) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
};