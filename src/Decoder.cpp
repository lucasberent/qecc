//
// Created by lucas on 21/04/2022.
//

#include "Decoder.hpp"

#include "TreeNode.hpp"

#include <cassert>
#include <queue>
#include <set>

std::vector<std::shared_ptr<TreeNode>> Decoder::decode(std::set<std::shared_ptr<TreeNode>>& syndrome) {
    auto                                   components = syndrome;
    std::vector<std::shared_ptr<TreeNode>> erasure;
    std::map<std::size_t, bool>            presentMap{}; // for step 4

    while (!components.empty()) {
        for (size_t i = 0; i < components.size(); i++) {
            // Step 1 growth // todo factor this out in order to compare different variants of growth
            std::vector<std::pair<std::shared_ptr<TreeNode>, std::shared_ptr<TreeNode>>> fusionEdges;
            for (auto& component: components) {
                presentMap.insert(std::make_pair(component->vertexIdx, true));
                // at this point we can assume that component represents root of the component
                assert(component->parent == nullptr);
                auto bndryNodes = component->boundaryVertices;

                for (const auto& bndryNode: bndryNodes) {
                    auto nbrs = code.tannerGraph.getNeighbours(bndryNode);
                    for (auto& nbr: nbrs) {
                        fusionEdges.emplace_back(std::pair(code.tannerGraph.getNodeForId(bndryNode), nbr));
                    }
                }
            }

            // Step 2 Fusion of clusters
            auto eIt = fusionEdges.begin();
            while (eIt != fusionEdges.end()) {
                auto root1 = TreeNode::Find(eIt->first);
                auto root2 = TreeNode::Find(eIt->second);

                //compares vertexIdx only
                if (*root1 == *root2) {
                    fusionEdges.erase(eIt++); // passes eIt to erase
                } else {
                    std::size_t root1Size = root1->clusterSize;
                    std::size_t root2Size = root2->clusterSize;
                    TreeNode::Union(root1, root2);

                    if (root1Size <= root2Size) { // Step 3 fusion of boundary lists
                        for (auto& boundaryVertex: root1->boundaryVertices) {
                            root2->boundaryVertices.insert(boundaryVertex);
                        }
                        root1->boundaryVertices.clear();
                    } else {
                        for (auto& boundaryVertex: root2->boundaryVertices) {
                            root1->boundaryVertices.insert(boundaryVertex);
                        }
                        root2->boundaryVertices.clear();
                    }
                    eIt++;
                }
            }

            // Step 4 Update roots avoiding duplicates
            auto it = components.begin();
            while (it != components.end()) {
                auto elem = *it;
                auto root = TreeNode::Find(elem);
                if (!presentMap.contains(root->vertexIdx)) {
                    components.erase(elem);
                    components.insert(root);
                }
                it++;
            }

            // Step 5 Update Boundary Lists, remove vertices that are not in boundary anymore
            // for each vertex v in boundary list of a component check if there is one neighbour that is not in the component, then it is still in the boundary
            // otherwise, if all its neighbours in the original graph are in the same component its not a boundary vertex anymore and we can remove it from the list
            for (auto& component: components) {
                auto iter = component->boundaryVertices.begin();
                while (iter != component->boundaryVertices.end()) {
                    auto nbrs = code.tannerGraph.getNeighbours(*iter);
                    auto currNode = code.tannerGraph.getNodeForId(*iter);
                    auto currRoot = TreeNode::Find(currNode);
                    auto nbrIt    = nbrs.begin();
                    auto end      = nbrs.end();
                    while (nbrIt != end) {
                        auto nbrRoot = TreeNode::Find(*nbrIt);
                        if (currRoot->vertexIdx != nbrRoot->vertexIdx) {
                            // if we find one neighbour that is not in the same component the currNode is in the boundary
                            iter++;
                            break;
                        }
                        if (nbrIt + 1 == end) {
                            // if we have checked all neighbours and found none in another component remove from boundary list
                            //toRemoveList.emplace_back(*iter);
                            component->boundaryVertices.erase(iter++);
                            break;
                        } else {
                            nbrIt++;
                        }
                    }
                }
            }
            extractValidComponents(components, erasure);
        }
    }
    return erasureDecoder(erasure);
}

std::vector<std::shared_ptr<TreeNode>> Decoder::erasureDecoder(std::vector<std::shared_ptr<TreeNode>>& erasure) {
    // compute set of interior vertices in linear time with DFS
    // each TreeNode is the root of a component
    std::set<std::shared_ptr<TreeNode>> interior;

    for (auto& currCompRoot: erasure) {
        // this will be the root of the new component representing the interior
        std::shared_ptr<TreeNode>             newRoot;
        std::queue<std::shared_ptr<TreeNode>> queue;

        queue.push(currCompRoot);
        if (!currCompRoot->boundaryVertices.contains(currCompRoot->vertexIdx)) {
            // create new node with no dependencies to other components
            std::shared_ptr<TreeNode> tmp;
            tmp->vertexIdx = currCompRoot->vertexIdx;
            // add to interior with union operation
            TreeNode::Union(newRoot, tmp);
        }

        while (!queue.empty()) {
            auto currV = queue.front();
            queue.pop();

            std::vector<std::shared_ptr<TreeNode>> nbrs;
            for (auto& i: currV->children) {
                nbrs.emplace_back(i);
            }
            for (auto& nbr: nbrs) {
                if (!interior.contains(nbr) && !currCompRoot->boundaryVertices.contains(nbr->vertexIdx)) {
                    std::shared_ptr<TreeNode> temp;
                    temp->vertexIdx = nbr->vertexIdx;
                    // add to interior by union operation
                    TreeNode::Union(newRoot, temp);
                    queue.push(nbr);
                }
            }
        }
        interior.insert(newRoot);
    }

    std::vector<std::set<std::shared_ptr<TreeNode>>> resList;
    auto                                             erasureSet = interior;
    for (auto& component: erasureSet) {
        //compute list of vertices in erasure with DFS in linear time

        std::set<std::shared_ptr<TreeNode>> xi;
        // go through nodes in erasure
        // if current node v is a check node remove B(v, 1)
        while (!erasureSet.empty()) {
            auto vertex = *erasureSet.begin();
            if (!vertex->isCheck && !component->boundaryVertices.contains(vertex->vertexIdx)) { // we can assume the node given is the root bc of step 4 above
                xi.insert(vertex);
                auto nbrs = code.tannerGraph.getNeighbours(vertex);
                for (size_t i = 0; i < nbrs.size(); i++) {
                    auto neibrs = code.tannerGraph.getNeighbours(nbrs.at(i));
                    for (size_t j = 0; j < neibrs.size(); j++) {
                        erasureSet.erase(neibrs.at(i));
                    }
                }
                for (auto& nbr: nbrs) {
                    erasureSet.erase(nbr);
                }
            }
        }
        resList.emplace_back(xi);
    }
    std::vector<std::shared_ptr<TreeNode>> result;
    for (auto& i: resList) {
        for (auto& n: i) {
            result.emplace_back(n);
        }
    }
    return result;
}

std::vector<std::size_t> Decoder::peelingDecoder(std::vector<std::shared_ptr<TreeNode>>& erasure, std::set<std::shared_ptr<TreeNode>>& syndrome) {
    std::set<std::size_t> eras;
    for (auto& i: erasure) {
        eras.insert(i->vertexIdx);
    }
    std::set<std::size_t> syndr;
    for (const auto& s: syndrome) {
        syndr.insert(s->vertexIdx);
    }

    std::vector<std::size_t> result;

    // compute SF
    std::vector<std::set<std::pair<std::size_t, std::size_t>>> spanningForest;
    std::vector<std::set<std::size_t>>                         forestNodes;
    std::set<std::size_t>                                      visited;
    std::size_t                                                idx          = 0;
    bool                                                       currNodeFlag = true;
    for (auto& currCompRoot: eras) {
        std::queue<std::size_t> queue;
        queue.push(currCompRoot);

        while (!queue.empty()) {
            auto currV = queue.front();
            queue.pop();
            auto nbrs = code.tannerGraph.getNeighboursIdx(currV);
            for (auto& nbr: nbrs) {
                if (!visited.contains(nbr)) {
                    visited.insert(nbr);
                    queue.push(nbr);
                    spanningForest.at(idx).insert(std::make_pair(currV, nbr));
                    if (currNodeFlag) { // avoid adding currV multiple times, maybe theres a more elegant way to do this
                        forestNodes.at(idx).emplace(nbr);
                        currNodeFlag = false;
                    }
                }
            }
            currNodeFlag = true;
        }
        idx++;
    }
    std::size_t                                                idxx = 0;
    std::vector<std::set<std::pair<std::size_t, std::size_t>>> pendantEdges;

    // compute pendant vertices of SF
    for (const auto& tree: spanningForest) {
        for (const auto& [v, w]: tree) {
            if (!forestNodes.at(idxx).contains(v)) {
                pendantEdges.at(0).insert(std::make_pair(v, w));
            } else if (!forestNodes.at(idxx).contains(w)) {
                pendantEdges.at(0).insert(std::make_pair(w, v));
            }
        }
        idxx++;
    }

    // peeling
    for (std::set<std::pair<std::size_t, std::size_t>>& tree: spanningForest) {
        while (!tree.empty()) {
            for (auto& [u, w]: tree) {
                tree.erase(std::make_pair(u, w));
                // pendant vertex is always left one
                if (syndr.contains(u)) {
                    // R1
                    result.emplace_back(w);
                    for (auto const& n: code.tannerGraph.getNeighboursIdx(w)) {
                        if (tree.contains(std::make_pair(w, n)) || tree.contains(std::make_pair(n, w))) {
                            if (syndr.contains(n)) {
                                syndr.erase(n);
                            } else {
                                syndr.insert(n);
                            }
                        }
                    }
                } // R2: else do nothing
            }
        }
    }

    return result;
}

void Decoder::extractValidComponents(std::set<std::shared_ptr<TreeNode>>& components, std::vector<std::shared_ptr<TreeNode>>& erasure) {
    auto it = components.begin();
    while (it != components.end()) {
        if (isValidComponent(*it)) {
            erasure.emplace_back(*it);
            components.erase(it++);
        } else{
            it++;
        }
    }
}
bool Decoder::isValidComponent(const std::shared_ptr<TreeNode>& component) {
    std::vector<bool> valid(component->clusterSize);
    std::size_t       i = 0;
    for (const auto& checkVertex: component->checkVertices) {
        auto nbrs = code.tannerGraph.getNeighbours(checkVertex);

        for (auto& nbr: nbrs) {
            if (std::find(component->boundaryVertices.begin(), component->boundaryVertices.end(), nbr->vertexIdx) == component->boundaryVertices.end()) {
                valid.at(i++) = true;
                break;
            }
        }
    }
    return std::all_of(valid.begin(), valid.end(), [](bool i) { return !i; });
}
