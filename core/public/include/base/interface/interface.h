#pragma once
#include "base/delegates/function_delegate.h"

#define METADATA(x) [[clang::annotate(#x)]]

namespace Arieo::Base
{
    template <typename TInstance, typename TInterface>
    inline TInstance* castInterfaceToInstance(TInterface* interface)
    {
        return static_cast<TInstance*>(interface);
    }

    class Interface
    {
    };

    template<typename T>
    class InterfaceInfo
    {
    };
}