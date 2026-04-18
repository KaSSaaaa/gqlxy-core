#pragma once

#include <algorithm>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace gqlxy::utils {

template <std::ranges::input_range R>
auto to_vector(R&& r) {
    using T = std::ranges::range_value_t<R>;
    return std::vector<T>(std::ranges::begin(r), std::ranges::end(r));
}

template <std::ranges::input_range R>
auto to_set(R&& r) {
    using T = std::ranges::range_value_t<R>;
    return std::set<T>(std::ranges::begin(r), std::ranges::end(r));
}

template <std::ranges::input_range R>
auto to_unordered_set(R&& r) {
    using T = std::ranges::range_value_t<R>;
    return std::unordered_set<T>(std::ranges::begin(r), std::ranges::end(r));
}

template <std::ranges::input_range R>
auto to_unordered_map(R&& r) {
    using T = std::ranges::range_value_t<R>;
    using TKey = T::first_type;
    using TValue = T::second_type;
    return std::unordered_map<TKey, TValue>(std::ranges::begin(r), std::ranges::end(r));
}

template <std::ranges::input_range R>
auto to_map(R&& r) {
    using T = std::ranges::range_value_t<R>;
    using TKey = T::first_type;
    using TValue = T::second_type;
    return std::map<TKey, TValue>(std::ranges::begin(r), std::ranges::end(r));
}

template <std::ranges::input_range R>
auto to_string(R&& r) {
    return std::string(std::ranges::begin(r), std::ranges::end(r));
}

template <typename T>
concept OStreamable = requires(std::ostream& os, const T& v) { os << v; };

template <OStreamable T>
    requires(!std::ranges::input_range<T>)
std::string to_string(const T& value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

template <std::ranges::input_range R>
auto trim(R&& s) {
    static auto is_space = [](auto c) { return std::isspace(static_cast<uint8_t>(c)); };
    auto r = s | std::views::drop_while(is_space) | std::views::reverse | std::views::drop_while(is_space) |
             std::views::reverse;
    return std::string(r.begin(), r.end());
}

inline auto split(std::string_view s, char delim) -> std::vector<std::string> {
    return to_vector(s | std::views::split(delim) | std::views::transform([](auto part) { return to_string(part); }));
}

template <typename F>
auto make_optional_if(bool cond, F&& f) -> std::optional<std::invoke_result_t<F>> {
    if (!cond) return std::nullopt;
    return f();
}

template <std::ranges::range R>
auto to_optional(R&& r, std::ranges::iterator_t<R> it) -> std::optional<std::ranges::range_value_t<R>> {
    if (it == std::ranges::end(r)) return std::nullopt;
    return *it;
}

template <std::ranges::range R, typename Pred>
auto find_optional(R&& r, Pred&& pred) -> std::optional<std::ranges::range_value_t<R>> {
    return to_optional(r, std::ranges::find_if(r, std::forward<Pred>(pred)));
}

template <typename T>
concept associative_range = std::ranges::input_range<T> && requires { typename std::remove_cvref_t<T>::mapped_type; };

template <std::ranges::input_range First, std::ranges::input_range... Rest>
    requires(!associative_range<First>) &&
            (std::convertible_to<std::ranges::range_value_t<Rest>, std::ranges::range_value_t<First>> && ...)
auto concat(First&& first, Rest&&... rest) {
    using T = std::ranges::range_value_t<First>;
    std::vector<T> result;
    std::ranges::copy(first, std::inserter(result, std::ranges::end(result)));
    (std::ranges::copy(rest, std::inserter(result, std::ranges::end(result))), ...);
    return result;
}

template <associative_range First, associative_range... Rest>
    requires(std::same_as<typename std::remove_cvref_t<First>::key_type,
                          typename std::remove_cvref_t<Rest>::key_type> &&
             ...) &&
            (std::same_as<typename std::remove_cvref_t<First>::mapped_type,
                          typename std::remove_cvref_t<Rest>::mapped_type> &&
             ...)
auto concat(First&& first, Rest&&... rest) {
    using K = typename std::remove_cvref_t<First>::key_type;
    using V = typename std::remove_cvref_t<First>::mapped_type;
    std::unordered_map<K, V> result(std::ranges::begin(first), std::ranges::end(first));
    (result.insert(std::ranges::begin(rest), std::ranges::end(rest)), ...);
    return result;
}

template <std::ranges::input_range R, typename F>
    requires std::ranges::input_range<std::decay_t<std::invoke_result_t<F, std::ranges::range_reference_t<R>>>>
auto flat_map(R&& r, F&& f) {
    using Inner = std::decay_t<std::invoke_result_t<F, std::ranges::range_reference_t<R>>>;
    using T = std::ranges::range_value_t<Inner>;
    std::vector<T> result;
    for (auto&& x : r) {
        auto sub = f(x);
        result.insert(result.end(), std::ranges::begin(sub), std::ranges::end(sub));
    }
    return result;
}

inline auto chunk_by_blank(const std::vector<std::string>& lines) -> std::vector<std::vector<std::string>> {
    std::vector<std::vector<std::string>> blocks;
    std::vector<std::string> current;
    for (const auto& line : lines) {
        if (line.empty()) {
            if (!current.empty()) {
                blocks.push_back(std::move(current));
                current = {};
            }
        } else {
            current.push_back(line);
        }
    }
    if (!current.empty()) blocks.push_back(std::move(current));
    return blocks;
}

template <typename Delim>
    requires requires(std::string s, Delim d) { s += d; }
struct JoinWithClosure {
    Delim _delim;

    template <std::ranges::input_range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, std::string_view>
    std::string operator()(R&& r) const {
        std::string result;
        bool first = true;
        for (const auto& elem : r) {
            if (!first) result += _delim;
            result += std::string_view(elem);
            first = false;
        }
        return result;
    }
};

template <std::ranges::input_range R, typename Delim>
    requires std::convertible_to<std::ranges::range_value_t<R>, std::string_view> &&
             requires(std::string s, Delim d) { s += d; }
std::string operator|(R&& r, const JoinWithClosure<Delim>& c) {
    return c(std::forward<R>(r));
}

template <typename Delim>
    requires requires(std::string s, Delim d) { s += d; }
auto join_with(Delim&& delim) {
    return JoinWithClosure<std::decay_t<Delim>> {std::forward<Delim>(delim)};
}

template <std::ranges::input_range R, typename Delim>
    requires std::convertible_to<std::ranges::range_value_t<R>, std::string_view> &&
             requires(std::string s, Delim d) { s += d; }
std::string join_with(R&& r, Delim&& delim) {
    return join_with(std::forward<Delim>(delim))(std::forward<R>(r));
}

}
