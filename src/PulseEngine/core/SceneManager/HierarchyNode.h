#pragma once

#include <vector>
#include <functional>

template<typename T>
struct HierarchyNode
{
    T* item = nullptr;
    HierarchyNode<T>* parent = nullptr;
    std::vector<HierarchyNode<T>*> children;

    HierarchyNode() : item(nullptr) {}
    explicit HierarchyNode(T* obj) : item(obj) {}

    ~HierarchyNode()
    {
        for (auto child : children)
            delete child;
        children.clear();
    }

    void AddChild(HierarchyNode<T>* child)
    {
        children.push_back(child);
    }

    void ForEach(const std::function<void(T*)>& func)
    {
        if (item) func(item);
        for (auto child : children)
            child->ForEach(func);
    }

    HierarchyNode<T>* Find(const std::function<bool(T*)>& predicate)
    {
        if (item && predicate(item)) return this;
        for (auto child : children)
        {
            HierarchyNode<T>* found = child->Find(predicate);
            if (found) return found;
        }
        return nullptr;
    }
        HierarchyNode<T>* Find(T* target)
    {
        if(item == target) return this;
        for(auto& child : children)
        {
            if(auto* found = child->Find(target))
                return found;
        }
        return nullptr;
    }
};