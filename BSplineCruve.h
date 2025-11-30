#ifndef BSPLINECRUVE_H
#define BSPLINECRUVE_H
#include <vector>
#include "glm/glm.hpp"
class BSplineCurve
{
public:
    int degree = 3; // 2 = quadratic, 3 = cubic
    std::vector<float> t; // knot vector, length of t is n + d + 2
    std::vector<glm::vec3> controlPoints;

private:
    float findKnotInterval(float x)
    {
        int my = controlPoints.size() - 1; // Index to last controlpoint

        while (x < t[my])
            my--;

        return my;
    }

public:
    glm::vec3 EvaluateBSpline(float x)
    {
        int my = findKnotInterval(x);
        std::vector<glm::vec3> a; // next point on the curve

        a.resize(degree+1);

        for (int i = 0; i <= degree; i++)
        {
            a[degree-i] = controlPoints[my-i];
        }

        for (int k = degree; k > 0; k--)
        {
            int j = my - k;

            for (int i = 0; i < k; i++)
            {
                j++;

                float w = (x - t[j])/(t[j+k] - t[j]);

                a[i] = a[i] * (1-w) + a[i+1] * w;
            }
        }

        return a[0];
    }
};



#endif // BSPLINECRUVE_H
