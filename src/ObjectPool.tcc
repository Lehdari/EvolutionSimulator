template <typename T>
typename ObjectPool<T>::Iterator& ObjectPool<T>::Iterator::operator++()
{
    while (!(++_it)->active && _it != *_end);
    return *this;
}

template <typename T>
T* ObjectPool<T>::Iterator::operator->()
{
    return &(_it->object);
}

template <typename T>
T& ObjectPool<T>::Iterator::operator*()
{
    return _it->object;
}

template <typename T>
bool ObjectPool<T>::Iterator::operator!=(const Iterator& it)
{
    return _it != it._it;
}

template <typename T>
bool ObjectPool<T>::Iterator::isActive() const
{
    return _it->active;
}

template <typename T>
void ObjectPool<T>::Iterator::deactivate()
{
    if (_it->active) {
        _it->active = false;
        --*_nActiveObjects;
    }
}

template <typename T>
ObjectPool<T>::Iterator::Iterator(typename std::vector<Wrapper>::iterator&& it,
    typename std::vector<Wrapper>::iterator* end,
    uint64_t* nActiveObjects) :
    _it             (it),
    _end            (end),
    _nActiveObjects (nActiveObjects)
{
}

template <typename T>
ObjectPool<T>::ObjectPool(uint64_t maxSize) :
    _nActiveObjects (0)
{
    _objects.reserve(maxSize);
}

template <typename T>
typename ObjectPool<T>::Iterator ObjectPool<T>::begin()
{
    return Iterator(_objects.begin(), &_end, &_nActiveObjects);
}

template <typename T>
typename ObjectPool<T>::Iterator ObjectPool<T>::end()
{
    return Iterator(_objects.end(), &_end, &_nActiveObjects);
}

template <typename T>
uint64_t ObjectPool<T>::getNActive() const
{
    return _nActiveObjects;
}
//  TEMP
template <typename T>
void ObjectPool<T>::tseg()
{
    uint64_t nActive = 0;
    for (auto& o : _objects) {
        if (o.active)
            ++nActive;
    }
    printf("ObjectPool tseg: %llu, %llu, %llu\n", _nActiveObjects, nActive, _objects.size());
}

template <typename T>
template <typename... Args>
void ObjectPool<T>::emplaceFirstDeactive(Args&&... args)
{
    auto it = _objects.begin();
    for (; it != _objects.end(); ++it) {
        if (!it->active) {
            it->object = std::move(T(std::forward<Args>(args)...));
            it->active = true;
            ++_nActiveObjects;
            return;
        }
    }
    _objects.emplace_back(std::forward<Args>(args)...);
    _end = _objects.end();
    ++_nActiveObjects;
}
