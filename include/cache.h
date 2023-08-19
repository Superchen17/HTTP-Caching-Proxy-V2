#ifndef __CACHE_H__
#define __CACHE_H__

#include <optional>

template<typename K, typename V, typename Hash = std::hash<K> >
class Cache{
  protected:
    int maxSize;

    virtual void evict() = 0;

  public:
    Cache(): maxSize(100){}
    Cache(int maxSize): maxSize(maxSize){}
    virtual ~Cache(){}

    virtual void put(K key, V value, bool force) = 0;
    virtual std::optional<V> get(K& key) = 0;
};

#endif
