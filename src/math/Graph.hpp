#pragma once

#include <cstddef>
#include <functional>
#include <list>
#include <stdexcept>
#include <unordered_map>

namespace Math {

    //! A node in a graph.
    template<typename T>
    struct Node {
        std::list<T> in;
        std::list<T> out;
    };

    //! Graph data structure.
    template<typename T>
    class Graph {

        public:

            using FilterExpression_t = std::function<bool(const Node<T>& node)>;

            //! Add a new node to the graph.
            void AddNode(T value) {
                m_node_map.emplace(value, Node<T>());
            }

            //! Get a node from the graph
            Node<T>& GetNode(T value) {
                return m_node_map.at(value);
            }

            //! Adds an incoming transition to the graph.
            //!
            //! @param[in] start The node that is transitioning to the target.
            //! @param[in] end The node that is being transitioned to.
            void AddTransition(T start, T end) {
                Node<T>& startNode = m_node_map[start];
                Node<T>& endNode = m_node_map[end];
                startNode.out.push_back(end);
                endNode.in.push_back(start);
                m_num_transitions++;
            }

            void RemoveTransition(T start, T end) {
                Node<T>& startNode = m_node_map[start];
                Node<T>& endNode = m_node_map[end];
                startNode.out.erase(
                    std::remove(startNode.out.begin(), startNode.out.end(), end),
                    startNode.out.end());
                endNode.in.erase(
                    std::remove(endNode.in.begin(), endNode.in.end(), start),
                    endNode.in.end());
                m_num_transitions--;
            }

            //! Check the total number of nodes.
            size_t GetNodeCount() {
                return m_node_map.size();
            }

            //! Check the total number of edges.
            size_t GetEdgeCount() {
                return m_num_transitions;
            }

            //! Get the set of node IDs in graph.
            std::list<T> GetNodeIDs(FilterExpression_t filter) {
                std::list<T> nodeIDs;
                for (auto nodeIter : m_node_map) {
                    if (filter(nodeIter.second)) {
                        nodeIDs.push_back(nodeIter.first);
                    }
                }
                return nodeIDs;
            }

        private:

            std::unordered_map<T, Node<T>> m_node_map;
            size_t m_num_transitions{0};
    };

    //! Implementing Kahn's algorithm for topological sorting:
    //! https://en.wikipedia.org/wiki/Topological_sorting
    template<typename T>
    std::list<T> TopologicalSort(Graph<T>& graph) {

        std::list<T> sorted;
        std::list<T> startNodes = graph.GetNodeIDs(
            [](const Node<T>& node) {
                return node.in.empty();
            });

        while (!startNodes.empty()) {
            T nodeN_id = startNodes.front();
            startNodes.pop_front();
            sorted.push_back(nodeN_id);

            Node<T>& nodeN = graph.GetNode(nodeN_id);
            std::list<T> outgoing = nodeN.out;
            for (T& nodeM_id : outgoing) {
                graph.RemoveTransition(nodeN_id, nodeM_id);
                Node<T>& nodeM = graph.GetNode(nodeM_id);
                if (nodeM.in.empty()) {
                    startNodes.push_back(nodeM_id);
                }
            }
        }

        if (graph.GetEdgeCount() > 0) {
            throw std::runtime_error("TopologicalSort(): Graph has at least one cycle.");
        }

        return sorted;
    }
}