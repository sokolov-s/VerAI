#include "uuid.h"
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

std::string common::GenerateUUID()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::ostringstream str;
    str << uuid;
    return str.str();
}
