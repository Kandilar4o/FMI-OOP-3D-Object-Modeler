#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include <stddef.h> /* for size_t */
#include <stdint.h> /* for int32_t, etc. */

namespace mc {

/// 3D point (double precision)
struct point {
	double x;
	double y;
	double z;
};

/// Axis-aligned bounding box
struct aabb {
	mc::point min;
	mc::point max;

	/// Default constructor initializes to unit cube from (0,0,0) to (1,1,1)
	aabb();
	/// Construct AABB from min and max points
	aabb(mc::point min_, mc::point max_);
	/// Construct AABB from center and extents in each dimension (hx, hy, hz are the full widths of the box in each
	/// dimension)
	aabb(mc::point center, double hx, double hy, double hz);
};

/// Signed Distance Function (SDF) interface
class SDF {
   public:
	virtual ~SDF() = default;

	/// Evaluate the SDF at a given point. The SDF value is negative inside the surface, zero on the surface, and
	/// positive outside.
	virtual double operator()(mc::point p) const = 0;
};

/// Writer interface for streaming output data.
class Writer {
   public:
	virtual ~Writer() = default;

	/// Write a chunk of data. The writer should return 0 on success and non-zero on failure (e.g. if the underlying
	/// stream is closed).
	virtual int operator()(const char *data, size_t length) = 0;
};

// Opaque mesh type. Users should not dereference or manipulate this directly; use the provided API functions.
typedef void *Mesh;

// A special value for Mesh equivalent to nullptr. Should not be dereferenced.
extern Mesh MESH_NONE;

/**
 * Generate a triangle mesh from an SDF using the marching cubes algorithm.
 *
 * @param sdf			The signed distance function to sample.
 * @param nx, ny, nz	The resolution of the grid to sample the SDF on. Must be >= 2.
 * @param aabb			The axis-aligned bounding box defining the region of space to sample.
 * @param out_mesh		Output parameter. On success, will point to a newly allocated Mesh object containing vertex and
 * index data. Must be freed with mesh_destroy().
 *
 * @return MC_OK on success, or an error code on failure (see defines below).
 */
int mesh_from_sdf(const SDF &sdf, int nx, int ny, int nz, mc::aabb aabb, Mesh *out_mesh);

/**
 * Export mesh as GLTF 2.0 binary (.glb) format.
 *
 * @param mesh		The mesh to export. Must be a valid Mesh object returned by mesh_from_sdf().
 * @param writer	A Writer callback to stream the output data. The writer should return 0 on success and non-zero on
 * failure (e.g. if the underlying stream is closed).
 *
 * @return MC_OK on success, or an error code on failure (see defines below).
 */
int mesh_write_to_gltf(const Mesh *mesh, Writer &writer);

/**
 * Free memory associated with a Mesh object. After calling this function, the Mesh pointer will be set to MESH_NONE.
 *
 * @param mesh	Pointer to the Mesh to free. Must be a valid Mesh object returned by mesh_from_sdf(), or MESH_NONE.
 * After this call, *mesh will be set to MESH_NONE.
 */
void mesh_destroy(Mesh *mesh);

/* Error codes (small set for C compatibility)
 *
 * These codes are returned by the library on failure. Users may compare return
 * values against these defines.
 */
#define MC_OK				0
#define MC_ERROR_GENERIC	1 /* unspecified internal error */
#define MC_ERROR_BAD_INPUT	2 /* invalid args or bad grid */
#define MC_ERROR_WRITE_FAIL 3 /* writer callback aborted with non-zero */

/**
 * SDF function for the union of two SDFs (returns the minimum of the two). The two SDFs are expected to be evaluated at
 * the same point before being passed to this function.
 *
 * @param a	First SDF value
 * @param b	Second SDF value
 *
 * @return	The SDF value representing the union of the two inputs
 */
double sdf_union(double a, double b);

/**
 * Soft union of two SDFs, which blends the two shapes together over a region defined by the parameter k. The two SDFs
 * are expected to be evaluated at the same point before being passed to this function.
 *
 * @param a	First SDF value
 * @param b	Second SDF value
 * @param k	Blending factor. Larger values result in a smoother blend but also a larger blended region. A value of 0
 * results in a hard union (equivalent to sdf_union).
 *
 * @return	The SDF value representing the blended union of the two inputs
 */
double sdf_union_soft(double a, double b, double k);

/**
 * SDF function for the union of multiple SDFs (returns the minimum of all inputs). The SDFs are expected to be
 * evaluated at the same point before being passed to this function.
 *
 * @param vals	Array of SDF values
 * @param count	Number of SDF values in the array
 *
 * @return	The SDF value representing the union of all inputs
 */
double sdf_union_soft_n(double *vals, size_t count, double k);

/**
 * SDF function for the intersection of two SDFs (returns the maximum of the two). The two SDFs are expected to be
 * evaluated at the same point before being passed to this function.
 * @param a	First SDF value
 * @param b	Second SDF value
 * @return	The SDF value representing the intersection of the two inputs
 */
double sdf_intersection(double a, double b);

/**
 * SDF function for the difference of two SDFs (returns the maximum of a and -b). The two SDFs are expected to be
 * evaluated at the same point before being passed to this function.
 *
 * @param a	First SDF value (the "base" shape)
 * @param b	Second SDF value (the shape to subtract from the base)
 *
 * @return	The SDF value representing the difference of the two inputs
 */
double sdf_difference(double a, double b);

/**
 * SDF function for a sphere centered at the origin. The SDF value is negative inside the sphere, zero on the surface,
 * and positive outside.
 *
 * @param p		The point at which to evaluate the SDF
 * @param radius	The radius of the sphere
 * @return	The SDF value representing the signed distance from the point to the surface of the sphere
 */
double sdf_sphere(mc::point p, double radius);

/**
 * SDF function for an axis-aligned box centered at the origin. The SDF value is negative inside the box, zero on the
 * surface, and positive outside.
 *
 * @param p	The point at which to evaluate the SDF
 * @param b	The dimensions of the box (width, height, depth). The box extends from -b/2 to +b/2 in each dimension.
 * @return	The SDF value representing the signed distance from the point to the surface of the box
 */
double sdf_box(mc::point p, mc::point b);

/**
 * SDF function for a torus centered at the origin and aligned with the XY plane. The SDF value is negative inside the
 * torus, zero on the surface, and positive outside.
 *
 * @param p		The point at which to evaluate the SDF
 * @param r1	The major radius of the torus (distance from the center of the tube to the center of the torus)
 * @param r2	The minor radius of the torus (radius of the tube)
 * @return	The SDF value representing the signed distance from the point to the surface of the torus
 */
double sdf_torus(mc::point p, double r1, double r2);

/**
 * Apply a twist deformation around the Y axis to a point. The amount of twist is defined by the angle_per_unit
 * parameter, which specifies how much to twist per unit of Y. For example, if angle_per_unit is 0.1, then a point at
 * Y=1 will be twisted by 0.1 radians, a point at Y=2 will be twisted by 0.2 radians, and so on.
 *
 * @param p					The point to twist
 * @param angle_per_unit		The amount of twist in radians per unit of Y
 * @return	The twisted point
 */
mc::point twist_Y(mc::point p, double angle_per_unit);

/**
 * Rotate a point around the Y axis by a specified angle in radians. The rotation is applied counterclockwise when
 * looking down the positive Y axis towards the origin.
 *
 * @param p		The point to rotate
 * @param angle	The angle of rotation in radians
 * @return	The rotated point
 */
mc::point rotate_Y(mc::point p, double angle);

}	  // namespace mc

#endif /* MARCHING_CUBES_H */
