/**
 *
 * @author Philipp Stiegernigg
 */

#pragma once

#include <string>
#include <memory>
#include <map>

#include "geometry/Mesh.h"
#include "materials/Material.h"
#include "geometry/Instance.h"

class MeshLoader {
public:
    MeshLoader();
    virtual ~MeshLoader();
    bool loadFromFile(const std::string&);
    
    /**
     * Returns an Instance object.
     * @return nullptr If no instances are left
     */
    std::unique_ptr<Instance> getNextInstance();
private:

    std::vector<std::unique_ptr<Instance>> mInstances;
};
