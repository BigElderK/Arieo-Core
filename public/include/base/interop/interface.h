#pragma once
#include "base/delegates/function_delegate.h"
#include "core/logger/logger.h"
#include <cstddef>
#include <type_traits>
#include "interop.h"

namespace Arieo::Base
{
    // Forward declaration for code-generated InterfaceInfo specializations
    template<typename T>
    class InterfaceInfo;
}

#define METADATA(x) [[clang::annotate(#x)]]





