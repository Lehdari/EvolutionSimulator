#ifndef OBJECT_POOL_HPP
#define OBJECT_POOL_HPP


#include <cstdio>   //  TEMP


template <typename T>
class ObjectPool {
private:
    struct Wrapper {
        T       object;
        bool    active;

        Wrapper(T&& object = T()) :
            object(object), active(true)
        {}

        template <typename... Args>
        Wrapper(Args&&... args) :
            object(std::forward<Args>(args)...), active(true)
        {}
    };

public:
    class Iterator {
    public:
        friend class ObjectPool;

        Iterator& operator++();
        T* operator->();
        T& operator*();

        bool operator!=(const Iterator& it);

        bool isActive() const;

        void deactivate();

    private:
        Iterator(typename std::vector<Wrapper>::iterator&& it,
                 typename std::vector<Wrapper>::iterator* end,
                 uint64_t* nActiveObjects);

        typename std::vector<Wrapper>::iterator     _it;
        typename std::vector<Wrapper>::iterator*    _end;
        uint64_t*   _nActiveObjects;
    };

    ObjectPool(uint64_t maxSize);

    Iterator begin();
    Iterator end();

    uint64_t getNActive() const;

    void tseg();    //  TEMP

    template <typename... Args>
    void emplaceFirstDeactive(Args&&... args);

private:
    std::vector<Wrapper>    _objects;
    typename std::vector<Wrapper>::iterator _end;
    uint64_t                _nActiveObjects;
};


#include "ObjectPool.tcc"


#endif // OBJECT_POOL_HPP
