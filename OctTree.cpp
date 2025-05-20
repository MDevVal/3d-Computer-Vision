#include "OctTree.h"
#include <algorithm>
#include <limits>

static void cubeEdges(const QVector3D &a, const QVector3D &b,
                      const std::function<void(QVector3D, QVector3D)> &L) {
  L({a.x(), a.y(), a.z()}, {b.x(), a.y(), a.z()});
  L({a.x(), a.y(), a.z()}, {a.x(), b.y(), a.z()});
  L({a.x(), a.y(), a.z()}, {a.x(), a.y(), b.z()});
  L({b.x(), b.y(), b.z()}, {a.x(), b.y(), b.z()});
  L({b.x(), b.y(), b.z()}, {b.x(), a.y(), b.z()});
  L({b.x(), b.y(), b.z()}, {b.x(), b.y(), a.z()});
  L({a.x(), b.y(), a.z()}, {b.x(), b.y(), a.z()});
  L({a.x(), b.y(), a.z()}, {a.x(), b.y(), b.z()});
  L({a.x(), a.y(), b.z()}, {b.x(), a.y(), b.z()});
  L({a.x(), a.y(), b.z()}, {a.x(), b.y(), b.z()});
  L({b.x(), a.y(), a.z()}, {b.x(), a.y(), b.z()});
  L({b.x(), a.y(), a.z()}, {b.x(), b.y(), a.z()});
}

OctTree::OctTree(PointCloud &cloud, int maxDepth, int minPoints,
                 int visualDepth)
    : m_cloud(cloud), m_maxDepth(maxDepth), m_minPoints(minPoints),
      m_visualDepth(visualDepth) {
  type = SceneObjectType::ST_OCT_TREE;

  QVector3D mn(std::numeric_limits<float>::max(),
               std::numeric_limits<float>::max(),
               std::numeric_limits<float>::max());
  QVector3D mx(-mn);

  for (const QVector4D &p : cloud) {
    mn.setX(std::min(mn.x(), p.x()));
    mx.setX(std::max(mx.x(), p.x()));
    mn.setY(std::min(mn.y(), p.y()));
    mx.setY(std::max(mx.y(), p.y()));
    mn.setZ(std::min(mn.z(), p.z()));
    mx.setZ(std::max(mx.z(), p.z()));
  }

  float side = std::max({mx.x() - mn.x(), mx.y() - mn.y(), mx.z() - mn.z()});
  QVector3D center = 0.5f * (mn + mx);
  QVector3D half = QVector3D(side, side, side) * 0.5f;

  m_root = build(0, cloud.size(), 0, center - half, center + half);
}

OctTree::~OctTree() {
  std::function<void(Node *)> freeNode = [&](Node *n) {
    if (!n)
      return;
    for (auto *c : n->child)
      freeNode(c);
    delete n;
  };
  freeNode(m_root);
}

OctTree::Node *OctTree::build(int begin, int end, int depth,
                              const QVector3D &min, const QVector3D &max) {
  Node *n = new Node{min, max, begin, end, {nullptr}, depth};

  if (depth >= m_maxDepth || end - begin <= m_minPoints)
    return n;

  QVector3D center = 0.5f * (min + max);
  int mid[8] = {begin};
  /* partition points into 8 octants in-place */
  auto octantIdx = [&](const QVector4D &p) -> int {
    return (p.x() >= center.x()) * 1 + (p.y() >= center.y()) * 2 +
           (p.z() >= center.z()) * 4;
  };
  int head[8];
  for (int i = 0; i < 8; ++i)
    head[i] = begin;
  for (int i = begin; i < end; ++i) {
    int idx = octantIdx(m_cloud[i]);
    std::swap(m_cloud[head[idx]], m_cloud[i]);
    ++head[idx];
  }
  mid[0] = begin;
  for (int i = 1; i < 8; ++i)
    mid[i] = head[i - 1];

  QVector3D half = 0.5f * (max - min);
  auto subMin = [&](int i) {
    return QVector3D((i & 1) ? center.x() : min.x(),
                     (i & 2) ? center.y() : min.y(),
                     (i & 4) ? center.z() : min.z());
  };
  auto subMax = [&](int i) {
    return QVector3D((i & 1) ? max.x() : center.x(),
                     (i & 2) ? max.y() : center.y(),
                     (i & 4) ? max.z() : center.z());
  };

  for (int i = 0; i < 8; ++i) {
    int b = mid[i];
    int e = (i == 7) ? end : mid[i + 1];
    if (b == e)
      continue;
    n->child[i] = build(b, e, depth + 1, subMin(i), subMax(i));
  }
  return n;
}

void OctTree::draw(const RenderCamera &renderer, const QColor &colour,
                   float lineWidth) const {
  drawNode(m_root, renderer, m_visualDepth, colour, lineWidth);
}

void OctTree::drawNode(const Node *n, const RenderCamera &renderer,
                       int maxVisDepth, const QColor &colour,
                       float lineWidth) const {
  if (!n || n->depth > maxVisDepth)
    return;

  cubeEdges(n->min, n->max, [&](QVector3D p1, QVector3D p2) {
    renderer.renderLine(p1.toVector4D(), p2.toVector4D(), colour, lineWidth);
  });

  for (auto *c : n->child)
    drawNode(c, renderer, maxVisDepth, colour, lineWidth);
}
