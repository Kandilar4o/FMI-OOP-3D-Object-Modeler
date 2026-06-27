#define _USE_MATH_DEFINES
#include "hm2_6MI0800779_mesh.hpp"
#include "hm2_6MI0800779_SDFserializer.hpp"
#include "hm2_6MI0800779_shapes.hpp"
#include <iostream>
#include <cmath>

int main() {
	try {
		mc::aabb box = { { -2 , -2, -2} , {2 , 2 , 2} };
		auto torus = TorusSDF(1.5, 0.2);
		auto rotated_torus = RotateYSDF(torus, M_PI / 2.0);
		auto torus_union = SoftUnionSDF(torus, rotated_torus, 0.01);
		auto twisted_torus = TwistYSDF(torus_union, 1.5);
		auto cage = SoftUnionSDF(twisted_torus, torus, 0.1);
		Mesh(torus, 100, 100, 100, box).write_to_gltf("torus.glb");
		Mesh(torus_union, 100, 100, 100, box).write_to_gltf("two_torus.glb");
		Mesh(twisted_torus, 100, 100, 100, box).write_to_gltf("twisted_torus.glb");
		Mesh(cage, 100, 100, 100, box).write_to_gltf("cage.glb");

		auto b = BoxSDF(mc::point{ .7 , .7 , .7 });
		auto s1 = SphereSDF(0.5);
		s1.position = { -0.4 , -0.4 , -0.4 };
		auto s2 = SphereSDF(0.5);
		s2.position = { 0.4 , 0.4 , 0.4 };
		auto u1 = UnionSDF(s1, b);
		auto u2 = SoftUnionSDF(s1, b, .1);
		auto u3 = SoftUnionNSDF(.1);
		u3.add(s1);
		u3.add(b);
		u3.add(s2);
		Mesh(u1, 100, 100, 100, box).write_to_gltf("u1.glb");
		Mesh(u2, 100, 100, 100, box).write_to_gltf("u2.glb");
		Mesh(u3, 100, 100, 100, box).write_to_gltf("u3.glb");

		auto final_shape = UnionSDF(cage, u3);
		Mesh(final_shape, 100, 100, 100, box).write_to_gltf("final_shape.glb");
	}
	catch (const std::exception& e) {
		std::cerr << "Problem! " << e.what() << "\n";
		return 1;
	}
	
	return 0;
}