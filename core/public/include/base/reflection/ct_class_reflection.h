#pragma once

#include <utility>
namespace Arieo::Base
{
    template <typename T, typename F>
    constexpr void for_each_member(T&& value, F&& func) {
        if constexpr (std::is_aggregate_v<std::decay_t<T>>) 
        {
            constexpr auto size = []() 
            {
                if constexpr (requires { std::tuple_size<std::decay_t<T>>{}; }) 
                {
                    return std::tuple_size_v<std::decay_t<T>>;
                } 
                else 
                {
                    return sizeof(T) / sizeof(std::byte); // Rough estimate
                }
            }();
            
            [&]<std::size_t... I>(std::index_sequence<I...>) {
                (func(std::get<I>(std::forward<T>(value))), ...);
            }(std::make_index_sequence<size>{});
        } 
        else 
        {
            func(std::forward<T>(value));
        }
    }
}

