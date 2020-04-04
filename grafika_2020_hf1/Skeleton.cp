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

const char * const fragmentSource = R"(
    #version 330            // Shader 3.3
    precision highp float;    // normal floats, makes no difference on desktop computers
    
    in vec3 color;        // uniform variable, the color of the primitive
    out vec4 outColor;        // computed color of the current pixel

    void main() {
        outColor = vec4(color, 1);    // computed color is the color of the primitive
    }
)";

GPUProgram gpuProgram;

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
            vertices.push_back(0.9f);
            vertices.push_back(0.9f);
            vertices.push_back(0.9f);
            
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
    unsigned int vao, vbo, vao1, vbo1;
    unsigned int nv = 50;
    
    vec4 origin;
    vec4 dummyOrigin;
    
    float r = 1.0f;
    float dummyRadius;
    
    float aSide = 0.0f;
    float bSide = 0.0f;
    float cSide = 0.0f;
    
    float alpha = 0.0f;
    float beta = 0.0f;
    float gamma = 0.0f;
    
    std::vector<float> origins;
    std::vector<float> triangleVertices;

    std::vector<float> vertices;
    std::vector<float> verticesCopy;
    std::vector<float> points;
        
public:
    std::vector<float> ears;
    void create(float a, float b, float c, float d, float e, float f) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
        

        calculateOrigin(a, b, c, d);
        aSide = calculateSides(a, b, c, d);

        calculateOrigin(c, d, e, f);
        bSide = calculateSides(c, d, e, f);

        calculateOrigin(e, f, a, b);
        cSide = calculateSides(e, f, a, b);
                
        copyVertices();
        
        calculateEars();
        
        calculateAngles();
        
        float angleSum = alpha + beta + gamma;
        printf("Alpha: %3.2f, Beta: %3.2f, Gamma: %3.2f, Angle sum: %3.2f \n", alpha, beta, gamma, angleSum);
        printf("a: %3.2f, b: %3.2f, c: %3.2f \n", aSide, bSide, cSide);
        
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
    }

    
    float calculateSides(float a, float b, float c, float d) {
        float sideLength = 0.0f;
        float cx = (-a * a * d - b * b * d + b * c * c + b * d * d + b - d) / 2 / ( b * c - a * d);
        float cy = (a * a * c - a * c * c - a * d * d - a + b * b * c + c) / 2 / (b * c - a * d);
        
        dummyOrigin = vec4(cx, cy, 0, 0);
        
        origins.push_back(dummyOrigin.x);
        origins.push_back(dummyOrigin.y);
        
        dummyRadius = sqrt((cx*cx) + (cy*cy) - 1);
        
        vec4 p1 = vec4(a, b, 0, 0);
        vec4 p2 = vec4(c, d, 0, 0);
        
        vec4 origo = vec4(0, 0, 0, 0);
        
        vec4 cToOrigo = dummyOrigin - origo;
        vec4 cToP1 = dummyOrigin - p1;
        vec4 cToP2 = dummyOrigin - p2;
        
        float x, y, dx, dy;
        
        float ctoorigoDotctop1 = dot(cToOrigo, cToP1);
        float ctoorigoDotctop2 = dot(cToOrigo, cToP2);
        
        float cToOrLength = sqrt((origo.x - dummyOrigin.x)*(origo.x - dummyOrigin.x) + (origo.y - dummyOrigin.y)*(origo.y - dummyOrigin.y) );
        
        float cToP1Length = sqrt((p1.x - dummyOrigin.x)*(p1.x - dummyOrigin.x) + (p1.y - dummyOrigin.y)*(p1.y - dummyOrigin.y) );
        float cToP2Length = sqrt((p2.x - dummyOrigin.x)*(p2.x - dummyOrigin.x) + (p2.y - dummyOrigin.y)*(p2.y - dummyOrigin.y) );
        
        float alpha1 = atan2f((p1.y - dummyOrigin.y), (p1.x - dummyOrigin.x));
        float alpha2 = atan2f((p2.y - dummyOrigin.y), (p2.x - dummyOrigin.x));
        
        
        if (alpha1 < 0) {
            alpha1 = alpha1 + 2 * M_PI;
        }
        
        if (alpha2 < 0) {
            alpha2 = alpha2 + 2 * M_PI;
        }
        
        float diff = alpha1 - alpha2;
        
        for (int i = 0; i < nv; i++) {
            float fi;
            if (diff < M_PI) {
                if (diff < 0) {
                    if (diff < - M_PI) {
                        if(i < (nv/2)) {
                            fi = alpha1 - (i * alpha1 / (nv/2));
                        }
                        else {
                            fi = 2 * M_PI - ((i - (nv/2)) * (2 * M_PI - alpha2) / (nv/2));
                        }
                    }
                    else {
                        fi = alpha1 + i * (alpha2 - alpha1) / nv;
                    }
                    
                }
                if (diff > 0) {
                    fi = alpha1 - (i * diff / nv);                }
            }
            else {
                if(alpha1 > M_PI) {
                    if(i < (nv/2)) {
                        fi = alpha1 + (i * (2 * M_PI - alpha1) / (nv/2));
                    }
                    else {
                        fi = (i - (nv/2)) * alpha2 / (nv/2);
                    }
                }
                if(alpha1 < M_PI) {
                   if(i < (nv/2)) {
                       fi = alpha1 - (i * alpha1 / (nv/2));
                   }
                   else {
                       fi = 2 * M_PI - ((i - (nv/2)) * (2 * M_PI - alpha2) / (nv/2));
                   }
                }
            }
            vec4 mVertex = vec4(cosf(fi), sinf(fi), 0, 1) * M();
            vertices.push_back(mVertex.x);
            vertices.push_back(mVertex.y);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            if(i == 0) {
                triangleVertices.push_back(mVertex.x);
                triangleVertices.push_back(mVertex.y);
            }
            if (i >= 1) {
                x = vertices[vertices.size() - 10];
                y = vertices[vertices.size() - 9];
                dx = vertices[vertices.size() - 5] - x;
                dy = vertices[vertices.size() - 4] - y;
                sideLength = sideLength + calculateSideLength(x, y, dx, dy);
            }
        }
        
        return sideLength;
    }
    
    void copyVertices() {
        for (int i = 0; i < vertices.size(); i++) {
            verticesCopy.push_back(vertices.at(i));
        }
    }
    
    void calculateAngles() {
        float a = triangleVertices[0] - origins[0];
        float b = triangleVertices[1] - origins[1];
        float c = triangleVertices[0] - origins[4];
        float d = triangleVertices[1] - origins[5];
        alpha = acos(dot(normalize(vec2(a, b)), normalize(vec2(c, d)))) / M_PI * 180;
        if (alpha > 90.0f) { alpha = 180.0f - alpha; }

        a = triangleVertices[2] - origins[0];
        b = triangleVertices[3] - origins[1];
        c = triangleVertices[2] - origins[2];
        d = triangleVertices[3] - origins[3];
        beta = acos(dot(normalize(vec2(a, b)), normalize(vec2(c, d)))) / M_PI * 180;
        if (beta > 90.0f) { beta = 180.0f - beta; }

        a = triangleVertices[4] - origins[2];
        b = triangleVertices[5] - origins[3];
        c = triangleVertices[4] - origins[4];
        d = triangleVertices[5] - origins[5];
        gamma = acos(dot(normalize(vec2(a, b)), normalize(vec2(c, d)))) / M_PI * 180;
        if (gamma > 90.0f) { gamma = 180.0f - gamma; }
    }
    
    int calculateIntersections(vec2 prev, vec2 next) {
        
        int numberOfIntersections = 0;
        
        float a = prev.x;
        float b = prev.y;
        float c = next.x;
        float d = next.y;
        
        for (int i = 0; i < vertices.size() - 5; i = i + 5) {
            
            vec2 first = vec2(vertices.at(i), vertices.at(i+1));
            vec2 second = vec2(vertices.at(i+5), vertices.at(i+6));
            
            float e = first.x;
            float f = first.y;
            float g = second.x;
            float h = second.y;
            
            // The equations below were all calculated via Wolfram Alpha from the lecture's equations
            float t2 = (g * (b - d) + a * (d - h) + c * (h - b)) / (g * (b - d) + e * (d - b) + (a - c) * (f - h));
            float t1 = ((e - g) * t2 + g - c) / (a - c);
            
            float error = 0.0001f;
            
            if (t1 > (0.0f + error) && t1 < (1.0f - error) && t2 > (0.0f + error) && t2 < (1.0f - error)) {
                numberOfIntersections++;
            }
            
        }
        
        return numberOfIntersections;
    }
    
    float calculateSideLength(float x, float y, float dx, float dy) {
        float ds = 0.0f;
        float up = sqrtf(dx * dx + dy * dy);
        float down = (1 - x * x - y * y);
        ds = up / down;
        return ds;
    }
    
    void calculateEars() {
        for (int i = 5; i < verticesCopy.size() - 10; i=i+5) {
            
            vec2 prev = vec2(verticesCopy.at(i-5), verticesCopy.at(i-4));
            vec2 current = vec2(verticesCopy.at(i), verticesCopy.at(i+1));
            vec2 next = vec2(verticesCopy.at(i+5), verticesCopy.at(i+6));
            
            int numberOfIntersections = calculateIntersections(prev, next);
            
            if(numberOfIntersections == 0) {
                vec2 half = vec2((prev.x + next.x) / 2, (prev.y + next.y) / 2);
                numberOfIntersections = calculateIntersections(half, vec2(2.0f, 2.0f));
                 if (numberOfIntersections % 2 != 0) {
                    ears.push_back(prev.x);
                    ears.push_back(prev.y);
                    ears.push_back(0.8f);
                    ears.push_back(0.8f);
                    ears.push_back(0.8f);
                    ears.push_back(current.x);
                    ears.push_back(current.y);
                    ears.push_back(1.0f);
                    ears.push_back(1.0f);
                    ears.push_back(1.0f);
                    ears.push_back(next.x);
                    ears.push_back(next.y);
                    ears.push_back(0.8f);
                    ears.push_back(0.8f);
                    ears.push_back(0.8f);
                    
                    verticesCopy.erase(verticesCopy.begin() + i, verticesCopy.begin() + i + 5);
                    i = 0;
                }
            }
        }
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
    
    mat4 Unit() {
        return mat4(1,   0,  0,  0,
                    0,   1,  0,  0,
                    0,   0,  1,  0,
                    0,   0,  0,  1);
    }
    

    void Draw() {
            gpuProgram.setUniform(Unit(), "MVP");
            glBindVertexArray(vao);
            glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / 5);
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
            vertices.push_back(0.5f);
            vertices.push_back(0.5f);
            vertices.push_back(0.5f);
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

    
class Painter {
    unsigned int vao, vbo;
    
    std::vector<float> addedEars;
    
public:
    void create(std::vector<float> ears) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
        
        for(int i=0; i < ears.size(); i++) {
            addedEars.push_back(ears.at(i));
        }
        
        glBufferData(GL_ARRAY_BUFFER, addedEars.size() * sizeof(float), &addedEars[0], GL_DYNAMIC_DRAW);
        
    }
    

    mat4 M() {
        return mat4(1,   0,  0,  0,
                   0,   1,  0,  0,
                   0,   0,  1,  0,
                   0,   0,  0,  1);
    }
    
    void Draw() {
        gpuProgram.setUniform(M(), "MVP");
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, addedEars.size() / 5);
    }
};


Circle circle;
Point point;
SiriusCircle siriusCircle;
Painter painter;

std::vector<float> points;

void onInitialization() {
    glViewport(0, 0, windowWidth, windowHeight);
    
    circle.create();
    point.create();

    gpuProgram.create(vertexSource, fragmentSource, "outColor");
}

void onDisplay() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    circle.Draw();
    point.Draw();
    if(points.size() == 15) {
        painter.Draw();
        siriusCircle.Draw();
    }

    glutSwapBuffers();
}

void onKeyboard(unsigned char key, int pX, int pY) {
    if (key == 'd') glutPostRedisplay();
}

void onKeyboardUp(unsigned char key, int pX, int pY) {
}

void onMouseMotion(int pX, int pY) {
    float cX = 2.0f * pX / windowWidth - 1;
    float cY = 1.0f - 2.0f * pY / windowHeight;
}

void onMouse(int button, int state, int pX, int pY) {
    float cX = 2.0f * pX / windowWidth - 1;
    float cY = 1.0f - 2.0f * pY / windowHeight;
    
    float fi = atanf(cY / cX);

    char * buttonStat;
    switch (state) {
    case GLUT_DOWN: buttonStat = "pressed"; break;
    case GLUT_UP:   buttonStat = "released"; break;
    }

    if (fabs(cY) <= fabs(sinf(fi))) {
       if (points.size() < 15) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (points.size() < 15) {
                point.AddPoint(cX, cY);
                vec4 mVertex = vec4(cX, cY, 0, 1);
                points.push_back(mVertex.x);
                points.push_back(mVertex.y);
                points.push_back(1);
                points.push_back(0);
                points.push_back(0);
            }
            
            siriusCircle.AddPoint(cX, cY);
            
            if (points.size() == 15) {
                siriusCircle.create(points.at(0), points.at(1), points.at(5), points.at(6), points.at(10), points.at(11));
                std::vector<float> ears = siriusCircle.ears;
                painter.create(ears);
            }

            glutPostRedisplay();
        }
    }
    
    }
    
}

void onIdle() {
    long time = glutGet(GLUT_ELAPSED_TIME);
}
