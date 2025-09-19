#ifndef GUIDCOLLECTION_H
#define GUIDCOLLECTION_H

#include "Common/common.h"
#include "Common/dllExport.h"

class PULSE_ENGINE_DLL_API GuidCollection
{
    private:
        std::string collectionName;
        std::unordered_map<std::string, std::string> files; // map<guid, filepath>

    public:
        GuidCollection(const std::string& collectionPath);
        ~GuidCollection() {}
        std::string GetCollectionName() const { return collectionName; }
        std::unordered_map<std::string, std::string> GetFiles() const { return files; }
        
        std::size_t InsertFile(const std::string& filePath);
        std::string GetFilePathFromGuid(const std::string& guid) const;


};

#endif