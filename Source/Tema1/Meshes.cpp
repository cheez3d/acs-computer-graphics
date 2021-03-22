#include "Scene2D.h"
#include "Transform2D.h"

#include <Core/Engine.h>

#include <numeric>

using BowAndArrow::Scene2D;

void Scene2D::InitMeshes() {
    using namespace Transform2D;

    {
        std::string name = "WhiteLine";
        vec3 color { 1 };

        std::vector<VertexFormat> vertexes {
            { { 0,     0, 0 }, color },
            { { 0.02f, 0, 0 }, color },
            { { 0.02f, 1, 0 }, color },
            { { 0,     1, 0 }, color },
        };

        std::vector<unsigned short> indexes { 0, 1, 2, 3, 0, 2 };

        Mesh* mesh = new Mesh(name);
        mesh->InitFromData(vertexes, indexes);

        AddMeshToList(mesh);
    }

    {
        std::string name = "BrownArc";
        vec3 color { 82.0f / 255, 48.0f / 255 , 0 };

        std::vector<VertexFormat> vertexes {};

        for (float f = -90; f <= 90; f += 1) {
            float phi = radians(f);

            vec3 v1 { 0.5f * cos(phi), sin(phi), 1 };
            vec3 v2 = Scale(1.15f) * v1;

            vertexes.push_back({ { v1.xy, 0 }, color });
            vertexes.push_back({ { v2.xy, 0 }, color });
        }

        std::vector<unsigned short> indexes(vertexes.size());
        std::iota(indexes.begin(), indexes.end(), 0);

        Mesh* mesh = new Mesh(name);
        mesh->InitFromData(vertexes, indexes);
        mesh->SetDrawMode(GL_TRIANGLE_STRIP);

        AddMeshToList(mesh);
    }

    {
        std::string name = "DarkGrayPointer";
        vec3 color(66.0f / 255);

        std::vector<VertexFormat> vertexes {
            { {  0,      0,    0 }, color },
            { { -0.1f,  -0.1f, 0 }, color },
            { {  0.3f,   0,    0 }, color },
            { { -0.1f,   0.1f, 0 }, color },
        };

        std::vector<unsigned short> indexes { 0, 1, 2, 0, 2, 3 };

        Mesh* mesh = new Mesh(name);
        mesh->InitFromData(vertexes, indexes);

        AddMeshToList(mesh);
    }

    for (auto&& [name, color] : {
        std::pair { "OrangeLine", vec3 { 212.0f / 255, 134.0f / 255, 0 } },
        std::pair { "DarkRedLine", vec3 { 184.0f / 255, 58.0f / 255, 0 } },
     }) {
        std::vector<VertexFormat> vertexes {
            { { 0, -0.025f, 0 }, color },
            { { 1, -0.025f, 0 }, color },
            { { 1,  0.025f, 0 }, color },
            { { 0,  0.025f, 0 }, color },
        };

        std::vector<unsigned short> indexes { 0, 1, 2, 3, 0, 2 };

        Mesh* mesh = new Mesh(name);
        mesh->InitFromData(vertexes, indexes);

        AddMeshToList(mesh);
    }

    {
        std::string name = "WhiteArc";
        vec3 color { 1, 1, 1 };

        std::vector<VertexFormat> vertexes {};

        for (float f = 30; f <= 60; f += 1) {
            float phi = radians(f);

            vec3 v1 { 0.8f * cos(phi), 0.8f * sin(phi), 1 };
            vec3 v2 = Scale(1.15f) * v1;

            vertexes.push_back({ { v1.xy, 0 }, color });
            vertexes.push_back({ { v2.xy, 0 }, color });
        }

        std::vector<unsigned short> indexes(vertexes.size());
        std::iota(indexes.begin(), indexes.end(), 0);

        Mesh* mesh = new Mesh(name);
        mesh->InitFromData(vertexes, indexes);
        mesh->SetDrawMode(GL_TRIANGLE_STRIP);

        AddMeshToList(mesh);
    }

    for (auto&& [name, color] : {
        std::pair { "RedCircle", vec3 { 1, 10.0f / 255, 34.0f / 255 } },
        std::pair { "YellowCircle", vec3 { 1, 200.0f / 255, 0 } },
    }) {
        std::vector<VertexFormat> vertexes {};

        vertexes.push_back({ { 0, 0, 0 } , color });

        for (float f = 0; f < 360; f += 1) {
            float phi = radians(f);

            vertexes.push_back({ { cos(phi), sin(phi), 0 }, color });
        }

        vertexes.push_back(vertexes[1]);

        std::vector<unsigned short> indexes(vertexes.size());
        std::iota(indexes.begin(), indexes.end(), 0);

        Mesh* mesh = new Mesh(name);
        mesh->InitFromData(vertexes, indexes);
        mesh->SetDrawMode(GL_TRIANGLE_FAN);

        AddMeshToList(mesh);
    }

    for (auto&& [name, color] : {
        std::pair { "RedTriangle", vec3 { 1, 10.0f / 255, 34.0f / 255 } },
        std::pair { "YellowTriangle", vec3 { 1, 200.0f / 255, 0 } },
    }) {
        std::vector<VertexFormat> vertexes {
            { {  0,     0,           0 }, color },
            { { -0.5f, -sqrt(3) / 2, 0 }, color },
            { {  0.5f, -sqrt(3) / 2, 0 }, color },
        };

        std::vector<unsigned short> indexes { 0, 1, 2 };

        Mesh* mesh = new Mesh(name);
        mesh->InitFromData(vertexes, indexes);

        AddMeshToList(mesh);
    }

    {
        std::string name = "DarkGrayTriangle";
        vec3 color { 66.0f / 255 };

        std::vector<VertexFormat> vertexes {
            { { 0, 0, 0 }, color },
            { { 1, 0, 0 }, color },
            { { 0, 1, 0 }, color },
        };

        std::vector<unsigned short> indexes { 0, 1, 2 };

        Mesh* mesh = new Mesh(name);
        mesh->InitFromData(vertexes, indexes);

        AddMeshToList(mesh);
    }

    for (auto&& [name, color] : {
        std::pair { "DarkGraySquare", vec3 { 66.0f / 255 } },
        std::pair { "BlackSquare", vec3 { 0 } },
    }) {
        std::vector<VertexFormat> vertexes {
            { { 0, 0, 0 }, color },
            { { 1, 0, 0 }, color },
            { { 1, 1, 0 }, color },
            { { 0, 1, 0 }, color },
        };

        std::vector<unsigned short> indexes { 0, 1, 2, 0, 2, 3 };

        Mesh* mesh = new Mesh(name);
        mesh->InitFromData(vertexes, indexes);

        AddMeshToList(mesh);
    }

    {
        std::string name = "RosaHeart";
        vec3 color { 1, 122.0f / 255, 246.0f / 255 };

        mat3 t = Translate({ 0, 1 }) * Scale(0.5f);

        std::vector<VertexFormat> vertexes {};

        vertexes.push_back({ { (t * vec3 { 0, 0, 1 }).xy, 0 }, color });

        for (float f = 0; f < 360; f += 1) {
            float phi = radians(f);
            float r = (sin(phi) * sqrt(abs(cos(phi)))) / (sin(phi) + 7.0f / 5) - 2 * sin(phi) + 2;

            vertexes.push_back({ { (t * vec3 { r * cos(phi), r * sin(phi), 1 }).xy, 0 }, color });
        }

        vertexes.push_back(vertexes[1]);

        std::vector<unsigned short> indexes(vertexes.size());
        std::iota(indexes.begin(), indexes.end(), 0);

        Mesh* mesh = new Mesh(name);
        mesh->InitFromData(vertexes, indexes);
        mesh->SetDrawMode(GL_TRIANGLE_FAN);

        AddMeshToList(mesh);
    }
}
