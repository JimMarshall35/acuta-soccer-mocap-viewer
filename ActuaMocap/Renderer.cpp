#include "Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_transform.hpp>
#include "Camera.h"
#include <string.h>
#include "MocapFrame.h"

#define DRAW_DISTANCE 10000.0f
#define NUM_CHANNELS 4


#pragma region colour shader

std::string colourVertGlsl =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"

"out vec3 FragPos;\n"
"out vec3 Normal;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"

"void main()\n"
"{\n"
"    FragPos = vec3(model * vec4(aPos, 1.0));\n"
"    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"}\n";

std::string colourFragGlsl =
"#version 330 core\n"
"out vec4 FragColor;\n"

"in vec3 Normal;\n"
"in vec3 FragPos;\n"

"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightColor;\n"
"uniform vec4 objectColor;\n"

"void main()\n"
"{\n"
"    // ambient\n"
"    float ambientStrength = 0.1;\n"
"    vec3 ambient = ambientStrength * lightColor;\n"
"    // diffuse\n"
"    vec3 norm = normalize(Normal);\n"
"    vec3 lightDir = normalize(lightPos - FragPos);\n"
"    float diff = max(dot(norm, lightDir), 0.0);\n"
"    vec3 diffuse = diff * lightColor;\n"
"    // specular\n"
"    float specularStrength = 0.5;\n"
"    vec3 viewDir = normalize(viewPos - FragPos);\n"
"    vec3 reflectDir = reflect(-lightDir, norm);\n"
"    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
"    vec3 specular = specularStrength * spec * lightColor;\n"

"    vec3 result = (ambient + diffuse + specular) * objectColor.xyz;\n"
"    FragColor = vec4(result, objectColor[3]);\n"
"}\n";

#pragma endregion

#pragma region line shader

std::string lineVertGlsl =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"

"out vec3 FragPos;\n"

"uniform mat4 view;\n"
"uniform mat4 projection;\n"

"void main()\n"
"{\n"
"    FragPos = vec3(vec4(aPos, 1.0));\n"
"    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"}\n";

std::string lineFragGlsl =
"#version 330 core\n"
"out vec4 FragColor;\n"

"uniform vec4 objectColor;\n"

"void main()\n"
"{\n"
"    FragColor = objectColor;\n"
"}\n";

#pragma endregion

#pragma region BillBoardShader

std::string billboardVertGlsl =
"#version 330 core\n"
"layout(location = 0) in vec4 vertex;"

"out vec2 TexCoords;\n"

"uniform vec3 CameraRight_worldspace;\n"
"uniform vec3 CameraUp_worldspace;\n"
"uniform vec3 particleCenter_wordspace;\n"
"uniform vec2 BillboardSize;\n"
"uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)\n"

"void main()\n"
"{\n"
    "vec4 vertexPosWorldSpace =\n"
    "vec4(particleCenter_wordspace\n"
    "+ CameraRight_worldspace * vertex.x * BillboardSize.x\n"
    "+ CameraUp_worldspace * vertex.y * BillboardSize.y, 1.0);\n"
    "gl_Position = VP * vertexPosWorldSpace;"
    "TexCoords = vertex.zw;\n"
"}\n"
;

std::string billBoardFragGlsl =
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"

"uniform sampler2D text;\n"
"uniform vec3 textColor;\n"

"void main()\n"
"{\n"
//"   if(texture(text,TexCoords).r < 3) discard;\n"
"   color = vec4(textColor.x, textColor.y, textColor.z, texture(text, TexCoords).r);\n"
"}\n"
;

#pragma endregion


Renderer::Renderer()
{
    Initialize();
}

#define use_CRT_SECURE_NO_WARNINGS 1
Renderer::Renderer(const RendererInitialisationData& initData)
    :m_scrWidth(initData.screenWidth), m_scrHeight(initData.screenHeight)
{
    Initialize();
    LoadFont(initData.font);
}


/// <summary>
/// should be called by screen sized change callback
/// </summary>
/// <param name="value"></param>
void Renderer::SetScreenDims(const glm::ivec2& value)
{
    m_scrWidth = value.x;
    m_scrHeight = value.y;
}

struct TexturedVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};


/// <summary>
/// get ready to render!
/// </summary>
void Renderer::Initialize()
{
    memset(m_linesCpuBuffer, 0, sizeof(m_linesCpuBuffer));

    InitFT();

    InitializeSphereVertices();
    InitializeLineVertices();

    // load all shaders
    m_colouredShader.LoadFromString(colourVertGlsl, colourFragGlsl);
    m_lineShader.LoadFromString(lineVertGlsl, lineFragGlsl);
    m_billboardShader.LoadFromString(billboardVertGlsl, billBoardFragGlsl);

    // load fonts

    // initialise the projection matrix for the ui
    SetScreenDims({ m_scrWidth, m_scrHeight });

}

/// <summary>
/// Load the sphere vertices into openGL memory
/// and set m_unitSphereVAO in the process.
/// </summary>
void Renderer::InitializeSphereVertices()
{
    unsigned int VBO;
    glGenVertexArrays(1, &m_unitSphereVAO);
    glGenBuffers(1, &VBO);

    glGenBuffers(1, &m_unitSphereEBO);


    glBindVertexArray(m_unitSphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_sphere.getInterleavedVertexSize(), m_sphere.getInterleavedVertices(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_unitSphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_sphere.getIndexSize(), m_sphere.getIndices(), GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_sphere.getInterleavedStride(), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, m_sphere.getInterleavedStride(), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


}

void Renderer::InitializeLineVertices()
{
    glGenVertexArrays(1, &m_linesVAO);
    glGenBuffers(1, &m_linesVBO);

    glBindVertexArray(m_linesVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_linesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_linesCpuBuffer), m_linesCpuBuffer, GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::SetLinesVertexBuffer(const MocapFrame& frame)
{
    for (int i = 1; i < PlayerPoints; i++) {
        m_linesCpuBuffer[i] = frame.points[i];
        m_linesCpuBuffer[i - 1] = frame.points[i - 1];
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_linesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_linesCpuBuffer), m_linesCpuBuffer, GL_DYNAMIC_DRAW);
}

glm::mat4 PositionAndScaleToModelMatrix(const glm::vec3& pos, const glm::vec3& dimensions) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    //model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, dimensions);
    return model;
}

void Renderer::DrawMocapFrame(const MocapFrame& frame, const Camera& cam)
{
    auto lineColour = glm::vec4{ 0,0,0,1.0 };
    //DrawLines(frame, cam, lineColour);
    for (int i = 0; i < PlayerPoints; i++) {
        DrawSphere(frame.points[i], glm::vec3(0.5,0.5,0.5), cam, glm::vec4(0.0, 1.0, 0.0, 1.0));
        DrawTextBillboard("index: "+std::to_string(i), glm::vec4(1.0, 0.0, 0.0, 1.0), frame.points[i] + glm::vec3(0.8, 0, 0), 0.1f, cam);
    }
}

void Renderer::DrawSphere(const glm::vec3& pos, const glm::vec3& dimensions, const Camera& camera, const glm::vec4& colour) const
{
    glm::mat4 model = PositionAndScaleToModelMatrix(pos, dimensions);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)m_scrWidth / (float)m_scrHeight, 0.1f, DRAW_DISTANCE);

    // vert
    m_colouredShader.use();
    m_colouredShader.setMat4("model", model);
    m_colouredShader.setMat4("view", camera.GetViewMatrix());
    m_colouredShader.setMat4("projection", projection);

    // frag
    m_colouredShader.setVec3("viewPos", camera.Position);
    m_colouredShader.setVec3("lightPos", m_lightPos);
    m_colouredShader.setVec3("lightColor", m_lightColour);
    m_colouredShader.setVec4("objectColor", colour);

    glBindVertexArray(m_unitSphereVAO);
    glDrawElements(GL_TRIANGLES, m_sphere.getIndexCount(), GL_UNSIGNED_INT, 0);

}

void Renderer::DrawLines(const MocapFrame& frame, const Camera& cam, const glm::vec4& colour)
{
    SetLinesVertexBuffer(frame);
    glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)m_scrWidth / (float)m_scrHeight, 0.1f, DRAW_DISTANCE);

    m_lineShader.use();

    // vert
    m_lineShader.setMat4("view", cam.GetViewMatrix());
    m_lineShader.setMat4("projection", projection);

    // frag
    m_lineShader.setVec4("objectColor", colour);

    glBindVertexArray(m_linesVAO);
    glDrawArrays(GL_LINES, 0, PlayerPoints * 2);
}

void Renderer::SetLightPos(const glm::vec3& value)
{
    m_lightPos = value;
}

void Renderer::SetLightColour(const glm::vec3& value)
{
    m_lightColour = value;
}

/// <summary>
/// initialise freetype library and vertices used
/// for text drawing.
/// </summary>
void Renderer::InitFT()
{
    if (FT_Init_FreeType(&m_ft))
    {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &m_freeTypeVAO);
    glGenBuffers(1, &m_freeTypeVBO);
    glBindVertexArray(m_freeTypeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_freeTypeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/// <summary>
/// load a ttf format font
/// </summary>
/// <param name="ttfFilePath">path to font</param>
void Renderer::LoadFont(std::string ttfFilePath)
{
    FT_Face face;
    if (FT_New_Face(m_ft, ttfFilePath.c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, m_baseTextSize);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            m_characters[c] = character;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(m_ft);
}

void Renderer::DrawTextBillboard(std::string text, const glm::vec3& textColour, const glm::vec3& woldPos, const float scale, const Camera& camera)
{
    m_billboardShader.use();

    auto ViewMatrix = camera.GetViewMatrix();
    auto CameraRight_worldspace = glm::vec3{ ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0] };
    auto CameraUp_worldspace = glm::vec3{ ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1] };

    m_billboardShader.setVec3("CameraRight_worldspace", CameraRight_worldspace);
    m_billboardShader.setVec3("CameraUp_worldspace", CameraUp_worldspace);

    m_billboardShader.setVec3("particleCenter_wordspace", woldPos);
    m_billboardShader.setVec2("BillboardSize", glm::vec2(scale, scale));
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)m_scrWidth / (float)m_scrHeight, 0.1f, DRAW_DISTANCE);
    auto vp = projection * ViewMatrix;
    m_billboardShader.setMat4("VP", vp);

    m_billboardShader.setVec3("textColor", textColour);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_freeTypeVAO);

    float x = 0;
    float y = 0;
    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = m_characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, m_freeTypeVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

}