#pragma once

#include <string>
#include <map>

#include "Shader.h"
#include "MocapFileDefinitions.h"
#include "Sphere.h"
#include <ft2build.h>
#include FT_FREETYPE_H

struct MocapFrame;
class MocapNode;
class Camera;

struct RendererInitialisationData {
    int screenWidth;
    int screenHeight;
    std::string font;
};

class Renderer
{
public:
    Renderer();
    Renderer(const RendererInitialisationData& initData);
    // Inherited via IRenderer
    void DrawMocapFrame(const MocapFrame& frame, const Camera& cam);
    void DrawSphere(const glm::vec3& centeredAt, const glm::vec3& dimensions, const Camera& camera, const glm::vec4& colour) const;
    void DrawTextBillboard(std::string text, const glm::vec3& textColour, const glm::vec3& woldPos, const float scale, const Camera& camera);

    void SetLightPos(const glm::vec3& value);
    void SetLightColour(const glm::vec3& value);
    void SetScreenDims(const glm::ivec2& value);
    void LoadFont(std::string ttfFilePath);
private:
    void DrawLines(const MocapFrame& frame, const Camera& cam, const glm::vec4& colour);
    void Initialize();
    void InitializeSphereVertices();
    void InitializeLineVertices();
    void SetLinesVertexBuffer(const MocapFrame& frame);
    void InitFT();
private:
    /// Holds all state information relevant to a character as loaded using FreeType
    struct Character {
        unsigned int TextureID; // ID handle of the glyph texture
        glm::ivec2   Size;      // Size of glyph
        glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Horizontal offset to advance to next glyph
    };
private:
    Sphere m_sphere = Sphere(1.0f); // class stolen from http://www.songho.ca/opengl/gl_sphere.html

    unsigned int m_unitSphereEBO;
    unsigned int m_unitSphereVAO;

    unsigned int m_linesVAO;
    unsigned int m_linesVBO;

    unsigned int m_freeTypeVAO;
    unsigned int m_freeTypeVBO;

    Shader m_colouredShader;
    Shader m_lineShader;
    Shader m_billboardShader;

    glm::vec3 m_lightPos;
    glm::vec3 m_lightColour;
    unsigned int m_scrWidth = 800;
    unsigned int m_scrHeight = 1200;
    glm::vec3 m_linesCpuBuffer[PlayerPoints * 2];

    FT_Library m_ft;
    Character m_characters[256];
    const unsigned int m_baseTextSize = 40;
};

