#ifndef SAMPLES_SHUFFLER_H
#define SAMPLES_SHUFFLER_H

#include <string>
#include <map>
#include <boost/crc.hpp>
#include <boost/format.hpp>

enum shuffle_type {
    HASH = 0,
    CONSITENT_HASH = 1
};

class vnode {
public:
    vnode() : address_(""), v_index_(0) {}
    vnode(std::string address, size_t index) : address_(address), v_index_(index) {}
    std::string to_str() const {
        return boost::str(boost::format("%1%_%2%") % address_ % v_index_);
    }
    std::string address_;
    size_t v_index_;
};

class crc32_hash {
public:
    crc32_hash() {}
    uint32_t operator()(const vnode& node) {
        boost::crc_32_type rst;
        std::string vnode = node.to_str();
        rst.process_bytes(vnode.c_str(), vnode.size());
        return rst.checksum();
    }
    uint32_t operator()(const std::string& key) {
        boost::crc_32_type rst;
        rst.process_bytes(key.c_str(), key.size());
        return rst.checksum();
    }
    typedef uint32_t result_type;
};

template <typename Hash,
    typename Alloc = std::allocator<std::pair<const typename Hash::result_type, vnode> > >
class shuffler {
public:
    typedef typename Hash::result_type size_type;
    typedef std::map<size_type, vnode, std::less<size_type>, Alloc> map_type;
    typedef typename  map_type::value_type value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef typename map_type::iterator iterator;
    typedef typename map_type::reverse_iterator reverse_iterator;
    typedef Alloc allocator_type;

    shuffler(shuffle_type type = CONSITENT_HASH, size_t vnode_num = 50) : type_(type), vnode_num_(vnode_num){}

    size_t size() const {
        return nodes_.size();
    }

    bool empty() const {
        return nodes_.empty();
    }

    iterator belongs_to(size_type hash) {
        if (nodes_.empty()) {
            return nodes_.end();
        }

        iterator it = nodes_.begin();
        if (type_ == CONSITENT_HASH) {
            it = nodes_.lower_bound(hash);
            if (it == nodes_.end()) {
                it = nodes_.begin();
            }

        } else if (type_ == HASH){
            size_t index = hash % nodes_.size();
            assert(index < nodes_.size());
            for (size_t i = 0; i < index; ++i, ++it) {
            }
        }
        return it;
    }

//        iterator belongs_to(const std::string& key) {
//            size_type hash = hash_(key);
//            return belongs_to(hash);
//        }
//
    std::string belongs_to(const std::string& key) {
        size_type hash = hash_(key);
        iterator it = belongs_to(hash);
        if (it == nodes_.end()) {
            return "";
        }
        return it->second.address_;
    }

    void add(const std::string& address) {
        for (size_t i = 0; i < vnode_num_; ++i) {
            vnode node(address, i);
            insert(node);
        }
    }

    void rm(const std::string& address) {
        for (size_t i = 0; i < vnode_num_; ++i) {
            vnode node(address, i);
            erase(node);
        }
    }

    void reset() {
        nodes_.clear();
    }

private:
    std::pair<iterator, bool> insert(const vnode& node) {
        size_type hash = hash_(node);
        return nodes_.insert(value_type(hash, node));
    };

    void erase(iterator it) {
        nodes_.erase(it);
    }

    size_t erase(const vnode& node) {
        size_type hash = hash_(node);
        return nodes_.erase(hash);
    }

private:
    shuffle_type type_;
    size_t vnode_num_;
    Hash hash_;
    map_type nodes_;
};

//    enum shuffle_type {
//        HASH = 0,
//        CONSITENT_HASH = 1
//    };
//    class shuffler {
//    public:
//        shuffler();
//        void update(const std::vector<std::string>& items);
//        void add(const std::string& key);
//        void rm(const std::string& key);
//        std::string get(const std::string& key);
//
//    private:
//        size_t hash(const std::string& key);
//    private:
//        shuffle_type type_;
//        std::map<std::string, int> items_;
//    };

#endif //SAMPLES_SHUFFLER_H
