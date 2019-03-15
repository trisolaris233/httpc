#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include "request.hpp"

namespace httpc {

    class RequestParser {
        
        enum ParseStatusEnum {
            kStart,
            kHTTPMethod,
            kURI,
            kH,
            kT1,
            kT2,
            kP,
            kSlash,
            kMajorVersion,
            kPointBetweenMajorAndMinor,
            kMinorVersion,
            kFirstLF,
            kHeaderLineStart,
            kHeaderName,
            kHeaderValue,
            kHeaderLws,
            kSpaceBeforeHeaderValue,
            kExceptingNormalNewLine,
            kExceptingNewLineEnding
        };

    public:
        enum ResultType {
            kGood,
            kBad,
            kIndeterminate
        };

        RequestParser() noexcept :
            parse_enum_(kStart) {
        }

        explicit RequestParser(const std::string_view& request) noexcept :
            request_str_(request),
            parse_enum_(kStart) {
        }

        void Reset() {
            this->parse_enum_ = kStart;
            this->power_ = 1;
            this->request_str_.clear();
        }

        template <typename InputIterator>
        std::tuple<ResultType, InputIterator> Parse(Request& request, 
            InputIterator begin, InputIterator end) {
            ResultType res = kBad;
            for (; begin != end; ++begin) {
                if(ParseRequest(request, *begin) == kGood) {
                    return std::make_tuple(kGood, begin);
                }

            }
            //std::cout << "end of parse" << std::endl;
            return std::make_tuple(kIndeterminate, begin);
        }
        

    private:
        std::string      request_str_;
        ParseStatusEnum  parse_enum_;
        int              power_{1};

        template <typename CharT>
        ResultType ParseRequest(
                Request& request_, CharT chr) {
            // there is a problem with the original parsing function
            // the case is that when request line is like this "GET HTTP/1.1"
            // is also available on many broswers, but error will occurs on
            // the original one.
            switch(parse_enum_) {
                case kStart:
                    if (!is_char(chr) || is_ctl(chr) || is_tspecial(chr)) {
                        return kBad;
                    } else {
                        request_.method.push_back(chr);
                        parse_enum_ = kHTTPMethod;
                        return kIndeterminate;
                    }

                case kHTTPMethod:
                    if (chr == ' ') {
                        parse_enum_ = kURI;
                        return kIndeterminate;
                    } else if (!is_char(chr) || is_ctl(chr) || is_tspecial(chr)) {
                        return kBad;
                    } else {
                        request_.method.push_back(chr);
                        return kIndeterminate;
                    }

                case kURI:
                    if (chr == ' ') {
                        parse_enum_ = kH;
                        return kIndeterminate;
                    } else if (is_ctl(chr)) {
                        return kBad;
                    } else {
                        request_.uri.Append(chr);
                        return kIndeterminate;
                    }

                case kH:
                    if (chr == 'H') {
                        parse_enum_ = kT1;
                        return kIndeterminate;
                    } else {
                        return kBad;
                    }

                case kT1:
                    if (chr == 'T') {
                        parse_enum_ = kT2;
                        return kIndeterminate;
                    } else {
                        return kBad;
                    }
                    
                case kT2:
                    if (chr == 'T') {
                        parse_enum_ = kP;
                        return kIndeterminate;
                    } else {
                        return kBad;
                    }

                case kP:
                    if (chr == 'P') {
                        parse_enum_ = kSlash;
                        return kIndeterminate;
                    } else {
                        return kBad;
                    }

                case kSlash:
                    if (chr == '/') {
                        parse_enum_ = kMajorVersion;
                        return kIndeterminate;
                    } else {
                        return kBad;
                    }

                case kMajorVersion:
                    if (chr == '.') {
                        power_ = 1;
                        parse_enum_ = kMinorVersion;
                        return kIndeterminate;
                    } else if (std::isdigit(chr)) {
                        request_.http_major_version += power_ * static_cast<int>(chr - 0x30);
                        return kIndeterminate;
                    } else {
                        return kBad;
                    }

                case kMinorVersion:
                    if (chr == '\r') {
                        parse_enum_ = kFirstLF;
                        return kIndeterminate;
                    } else if (std::isdigit(chr)) {
                        request_.http_minor_version += power_ * static_cast<int>(chr - 0x30);
                        return kIndeterminate;
                    } else {
                        return kBad;
                    }

                case kFirstLF:
                    if (chr == '\n') {
                        parse_enum_ = kHeaderLineStart;
                        return kIndeterminate;
                    } else {
                        return kBad;
                    }

                case kHeaderLineStart:
                    if (chr == '\r') {
                        parse_enum_ = kExceptingNewLineEnding;
                        return kIndeterminate;
                    } else if (!request_.headers.empty() && (chr == ' ' || chr == '\t')) {
                        parse_enum_ = kHeaderLws;
                        return kIndeterminate;
                    } else if (!is_char(chr) || is_ctl(chr) || is_tspecial(chr)) {
                        return kBad;
                    } else {
                        request_.headers.push_back(Header());
                        request_.headers.back().field.push_back(chr);
                        parse_enum_ = kHeaderName;
                        return kIndeterminate;
                    }

                case kHeaderLws:
                    if (chr == '\r') {
                        parse_enum_ = kExceptingNormalNewLine;
                        return kIndeterminate;
                    } else if (chr == ' ' || chr == '\t') {
                        return kIndeterminate;
                    } else if (is_ctl(chr)) {
                        return kBad;
                    } else {
                        parse_enum_ = kHeaderValue;
                        request_.headers.back().value.push_back(chr);
                        return kIndeterminate;;
                    }

                case kHeaderName:
                    if (chr == ':') {
                        parse_enum_ = kSpaceBeforeHeaderValue;
                        return kIndeterminate;
                    } else if (!is_char(chr) || is_ctl(chr) || is_tspecial(chr)) {
                        return kBad;
                    } else {
                        request_.headers.back().field.push_back(chr);
                        return kIndeterminate;
                    }

                case kSpaceBeforeHeaderValue:
                    if (chr == ' ') {
                        parse_enum_ = kHeaderValue;
                        return kIndeterminate;
                    } else {
                        return kBad;
                    }

                case kHeaderValue:
                    if (chr == '\r') {
                        parse_enum_ = kExceptingNormalNewLine;
                        return kIndeterminate;
                    } else if (is_ctl(chr)) {
                        return kBad;
                    } else {
                        request_.headers.back().value.push_back(chr);
                        return kIndeterminate;
                    }

                case kExceptingNormalNewLine:
                    if (chr == '\n') {
                        parse_enum_ = kHeaderLineStart;
                        return kIndeterminate;
                    } else {
                        return kBad;
                    }

                case kExceptingNewLineEnding:
                    return chr == '\n' ? kGood : kBad;
                    
                
                default:
                    return kBad;
            }
        }

    };

} // httpc