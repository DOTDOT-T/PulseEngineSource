#ifndef NODEPULSESYNAPSE_H
#define NODEPULSESYNAPSE_H

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>

struct NodeEntry
{
    std::string name;
    std::function<void()> onClick; // action when clicked
};

struct NodeCategory
{
    std::string name;
    std::vector<NodeEntry> nodes;
    std::vector<NodeCategory> subCategories;
};



#endif