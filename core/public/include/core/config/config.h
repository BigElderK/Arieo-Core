#pragma once
#include "base/prerequisites.h"

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>
#include <yaml-cpp/depthguard.h>
#include <yaml-cpp/parser.h>
#include <yaml-cpp/exceptions.h>

namespace Arieo::Core
{
    namespace ConfigFile = YAML;
    using ConfigNode = YAML::Node;
}