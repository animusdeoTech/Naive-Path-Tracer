#include <write_ppm.h>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <utility>
#include "WaveFrontParser.h"

using Eigen::Vector3d;
using Eigen::Vector2d;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
extern vector<shared_ptr<Vector3d>> normal_map;

namespace {
struct FaceIndex {
    int v = -1;
    int vt = -1;
    int vn = -1;
};

FaceIndex parseFaceIndex(const String& token) {
    FaceIndex idx;
    std::stringstream ss(token);
    String part;
    int component = 0;
    while (std::getline(ss, part, '/')) {
        if (!part.empty()) {
            const int value = std::stoi(part) - 1;
            if (component == 0) idx.v = value;
            else if (component == 1) idx.vt = value;
            else if (component == 2) idx.vn = value;
        }
        ++component;
    }
    return idx;
}
}

void parseTexture(const String& filename, const shared_ptr<Material>& mat) {
    char pSix[10];
    int width, height, maximum = 0;
    std::ifstream file(filename, ios::in | ios::binary);
    file >> pSix >> width >> height >> maximum;
    char delimiter;
    file.read(&delimiter, 1);
    const int data_begin = file.tellg();
    file.seekg(0, file.end);
    const int data_end = file.tellg();
    file.seekg(data_begin, file.beg);
    const int data_size = data_end - data_begin;
    if (data_size != 3 * width * height) {
        cout << ("Failed to read PPM, unexpected data size %s.", filename.c_str());
    }
    vector<unsigned char> image_buffer = std::vector<unsigned char>(std::istreambuf_iterator<char>(file),
                                                                    std::istreambuf_iterator<char>());
    for (unsigned i=0; i < image_buffer.size() / 3; i++) {
        texture.push_back(std::make_shared<rgb>(image_buffer[0 + i*3], image_buffer[1 + i*3], image_buffer[2 + i*3]));
    }
}

void parseNormalMap(String filename) {
    char pSix[10];
    int width, height, maximum = 0;
    std::ifstream file(filename, ios::in | ios::binary);
    file >> pSix >> width >> height >> maximum;
    char delimiter;
    file.read(&delimiter, 1);
    const int data_begin = file.tellg();
    file.seekg(0, file.end);
    const int data_end = file.tellg();
    file.seekg(data_begin, file.beg);
    const int data_size = data_end - data_begin;
    if (data_size != 3 * width * height) {
        cout << ("Failed to read PPM, unexpected data size %s.", filename.c_str());
    }
    vector<unsigned char> image_buffer = std::vector<unsigned char>(std::istreambuf_iterator<char>(file),
                                                                    std::istreambuf_iterator<char>());
    for (unsigned i=0; i < image_buffer.size() / 3; i++) {
        normal_map.push_back(std::make_shared<Vector3d>(image_buffer[0 + i*3], image_buffer[1 + i*3], image_buffer[2 + i*3]));
    }
}

bool parseWaveFrontFile(const String& filename, shared_ptr<BVH>& bvh) {
    vector<shared_ptr<Vector3d>> vertexes, normals;
    vector<shared_ptr<Vector2d>> textures;
    vector<shared_ptr<Object>> triangles;
    String s;
    std::ifstream fin(filename);
    if (!fin) {
        return false;
    }
    while (fin >> s) {
        switch (*s.c_str()) {
            case 'v': {
                if (s.size() > 1 && s[1] == 't') {
                    double u, v;
                    fin >> u >> v;
                    shared_ptr<Vector2d> vertex(new Vector2d());
                    vertex->x() = u;
                    vertex->y() = v;
                    textures.push_back(vertex);
                } else if (s.size() > 1 && s[1] == 'n') {
                    double x, y, z;
                    fin >> x >> y >> z;
                    shared_ptr<Vector3d> vertex(new Vector3d());
                    vertex->x() = x;
                    vertex->y() = y;
                    vertex->z() = z;
                    normals.push_back(vertex);
                } else {
                    double x, y, z;
                    fin >> x >> y >> z;
                    shared_ptr<Vector3d> vertex(new Vector3d());
                    vertex->x() = x;
                    vertex->y() = y;
                    vertex->z() = z;
                    vertexes.push_back(vertex);
                }
            } break;
            case 'f': {
                String f1, f2, f3;
                fin >> f1 >> f2 >> f3;
                const FaceIndex i1 = parseFaceIndex(f1);
                const FaceIndex i2 = parseFaceIndex(f2);
                const FaceIndex i3 = parseFaceIndex(f3);
                if (i1.v < 0 || i2.v < 0 || i3.v < 0) {
                    continue;
                }
                if (i1.v >= static_cast<int>(vertexes.size()) ||
                    i2.v >= static_cast<int>(vertexes.size()) ||
                    i3.v >= static_cast<int>(vertexes.size())) {
                    continue;
                }

                Matrix3d vertices, norms;
                vertices << *vertexes[i1.v], *vertexes[i2.v], *vertexes[i3.v];

                const Vector3d geometric_normal =
                    ((*vertexes[i2.v] - *vertexes[i1.v]).cross(*vertexes[i3.v] - *vertexes[i1.v])).normalized();

                const Vector3d n1 = (i1.vn >= 0 && i1.vn < static_cast<int>(normals.size())) ? *normals[i1.vn] : geometric_normal;
                const Vector3d n2 = (i2.vn >= 0 && i2.vn < static_cast<int>(normals.size())) ? *normals[i2.vn] : geometric_normal;
                const Vector3d n3 = (i3.vn >= 0 && i3.vn < static_cast<int>(normals.size())) ? *normals[i3.vn] : geometric_normal;
                norms << n1, n2, n3;

                const Vector2d t1 = (i1.vt >= 0 && i1.vt < static_cast<int>(textures.size())) ? *textures[i1.vt] : Vector2d(0.0, 0.0);
                const Vector2d t2 = (i2.vt >= 0 && i2.vt < static_cast<int>(textures.size())) ? *textures[i2.vt] : Vector2d(0.0, 0.0);
                const Vector2d t3 = (i3.vt >= 0 && i3.vt < static_cast<int>(textures.size())) ? *textures[i3.vt] : Vector2d(0.0, 0.0);

                shared_ptr<Triangle> triangle(new Triangle(vertices, norms, t1, t2, t3));
                shared_ptr<Material> mat1(new Material());
                mat1->kd = rgb(0.25098,0.84158,0.815686) * 0.3;
                triangle->material = mat1;
                triangles.push_back(triangle);
            } break;
            default: {
                std::getline(fin, s);
            } break;
        }
    }
    bvh = top_down(triangles);
    return static_cast<bool>(bvh);
}
