#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <limits>

#include <random>
#include <unordered_map>
#include <chrono>

#include <cmath>

#include "Utils.hpp"
#include "PixelGrid.hpp"
#include "Screen.hpp"
#include "Mat4.hpp"
#include "Vec4.hpp"

void parse(std::string fileName, Screen &screen, Mat4 &edges, Mat4 &transform, Color color)
{
    std::ifstream infile(fileName);

    std::string line;

    while (std::getline(infile, line))
    {
        std::cout << "command: " << line << std::endl;
        if (line == "line")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            Vec4 v0, v1;
            iss >> v0 >> v1;
            edges.addEdge(v0, v1);
        }
        else if (line == "ident")
        {
            transform = Mat4::identity();
        }
        else if (line == "scale")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            Vec4 v;
            iss >> v;
            Mat4 const &temp = Mat4::scale(v);
            transform.multiplyMutate(temp);
        }
        else if (line == "move" || line == "translate")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            Vec4 v;
            iss >> v;
            Mat4 const &temp = Mat4::translate(v);
            transform.multiplyMutate(temp);
        }
        else if (line == "rotate")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            char axis;
            double angle;
            iss >> axis >> angle;

            double PI = std::atan(1) * 4;

            angle *= PI / 180;
            Mat4 temp(0);
            temp.identity();
            if (axis == 'x')
            {
                temp = Mat4::rotX(angle);
            }
            else if (axis == 'y')
            {
                temp = Mat4::rotY(angle);
            }
            else if (axis == 'z')
            {
                temp = Mat4::rotZ(angle);
            }
            transform.multiplyMutate(temp);
        }
        else if (line == "apply")
        {
            edges.multiplyMutate(transform);
        }
        else if (line == "display")
        {
            screen.clear({0, 0, 0, 255});
            screen.drawEdges(edges, color);
            screen.display();
        }
        else if (line == "save")
        {
            std::getline(infile, line);
            std::cout << "file name " << line << std::endl;
            screen.clear({0, 0, 0, 255});
            screen.drawEdges(edges, color);
            screen.toFileExtension(line);
        }
        else if (line == "circle")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            Vec4 v;
            double r;
            iss >> v >> r;
            edges.addCircle(v, r, 100);
        }
        else if (line == "hermite")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            double x0, y0, x1, y1, rx0, ry0, rx1, ry1;
            iss >> x0 >> y0 >> x1 >> y1 >> rx0 >> ry0 >> rx1 >> ry1;
            edges.addCurve(x0, y0, x1, y1, rx0, ry0, rx1, ry1, 20, CurveType::Hermite);
        }
        else if (line == "bezier")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            double x0, y0, x1, y1, x2, y2, x3, y3;
            iss >> x0 >> y0 >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
            edges.addCurve(x0, y0, x1, y1, x2, y2, x3, y3, 20, CurveType::Bezier);
            //edges.addCurve(x0, y0, x3, y3, 3*(x1-x0), 3*(y1-y0), 3*(x3-x2), 3*(y3-y2), 50, CurveType::Hermite);
        }
        else
        {
            std::cout << "Unrecognized command!" << std::endl;
        }
    }
}

// // https://www.sfu.ca/~rpyke/335/projects/tsai/report1.htm

// void addRect(Mat4 &edges, Vec4 const &p0, Vec4 const &p1, Vec4 const &p2, Vec4 const &p3)
// {
//     edges.addEdge(p0, p1);
//     edges.addEdge(p1, p2);
//     edges.addEdge(p2, p3);
//     edges.addEdge(p3, p0);
// }

// std::unordered_map<Vec4, double> points;

// void recurse(Mat4 &edges, Vec4 const &p0, Vec4 const &p1, Vec4 const &p2, Vec4 const &p3, int k, double roughness, std::mt19937 &gen, std::uniform_real_distribution<> &dis, int startK, double scale)
// {
//     if (k == 0)
//     {
//         addRect(edges, p0, p1, p2, p3);
//     }
//     else
//     {
//         Vec4 pCenter = (p0 + p1 + p2 + p3) / 4;
//         pCenter = pCenter.round(1000000.0);

//         if (points.count(pCenter) == 0)
//         {
//             double displacement = 1 / std::pow(1.5, (float)(startK - k)) * dis(gen) * scale;
//             displacement = std::max(displacement, 0.0);
//             points.insert({pCenter, displacement});
//             pCenter.set(1, pCenter[1] + displacement);
//         }
//         else
//         {
//             pCenter.set(1, pCenter[1] + points[pCenter]);
//         }

//         Vec4 p01 = (p0 + p1) / 2;
//         p01 = p01.round(1000000.0);

//         if (points.count(p01) == 0)
//         {
//             double displacement = 1 / std::pow(1.5, (float)(startK - k)) * dis(gen) * scale;
//             displacement = std::max(displacement, 0.0);
//             points.insert({p01, displacement});
//             p01.set(1, p01[1] + displacement);
//         }
//         else
//         {
//             p01.set(1, p01[1] + points[p01]);
//         }

//         Vec4 p12 = (p1 + p2) / 2;
//         p12 = p12.round(1000000.0);

//         if (points.count(p12) == 0)
//         {
//             double displacement = 1 / std::pow(1.5, (float)(startK - k)) * dis(gen) * scale;
//             displacement = std::max(displacement, 0.0);
//             points.insert({p12, displacement});
//             p12.set(1, p12[1] + displacement);
//         }
//         else
//         {
//             p12.set(1, p12[1] + points[p12]);
//         }

//         Vec4 p23 = (p2 + p3) / 2;
//         p23 = p23.round(1000000.0);

//         if (points.count(p23) == 0)
//         {
//             double displacement = 1 / std::pow(1.5, (float)(startK - k)) * dis(gen) * scale;
//             displacement = std::max(displacement, 0.0);
//             points.insert({p23, displacement});
//             p23.set(1, p23[1] + displacement);
//         }
//         else
//         {
//             p23.set(1, p23[1] + points[p23]);
//         }

//         Vec4 p30 = (p3 + p0) / 2;
//         p30 = p30.round(1000000.0);

//         if (points.count(p30) == 0)
//         {
//             double displacement = 1 / std::pow(1.5, (float)(startK - k)) * dis(gen) * scale;
//             displacement = std::max(displacement, 0.0);
//             points.insert({p30, displacement});
//             p30.set(1, p30[1] + displacement);
//         }
//         else
//         {
//             p30.set(1, p30[1] + points[p30]);
//         }

//         recurse(edges, p0, p01, pCenter, p30, k - 1, roughness, gen, dis, startK, scale);
//         recurse(edges, p1, p12, pCenter, p01, k - 1, roughness, gen, dis, startK, scale);
//         recurse(edges, p2, p23, pCenter, p12, k - 1, roughness, gen, dis, startK, scale);
//         recurse(edges, p3, p30, pCenter, p23, k - 1, roughness, gen, dis, startK, scale);
//     }
// }

int main()
{
    Screen screen(500, 500);
    Mat4 edges(0);
    Mat4 transform = Mat4::identity();

    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_real_distribution<> dis(-1, 1);

    // int k = 5;

    // for (int i = 0; i < 60; i++)
    // {
    //     screen.clear({0, 0, 0, 255});
    //     double angle = (i / 60.0) * (2 * M_PI);
    //     recurse(edges, {-200, 0, -200}, {200, 0, -200}, {200, 0, 200}, {-200, 0, 200}, k, 0, gen, dis, k, 100);
    //     edges.multiplyMutate(Mat4::rotY(angle));
    //     edges.multiplyMutate(Mat4::rotX(-M_PI / 7));
    //     //edges.multiplyMutate(Mat4::rotX(-M_PI));
    //     edges.multiplyMutate(Mat4::translate({250, 250}));
    //     screen.drawEdges(edges, {255, 255, 255, 255});
    //     edges.clear();

    //     std::string s = std::to_string(i);
    //     s.insert(s.begin(), 3 - s.size(), '0');

    //     screen.toFileExtension("img" + s + ".png");
    // }

    parse("script", screen, edges, transform, {255, 127, 127, 255});

    return 0;
}
