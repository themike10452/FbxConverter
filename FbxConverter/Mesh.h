#pragma once

#include "Triangle.h"
#include <vector>

class Mesh
{
public:
    std::vector<Triangle*> Triangles;

    Mesh()
    {
    }

    ~Mesh()
    {
        for (auto& t : this->Triangles)
        {
            if (t)
            {
                delete t;
                t = nullptr;
            }
        }

        this->Triangles.clear();
    }
};
