#pragma once
#include <stdint.h>
#include "base/types/equivalence_type.h"
#include "base/compile/ct_string_id_gen.h"
#include "base/types/equivalence_type.h"
namespace Arieo::Base
{
    /*
    class StringID
        : public EquivalType<StringID, uint32_t>
    {
    public:
        StringID(uint32_t value)
            : EquivalType<StringID, uint32_t>(value)
        {
            m_value = value;
        }

        static const StringID& null()
        {
            static StringID string_id_null(0);
            return string_id_null;
        }
    };*/

    ARIEO_DEFINE_EQUIVAL_TYPE(StringID, std::uint32_t);
}