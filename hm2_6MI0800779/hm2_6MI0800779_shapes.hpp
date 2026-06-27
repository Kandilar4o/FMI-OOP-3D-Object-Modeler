#ifndef SHAPES_HPP
#define SHAPES_HPP

#include "marching_cubes.hpp"
#include <iostream>
#include <string>
#include <vector>

constexpr double defaultRadius = 1.0;
constexpr double defaultWidth = 0.5;

class ShapeSDF : public mc::SDF {
public:
    mc::point position{ 0.0, 0.0, 0.0 };

    virtual ~ShapeSDF() = default;

    virtual std::string get_type() const = 0;

    virtual void serialize_custom(std::ostream& out) const = 0;

    virtual void deserialize_custom(std::istream& in) = 0;

    virtual double evaluate(mc::point local_p) const = 0;

    virtual ShapeSDF* clone() const = 0;

    double operator()(mc::point p) const override;
};

class SphereSDF : public ShapeSDF {
    double radius;
public:
    SphereSDF(double radius_ = defaultRadius);

    double evaluate(mc::point p) const final;

    std::string get_type() const final;

    void serialize_custom(std::ostream& out) const final;

    void deserialize_custom(std::istream& in) final;

    SphereSDF* clone() const final;
};

class BoxSDF : public ShapeSDF {
    mc::point size;
public:
    BoxSDF(mc::point size_);

    double evaluate(mc::point p) const final;

    std::string get_type() const final;

    void serialize_custom(std::ostream& out) const final;

    void deserialize_custom(std::istream& in) final;

    BoxSDF* clone() const final;
};

class TorusSDF : public ShapeSDF {
    double radius;
    double width;
public:
    TorusSDF(double radius_ = defaultRadius, double width_ = defaultWidth);

    double evaluate(mc::point p) const final;

    std::string get_type() const final;

    void serialize_custom(std::ostream& out) const final;

    void deserialize_custom(std::istream& in) final;

    TorusSDF* clone() const final;
};

class UnionSDF : public ShapeSDF {
    ShapeSDF* shape1;
    ShapeSDF* shape2;
public:
    UnionSDF(const ShapeSDF& shape1_, const ShapeSDF& shape2_);

    ~UnionSDF() override;

    double evaluate(mc::point p) const final;

    std::string get_type() const final;

    void serialize_custom(std::ostream& out) const final;

    void deserialize_custom(std::istream& in) final;

    UnionSDF* clone() const final;
};

class IntersectionSDF : public ShapeSDF {
    ShapeSDF* shape1;
    ShapeSDF* shape2;
public:
    IntersectionSDF(const ShapeSDF& shape1_, const ShapeSDF& shape2_);

    ~IntersectionSDF() override;

    double evaluate(mc::point p) const final;

    std::string get_type() const final;

    void serialize_custom(std::ostream& out) const final;

    void deserialize_custom(std::istream& in) final;

    IntersectionSDF* clone() const final;
};

class DifferenceSDF : public ShapeSDF {
    ShapeSDF* shape1;
    ShapeSDF* shape2;
public:
    DifferenceSDF(const ShapeSDF& shape1_, const ShapeSDF& shape2_);

    ~DifferenceSDF() override;

    double evaluate(mc::point p) const final;

    std::string get_type() const final;

    void serialize_custom(std::ostream& out) const final;

    void deserialize_custom(std::istream& in) final;

    DifferenceSDF* clone() const final;
};

class SoftUnionSDF : public ShapeSDF {
    ShapeSDF* shape1;
    ShapeSDF* shape2;
    double smoothness;
public:
    SoftUnionSDF(const ShapeSDF& shape1_, const ShapeSDF& shape2_, double smoothness_);

    ~SoftUnionSDF() override;

    double evaluate(mc::point p) const final;

    std::string get_type() const final;

    void serialize_custom(std::ostream& out) const final;

    void deserialize_custom(std::istream& in) final;

    SoftUnionSDF* clone() const final;
};

class SoftUnionNSDF : public ShapeSDF {
    std::vector<ShapeSDF*> shapes;
    double smoothness;
public:
    SoftUnionNSDF(double smoothness_);

    ~SoftUnionNSDF() override;

    void add(const ShapeSDF& shape);

    double evaluate(mc::point p) const final;

    std::string get_type() const final;

    void serialize_custom(std::ostream& out) const final;

    void deserialize_custom(std::istream& in) final;

    SoftUnionNSDF* clone() const final;
};

class RotateYSDF : public ShapeSDF {
    ShapeSDF* shape;
    double angle_rad;
public:
    RotateYSDF(const ShapeSDF& shape_, double angle_rad_);

    ~RotateYSDF() override;

    double evaluate(mc::point p) const final;

    std::string get_type() const final;

    void serialize_custom(std::ostream& out) const final;

    void deserialize_custom(std::istream& in) final;

    RotateYSDF* clone() const final;
};

class TwistYSDF : public ShapeSDF {
    ShapeSDF* shape;
    double twist;
public:
    TwistYSDF(const ShapeSDF& shape_, double twist_);

    ~TwistYSDF() override;

    double evaluate(mc::point p) const final;

    std::string get_type() const final;

    void serialize_custom(std::ostream& out) const final;

    void deserialize_custom(std::istream& in) final;

    TwistYSDF* clone() const final;
};

#endif