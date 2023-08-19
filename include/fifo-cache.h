#ifndef __FIFO_CACHE_H__
#define __FIFO_CACHE_H__

#include <unordered_map>
#include <queue>

#include "cache.h"
#include "request.h"

template<typename K, typename V, typename Hash = std::hash<K> >
class FifoCache: public Cache<K, V, Hash>{
  
  private:
    std::queue<K> fifoQueue;
    std::unordered_map<K, V, Hash> store;

    virtual void evict(){
      if(this->fifoQueue.empty()){
        return;
      }

      K key = this->fifoQueue.front();
      this->fifoQueue.pop();

      if(this->store.find(key) != this->store.end()){
        this->store.erase(key);
      }
    }

  public:
    FifoCache(): Cache<K, V, Hash>::Cache(100){};
    FifoCache(int maxSize): Cache<K, V, Hash>::Cache(maxSize){}
    virtual ~FifoCache(){}

    virtual void put(K key, V value, bool force){
      if(this->store.find(key) != this->store.end()){
        if(force){
          this->store[key] = value;
        }
      }
      else{
        if(this->fifoQueue.size() == this->maxSize){
          this->evict();
        }
        this->store[key] = value;
        this->fifoQueue.push(key);
      }
    }

    virtual std::optional<V> get(K& key){
      if(this->store.find(key) == this->store.end()){
        return std::nullopt;
      }
      return this->store[key];
    }
};

#endif
