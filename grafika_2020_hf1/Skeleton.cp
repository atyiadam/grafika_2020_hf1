//=============================================================================================
// Mintaprogram: Zšld h‡romszšg. Ervenyes 2019. osztol.
//
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat, BOM kihuzando.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kiveve
// - Mashonnan atvett programresszleteket forrasmegjeloles nelkul felhasznalni es
// - felesleges programsorokat a beadott programban hagyni!!!!!!!
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak
// A keretben nem szereplo GLUT fuggvenyek tiltottak.
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Atyi Adam
// Neptun : PTZY5J
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================
#include "framework.h"

// vertex shader in GLSL: It is a Raw string (C++11) since it contains new line characters
const char * const vertexSource = R"(
    #version 330                // Shader 3.3
    precision highp float;        // normal floats, makes no difference on desktop computers

    uniform mat4 MVP;            // uniform variable, the Model-View-Projection transformation matrix
    
    layout(location = 0) in vec2 vertexPosition;    // Varying input: vp = vertex position is expected in attrib array 0
    layout(location = 1) in vec3 vertexColor;

    out vec3 color;

    void main() {
        color = vertexColor;
        gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0, 1) * MVP;        // transform vp from modeling space to normalized device space
    }
)";

// fragment shader in GLSL
const char * const fragmentSource = R"(
    #version 330            // Shader 3.3
    precision highp float;    // normal floats, makes no difference on desktop computers
    
    in vec3 color;        // uniform variable, the color of the primitive
    out vec4 outColor;        // computed color of the current pixel

    void main() {
        outColor = vec4(color, 1);    // computed color is the color of the primitive
    }
)";

GPUProgram gpuProgram; // vertex and fragment shaders

class Point {
    unsigned int vao, vbo;
    std::vector<float> vertices;
    
public:
    void create() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    }
    
    mat4 M() {
        return mat4(1,   0,  0,  0,
                   0,   1,  0,  0,
                   0,   0,  1,  0,
                   0,   0,  0,  1);
    }
    
    void AddPoint(float x, float y) {
        if (vertices.size() <= 15) {
            if (vertices.size() == 15) {
                vertices.clear();
            }
            vec4 mVertex = vec4(x, y, 0, 1);
            vertices.push_back(mVertex.x);
            vertices.push_back(mVertex.y);
            vertices.push_back(1);
            vertices.push_back(0);
            vertices.push_back(0);
            
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
        }
    }
    
    void Draw() {
        if (vertices.size() > 0) {
            mat4 m = M();
            gpuProgram.setUniform(m, "MVP");
            glBindVertexArray(vao);
            glDrawArrays(GL_POINTS, 0, vertices.size() / 5);
            glPointSize(12.0f);
        }
    }
};


class SiriusCircle {
    unsigned int vao, vbo;
    unsigned int nv = 5000;
    
    vec4 origin;
    
    float r = 1.0f;
    
    std::vector<float> vertices;
    std::vector<float> points;
    
public:
    void create() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
        
        for (int i = 0; i < nv; i++) {
            float fi = i * 2 * M_PI / nv;
            vec4 mVertex = vec4(cosf(fi), sinf(fi), 0, 1) * M();
            vertices.push_back(mVertex.x);
            vertices.push_back(mVertex.y);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
        }
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
    }
    
    
    
    
    void AddPoint(float x, float y) {
        if (points.size() <= 15) {
            if (points.size() == 15) {
                points.clear();
            }
            vec4 mVertex = vec4(x, y, 0, 1);
            points.push_back(mVertex.x);
            points.push_back(mVertex.y);
            points.push_back(1);
            points.push_back(0);
            points.push_back(0);
        }
    }
    
    
    // The equations below were all calculated via Wolfram Alpha
    void calculateOrigin(float a, float b, float c, float d) {
        float cx = (-a * a * d - b * b * d + b * c * c + b * d * d + b - d) / 2 / ( b * c - a * d);
        float cy = (a * a * c - a * c * c - a * d * d - a + b * b * c + c) / 2 / (b * c - a * d);
        
        origin = vec4(cx, cy, 0, 0);
        r = sqrt((cx*cx) + (cy*cy) - 1);
    }
    
    
    mat4 M() {
        return mat4(r,   0,  0,  0,
                   0,   r,  0,  0,
                   0,   0,  1,  0,
                   origin.x,   origin.y,  0,  1);
    }
    
    void Draw() {
        if (points.size() == 15) {
            calculateOrigin(points.at(0), points.at(1), points.at(5), points.at(6));
            gpuProgram.setUniform(M(), "MVP");
            glBindVertexArray(vao);
            glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / 5);
            calculateOrigin(points.at(5), points.at(6), points.at(10), points.at(11));
            gpuProgram.setUniform(M(), "MVP");
            glBindVertexArray(vao);
            glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / 5);
            calculateOrigin(points.at(10), points.at(11), points.at(0), points.at(1));
            gpuProgram.setUniform(M(), "MVP");
            glBindVertexArray(vao);
            glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / 5);
        }
    }
};



class Circle {
    unsigned int vao, vbo;
    unsigned int nv = 100;
    
    std::vector<float> vertices;
    
public:
    void create() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
        
        for (int i = 0; i < nv; i++) {
            float fi = i * 2 * M_PI / nv;
            vec4 mVertex = vec4(cosf(fi), sinf(fi), 0, 1) * M();
            vertices.push_back(mVertex.x);
            vertices.push_back(mVertex.y);
            vertices.push_back(0.75f);
            vertices.push_back(0.75f);
            vertices.push_back(0.75f);
        }
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
    }
    
    
    
    mat4 M() {
        return mat4(1,   0,  0,  0,
                   0,   1,  0,  0,
                   0,   0,  1,  0,
                   0,   0,  0,  1);
    }
    
    void Draw() {
        if(vertices.size() > 0) {
            gpuProgram.setUniform(M(), "MVP");
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size() / 5);
        }
    }
};

Circle circle;
Point point;
SiriusCircle siriusCircle;

// Initialization, create an OpenGL context
void onInitialization() {
    glViewport(0, 0, windowWidth, windowHeight);
    
    circle.create();
    point.create();
    siriusCircle.create();

    gpuProgram.create(vertexSource, fragmentSource, "outColor");
}

// Window has become invalid: Redraw
void onDisplay() {
    glClearColor(0, 0, 0, 0);     // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear frame buffer
    
    circle.Draw();
    point.Draw();
    siriusCircle.Draw();

    glutSwapBuffers(); // exchange buffers for double buffering
}

// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {
    if (key == 'd') glutPostRedisplay();         // if d, invalidate display, i.e. redraw
}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {    // pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
    // Convert to normalized device space
    float cX = 2.0f * pX / windowWidth - 1;    // flip y axis
    float cY = 1.0f - 2.0f * pY / windowHeight;
}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) { // pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
    // Convert to normalized device space
    float cX = 2.0f * pX / windowWidth - 1;    // flip y axis
    float cY = 1.0f - 2.0f * pY / windowHeight;

    char * buttonStat;
    switch (state) {
    case GLUT_DOWN: buttonStat = "pressed"; break;
    case GLUT_UP:   buttonStat = "released"; break;
    }

    switch (button) {
    case GLUT_LEFT_BUTTON:   printf("Left button %s at (%3.2f, %3.2f)\n", buttonStat, cX, cY);   break;
    case GLUT_MIDDLE_BUTTON: printf("Middle button %s at (%3.2f, %3.2f)\n", buttonStat, cX, cY); break;
    case GLUT_RIGHT_BUTTON:  printf("Right button %s at (%3.2f, %3.2f)\n", buttonStat, cX, cY);  break;
    }
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        point.AddPoint(cX, cY);
        siriusCircle.AddPoint(cX, cY);
        glutPostRedisplay();
    }
}

// Idle event indicating that some time elapsed: do animation here
void onIdle() {
    long time = glutGet(GLUT_ELAPSED_TIME); // elapsed time since the start of the program
}
