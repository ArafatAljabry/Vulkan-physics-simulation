#include "TriangleSurface.h"
#include <fstream>
#include <QDebug>
TriangleSurface::TriangleSurface() : VisualObject()
{
    Vertex v1{-2.0f,  0.5f,  -2.0f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f};  //  bottom-left corner
    Vertex v2{ 2.0f,  0.5f,  -2.0f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f }; //  bottom-right corner
    Vertex v3{ -2.0f, 0.0f,  0.0f,    0.0f, 1.0f, 0.0f,  0.0f, 1.0f }; //  top-left corner
    Vertex v4{ 2.0f,  0.0f,  0.0f,    0.0f, 1.0f, 0.0f,  1.0f, 1.0f }; //  top-right corner
    Vertex v5{-2.0f,  0.5f,  2.0f,    0.0f, 0.0f, 1.0f,  0.0f, 0.0f};
    Vertex v6{2.0f,   0.5,   2.0f,    0.0f, 0.0f, 0.0f,  0.0f, 0.0f};


    //Pushing 1st triangle,
    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);
    mVertices.push_back(v4);
    mVertices.push_back(v5);
    mVertices.push_back(v6);

    //Indexes for the two triangles to form a quad
    mIndices.push_back(0);
    mIndices.push_back(1);
    mIndices.push_back(2);
    mIndices.push_back(1);
    mIndices.push_back(3);
    mIndices.push_back(2);

    mIndices.push_back(2);
    mIndices.push_back(3);
    mIndices.push_back(4);
    mIndices.push_back(3);
    mIndices.push_back(5);
    mIndices.push_back(4);

    //Temporary scale and positioning
    //mMatrix.scale(0.5f);
    //mMatrix.translate(0.5f, 0.1f, 0.1f);
}

TriangleSurface::TriangleSurface(const std::string &filename)
{
    std::ifstream inn(filename);
    if (!inn.is_open())
        return;

    int n;
    float x,y,z,r,g,b,u,v;
    inn >> n;
    std::vector<Vertex> tempVertices;

    for (auto i=0; i<n; i++)
    {
        inn >> x >> y  >> z  >> r  >> g  >> b >> u  >> v;
        mVertices.push_back(Vertex(x,y,z,r,g,b,u,v));
        qDebug() << x << y << z << u << v << r << g << b;
    }
    inn.close();
}
