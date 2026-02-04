#pragma once

// Bitmask operators for enum classes
#define ARIEO_ENABLE_BITMASK_OPERATORS(x)  \
constexpr x operator|(x lhs, x rhs) { \
    using T = std::underlying_type_t<x>; \
    return static_cast<x>(static_cast<T>(lhs) | static_cast<T>(rhs)); \
} \
constexpr x operator&(x lhs, x rhs) { \
    using T = std::underlying_type_t<x>; \
    return static_cast<x>(static_cast<T>(lhs) & static_cast<T>(rhs)); \
} \
constexpr x operator^(x lhs, x rhs) { \
    using T = std::underlying_type_t<x>; \
    return static_cast<x>(static_cast<T>(lhs) ^ static_cast<T>(rhs)); \
} \
constexpr x operator~(x rhs) { \
    using T = std::underlying_type_t<x>; \
    return static_cast<x>(~static_cast<T>(rhs)); \
} \
constexpr x& operator|=(x& lhs, x rhs) { \
    lhs = lhs | rhs; \
    return lhs; \
} \
constexpr x& operator&=(x& lhs, x rhs) { \
    lhs = lhs & rhs; \
    return lhs; \
} \
constexpr x& operator^=(x& lhs, x rhs) { \
    lhs = lhs ^ rhs; \
    return lhs; \
}


#define ARIEO_ENUM_MAPPER(SRC_TYPE, DEST_TYPE)\
    template<>\
    Base::EnumMapper<SRC_TYPE, DEST_TYPE>::TMapType\
    Base::EnumMapper<SRC_TYPE, DEST_TYPE>::m_convert_map =

// Macro to easily enable bitwise operators for a specific enum class
#define ARIEO_ENABLE_BITMASK_OPERATORS_2(Enum) \
template<> \
struct ::Arieo::Base::EnableBitmaskOperators<Enum>\
{ \
    static const bool enable = true; \
};

namespace Arieo::Base
{
    struct BitMaskEnum
    {
    };
    
    // Template to enable bitwise operations for enum class
    template<typename Enum>
    struct EnableBitmaskOperators {
        static const bool enable = false;
    };

    // Bitwise OR operator
    template<typename Enum>
    constexpr typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum>::type
    operator|(Enum lhs, Enum rhs) {
        using underlying = typename std::underlying_type<Enum>::type;
        return static_cast<Enum>(
            static_cast<underlying>(lhs) | 
            static_cast<underlying>(rhs));
    }

    // Bitwise AND operator
    template<typename Enum>
    constexpr typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum>::type
    operator&(Enum lhs, Enum rhs) {
        using underlying = typename std::underlying_type<Enum>::type;
        return static_cast<Enum>(
            static_cast<underlying>(lhs) & 
            static_cast<underlying>(rhs));
    }

    // Bitwise XOR operator
    template<typename Enum>
    constexpr typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum>::type
    operator^(Enum lhs, Enum rhs) {
        using underlying = typename std::underlying_type<Enum>::type;
        return static_cast<Enum>(
            static_cast<underlying>(lhs) ^ 
            static_cast<underlying>(rhs));
    }

    // Bitwise NOT operator
    template<typename Enum>
    constexpr typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum>::type
    operator~(Enum rhs) {
        using underlying = typename std::underlying_type<Enum>::type;
        return static_cast<Enum>(~static_cast<underlying>(rhs));
    }

    // Compound OR assignment
    template<typename Enum>
    constexpr typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum&>::type
    operator|=(Enum& lhs, Enum rhs) {
        using underlying = typename std::underlying_type<Enum>::type;
        lhs = static_cast<Enum>(
            static_cast<underlying>(lhs) | 
            static_cast<underlying>(rhs));
        return lhs;
    }

    // Compound AND assignment
    template<typename Enum>
    constexpr typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum&>::type
    operator&=(Enum& lhs, Enum rhs) {
        using underlying = typename std::underlying_type<Enum>::type;
        lhs = static_cast<Enum>(
            static_cast<underlying>(lhs) & 
            static_cast<underlying>(rhs));
        return lhs;
    }

    // Compound XOR assignment
    template<typename Enum>
    constexpr typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum&>::type
    operator^=(Enum& lhs, Enum rhs) {
        using underlying = typename std::underlying_type<Enum>::type;
        lhs = static_cast<Enum>(
            static_cast<underlying>(lhs) ^ 
            static_cast<underlying>(rhs));
        return lhs;
    }

    template<typename TSource, typename TDest>
    class EnumMapper
    {
    private:
        
    public:
        struct ConvertMapItem
        {
            TSource from;
            TDest to;
        };
        using TMapType = std::vector<ConvertMapItem>;
        static TMapType m_convert_map;
        
        static TDest convert(TSource source)
        {
ARIEO_CLANG_PRAGMA("GCC diagnostic push");
ARIEO_CLANG_PRAGMA("GCC diagnostic ignored \"-Wundefined-var-template\"");
            TDest dest = TDest(0);
            if constexpr (requires(EnumMapper) { EnumMapper::m_convert_map[0]; }) 
            {
                if constexpr (requires(TSource a1, TSource a2, TDest b1, TDest b2) { a1 | a2; b1 | b2;  }) 
                {
                    for (int i = 0; i < m_convert_map.size(); i++) 
                    {
                        if(std::underlying_type_t<TSource>((m_convert_map[i].from & source)) != 0)
                        {
                            dest = TDest(dest | m_convert_map[i].to);
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < m_convert_map.size(); i++) 
                    {
                        if(std::underlying_type_t<TSource>(m_convert_map[i].from) == std::underlying_type_t<TSource>(source))
                        {
                            return m_convert_map[i].to;
                        }
                    }
                }
            }
            else
            {
                static_assert(false, "Enum map not defined.");
            }
            return dest;
ARIEO_CLANG_PRAGMA("GCC diagnostic pop");
        }
    };

    template<typename TDest, typename TSource>
    TDest mapEnum(TSource source)
    {
        return EnumMapper<TSource, TDest>::convert(source);
    }
}