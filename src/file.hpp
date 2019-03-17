#pragma once

#include <string>
#include <utility>
#include <iostream>
// #include <unordered_map>

namespace httpc {

    // class Request;
    class File {
        // friend class Request;
    public:
        File() noexcept = default;
        File(std::string const& name) noexcept 
            : name_(name) 
        {}
        template <typename StringT>
        File(std::string const& name, StringT&& content) noexcept
            : name_(name)
            , content_(std::forward<StringT>(content))
        {}

        inline std::string GetName() const noexcept {
            return name_;
        }

        inline const std::string& GetContent() const noexcept {
            return content_;
        }

        inline std::string GetContentType() const noexcept {
            return content_type_;
        }

        inline std::size_t GetLength() const noexcept {
            return content_.length();
        }

        template <typename StringT>
        inline void SetContent(StringT&& content) {
            this->content_.assign(std::forward<StringT>(content));
        }

        template <typename StringT>
        inline void AppendContent(StringT&& content) {
            this->content_.append(std::forward<StringT>(content));
        }

        inline void SetName(std::string const& name) {
            this->name_.assign(name);
        }

        inline void SetType(std::string const& type) {
            this->content_type_.assign(type);
        }

        friend std::ostream& operator<<(std::ostream& os, const File& f) {
            os << f.GetName() << std::endl << f.GetContentType() << std::endl << f.GetContent();
            return os;
        }

    private:
        std::string name_;
        std::string content_;
        std::string content_type_;
        // std::size_t file_length_;

    };

}