#include <algorithm>
#include <fstream>
#include <iostream>
#include <new>
#include <stdexcept>
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
  try {
    Vector< std::pair< std::string, Graph > > graphsDict;

    if (argc > 1) {
      std::ifstream infile(argv[1]);
      if (infile.is_open()) {
        std::string line;
        while (std::getline(infile, line)) {
          Vector< std::string > tokens = utils::splitBySpace(line);
          if (tokens.empty()) {
            continue;
          }

          if (tokens.size() == 4 && utils::isUnsignedNumber(tokens[3])) {
            std::string graphName = tokens[0];
            Graph *currentGraph = findGraph(graphsDict, graphName);
            if (!currentGraph) {
              graphsDict.pushBack({graphName, Graph()});
              currentGraph = &graphsDict[graphsDict.size() - 1].second;
            }
            currentGraph->addEdge(tokens[1], tokens[2], std::stoul(tokens[3]));
          } else {
            std::cerr << "Invalid format in file: " << line << "\n";
            return 1;
          }
        }
        infile.close();
      } else {
        std::cerr << "Failed to open file: " << argv[1] << "\n";
        return 1;
      }
    }

    std::string commandLine;
    while (std::getline(std::cin, commandLine)) {
      Vector< std::string > tokens = utils::splitBySpace(commandLine);
      if (tokens.empty()) {
        continue;
      }

      const std::string &cmd = tokens[0];

      if (cmd == "graphs" && tokens.size() == 1) {
        Vector< std::string > names;
        for (std::size_t i = 0; i < graphsDict.size(); ++i) {
          names.pushBack(graphsDict[i].first);
        }
        std::sort(names.begin(), names.end());
        for (std::size_t i = 0; i < names.size(); ++i) {
          std::cout << names[i] << (i + 1 == names.size() ? "" : " ");
        }
        std::cout << "\n";
      } else if (cmd == "vertexes" && tokens.size() == 2) {
        Graph *targetGraph = findGraph(graphsDict, tokens[1]);
        if (targetGraph) {
          Vector< std::string > verts = targetGraph->getVertices();
          std::sort(verts.begin(), verts.end());
          for (std::size_t i = 0; i < verts.size(); ++i) {
            std::cout << verts[i] << (i + 1 == verts.size() ? "" : " ");
          }
          std::cout << "\n";
        } else {
          std::cerr << "Graph not found: " << tokens[1] << "\n";
          return 1;
        }
      } else if (cmd == "outbound" && tokens.size() == 3) {
        Graph *targetGraph = findGraph(graphsDict, tokens[1]);
        if (targetGraph) {
          Vector< EdgeInfo > edges = targetGraph->getOutboundEdges(tokens[2]);
          std::sort(edges.begin(), edges.end(), [](const EdgeInfo &a, const EdgeInfo &b) {
            if (a.to != b.to) {
              return a.to < b.to;
            }
            return a.weight < b.weight;
          });
          for (std::size_t i = 0; i < edges.size(); ++i) {
            std::cout << edges[i].to << " " << edges[i].weight << "\n";
          }
        } else {
          std::cerr << "Graph not found: " << tokens[1] << "\n";
          return 1;
        }
      } else if (cmd == "inbound" && tokens.size() == 3) {
        Graph *targetGraph = findGraph(graphsDict, tokens[1]);
        if (targetGraph) {
          Vector< EdgeInfo > edges = targetGraph->getInboundEdges(tokens[2]);
          std::sort(edges.begin(), edges.end(), [](const EdgeInfo &a, const EdgeInfo &b) {
            if (a.from != b.from) {
              return a.from < b.from;
            }
            return a.weight < b.weight;
          });
          for (std::size_t i = 0; i < edges.size(); ++i) {
            std::cout << edges[i].from << " " << edges[i].weight << "\n";
          }
        } else {
          std::cerr << "Graph not found: " << tokens[1] << "\n";
          return 1;
        }
      } else if (cmd == "bind" && tokens.size() == 5 && utils::isUnsignedNumber(tokens[4])) {
        Graph *targetGraph = findGraph(graphsDict, tokens[1]);
        if (targetGraph) {
          targetGraph->addEdge(tokens[2], tokens[3], std::stoul(tokens[4]));
        } else {
          std::cerr << "Graph not found: " << tokens[1] << "\n";
          return 1;
        }
      } else if (cmd == "cut" && tokens.size() == 4) {
        Graph *targetGraph = findGraph(graphsDict, tokens[1]);
        if (targetGraph) {
          targetGraph->removeEdges(tokens[2], tokens[3]);
        } else {
          std::cerr << "Graph not found: " << tokens[1] << "\n";
          return 1;
        }
      } else if (cmd == "create" && tokens.size() == 2) {
        if (!findGraph(graphsDict, tokens[1])) {
          graphsDict.pushBack({tokens[1], Graph()});
        } else {
          std::cerr << "Graph already exists: " << tokens[1] << "\n";
          return 1;
        }
      } else if (cmd == "merge" && tokens.size() == 4) {
        Graph *g1 = findGraph(graphsDict, tokens[2]);
        Graph *g2 = findGraph(graphsDict, tokens[3]);
        if (g1 && g2 && !findGraph(graphsDict, tokens[1])) {
          Graph newGraph;

          const Vector< std::string > &v1 = g1->getVertices();
          for (std::size_t i = 0; i < v1.size(); ++i) {
            newGraph.addVertex(v1[i]);
          }
          const Vector< std::string > &v2 = g2->getVertices();
          for (std::size_t i = 0; i < v2.size(); ++i) {
            newGraph.addVertex(v2[i]);
          }

          Vector< EdgeInfo > e1 = g1->getAllEdges();
          for (std::size_t i = 0; i < e1.size(); ++i) {
            newGraph.addEdge(e1[i].from, e1[i].to, e1[i].weight);
          }
          Vector< EdgeInfo > e2 = g2->getAllEdges();
          for (std::size_t i = 0; i < e2.size(); ++i) {
            newGraph.addEdge(e2[i].from, e2[i].to, e2[i].weight);
          }

          graphsDict.pushBack({tokens[1], std::move(newGraph)});
        } else {
          std::cerr << "Invalid merge arguments or graph already exists\n";
          return 1;
        }
      } else if (cmd == "extract" && tokens.size() == 4) {
        Graph *sourceGraph = findGraph(graphsDict, tokens[2]);
        if (sourceGraph && sourceGraph->hasVertex(tokens[3]) && !findGraph(graphsDict, tokens[1])) {
          Graph extractedGraph;
          Vector< std::string > queueNodes;
          Vector< std::string > visitedNodes;

          std::string startVertex = tokens[3];
          queueNodes.pushBack(startVertex);
          visitedNodes.pushBack(startVertex);
          extractedGraph.addVertex(startVertex);

          std::size_t headIndex = 0;
          while (headIndex < queueNodes.size()) {
            std::string currentNode = queueNodes[headIndex++];
            Vector< EdgeInfo > outEdges = sourceGraph->getOutboundEdges(currentNode);

            for (std::size_t i = 0; i < outEdges.size(); ++i) {
              const EdgeInfo &edge = outEdges[i];
              extractedGraph.addEdge(edge.from, edge.to, edge.weight);

              bool isVisited = false;
              for (std::size_t j = 0; j < visitedNodes.size(); ++j) {
                if (visitedNodes[j] == edge.to) {
                  isVisited = true;
                  break;
                }
              }

              if (!isVisited) {
                visitedNodes.pushBack(edge.to);
                queueNodes.pushBack(edge.to);
              }
            }
          }
          graphsDict.pushBack({tokens[1], std::move(extractedGraph)});
        } else {
          std::cerr << "Invalid extract arguments, graph already exists, or source node not found\n";
          return 1;
        }
      } else {
        std::cerr << "Unknown or invalid command: " << commandLine << "\n";
        return 1;
      }
    }
  } catch (const std::bad_alloc &e) {
    std::cerr << "Memory allocation error: " << e.what() << "\n";
    return 2;
  } catch (const std::exception &e) {
    std::cerr << "Standard exception caught: " << e.what() << "\n";
    return 2;
  } catch (...) {
    std::cerr << "Unknown error occurred\n";
    return 2;
  }

  return 0;
}
