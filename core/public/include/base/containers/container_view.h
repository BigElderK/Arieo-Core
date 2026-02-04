#pragma once
#include <ranges>
#include <iostream>
namespace Arieo::Base::View
{
    template<typename TStdView>
    class ContainerViewOp;

    template<class TContainer>
    auto from(TContainer&& c)
    {
        return ContainerViewOp(std::forward<TContainer>(c) | std::views::all);
    }

    template<class TStdView>
    auto generateOpChain(TStdView&& v)
    {
        return ContainerViewOp(std::forward<TStdView>(v));
    }
    
    template<typename TStdView>
    class ContainerViewOp 
    {
    private:
        TStdView m_std_view;  // Reference to avoid copies
    public:
        ContainerViewOp(TStdView&& v) : m_std_view(std::move(v))
        {
        }

        ContainerViewOp(ContainerViewOp&) = delete;
        ContainerViewOp& operator=(const ContainerViewOp& other) = delete;

        ContainerViewOp(ContainerViewOp&& other)
            : m_std_view(std::move(other.m_std_view))
        {
        }

        auto range(size_t start, size_t count)
        {
            return Base::View::generateOpChain(m_std_view | std::views::drop(start) | std::views::take(count));
        }

        auto where(auto predicate_func) 
        {
            return Base::View::generateOpChain(m_std_view | std::views::filter(predicate_func));
        }

        auto select(auto transform_func)
        {
            return Base::View::generateOpChain(m_std_view | std::views::transform(transform_func));
        }

        auto foreach(auto func) 
        {
            for (auto&& item : m_std_view) 
            {
                func(item);
            }
            return Base::View::generateOpChain(m_std_view | std::views::all);
        }

        auto begin() { return std::ranges::begin(m_std_view); }
        auto end() { return std::ranges::end(m_std_view); }
    };
};