#ifndef CGPROJECT_WAVEFRONTPARSER_H
#define CGPROJECT_WAVEFRONTPARSER_H

#include "Light.h"
#include "TriangleMesh.h"
#include <BVH.h>
#include <fstream>

using String = std::string;
using std::vector;
using std::shared_ptr;

bool parseWaveFrontFile(const String& filename, shared_ptr<BVH>& bvh);

void parseTexture(const String& filename, const shared_ptr<Material>& mat);

void parseNormalMap(String filename);

#endif //CGPROJECT_WAVEFRONTPARSER_H
