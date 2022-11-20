#include "data.h"
#include "person.h"

std::ostream &operator<<(std::ostream &osm, const Person &person)
{
    osm << person._person_impl->DebugString() << "\n";
    return osm;
}
