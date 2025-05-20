#pragma once
#include "PointCloud.h"
#include "SceneObject.h"

class OctTree : public SceneObject {
public:
  struct Node {
    QVector3D min, max;
    int begin, end;
    Node *child[8]{nullptr, nullptr, nullptr, nullptr,
                   nullptr, nullptr, nullptr, nullptr};
    int depth = 0;
  };

  OctTree(PointCloud &cloud, int maxDepth = 10, int minPoints = 20,
          int visualDepth = 3);
  ~OctTree() override;

  void affineMap(const QMatrix4x4 &) override {}
  void draw(const RenderCamera &renderer,
            const QColor &colour = QColorConstants::Yellow,
            float lineWidth = 2.0f) const override;

  void setVisualDepth(int d) { m_visualDepth = std::max(1, d); }
  int visualDepth() const { return m_visualDepth; }

private:
  PointCloud &m_cloud;
  Node *m_root = nullptr;
  int m_maxDepth;
  int m_minPoints;
  int m_visualDepth;

  Node *build(int begin, int end, int depth, const QVector3D &min,
              const QVector3D &max);
  void drawNode(const Node *n, const RenderCamera &renderer, int maxVisDepth,
                const QColor &colour, float lineWidth) const;
};
