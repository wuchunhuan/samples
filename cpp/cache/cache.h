#ifndef PROJECT_CACHE_H
#define PROJECT_CACHE_H

#include <cstdint>
#include <unordered_map>
#include <iostream>
#include <map>

//Hash map based memory cache with features as bellow:
//1.Support cache item timeout both globally and individually
//2.Support washing out item by LRU or FIFO strategy or no washing out strategy at all
//3.Arbitrary key value type supported
//Note: Lock mechanism is needed in multi-threaded environment

enum err_code {
    SUCCESS = 0,
    ERR_STRATEGY_TYPE = -1,
    ERR_WASHOUT_REMOVE = -2,
    ERR_WASHOUT_NEWNODE = -3,
    ERR_ITEM_NOTFOUND = -4,
    ERR_ITEM_EXPIRED = -5,
    ERR_CACHE_FULL = -6
};

enum strategy {
    LRU = 0,
    FIFO = 1,
    NO_WASHOUT = 2
};

template<class Key, class Val>
class Cache {
public:
    Cache():m_ItemLifespan(0),m_MaxItemNum(0),m_Strategy(LRU),m_Init(false)
    {}

    int Init(uint32_t maxItemNum, strategy stra, uint32_t lifespan=0);

    int Insert(const Key& key, Val& val, uint32_t lifespan=0);

    int Find(const Key& key, Val& val);

    int Delete(const Key& key);

#ifdef ENABLE_RM_TIMEOUT
    int RmTimeoutItem();
#endif

private:
    class WashOutNode {
    public:
        WashOutNode * next;
        WashOutNode * pre;
        Key key;
    };
    class CacheValue {
    public:
        uint32_t m_Version;
        time_t m_UpdateTime;
        uint32_t m_Lifespan;
        WashOutNode * m_WashoutNode;
        Val m_val;
    };

private:
    uint32_t m_ItemLifespan;
    uint32_t m_MaxItemNum;
    strategy m_Strategy;
    bool m_Init;
    std::unordered_map<Key, CacheValue> m_HashMap;
#ifdef ENABLE_RM_TIMEOUT
    std::map<time_t, Key, std::less<time_t> > m_TimeOutSorter;
#endif
    WashOutNode * m_WashoutHead;
    WashOutNode * m_WashoutTail;
};

template<class Key, class Val>
int Cache<Key, Val>::Init(uint32_t maxItemNum, strategy stra, uint32_t lifespan) {
    if(m_Init) {
        return SUCCESS;
    }

    if (stra < LRU || stra > NO_WASHOUT) {
        std::cout << "Strategy error!" << std::endl;
        return ERR_STRATEGY_TYPE;
    }

    m_MaxItemNum  = maxItemNum;
    m_ItemLifespan = lifespan;
    m_Strategy = stra;
    m_HashMap.clear();
#ifdef ENABLE_RM_TIMEOUT
    m_TimeOutSorter.clear();
#endif
    m_WashoutHead = NULL;
    m_WashoutTail = NULL;
    m_Init = true;

    std::cout << "Init CAo_Cache ok! dwCacheNum " << maxItemNum << ", dwCacheTime "
              << lifespan << ", strategy: " << stra << std::endl;

    return SUCCESS;
}

template<class Key, class Val>
int Cache<Key, Val>::Insert(const Key &key, Val &val, uint32_t lifespan) {
    //当缓存大于最大值时，tail淘汰
    if (m_HashMap.size() >= m_MaxItemNum) {
        if (m_Strategy == NO_WASHOUT) {
            return ERR_CACHE_FULL;
        }
        std::cout << "cache size: " << m_HashMap.size() << "max: " << m_MaxItemNum << " start to wash out." << std::endl;
        WashOutNode * tmp = NULL;
        if (m_Strategy == LRU) {
            tmp = m_WashoutTail;
        } else if (m_Strategy == FIFO){
            tmp = m_WashoutHead;
        }
        if (m_HashMap.erase(tmp->key)) {
            //更新淘汰链
            if (m_Strategy == LRU) {
                WashOutNode * pre = m_WashoutTail->pre;
                if(pre != NULL) {
                    pre->next = NULL;
                }
                delete tmp;
                if(m_WashoutTail == m_WashoutHead) {
                    m_WashoutTail = m_WashoutHead = NULL;
                } else {
                    m_WashoutTail = pre;
                }
            } else if (m_Strategy == FIFO){
                WashOutNode * next = m_WashoutHead->next;
                if (next != NULL) {
                    next->pre = NULL;
                }
                delete tmp;
                if(m_WashoutTail == m_WashoutHead) {
                    m_WashoutTail = m_WashoutHead = NULL;
                } else {
                    m_WashoutHead = next;
                }
            }
        } else {
            return ERR_WASHOUT_REMOVE;
        }

    }

    //更新淘汰链
    WashOutNode * pWashNode = NULL;
    if (m_Strategy == LRU || m_Strategy == FIFO) {
        pWashNode = new WashOutNode;
        if(pWashNode == NULL) {
            return ERR_WASHOUT_NEWNODE;
        }
        pWashNode->key = key;
        pWashNode->pre = NULL;
        pWashNode->next = NULL;

        if (m_Strategy == LRU) {
            if(m_WashoutHead == NULL) {
                m_WashoutHead = pWashNode;
                m_WashoutTail = pWashNode;
            } else {
                //head插入
                m_WashoutHead->pre = pWashNode;
                pWashNode->next = m_WashoutHead;
                m_WashoutHead = pWashNode;

            }
        } else if (m_Strategy == FIFO){
            if (m_WashoutTail == NULL) {
                m_WashoutHead = pWashNode;
                m_WashoutTail = pWashNode;
            } else {
                m_WashoutTail->next = pWashNode;
                pWashNode->pre = m_WashoutTail;
                m_WashoutTail = pWashNode;
            }
        }
    }


    //插入cache
    CacheValue oCacheValue;
    oCacheValue.m_UpdateTime = time(NULL);
    oCacheValue.m_Lifespan = lifespan;
    oCacheValue.m_val = val;
    oCacheValue.m_WashoutNode = pWashNode;

    m_HashMap.insert(std::pair<Key, CacheValue>(key, oCacheValue));
#ifdef ENABLE_RM_TIMEOUT
    time_t washoutTime = 0;
    if (lifespan > 0) {
        washoutTime = oCacheValue.m_UpdateTime + lifespan;
        m_TimeOutSorter.insert(std::pair<time_t, Key>(washoutTime, key));
    } else if (m_ItemLifespan > 0) {
        washoutTime = oCacheValue.m_UpdateTime + m_ItemLifespan;
        m_TimeOutSorter.insert(std::pair<time_t, Key>(washoutTime, key));
    }
#endif

    //std::cout << "Insert key: " << Key << " now: " << oCacheValue.m_UpdateTime << std::endl;
    //std::cout << "cache size: " << m_HashMap.size() << std::endl;

    return SUCCESS;
}

template<class Key, class Val>
int Cache<Key, Val>::Find(const Key &key, Val &val) {

    typename std::unordered_map<Key, CacheValue>::iterator itFind = m_HashMap.find(key);

    if (itFind == m_HashMap.end()) {
        std::cout << "not find in cache!" << std::endl;
        return ERR_ITEM_NOTFOUND;
    }

    time_t tNow = time(NULL);
    WashOutNode * tmp = itFind->second.m_WashoutNode;

    bool timeOut = false;
    if (itFind->second.m_Lifespan > 0) {
        if (tNow - itFind->second.m_UpdateTime > itFind->second.m_Lifespan) {
            std::cout << "timeout, tNow: " << tNow << ", oValue.m_UpdateTime: "
                      << itFind->second.m_UpdateTime << ", m_Lifespan: " << itFind->second.m_Lifespan << std::endl;
            timeOut = true;
        }
    } else {
        if(m_ItemLifespan > 0 && tNow - itFind->second.m_UpdateTime > m_ItemLifespan) {
            std::cout << "timeout, tNow: " << tNow << ", oValue.m_UpdateTime: "
                      << itFind->second.m_UpdateTime << ", m_ItemLifespan: " << m_ItemLifespan << std::endl;
            timeOut = true;
        }
    }

    if(timeOut) {
        //超时更新淘汰链
        if(tmp != NULL) {
            if(tmp->pre != NULL) {
                tmp->pre->next = tmp->next;
            }
            if(tmp->next != NULL) {
                tmp->next->pre = tmp->pre;
            }

            if(tmp == m_WashoutHead) {
                m_WashoutHead = tmp->next;
            }
            if(tmp == m_WashoutTail) {
                m_WashoutTail = tmp->pre;
            }
            delete tmp;
        }
        //从cache中删除
        m_HashMap.erase(itFind);
        //std::cout << "timeout, delete key: " << itFind->first << std::endl;
        return ERR_ITEM_EXPIRED;
    }

    val = itFind->second.m_val;
    //更新淘汰链,放到head
    if (m_Strategy == LRU) {
        if(tmp != m_WashoutHead) {

            if(tmp->pre != NULL) {
                tmp->pre->next = tmp->next;
            }
            if(tmp->next != NULL) {
                tmp->next->pre = tmp->pre;
            }
            if(tmp == m_WashoutTail) {
                m_WashoutTail = tmp->pre;
            }

            tmp->next = m_WashoutHead;
            tmp->pre = NULL;
            m_WashoutHead->pre = tmp;
            m_WashoutHead = tmp;

        }
    }

    return SUCCESS;
}

template<class Key, class Val>
int Cache<Key, Val>::Delete(const Key &key) {
    typename std::unordered_map<Key, CacheValue>::iterator itFind = m_HashMap.find(key);

    if (itFind == m_HashMap.end()) {
        std::cout << "not find in cache!" << std::endl;
        return ERR_ITEM_NOTFOUND;
    }
    WashOutNode * tmp = itFind->second.m_WashoutNode;
    //超时更新淘汰链
    if(tmp != NULL) {
        if(tmp->pre != NULL) {
            tmp->pre->next = tmp->next;
        }
        if(tmp->next != NULL) {
            tmp->next->pre = tmp->pre;
        }

        if(tmp == m_WashoutHead) {
            m_WashoutHead = tmp->next;
        }
        if(tmp == m_WashoutTail) {
            m_WashoutTail = tmp->pre;
        }
        delete tmp;
    }
    //从cache中删除
    m_HashMap.erase(itFind);

    return SUCCESS;
}

#ifdef ENABLE_RM_TIMEOUT
template<class Key, class Val>
int Cache<Key, Val>::RmTimeoutItem() {
    time_t tNow = time(NULL);
    std::map<time_t, Key>::iterator iter = m_TimeOutSorter.begin();
    while (iter != m_TimeOutSorter.end()) {
        if (iter->first < tNow) {
            iter++;
        }
    }

    m_TimeOutSorter.erase(m_TimeOutSorter.begin(), iter);
    return SUCCESS;
}

#endif


#endif //PROJECT_CACHE_H
