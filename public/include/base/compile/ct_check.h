#pragma once
#include <stdint.h>
#include <string>
namespace Arieo::Base
{
    namespace ct
    {
        // Value types crossing DLL boundaries must be standard layout, trivially copyable, and have no padding
        template<typename T>
        inline constexpr bool DLLBoundarySafeCheck =
            std::is_standard_layout_v<T> &&
            std::is_trivially_copyable_v<T> &&
            std::has_unique_object_representations_v<T>;

        // A parameter/return type is safe across DLL if it's:
        //   - void (return only)
        //   - a pointer or reference (caller/callee share address space)
        //   - a value type that passes DLLBoundarySafeCheck
        template<typename T>
        inline constexpr bool DLLBoundarySafeParam =
            std::is_void_v<T> ||
            std::is_pointer_v<T> ||
            std::is_reference_v<T> ||
            DLLBoundarySafeCheck<std::remove_cv_t<T>>;

        // Structural layout check for resource-managing types (e.g., ref-counted smart pointers)
        // that are safe across DLL boundaries due to having a fixed, standard layout,
        // but are NOT trivially copyable (non-trivial dtor / copy / move).
        template<typename T>
        inline constexpr bool DLLBoundaryLayoutSafe =
            std::is_standard_layout_v<T>;

        // Decompose a member function pointer and check all parameter + return types
        template<typename T>
        struct DLLSafeMemberFunctionCheck : std::false_type {};

        template<typename R, typename C, typename... Args>
        struct DLLSafeMemberFunctionCheck<R(C::*)(Args...)>
            : std::bool_constant<DLLBoundarySafeParam<R> && (DLLBoundarySafeParam<Args> && ...)> {};

        // const-qualified overload
        template<typename R, typename C, typename... Args>
        struct DLLSafeMemberFunctionCheck<R(C::*)(Args...) const>
            : std::bool_constant<DLLBoundarySafeParam<R> && (DLLBoundarySafeParam<Args> && ...)> {};
    }
}