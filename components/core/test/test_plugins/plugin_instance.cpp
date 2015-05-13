//          Copyright Jeremy Coulon 2012-2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//[MyPlugin_cpp_file
#include "plugin_instance.hpp"

// The only important thing is to call the macro PLUGIN_FACTORY_DEFINITION(T).
// Define plugin factory. Must be in the global namespace.
PLUGIN_FACTORY_DEFINITION( Example::MyPlugin )

Example::MyPlugin::MyPlugin()
    : name_("Example")
    , version_(1, 3, 4)
{
    // Empty
}

Example::MyPlugin::~MyPlugin()
{
    // Empty
}

const std::string& Example::MyPlugin::getName() const
{
    return name_;
}

const ssf::VersionInfo& Example::MyPlugin::getVersion() const
{
    return version_;
}
//]
