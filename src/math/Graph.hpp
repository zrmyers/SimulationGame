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
        std::list<T> incomingTransitions;
        std::list<T> outgoingTransitions;
    };

    //! Graph data structure.
    template<typename T>
    class Graph {

        public:

            using FilterExpression_t = std::function<bool(const Node<T>& node)>;

            //! Add a new node to the graph.
            void AddNode(T value) {
                m_node_map.emplace(value, {});
            }

            //! Get a node from the graph
            Node<T>& GetNode(T value) {
                return m_node_map.at(value);
            }

            //! Adds an incoming transition to the graph.
            //!
            //! @param[in] src The node that is transitioning to the target.
            //! @param[in] dst The node that is being transitioned to.
            void AddIncomingTransition(T src, T dst) {
                Node<T>& srcNode = m_node_map[src];
                Node<T>& dstNode = m_node_map[dst];
                srcNode.outgoingTransitions.push_back(dst);
                dstNode.incomingTransitions.push_back(src);
                m_num_transitions++;
            }

            void AddOutgoingTransition(T src, T dst) {
                AddIncomingTransition(dst, src); // NOLINT(readability-suspicious-call-argument) swapping is on purpose.
            }

            void RemoveIncomingTransition(T src, T dst) {
                Node<T>& srcNode = m_node_map[src];
                Node<T>& dstNode = m_node_map[dst];
                std::remove(srcNode.outgoingTransitions.begin(), srcNode.outgoingTransitions.end(), dst);
                std::remove(dstNode.incomingTransitions.begin(), dstNode.incomingTransitions.end(), src);
                m_num_transitions--;
            }

            void RemoveOutgoingTransition(T src, T dst) {
                RemoveIncomingTransition(dst, src); // NOLINT swapping is on purpose, since changing from outgoing to incoming.
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
            size_t m_num_transitions;
    };

    //! Implementing Kahn's algorithm for topological sorting:
    //! https://en.wikipedia.org/wiki/Topological_sorting
    template<typename T>
    std::list<T> TopologicalSort(Graph<T>& graph) {

        std::list<T> sorted;
        std::list<T> startNodes = graph.GetNodeIDs(
            [](const Node<T>& node) {
                return node.incoming_transitions.empty();
            });

        while (!startNodes.empty()) {
            T nodeN_id = startNodes.front();
            startNodes.pop_front();
            sorted.push_back(nodeN_id);

            Node<T>& nodeN = graph.GetNode(nodeN_id);
            std::list<T> outgoing = nodeN.outgoingTransitions;
            for (T& nodeM_id : outgoing) {
                graph.RemoveOutgoingTransition(nodeN, nodeM_id);
                Node<T>& nodeM = graph.GetNode(nodeM_id);
                if (nodeM.incomingTransitions.empty()) {
                    startNodes.push_back();
                }
            }
        }

        if (graph.GetEdgeCount() > 0) {
            throw std::runtime_error("TopologicalSort(): Graph has at least one cycle.");
        }

        return sorted;
    }
}