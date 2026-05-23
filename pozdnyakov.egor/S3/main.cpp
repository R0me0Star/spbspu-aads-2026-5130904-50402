#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include "graph.hpp"
#include "stringUtils.hpp"
#include "vector.hpp"

using namespace pozdnyakov;

Graph *findGraph(Vector< std::pair< std::string, Graph > > &dict, const std::string &name)
{
  for (std::size_t i = 0; i < dict.size(); ++i) {
    if (dict[i].first == name) {
      return &dict[i].second;
    }
  }
  return nullptr;
}

int main(int argc, char *argv[])
{
  Vector< std::pair< std::string, Graph > > graphs_dict;

  if (argc > 1) {
    std::ifstream infile(argv[1]);
    if (infile.is_open()) {
      std::string line;
      while (std::getline(infile, line)) {
        Vector< std::string > tokens = utils::splitBySpace(line);
        if (tokens.empty())
          continue;

        if (tokens.size() == 4 && utils::isUnsignedNumber(tokens[3])) {
          std::string g_name = tokens[0];
          Graph *g = findGraph(graphs_dict, g_name);
          if (!g) {
            graphs_dict.pushBack({g_name, Graph()});
            g = &graphs_dict[graphs_dict.size() - 1].second;
          }
          g->addEdge(tokens[1], tokens[2], std::stoul(tokens[3]));
        }
      }
      infile.close();
    } else {
      std::cerr << "Не удалось открыть файл: " << argv[1] << "\n";
    }
  }

  std::string command_line;
  while (std::getline(std::cin, command_line)) {
    Vector< std::string > tokens = utils::splitBySpace(command_line);
    if (tokens.empty())
      continue;

    const std::string &cmd = tokens[0];

    if (cmd == "graphs" && tokens.size() == 1) {
      Vector< std::string > names;
      for (std::size_t i = 0; i < graphs_dict.size(); ++i) {
        names.pushBack(graphs_dict[i].first);
      }
      std::sort(names.begin(), names.end());
      for (std::size_t i = 0; i < names.size(); ++i) {
        std::cout << names[i] << (i + 1 == names.size() ? "" : " ");
      }
      std::cout << "\n";
    } else if (cmd == "vertexes" && tokens.size() == 2) {
      Graph *g = findGraph(graphs_dict, tokens[1]);
      if (g) {
        Vector< std::string > verts = g->getVertices();
        std::sort(verts.begin(), verts.end());
        for (std::size_t i = 0; i < verts.size(); ++i) {
          std::cout << verts[i] << (i + 1 == verts.size() ? "" : " ");
        }
        std::cout << "\n";
      }
    } else if (cmd == "outbound" && tokens.size() == 3) {
      Graph *g = findGraph(graphs_dict, tokens[1]);
      if (g) {
        Vector< EdgeInfo > edges = g->getOutboundEdges(tokens[2]);
        std::sort(edges.begin(), edges.end(), [](const EdgeInfo &a, const EdgeInfo &b) {
          if (a.to != b.to)
            return a.to < b.to;
          return a.weight < b.weight;
        });
        for (std::size_t i = 0; i < edges.size(); ++i) {
          std::cout << edges[i].to << " " << edges[i].weight << "\n";
        }
      }
    } else if (cmd == "inbound" && tokens.size() == 3) {
      Graph *g = findGraph(graphs_dict, tokens[1]);
      if (g) {
        Vector< EdgeInfo > edges = g->getInboundEdges(tokens[2]);
        std::sort(edges.begin(), edges.end(), [](const EdgeInfo &a, const EdgeInfo &b) {
          if (a.from != b.from)
            return a.from < b.from;
          return a.weight < b.weight;
        });
        for (std::size_t i = 0; i < edges.size(); ++i) {
          std::cout << edges[i].from << " " << edges[i].weight << "\n";
        }
      }
    } else if (cmd == "bind" && tokens.size() == 5 && utils::isUnsignedNumber(tokens[4])) {
      Graph *g = findGraph(graphs_dict, tokens[1]);
      if (g) {
        g->addEdge(tokens[2], tokens[3], std::stoul(tokens[4]));
      }
    } else if (cmd == "cut" && tokens.size() == 4) {
      Graph *g = findGraph(graphs_dict, tokens[1]);
      if (g) {
        g->removeEdges(tokens[2], tokens[3]);
      }
    } else if (cmd == "create" && tokens.size() == 2) {
      if (!findGraph(graphs_dict, tokens[1])) {
        graphs_dict.pushBack({tokens[1], Graph()});
      }
    } else if (cmd == "merge" && tokens.size() == 4) {
      Graph *g1 = findGraph(graphs_dict, tokens[2]);
      Graph *g2 = findGraph(graphs_dict, tokens[3]);
      if (g1 && g2 && !findGraph(graphs_dict, tokens[1])) {
        Graph new_graph;

        const Vector< std::string > &v1 = g1->getVertices();
        for (std::size_t i = 0; i < v1.size(); ++i)
          new_graph.addVertex(v1[i]);
        const Vector< std::string > &v2 = g2->getVertices();
        for (std::size_t i = 0; i < v2.size(); ++i)
          new_graph.addVertex(v2[i]);

        Vector< EdgeInfo > e1 = g1->getAllEdges();
        for (std::size_t i = 0; i < e1.size(); ++i)
          new_graph.addEdge(e1[i].from, e1[i].to, e1[i].weight);
        Vector< EdgeInfo > e2 = g2->getAllEdges();
        for (std::size_t i = 0; i < e2.size(); ++i)
          new_graph.addEdge(e2[i].from, e2[i].to, e2[i].weight);

        graphs_dict.pushBack({tokens[1], std::move(new_graph)});
      }
    } else if (cmd == "extract" && tokens.size() == 4) {
      Graph *source = findGraph(graphs_dict, tokens[2]);
      if (source && source->hasVertex(tokens[3]) && !findGraph(graphs_dict, tokens[1])) {
        Graph extracted;
        Vector< std::string > queue;
        Vector< std::string > visited;

        std::string start_v = tokens[3];
        queue.pushBack(start_v);
        visited.pushBack(start_v);
        extracted.addVertex(start_v);

        std::size_t head = 0;
        while (head < queue.size()) {
          std::string current = queue[head++];
          Vector< EdgeInfo > out_edges = source->getOutboundEdges(current);

          for (std::size_t i = 0; i < out_edges.size(); ++i) {
            const EdgeInfo &e = out_edges[i];
            extracted.addEdge(e.from, e.to, e.weight);

            bool is_visited = false;
            for (std::size_t j = 0; j < visited.size(); ++j) {
              if (visited[j] == e.to) {
                is_visited = true;
                break;
              }
            }

            if (!is_visited) {
              visited.pushBack(e.to);
              queue.pushBack(e.to);
            }
          }
        }
        graphs_dict.pushBack({tokens[1], std::move(extracted)});
      }
    }
  }

  return 0;
}
