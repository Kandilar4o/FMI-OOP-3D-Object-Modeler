#include "hm2_6MI0800779_SDFserializer.hpp"
#include <string>

void SDFSerializer::serialize(const ShapeSDF& sdf, std::ostream& out) {
    out << sdf.get_type() << "\n";
    out << sdf.position.x << " " << sdf.position.y << " " << sdf.position.z << "\n";
    sdf.serialize_custom(out);
}

ShapeSDF* SDFSerializer::deserialize(std::istream& in) {
    ShapeSDF* shape = nullptr;
    std::string shapeName;
    mc::point pos;
    if (!(in >> shapeName) || !(in >> pos.x >> pos.y >> pos.z)) {
        return nullptr;
    }
    if (shapeName == "SphereSDF") {
        shape = new SphereSDF(1.0);
        shape->deserialize_custom(in);
    }
    else if (shapeName == "BoxSDF") {
        shape = new BoxSDF({ 1.0, 1.0, 1.0 });
        shape->deserialize_custom(in);
    }
    else if (shapeName == "TorusSDF") {
        shape = new TorusSDF(1.0, 0.5);
        shape->deserialize_custom(in);
    }
    else if (shapeName == "UnionSDF") {
        ShapeSDF* shape1 = deserialize(in);
        ShapeSDF* shape2 = deserialize(in);
        if (shape1 && shape2) {
            shape = new UnionSDF(*shape1, *shape2);
        }
        delete shape1;
        delete shape2;
    }
    else if (shapeName == "IntersectionSDF") {
        ShapeSDF* shape1 = deserialize(in);
        ShapeSDF* shape2 = deserialize(in);
        if (shape1 && shape2) {
            shape = new IntersectionSDF(*shape1, *shape2);
        }
        delete shape1;
        delete shape2;
    }
    else if (shapeName == "DifferenceSDF") {
        ShapeSDF* shape1 = deserialize(in);
        ShapeSDF* shape2 = deserialize(in);
        if (shape1 && shape2) {
            shape = new DifferenceSDF(*shape1, *shape2);
        }
        delete shape1;
        delete shape2;
    }
    else if (shapeName == "SoftUnionSDF") {
        double smooth;
        if (in >> smooth) {
            ShapeSDF* shape1 = deserialize(in);
            ShapeSDF* shape2 = deserialize(in);

            if (shape1 && shape2) {
                shape = new SoftUnionSDF(*shape1, *shape2, smooth);
            }
            delete shape1;
            delete shape2;
        }
    }
    else if (shapeName == "SoftUnionNSDF") {
        double smooth;
        size_t count;
        if (in >> smooth >> count) {
            SoftUnionNSDF* unionNSDF = new SoftUnionNSDF(smooth);
            bool success = true;
            for (size_t i = 0; i < count; ++i) {
                ShapeSDF* shapePart = deserialize(in);
                if (shapePart) {
                    unionNSDF->add(*shapePart);
                    delete shapePart;
                }
                else {
                    success = false;
                    break;
                }
            }

            if (success) {
                shape = unionNSDF;
            }
            else {
                delete unionNSDF;
            }
        }
    }
    else if (shapeName == "RotateYSDF") {
        double angle_radiant;
        if (in >> angle_radiant) {
            ShapeSDF* shapePart = deserialize(in);
            if (shapePart) {
                shape = new RotateYSDF(*shapePart, angle_radiant);
                delete shapePart;
            }
        }
    }
    else if (shapeName == "TwistYSDF") {
        double factor;
        if (in >> factor) {
            ShapeSDF* shapePart = deserialize(in);
            if (shapePart) {
                shape = new TwistYSDF(*shapePart, factor);
                delete shapePart;
            }
        }
    }
    if (shape) {
        shape->position = pos;
    }
    return shape;
}