/*-------------------------------------------------------------

Copyright (c) 2016 Mikel Negugogor (http://github.com/mikelneg)
MIT license. See LICENSE.txt in project root for details.

---------------------------------------------------------------*/

#ifndef WOBJOAOWEFWEFG_VN_FLAT_MAP_H_
#define WOBJOAOWEFWEFG_VN_FLAT_MAP_H_

#include <utility>
#include <algorithm>
#include <vector>

namespace vn {

// template <typename K, typename T>
// class flat_map {
//
//    std::vector<std::pair<K,T>> data_;
//
//    inline auto contains(K const& key) const {
//        auto it = std::find_if(begin(data_),end(data_),[&](auto const& v) { return v.first == key; });
//        return std::make_pair(it != end(data_),it);
//    }
//
// public:
//
//    template <typename ...Args>
//    void try_emplace(K const& key, Args&&...args) {
//
//        if (contains(key).first) throw std::runtime_error{};
//
//    }
//
//};

} // namespace
#endif
