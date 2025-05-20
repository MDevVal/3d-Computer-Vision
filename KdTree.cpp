#include "KdTree.h"
#include <algorithm>

static int axisForDepth(int d) { return d % 3; }
KdTree::KdTree(PointCloud &cloud, int maxDepth, int minPoints, int visualDepth)
    : m_cloud(cloud), m_maxDepth(maxDepth), m_minPoints(minPoints),
      m_visualDepth(visualDepth) {
  type = SceneObjectType::ST_KD_TREE;
  m_root = build(0, cloud.size(), 0);
}

KdTree::~KdTree() {
  std::function<void(Node *)> freeNode = [&](Node *n) {
    if (!n)
      return;
    freeNode(n->left);
    freeNode(n->right);
    delete n;
  };
  freeNode(m_root);
}

KdTree::Node *KdTree::build(int begin, int end, int depth) {
  Node *n = new Node;
  n->begin = begin;
  n->end = end;
  n->depth = depth;

  QVector3D mn(std::numeric_limits<float>::max(),
               std::numeric_limits<float>::max(),
               std::numeric_limits<float>::max());
  QVector3D mx(-mn);
  for (int i = begin; i < end; ++i) {
    const QVector4D &p = m_cloud[i];
    mn.setX(std::min(mn.x(), p.x()));
    mx.setX(std::max(mx.x(), p.x()));
    mn.setY(std::min(mn.y(), p.y()));
    mx.setY(std::max(mx.y(), p.y()));
    mn.setZ(std::min(mn.z(), p.z()));
    mx.setZ(std::max(mx.z(), p.z()));
  }
  n->min = mn;
  n->max = mx;

  if (depth >= m_maxDepth || end - begin <= m_minPoints)
    return n;

  int axis = axisForDepth(depth);
  int mid = (begin + end) / 2;
  std::nth_element(m_cloud.begin() + begin, m_cloud.begin() + mid,
                   m_cloud.begin() + end,
                   [axis](const QVector4D &a, const QVector4D &b) {
                     return a[axis] < b[axis];
                   });
  n->left = build(begin, mid, depth + 1);
  n->right = build(mid, end, depth + 1);
  return n;
}

void KdTree::draw(const RenderCamera &renderer, const QColor &colour,
                  float lineWidth) const {
  drawNode(m_root, renderer, m_visualDepth, colour, lineWidth);
}

void KdTree::drawNode(const Node *n, const RenderCamera &renderer,
                      int maxVisualDepth, const QColor &colour,
                      float lineWidth) const {
  if (!n || n->depth > maxVisualDepth)
    return;

  const QVector3D &a = n->min;
  const QVector3D &b = n->max;

  // 12 wire-frame edges of the AABB
  auto L = [&](const QVector3D &p1, const QVector3D &p2) {
    renderer.renderLine(p1.toVector4D(), p2.toVector4D(), colour, lineWidth);
  };

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

  drawNode(n->left, renderer, maxVisualDepth, colour, lineWidth);
  drawNode(n->right, renderer, maxVisualDepth, colour, lineWidth);
}
