#pragma once

#ifdef _REFLECT_
#define PROPERTY_META(...) __attribute__((annotate("kos_meta_data:" #__VA_ARGS__)))
#define FUNCTION_META(...) __attribute__((annotate("kos_meta_data:" #__VA_ARGS__)))
#else
#define PROPERTY_META(...)
#define FUNCTION_META(...)
#endif

namespace Arieo::Base
{
/*
    template<class T>
    class ClassInfo
    {

    };

#ifdef _REFLECT_
    #define CLASS_META(TCLASS, ...)\
    template<>\
    class ClassInfo<class TCLASS>\
    {\
        static const char* m_meta = #__VA_ARGS__\
    };
#else
    #define CLASS_META(TCLASS, ...)\
    template<>\
    class ClassInfo<class TCLASS>\
    {\
        static uint64_t m_type_id;\
    };
#endif
*/
    class TypeInfoBase
    {
    };

    template<class T>
    class TypeInfo;
    
    template<class T>
    class TypeInfo
    {
    public:
        static uint64_t getTypeID()
        {
            return T::MetaInfo::m_type_id;
        }
    };

#ifdef _REFLECT_
    #define CLASS_META(...) \
    class ClassMETAXXX\
    {\
        static constexpr const char* m_meta = "kos_meta_data:"#__VA_ARGS__;\
    };
#else
    #define CLASS_META(...) \
    template<typename>\
    friend class TypeInfo;\
    class MetaInfo\
    {\
        template<typename>\
        friend class TypeInfo;\
        static const uint64_t m_type_id;\
    };
#endif
}