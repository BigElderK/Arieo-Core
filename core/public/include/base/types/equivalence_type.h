#pragma once
#include <limits>
#include "base/compile/ct_string_id_gen.h"
namespace Arieo::Base
{
    template<typename TType, typename EType>
    class EquivalType
    { 
    public:
        EType m_value;

        constexpr EquivalType() noexcept
        {
            m_value = std::numeric_limits<EType>::max();
        }

        constexpr EquivalType(EType& value) noexcept
        {
            m_value = value;
        }

        constexpr EquivalType(const EType& value) noexcept
        {
            m_value = value;
        }

        constexpr EquivalType(const EquivalType& other) noexcept
        {
            m_value = other.m_value;
        }

        EquivalType& operator=(const EquivalType& other) noexcept
        {
            m_value = other.m_value;
            return *this;
        }

        inline EType& get(){return m_value;}
        inline const EType& get() const{return m_value;}

        inline bool operator==(const EquivalType& rhs) const noexcept { return this->m_value == rhs.m_value; }
        inline bool operator!=(const EquivalType& rhs) const noexcept { return !(this->m_value == rhs); }
        inline bool operator< (const EquivalType& rhs) const noexcept { return this->m_value < rhs.m_value; }
        inline bool operator> (const EquivalType& rhs) const noexcept { return rhs.m_value < this->m_value; }
        inline bool operator<=(const EquivalType& rhs) const noexcept { return !(this->m_value > rhs.m_value); }
        inline bool operator>=(const EquivalType& rhs) const noexcept { return !(this->m_value < rhs.m_value); }
        inline std::ostream& operator<<(std::ostream& os) noexcept {os << this->m_value; return os;}

//#ifdef _DEBUG_ENABLE_EQUIVAL_TYPE_CHECK
//        using type = EquivalTypeImpl;
//#else
//        using type = EquivalTypeImpl;
//#endif
    };

    #define ARIEO_DEFINE_EQUIVAL_TYPE(TType, EType) struct TType##_1{}; using TType = EquivalType<TType##_1, EType>;
} // namespace Arieo

template<typename TType, typename EType>
struct std::hash<typename Arieo::Base::EquivalType<TType, EType>> {
size_t operator()(const typename Arieo::Base::EquivalType<TType, EType>& p) const noexcept {
    return std::hash<EType>{}(p.m_value);
}
};