#include "graph.hpp"

namespace pozdnyakov
{
  void Graph::addVertex(const std::string &v)
  {
    for (std::size_t i = 0; i < vertices_.size(); ++i) {
      if (vertices_[i] == v) {
        return;
      }
    }
    vertices_.pushBack(v);
  }

  bool Graph::hasVertex(const std::string &v) const
  {
    for (std::size_t i = 0; i < vertices_.size(); ++i) {
      if (vertices_[i] == v)
        return true;
    }
    return false;
  }

  void Graph::addEdge(const std::string &from, const std::string &to, unsigned int weight)
  {
    addVertex(from);
    addVertex(to);

    std::pair< std::string, std::string > key{from, to};

    if (edges_.has(key)) {
      edges_.at(key).pushBack(weight);
    } else {
      Vector< unsigned int > weights;
      weights.pushBack(weight);
      edges_.add(key, std::move(weights));
    }
  }

  bool Graph::removeEdges(const std::string &from, const std::string &to)
  {
    std::pair< std::string, std::string > key{from, to};
    if (edges_.has(key)) {
      edges_.drop(key);
      return true;
    }
    return false;
  }

  const Vector< std::string > &Graph::getVertices() const
  {
    return vertices_;
  }

  Vector< EdgeInfo > Graph::getOutboundEdges(const std::string &from)
  {
    Vector< EdgeInfo > result;
    for (auto it = edges_.begin(); it != edges_.end(); ++it) {
      if ((*it).first.first == from) {
        const std::string &to_ = (*it).first.second;
        const Vector< unsigned int > &weights = (*it).second;
        for (std::size_t i = 0; i < weights.size(); ++i) {
          result.pushBack({from, to_, weights[i]});
        }
      }
    }
    return result;
  }

  Vector< EdgeInfo > Graph::getInboundEdges(const std::string &to)
  {
    Vector< EdgeInfo > result;
    for (auto it = edges_.begin(); it != edges_.end(); ++it) {
      if ((*it).first.second == to) {
        const std::string &from_ = (*it).first.first;
        const Vector< unsigned int > &weights = (*it).second;
        for (std::size_t i = 0; i < weights.size(); ++i) {
          result.pushBack({from_, to, weights[i]});
        }
      }
    }
    return result;
  }

  Vector< EdgeInfo > Graph::getAllEdges()
  {
    Vector< EdgeInfo > result;
    for (auto it = edges_.begin(); it != edges_.end(); ++it) {
      const std::string &from_ = (*it).first.first;
      const std::string &to_ = (*it).first.second;
      const Vector< unsigned int > &weights = (*it).second;
      for (std::size_t i = 0; i < weights.size(); ++i) {
        result.pushBack({from_, to_, weights[i]});
      }
    }
    return result;
  }
}
