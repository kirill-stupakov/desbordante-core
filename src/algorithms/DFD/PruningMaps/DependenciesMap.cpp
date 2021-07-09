//
// Created by alexandrsmirn
//

#include "DependenciesMap.h"

DependenciesMap::DependenciesMap(RelationalSchema const* schema) {
    for (auto const& column : schema->getColumns()) {
        this->insert(std::make_pair(Vertical(*column), vertical_set()));
    }
}

std::unordered_set<Vertical> DependenciesMap::getPrunedSubsets(std::unordered_set<Vertical> const& subsets) const {
    vertical_set prunedSubsets;
    for (auto const& node : subsets) {
        if (canBePruned(node)) {
            prunedSubsets.insert(node);
        }
    }
    return prunedSubsets;
}

void DependenciesMap::addNewDependency(Vertical const& nodeToAdd) { //пока версия без балансировок
    for (auto& mapRow : *this) {
        Vertical const& key = mapRow.first;

        if (nodeToAdd.contains(key)) {
            std::unordered_set<Vertical>& depsForKey = mapRow.second;
            bool hasSubsetEntry = false;

            for (auto iter = depsForKey.begin(); iter != depsForKey.end(); ) {
                //если совпадают, то contains = true
                Vertical const& dep = *iter;
                if (nodeToAdd.contains(dep)) {
                    hasSubsetEntry = true;
                    break;
                } else if (dep.contains(nodeToAdd)) {
                    iter = depsForKey.erase(iter);
                } else {
                    iter++;
                }
            }

            if (!hasSubsetEntry) {
                depsForKey.insert(nodeToAdd);
            }
        }
    }
    //rebalance();
}

bool DependenciesMap::canBePruned(Vertical const& node) const {
    for (auto const& mapRow : *this) {
        Vertical const& key = mapRow.first;
        if (node.contains(key)) {
            for (Vertical const& dependency : mapRow.second) {
                if (node.contains(dependency)) {
                    return true;
                }
            }
        }
    }
}
