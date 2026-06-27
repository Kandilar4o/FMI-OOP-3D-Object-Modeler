#include "hm2_6MI0800779_shapes.hpp"
#include "hm2_6MI0800779_SDFserializer.hpp"
#include <limits>

double ShapeSDF::operator()(mc::point p) const {
    mc::point local_p = { p.x - position.x, p.y - position.y, p.z - position.z };
    return evaluate(local_p);
}

SphereSDF::SphereSDF(double radius_) : radius(radius_) {}

double SphereSDF::evaluate(mc::point p) const {
    return mc::sdf_sphere(p, radius);
}

std::string SphereSDF::get_type() const {
    return "SphereSDF";
}

void SphereSDF::serialize_custom(std::ostream& out) const {
    out << radius << "\n";
}

void SphereSDF::deserialize_custom(std::istream& in) {
    in >> radius;
}

SphereSDF* SphereSDF::clone() const {
    return new SphereSDF(*this);
}

BoxSDF::BoxSDF(mc::point size_) : size(size_) {}

double BoxSDF::evaluate(mc::point p) const {
    return mc::sdf_box(p, size);
}

std::string BoxSDF::get_type() const {
    return "BoxSDF";
}

void BoxSDF::serialize_custom(std::ostream& out) const {
    out << size.x << " " << size.y << " " << size.z << "\n";
}

void BoxSDF::deserialize_custom(std::istream& in) {
    in >> size.x >> size.y >> size.z;
}

BoxSDF* BoxSDF::clone() const {
    return new BoxSDF(*this);
}

TorusSDF::TorusSDF(double radius_, double width_) : radius(radius_), width(width_) {}

double TorusSDF::evaluate(mc::point p) const {
    return mc::sdf_torus(p, radius, width);
}

std::string TorusSDF::get_type() const {
    return "TorusSDF";
}

void TorusSDF::serialize_custom(std::ostream& out) const {
    out << radius << " " << width << "\n";
}

void TorusSDF::deserialize_custom(std::istream& in) {
    in >> radius >> width;
}

TorusSDF* TorusSDF::clone() const {
    return new TorusSDF(*this); 
}

UnionSDF::UnionSDF(const ShapeSDF& shape1_, const ShapeSDF& shape2_) : shape1(shape1_.clone()), shape2(shape2_.clone()) {}

UnionSDF::~UnionSDF() {
    delete shape1; 
    delete shape2; 
}

double UnionSDF::evaluate(mc::point p) const {
    return mc::sdf_union((*shape1)(p), (*shape2)(p));
}

std::string UnionSDF::get_type() const {
    return "UnionSDF";
}

void UnionSDF::serialize_custom(std::ostream& out) const {
    SDFSerializer::serialize(*shape1, out);
    SDFSerializer::serialize(*shape2, out);
}

void UnionSDF::deserialize_custom(std::istream& in) {}

UnionSDF* UnionSDF::clone() const {
    UnionSDF* copy = new UnionSDF(*shape1, *shape2);
    copy->position = this->position;
    return copy;
}

IntersectionSDF::IntersectionSDF(const ShapeSDF& shape1_, const ShapeSDF& shape2_) : shape1(shape1_.clone()), shape2(shape2_.clone()) {}

IntersectionSDF::~IntersectionSDF() {
    delete shape1;
    delete shape2; 
}

double IntersectionSDF::evaluate(mc::point p) const {
    return mc::sdf_intersection((*shape1)(p), (*shape2)(p));
}

std::string IntersectionSDF::get_type() const {
    return "IntersectionSDF";
}

void IntersectionSDF::serialize_custom(std::ostream& out) const {
    SDFSerializer::serialize(*shape1, out);
    SDFSerializer::serialize(*shape2, out);
}

void IntersectionSDF::deserialize_custom(std::istream& in) {}

IntersectionSDF* IntersectionSDF::clone() const {
    IntersectionSDF* copy = new IntersectionSDF(*shape1, *shape2);
    copy->position = this->position;
    return copy;
}

DifferenceSDF::DifferenceSDF(const ShapeSDF& shape1_, const ShapeSDF& shape2_) : shape1(shape1_.clone()), shape2(shape2_.clone()) {}

DifferenceSDF::~DifferenceSDF() {
    delete shape1;
    delete shape2;
}

double DifferenceSDF::evaluate(mc::point p) const {
    return mc::sdf_difference((*shape1)(p), (*shape2)(p));
}

std::string DifferenceSDF::get_type() const {
    return "DifferenceSDF";
}

void DifferenceSDF::serialize_custom(std::ostream& out) const {
    SDFSerializer::serialize(*shape1, out);
    SDFSerializer::serialize(*shape2, out);
}

void DifferenceSDF::deserialize_custom(std::istream& in) {}

DifferenceSDF* DifferenceSDF::clone() const {
    DifferenceSDF* copy = new DifferenceSDF(*shape1, *shape2);
    copy->position = this->position;
    return copy;
}

SoftUnionSDF::SoftUnionSDF(const ShapeSDF& shape1_, const ShapeSDF& shape2_, double smoothness_) : shape1(shape1_.clone()), shape2(shape2_.clone()), smoothness(smoothness_) {}

SoftUnionSDF::~SoftUnionSDF() {
    delete shape1;
    delete shape2;
}

double SoftUnionSDF::evaluate(mc::point p) const {
    return mc::sdf_union_soft((*shape1)(p), (*shape2)(p), smoothness);
}

std::string SoftUnionSDF::get_type() const {
    return "SoftUnionSDF";
}

void SoftUnionSDF::serialize_custom(std::ostream& out) const {
    out << smoothness << "\n";
    SDFSerializer::serialize(*shape1, out);
    SDFSerializer::serialize(*shape2, out);
}

void SoftUnionSDF::deserialize_custom(std::istream& in) {}

SoftUnionSDF* SoftUnionSDF::clone() const {
    SoftUnionSDF* copy = new SoftUnionSDF(*shape1, *shape2, smoothness);
    copy->position = this->position;
    return copy;
}

SoftUnionNSDF::SoftUnionNSDF(double smoothness_) : smoothness(smoothness_) {}

SoftUnionNSDF::~SoftUnionNSDF() {
    for (ShapeSDF* shape : shapes) {
        delete shape;
    }
}

void SoftUnionNSDF::add(const ShapeSDF& shape) {
    shapes.push_back(shape.clone());
}

double SoftUnionNSDF::evaluate(mc::point p) const {
    if (shapes.empty())
    {
        return std::numeric_limits<double>::max();
    }
    size_t numOfShapes = shapes.size();
    std::vector<double> vals(numOfShapes);
    for (size_t i = 0; i < numOfShapes; i++)
    {
        vals[i] = (*shapes[i])(p);
    }

    return mc::sdf_union_soft_n(vals.data(), numOfShapes, smoothness);
}

std::string SoftUnionNSDF::get_type() const {
    return "SoftUnionNSDF";
}

void SoftUnionNSDF::serialize_custom(std::ostream& out) const {
    out << smoothness << " " << shapes.size() << "\n";
    for (const ShapeSDF* shape : shapes)
    {
        SDFSerializer::serialize(*shape, out);
    }
}

void SoftUnionNSDF::deserialize_custom(std::istream& in) {}

SoftUnionNSDF* SoftUnionNSDF::clone() const {
    SoftUnionNSDF* copy = new SoftUnionNSDF(smoothness);
    for (const ShapeSDF* shape : shapes) {
        copy->add(*shape);
    }
    copy->position = this->position;
    return copy;
}

RotateYSDF::RotateYSDF(const ShapeSDF& shape_, double angle_rad_) : shape(shape_.clone()), angle_rad(angle_rad_) {}

RotateYSDF::~RotateYSDF() {
    delete shape;
}

double RotateYSDF::evaluate(mc::point p) const {
    mc::point rotated_p = mc::rotate_Y(p, angle_rad);
    return (*shape)(rotated_p);
}

std::string RotateYSDF::get_type() const {
    return "RotateYSDF";
}

void RotateYSDF::serialize_custom(std::ostream& out) const {
    out << angle_rad << "\n";
    SDFSerializer::serialize(*shape, out);
}

void RotateYSDF::deserialize_custom(std::istream& in) {}

RotateYSDF* RotateYSDF::clone() const {
    RotateYSDF* copy = new RotateYSDF(*shape, angle_rad);
    copy->position = this->position;
    return copy;
}

TwistYSDF::TwistYSDF(const ShapeSDF& shape_, double twist_) : shape(shape_.clone()), twist(twist_) {}

TwistYSDF::~TwistYSDF() {
    delete shape;
}

double TwistYSDF::evaluate(mc::point p) const {
    mc::point twisted_p = mc::twist_Y(p, twist);
    return (*shape)(twisted_p);
}

std::string TwistYSDF::get_type() const {
    return "TwistYSDF";
}

void TwistYSDF::serialize_custom(std::ostream& out) const {
    out << twist << "\n";
    SDFSerializer::serialize(*shape, out);
}

void TwistYSDF::deserialize_custom(std::istream& in) {}

TwistYSDF* TwistYSDF::clone() const {
    TwistYSDF* copy = new TwistYSDF(*shape, twist);
    copy->position = this->position;
    return copy;
}