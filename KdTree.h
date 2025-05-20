#pragma once
#include "PointCloud.h"
#include "SceneObject.h"

class KdTree : public SceneObject {
public:
  struct Node {
    QVector3D min, max;
    int begin, end;
    Node *left = nullptr;
    Node *right = nullptr;
    int depth = 0;
  };

  explicit KdTree(PointCloud &cloud, int maxDepth = 10, int minPoints = 20,
                  int visualDepth = 3);
  ~KdTree() override;
  void setVisualDepth(int d) { m_visualDepth = std::max(1, d); }
  int visualDepth() const { return m_visualDepth; }

  void affineMap(const QMatrix4x4 &) override {}
  void draw(const RenderCamera &renderer,
            const QColor &colour = QColorConstants::Yellow,
            float lineWidth = 2.0f) const override;

  const Node *root() const { return m_root; }

private:
  PointCloud &m_cloud;
  Node *m_root = nullptr;
  int m_maxDepth;
  int m_minPoints;
  int m_visualDepth;

  Node *build(int begin, int end, int depth);
  void drawNode(const Node *n, const RenderCamera &renderer, int maxVisualDepth,
                const QColor &colour, float lineWidth) const;
};
