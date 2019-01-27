#ifndef POLYOBJECT_H
#define POLYOBJECT_H

#include <iostream>
#include <vector>
#include "Box.h"
#include "ray.h"
#include <math.h>
#include <algorithm>
#include <cassert>
#include <climits>
#include "OrderedList.h"
#include "PolyObject.h"
#include "rgb.h"

#include <fstream>
#include "./tinyply/source/tinyply.h"
namespace RayTracerxx {

struct Triangle {
        Point<3>  vertex[3];
        Vector<3> normal;
        RGB       color;
        Number_t  ks;
        Number_t  kd;
        RGB       ka;

        Triangle() : vertex{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, normal{0, 0, 0} {
                setColor(127, 127, 127);
        }

        Triangle(const Point<3>& p, const Point<3>& p2, const Point<3>& p3)
            : Triangle() {
                Point<3> ppp[3] = {p, p2, p3};
                setVertices(ppp);
        }

        // Assuming that points are given in Counter Clockwise order
        void setVertices(Point<3>* points) {
                for (int i = 0; i < 3; i++) {
                        vertex[i] = points[i];
                }

                Vector<3> v1(vertex[2] - vertex[1]);
                Vector<3> v2(vertex[0] - vertex[1]);

                normal = v1.cross(v2);
                normal.normalize();
        }

        void setColor(Number_t r, Number_t g, Number_t b) {
                color.setRGB(r, g, b);
                ka = color * (1 / 10.0);
                kd = 1;
                ks = 1;
        }

        Box CalcBounds() const {
                Number_t xMax, yMax, zMax, xMin, yMin, zMin;
                using std::max;
                using std::min;
                xMax = max(vertex[0][0], max(vertex[1][0], vertex[2][0]));
                yMax = max(vertex[0][1], max(vertex[1][1], vertex[2][1]));
                zMax = max(vertex[0][2], max(vertex[1][2], vertex[2][2]));

                xMin = min(vertex[0][0], min(vertex[1][0], vertex[2][0]));
                yMin = min(vertex[0][1], min(vertex[1][1], vertex[2][1]));
                zMin = min(vertex[0][2], min(vertex[1][2], vertex[2][2]));

                return Box(xMax, yMax, zMax, xMin, yMin, zMin);
        }

        bool operator==(const Triangle& t) const {
                return vertex[0] == t.vertex[0] && vertex[1] == t.vertex[1] &&
                       vertex[2] == t.vertex[2];
        }

        // Möller–Trumbore intersection algorithm
        // http://webserver2.tecgraf.puc-rio.br/~mgattass/cg/trbRR/
        // Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
        void Intersect(Ray& tracer) {
                const Number_t EPSILON = 0.0000001;
                Vector<3>      edge1   = vertex[1] - vertex[0];
                Vector<3>      edge2   = vertex[2] - vertex[0];
                Vector<3>      h       = tracer.direction.cross(edge2);
                Number_t       a       = edge1.dot(h);
                if (a > -EPSILON && a < EPSILON)
                        return;  // This ray is parallel to this triangle.
                Number_t  f = 1.0 / a;
                Vector<3> s = tracer.origin - vertex[0];
                Number_t  u = f * (s.dot(h));
                if (u < 0.0 || u > 1.0)
                        return;
                Vector<3> q = s.cross(edge1);
                Number_t  v = f * tracer.direction.dot(q);
                if (v < 0.0 || u + v > 1.0)
                        return;
                // At this stage we can compute t to find out where the
                // intersection point is on the line.
                Number_t t = f * edge2.dot(q);
                if (t > EPSILON)  // ray intersection
                {
                        tracer.hit = (t < tracer.t) ? this : tracer.hit;
                        tracer.t   = (t < tracer.t) ? t : tracer.t;
                        return;
                } else  // This means that there is a line intersection but not
                        // a ray intersection.
                        return;
        }
};
}  // namespace RayTracerxx

namespace RayTracerxx {

static void bounds(Number_t lo[], Number_t hi[], Point<3> tri[]) {
        using std::max;
        using std::min;
        hi[0] = max(hi[0], max(tri[0][0], max(tri[1][0], tri[2][0])));
        hi[1] = max(hi[1], max(tri[0][1], max(tri[1][1], tri[2][1])));
        hi[2] = max(hi[2], max(tri[0][2], max(tri[1][2], tri[2][2])));

        lo[0] = min(lo[0], min(tri[0][0], min(tri[1][0], tri[2][0])));
        lo[1] = min(lo[1], min(tri[0][1], min(tri[1][1], tri[2][1])));
        lo[2] = min(lo[2], min(tri[0][2], min(tri[1][2], tri[2][2])));
}

static void getProperties(std::vector<float>&    verts,
                          std::vector<uint8_t>&  color,
                          std::vector<uint32_t>& f, std::string filename) {
        using namespace tinyply;
        (void)color;
        try {
                std::ifstream ss(filename, std::ios::binary);
                if (ss.fail())
                        throw std::runtime_error("failed to open " + filename);

                PlyFile file;
                file.parse_header(ss);

                std::cout << "................................................"
                             "........................\n";
                for (auto c : file.get_comments())
                        std::cout << "Comment: " << c << std::endl;
                for (auto e : file.get_elements()) {
                        std::cout << "element - " << e.name << " (" << e.size
                                  << ")" << std::endl;
                        for (auto p : e.properties)
                                std::cout
                                    << "\tproperty - " << p.name << " ("
                                    << tinyply::PropertyTable[p.propertyType]
                                           .str
                                    << ")" << std::endl;
                }
                std::cout << "................................................"
                             "........................\n";

                // Tinyply treats parsed data as untyped byte buffers. See
                // below for examples.
                std::shared_ptr<PlyData> vertices, normals, faces, texcoords;

                // The header information can be used to programmatically
                // extract properties on elements known to exist in the header
                // prior to reading the data. For brevity of this sample,
                // properties like vertex position are hard-coded:
                try {
                        vertices = file.request_properties_from_element(
                            "vertex", {"x", "y", "z"});
                } catch (const std::exception& e) {
                        std::cerr << "tinyply exception: " << e.what()
                                  << std::endl;
                }

                // try { normals =
                // file.request_properties_from_element("vertex", { "nx", "ny",
                // "nz" }); } catch (const std::exception & e) { std::cerr <<
                // "tinyply exception: " << e.what() << std::endl; }

                // Providing a list size hint (the last argument) is a 2x
                // performance improvement. If you have arbitrary ply files, it
                // is best to leave this 0. (ALERT DEVELOPER TO ADD
                // DOCUMENTATION) : Hint causes segfault if hint is wrong (too
                // small)
                try {
                        faces = file.request_properties_from_element(
                            "face", {"vertex_indices"}, 0);
                } catch (const std::exception& e) {
                        std::cerr << "tinyply exception: " << e.what()
                                  << std::endl;
                }

                // manual_timer read_timer;

                // read_timer.start();
                file.read(ss);
                // read_timer.stop();

                // std::cout << "Reading took " << read_timer.get() / 1000.f <<
                // " seconds." << std::endl; if (vertices) std::cout << "\tRead
                // " << vertices->count << " total vertices "<< std::endl; if
                // (normals) std::cout << "\tRead " << normals->count << "
                // total vertex normals " << std::endl; if (texcoords)
                // std::cout << "\tRead " << texcoords->count << " total vertex
                // texcoords " << std::endl;
                if (faces)
                        std::cout << "\tRead " << faces->count
                                  << " total faces (triangles) " << std::endl;

                // type casting to your own native types - Option B
                {
                        assert(vertices->t == tinyply::Type::FLOAT32);
                        decltype(&verts[0]) tmp =
                            reinterpret_cast<decltype(&verts[0])>(
                                vertices->buffer.get());
                        verts.assign(tmp, tmp + vertices->buffer.size_bytes() /
                                                    sizeof(tmp[0]));

                        assert(faces->t == tinyply::Type::UINT32 ||
                               faces->t == tinyply::Type::INT32);
                        decltype(&f[0]) tmp2 =
                            reinterpret_cast<decltype(&f[0])>(
                                faces->buffer.get());
                        f.assign(tmp2, tmp2 + faces->buffer.size_bytes() /
                                                  sizeof(tmp2[0]));
                }

        } catch (const std::exception& e) {
                std::cerr << "Caught tinyply exception: " << e.what()
                          << std::endl;
        }
}
static void getVertices(size_t i, const std::vector<uint32_t>& faces,
                        const std::vector<float>& verts, Point<3> tri[3]) {
        uint32_t v1 = 3 * faces[i * 3];
        uint32_t v2 = 3 * faces[i * 3 + 1];
        uint32_t v3 = 3 * faces[i * 3 + 2];

        for (unsigned j = 0; j < 3; j++) {
                tri[0].data[j] = verts[v1 + j];
                tri[1].data[j] = verts[v2 + j];
                tri[2].data[j] = verts[v3 + j];
        }
}

static void setColors(Triangle& toRender, int i, std::vector<uint8_t>& color) {
        if (color.size() != 0)
                toRender.color.setRGB(color[i * 3], color[i * 3 + 1],
                                      color[i * 3 + 2]);
}

struct PolyObject {
        std::vector<Triangle> mesh;
        Box                   bbox;

        PolyObject(const std::string& filename) { read_ply_file(filename); }

        void read_ply_file(const std::string& filename) {
                using namespace tinyply;
                Number_t hi[3], lo[3];
                hi[0] = hi[1] = hi[2] = -Ray::Infinity;
                lo[0] = lo[1] = lo[2] = Ray::Infinity;

                std::vector<float>    verts;
                std::vector<uint8_t>  color;
                std::vector<uint32_t> faces;
                Point<3>              tri[3];

                getProperties(verts, color, faces, filename);
                mesh.reserve(faces.size() / 3);
                for (size_t i = 0; i < faces.size() / 3; i++) {
                        getVertices(i, faces, verts, tri);
                        bounds(lo, hi, tri);
                        mesh.emplace_back(tri[0], tri[1], tri[2]);
                        setColors(mesh.back(), i, color);
                }
                bbox = Box(hi[0], hi[1], hi[2], lo[0], lo[1], lo[2]);
                std::cout << bbox << "\n";
        }
};
}  // namespace RayTracerxx
#endif