#include "VisualObject.h"

VisualObject::VisualObject()
{
    mMatrix.setToIdentity();
}

void VisualObject::move(float x, float y, float z)
{
    mMatrix.translate(x, y, z);
}

void VisualObject::scale(float s)
{
    mMatrix.scale(s);
}

void VisualObject::rotate(float t, float x, float y, float z)
{
    mMatrix.rotate(t, x, y, z);
}

QVector3D VisualObject::getPosition()
{
    float x = mMatrix(0, 3); // Position in the x-axis
    float y = mMatrix(1, 3); // Position in the y-axis
    float z = mMatrix(2, 3); // Position in the z-axis
    return QVector3D(x, y, z);
}

void VisualObject::setPosition(float x, float y, float z)
{
    mMatrix(0, 3) = x; // Position in the x-axis
    mMatrix(1, 3) = y; // Position in the y-axis
    mMatrix(2, 3) = z; // Position in the z-axis
}
void VisualObject::setPosition(QVector3D newPosition)
{
    mMatrix(0, 3) = newPosition.x(); // Position in the x-axis
    mMatrix(1, 3) = newPosition.y(); // Position in the y-axis
    mMatrix(2, 3) = newPosition.z(); // Position in the z-axis
}

TerrainInfo VisualObject::barysentricCoordFromTerrain(const QVector3D& position)
{
    QVector2D p(position.x(), position.z());

    for (int i = 0; i < mIndices.size() - 2; i += 3)
    {
        const Vertex& v0 = mVertices[mIndices[i]];
        const Vertex& v1 = mVertices[mIndices[i + 1]];
        const Vertex& v2 = mVertices[mIndices[i + 2]];

        QVector2D a(v0.x, v0.z);
        QVector2D b(v1.x, v1.z);
        QVector2D c(v2.x, v2.z);

        // Beregn areal av trekant ABC
        float areaABC = (b - a).x() * (c - a).y() - (b - a).y() * (c - a).x();

        // Unngå deling på null
        if (fabs(areaABC) < 1e-6f) continue;

        // Beregn barysentriske koordinater
        float lambda1 = ((b - p).x() * (c - p).y() - (b - p).y() * (c - p).x()) / areaABC;
        float lambda2 = ((c - p).x() * (a - p).y() - (c - p).y() * (a - p).x()) / areaABC;
        float lambda3 = 1.0f - lambda1 - lambda2;

        // Sjekk om punktet ligger i trekanten
        if (lambda1 >= 0.0f && lambda2 >= 0.0f && lambda3 >= 0.0f)
        {
            // Interpoler høyde (Y)
            float height = lambda1 * v0.y + lambda2 * v1.y + lambda3 * v2.y;

            // Beregn geometrisk normal fra 3D-punktene
            QVector3D pointA(v0.x, v0.y, v0.z);
            QVector3D pointB(v1.x, v1.y, v1.z);
            QVector3D pointC(v2.x, v2.y, v2.z);
            QVector3D normal = QVector3D::crossProduct(pointB - pointA, pointC - pointA).normalized();

            return TerrainInfo{height, normal};
        }
    }

    // Returner standardverdi hvis punktet ikke ligger i noen trekant
    return TerrainInfo{0.0f, QVector3D(0.0f, 1.0f, 0.0f)};
}
