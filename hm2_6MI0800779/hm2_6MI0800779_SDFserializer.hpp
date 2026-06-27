#ifndef SDF_SERIALIZER_HPP
#define SDF_SERIALIZER_HPP

#include "hm2_6MI0800779_shapes.hpp"
#include <iostream>

class SDFSerializer {
public:
    static void serialize(const ShapeSDF& sdf, std::ostream& out);
    static ShapeSDF* deserialize(std::istream& in);
};

#endif