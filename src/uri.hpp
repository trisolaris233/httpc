#pragma once

#include <map>
#include <string>
#include <utility>

namespace httpc {

    class Uri {
    public:
        Uri() = default;
        Uri(std::string const& str) noexcept :
            original_(str) {
            ParseUri_(std::forward<const std::string>(str));
        }
        Uri(std::string&& str) noexcept : 
            original_(std::move(str)) {
            ParseUri_(std::forward<std::string>(str));
        }

        inline const std::string& GetUri() const noexcept {
            return this->uri_;
        }

        inline std::string const& GetOriginal() const noexcept {
            return this->original_;
        }

        inline auto GetQueryBegin() noexcept {
            return query_.begin();
        }

        inline auto GetQueryEnd() noexcept {
            return query_.end();
        }

        inline auto GetQueryCBegin() const noexcept {
            return query_.cbegin();
        }

        inline auto GetQueryCEnd() const noexcept {
            return query_.cend();
        }

        template <typename T>
        inline auto operator[](T&& index) {
            return query_[index];
        }
        
        template <typename T>
        inline bool Has(T&& index) const {
            return query_.count(index);
        }

        template <typename T>
        inline auto Find(T&& index) {
            return query_.find(index);
        }

        template <typename T>
        inline void Append(T&& str) {
            using StrType = 
                typename std::remove_reference_t<std::remove_cv_t<decltype(str)>>;
            
            if constexpr (std::is_same<StrType, decltype(this->original_)>::value || 
                std::is_same<StrType,
                    std::add_pointer_t<typename decltype(this->original_)::value_type>
                >::value) {
                original_.append(str);
            } else if constexpr (std::is_same<StrType, typename std::string::value_type>::value) {
                original_.push_back(str);
            }
        }

        

        inline void Update() {
            ParseUri_(original_);
        }

        friend std::ostream& operator<<(std::ostream& os, const Uri& uri) {
            os << uri.GetOriginal();
            return os;
        }

    private:
        std::string                         original_;
        std::string                         uri_;
        std::map<std::string, std::string>  query_;

        template <typename T>
        typename std::enable_if_t<
            std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, std::string>,
            void
        >
        ParseUri_(T&& str) {
            bool    query_flag          = false,
                    query_value_flag    = false;

            std::string tmp(std::forward<std::remove_reference_t<decltype(str)>>(str));
            std::pair<std::string, std::string> tmp_pair;
            
            for (auto itr = tmp.begin(); itr != tmp.end(); ++itr) {
                auto chr = *itr;
                if (chr == '?' && !query_flag) {
                    query_flag = true;
                }
                if (chr == '&' && 1 + itr != tmp.end()) {
                    if (!tmp_pair.first.empty() && !tmp_pair.second.empty()) {
                        this->query_.insert(tmp_pair);
                    }      
                    tmp_pair.first.clear();
                    tmp_pair.second.clear();
                    query_value_flag = false;
                } else if (chr == '=' && !query_value_flag) {
                    query_value_flag = true;
                } else if (!query_flag && !query_value_flag) {
                    uri_.push_back(chr);
                } else if (query_flag) {
                    (query_value_flag) ? tmp_pair.second.push_back(chr) : 
                    tmp_pair.first.push_back(chr);
                }
                
            }
        }

    };

}