#pragma once

#include "Vertex.h"

class Triangle
{
public:
    Vertex* Vertices[3];

    Triangle()
    {
    }

    ~Triangle()
    {
        for (auto& v : this->Vertices)
        {
            if (v)
            {
                delete v;
                v = nullptr;
            }
        }
    }
};