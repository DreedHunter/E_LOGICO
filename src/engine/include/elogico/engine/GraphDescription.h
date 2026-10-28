// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/engine/Node.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace elogico
{

using NodeId = std::uint32_t;

/// Descrizione del grafo prodotta dal modello progetto (thread non RT).
/// Il compilatore la trasforma in un CompiledGraph pronto per il thread audio.
struct GraphDescription
{
    struct NodeEntry
    {
        NodeId id = 0;
        std::shared_ptr<Node> node;
    };

    struct Connection
    {
        NodeId source = 0;
        NodeId destination = 0;
    };

    std::vector<NodeEntry> nodes;
    std::vector<Connection> connections;
    NodeId output = 0; // nodo la cui uscita va al driver (di norma il master)

    void addNode(NodeId id, std::shared_ptr<Node> node) { nodes.push_back({id, std::move(node)}); }
    void connect(NodeId source, NodeId destination) { connections.push_back({source, destination}); }
};

} // namespace elogico
