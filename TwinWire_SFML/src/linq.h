#pragma once
#include <vector>
#include <type_traits>
#include <algorithm>
#include <utility>

// Where
template <typename Range, typename Pred>
auto where(const Range& rng, Pred pred)
{
    using T = typename Range::value_type;
    std::vector<T> out;
    out.reserve(rng.size());
    for (const auto& x : rng)
        if (pred(x)) out.push_back(x);
    return out;
}

// select
template <typename Range, typename Func>
auto select(const Range& rng, Func f)
{
    using In = typename Range::value_type;
    using Out = std::decay_t<std::invoke_result_t<Func, In>>;
    std::vector<Out> out;
    out.reserve(rng.size());
    for (const auto& x : rng)
        out.push_back(f(x));
    return out;
}

// order_by
template <typename Range, typename KeySel>
auto order_by(const Range& rng, KeySel key)
{
    using T = typename Range::value_type;
    std::vector<T> out(rng.begin(), rng.end());
    std::sort(out.begin(), out.end(),
        [&](const T& a, const T& b) { return key(a) < key(b); });
    return out;
}

// first_or_default
template <typename Range, typename T = typename Range::value_type>
T first_or_default(const Range& rng, T def = T{})
{
    if (!rng.empty()) return rng.front();
    return def;
}

// aggregate
template <typename Range, typename Acc, typename Agg>
Acc aggregate(const Range& rng, Acc seed, Agg agg )
{
    Acc acc = std::move(seed);
    for (const auto& x : rng) acc = agg(acc, x);
    return acc;
}
