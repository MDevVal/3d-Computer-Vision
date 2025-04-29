//
//  A class specializing hexahedra to cubes, i.e. eqilateral hexahedra
//
// (c) Georg Umlauf, 2021
//
#include "Cube.h"

#include "Axes.h"
#include "GLConvenience.h"

Cube::Cube(QVector4D _origin, float _sideLength)
    : Hexahedron(_origin, _sideLength, _sideLength, _sideLength) {
  type = SceneObjectType::ST_CUBE;
}

Cube::Cube(const Cube &c) : Hexahedron(c) { type = SceneObjectType::ST_CUBE; }
