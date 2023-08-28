#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include<bits/stdc++.h>
#include "bitmap_image.hpp"

using namespace std;
using Tf = GLdouble;
using Tfl = long double;
using Ti = GLint;
using Ts = size_t;

const Tf PI = acos(-1), EPS = 1e-9, EPS2 = 1e-3;

struct Point;
struct Line;
struct Color;
struct Object;
struct Sphere;
struct Pyramid;
struct Cube;
struct CheckerBoard;
struct Light;
struct SpotLight;



int dcmp(Tf x){return abs(x)<EPS?0:(x<0?-1:1);}
struct Point {
  Tf x, y, z;
  Point() : x(0), y(0), z(0) {}
  Point(Tf X, Tf Y, Tf Z) :
      x(X), y(Y), z(Z) {}
  Point operator + (const Point& u) const {
    return Point(x + u.x, y + u.y, z + u.z); }
  Point operator - (const Point& u) const {
    return Point(x - u.x, y - u.y, z - u.z); }
  Point operator - () const {
    return Point(-x, -y, -z); }
  Point operator * (const Tf u) const {
    return Point(x * u, y * u, z * u); }
  Point operator / (const Tf u) const {
    return Point(x / u, y / u, z / u); }
  friend std::ostream &operator << (
            std::ostream &os, const Point &p) {
    return os << p.x << " " << p.y <<" "<<p.z; }
  friend std::istream &operator >>
            (std::istream &is, Point &p) {
    return is >> p.z >> p.x >> p.y; }
};
Tf dot(Point a, Point b) {
  return a.x * b.x + a.y * b.y + a.z * b.z; }
Point cross(Point a, Point b) {
  return Point(a.y*b.z - a.z*b.y,
     a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x); }
Tf length(Point a) { return sqrt(dot(a, a));}
Tf distance(Point a, Point b) {
  return length(a-b); }
Point unit(const Point &p) { return p/length(p); }
// Rotate p around axis x, with angle radians.
Point rotate(Point p, Point axis, Tf angle) {
  axis = unit(axis);Point comp1 = p * cos(angle);
  Point comp2 = axis*(1-cos(angle))*dot(axis,p);
  Point comp3 = cross(axis, p) * sin(angle);
  return comp1 + comp2 + comp3;
}
struct Line {Point a, v;};  ///a+tv
Tf distancePointLine(Point p, Line l) {
  return length(cross(l.v, p - l.a)) / length(l.v);
}
/// distance from Line ab to Line cd
Tf distanceLineLine(Line a, Line b) {
  Point cr = cross(a.v, b.v);
  Tf crl = length(cr);
  if(dcmp(crl)==0)return distancePointLine(a.a,b);
  return abs(dot(cr, a.a-b.a))/crl;
}


bool isInsideTriangle(Point p1, Point p2, Point p3, Point p){
    Point v1 = unit(cross(p1 - p, p2 - p));
    Point v2 = unit(cross(p2 - p, p3 - p));
    Point v3 = unit(cross(p3 - p, p1 - p));
    if(dcmp(dot(v1, v2)) < 0)return false;
    if(dcmp(dot(v2, v3)) < 0)return false;
    if(dcmp(dot(v3, v1)) < 0)return false;

    if(dcmp(length(cross(v1, v2))) != 0)return false;
    if(dcmp(length(cross(v2, v3))) != 0)return false;
    if(dcmp(length(cross(v3, v1))) != 0)return false;

    return true;
}


bool isInsideSquare(Point p1, Point p2, Point p3, Point p4, Point p){
    if(!isInsideTriangle(p1, p2, p3, p))return false;
    if(!isInsideTriangle(p1, p3, p4, p))return false;
    return true;   
}

int Gauss(vector<vector<Tf>> &mat, vector<Tf> &ans){        //returns -1 if impossible, 1 if solution is unique, 0 otherwise
    int n = mat.size(), m = mat[0].size() - 1;             
    vector<int> w(m, -1);
    int ret = 1;
    for(int i = 0, j = 0; j < n && i < m; i++){
        int mx = j;
        for(int k = j + 1; k < n; k++)if(mat[mx][i] < mat[k][i])mx = k;
        if(abs(mat[mx][i]) < EPS){
            ret = 0;
            continue;
        }
        for(int k = i; k <= m; k++)swap(mat[j][k], mat[mx][k]);
        for(int k = 0; k < n; k++){
            if(k == j)continue;
            Tf d = mat[k][i] / mat[j][i];
            for(int l = i; l <= m; l++)mat[k][l] -= d * mat[j][l];
        }
        w[i] = j++;
    }
    ans.assign(m, 0);
    for(int i = 0; i < m; i++)if(w[i] != -1) ans[i] = mat[w[i]][m] / mat[w[i]][i];
    for(int i = 0; i < n; i++){
        Tf d = 0;
        for(int j = 0; j < m; j++)d += ans[j] * mat[i][j];
        if(abs(d - mat[i][m]) > EPS)return -1;
    }
    return ret;
}




Tf TriangleIntersection(Line ray, Point p1, Point p2, Point p3){
    Point normal = unit(cross(p2 - p1, p3 - p1));

    Tf temp = dot(normal, ray.v);

    if(dcmp(temp) == 0)return -1;

    Tf t = dot(normal, p1 - ray.a) / temp;
    if(t < 0)return -1;
    Point p = ray.a + ray.v * t;
    if(!isInsideTriangle(p1, p2, p3, p))return -1;


    return t;
    
    // vector<vector<Tf>> mat(4, vector<Tf>(5));

    // //For X co-oridnate
    // mat[0][0] = p1.x;
    // mat[0][1] = p2.x;
    // mat[0][2] = p3.x;
    // mat[0][3] = -ray.v.x;
    // mat[0][4] = ray.a.x;

    // //For Y co-oridnate
    // mat[1][0] = p1.y;
    // mat[1][1] = p2.y;
    // mat[1][2] = p3.y;
    // mat[1][3] = -ray.v.y;
    // mat[1][4] = ray.a.y;

    // //For Z co-oridnate
    // mat[2][0] = p1.z;
    // mat[2][1] = p2.z;
    // mat[2][2] = p3.z;
    // mat[2][3] = -ray.v.z;
    // mat[2][4] = ray.a.z;

    // mat[3][0] = 1;
    // mat[3][1] = 1;
    // mat[3][2] = 1;
    // mat[3][3] = 0;
    // mat[3][4] = 1;

    // vector<Tf> ans(4);
    // int ret = Gauss(mat, ans);
    // if(ret == -1)return -1;
    
    // for(int i = 0; i < 3; i++){
    //     if(ans[i] < 0 || ans[i] > 1)return -1;
    // }


    // return ans[3];

}

Tf SquareIntersection(Line ray, Point p1, Point p2, Point p3, Point p4){
    Tf t1 = TriangleIntersection(ray, p1, p2, p3);
    Tf t2 = TriangleIntersection(ray, p1, p3, p4);

    if(t1 < 0 && t2 < 0)return -1;
    if(t1 < 0)return t2;
    if(t2 < 0)return t1;
    return min(t1, t2);
}

namespace Data{
    Tf zNear, zFar, fov, aspectRatio;
    Ti levelOfRecursion;
    Ti width, height;
    Ts numberOfObject;
    Ts numberOfLight;
    Ts numberOfSpotLight;
    vector<Object*> objects;
    vector<Light*> lights;
    vector<SpotLight*> spotLights;
    Point pos;   // position of the eye
    Point l;     // look/forward direction
    Point r;     // right direction
    Point u;     // up direction
    Tf D;

};
struct Color:public Point{
    void prepare(){
        // x *= 255;
        // y *= 255;
        // z *= 255;
    }
    Color():Point(0,0,0){
        prepare();
    }
    Color(Tf _x, Tf _y, Tf _z):Point(_x, _y, _z){
        prepare();
    }
    friend std::istream &operator >>
            (std::istream &is, Color &p) {
        is >> p.x >> p.y >> p.z; 
        // cout << p.x << " " << p.y << " " << p.z << endl;
        p.prepare();
        return is;
    }

    Color operator = (const Point &p){
        x = p.x;
        y = p.y;
        z = p.z;
        return *this;
    }

    



};

struct Object{
    string type;
    int typeID;
    Color color;
    Tf ambient, diffuse, specular, reflection, shininess;
    Object(){
        type = "Object";
        typeID = 0;
        color = Color(0,0,0);
        ambient = diffuse = specular = reflection = shininess = 0;
    }  
      
    Object(string _type, int _typeID){
        type = _type;
        typeID = _typeID;
        color = Color(0,0,0);
        ambient = diffuse = specular = reflection = shininess = 0;
    }  

    Object(string _type, int _typeID, Color _color, Tf _ambient, Tf _diffuse, Tf _specular, Tf _reflection, Tf _shininess){
        type = _type;
        typeID = _typeID;
        color = _color;
        ambient = _ambient;
        diffuse = _diffuse;
        specular = _specular;
        reflection = _reflection;
        shininess = _shininess;
    }

    virtual void draw() = 0;
    virtual Tf intersect(Line ray) = 0;
    virtual Color getColor(Point p) = 0;
    virtual Point getNormal(Line ray, Point p) = 0;

};




struct Sphere : public Object{
    Point center;
    Tf radius;
    Sphere():Object("Sphere", 1){
        center = Point(0,0,0);
        radius = 0;
    }
    Sphere(Point _center, Tf _radius, Color _color, Tf _ambient, Tf _diffuse, Tf _specular, Tf _reflection, Tf _shininess)
        :Object("Sphere", 1, _color, _ambient, _diffuse, _specular, _reflection, _shininess){
        center = _center;
        radius = _radius;
    }

    void draw(){
        // cout << "Sphere: " << center << " " << radius << endl;
        // cout << "Color: " << color << endl;
        glColor3d(color.x, color.y, color.z);
        glPushMatrix();
        glTranslated(center.x, center.y, center.z);
        // drawSphere(radius, 100, 100);
        glutSolidSphere(radius, 700, 700);
        glPopMatrix();
    }

    Tf intersect(Line ray){
        Point p = ray.a - center;
        Tf a = dot(ray.v, ray.v);
        Tf b = dot(ray.v, p) / a;
        Tf c = dot(p, p) - b * b;
        Tf d = radius * radius - c;
        if(d<0)return -1;
        Tf dt = sqrt(d);
        Tf t1 = (-b + dt) / a;
        Tf t2 = (-b - dt) / a;
        if(t1<0 && t2<0)return -1;
        if(t1<0)return t2;
        if(t2<0)return t1;
        return min(t1,t2);
    }
    Color getColor(Point p){
        return color;
    }
    Point getNormal(Line ray, Point p){
        return unit(p-center);
    }
};

struct Pyramid : public Object{
    Point lowestPoint;
    Tf height;
    Tf width;
    Pyramid():Object("Pyramid", 2){
        lowestPoint = Point(0,0,0);
        height = width = 0;
    }
    Pyramid(Point _lowestPoint, Tf _height, Tf _width, Color _color, Tf _ambient, Tf _diffuse, Tf _specular, Tf _reflection, Tf _shininess)
        :Object("Pyramid", 2, _color, _ambient, _diffuse, _specular, _reflection, _shininess){
        lowestPoint = _lowestPoint;
        height = _height;
        width = _width;
    }


    void draw(){
        Point up = lowestPoint + Point(width / 2, height, width / 2);

        glColor3d(color.x, color.y, color.z);

        glBegin(GL_TRIANGLES);
        glVertex3d(lowestPoint.x, lowestPoint.y, lowestPoint.z);
        glVertex3d(lowestPoint.x + width, lowestPoint.y, lowestPoint.z);
        glVertex3d(up.x, up.y, up.z);
        glEnd();

        glBegin(GL_TRIANGLES);
        glVertex3d(lowestPoint.x, lowestPoint.y, lowestPoint.z);
        glVertex3d(lowestPoint.x, lowestPoint.y, lowestPoint.z + width);
        glVertex3d(up.x, up.y, up.z);
        glEnd();

        glBegin(GL_TRIANGLES);
        glVertex3d(lowestPoint.x + width, lowestPoint.y, lowestPoint.z);
        glVertex3d(lowestPoint.x + width, lowestPoint.y, lowestPoint.z + width);
        glVertex3d(up.x, up.y, up.z);
        glEnd();

        glBegin(GL_TRIANGLES);
        glVertex3d(lowestPoint.x, lowestPoint.y, lowestPoint.z + width);
        glVertex3d(lowestPoint.x + width, lowestPoint.y, lowestPoint.z + width);
        glVertex3d(up.x, up.y, up.z);
        glEnd();

        glBegin(GL_QUADS);
        glVertex3d(lowestPoint.x, lowestPoint.y, lowestPoint.z);
        glVertex3d(lowestPoint.x + width, lowestPoint.y, lowestPoint.z);
        glVertex3d(lowestPoint.x + width, lowestPoint.y, lowestPoint.z + width);
        glVertex3d(lowestPoint.x, lowestPoint.y, lowestPoint.z + width);
        glEnd();

    }

    Tf intersect(Line ray){
        Point base1 = lowestPoint;
        Point base2 = lowestPoint + Point(width, 0, 0);
        Point base3 = lowestPoint + Point(width, 0, width);
        Point base4 = lowestPoint + Point(0, 0, width);
        Point up = lowestPoint + Point(width / 2, height, width / 2);

        Tf t1 = TriangleIntersection(ray, base1, base2, up);
        Tf t2 = TriangleIntersection(ray, base2, base3, up);
        Tf t3 = TriangleIntersection(ray, base3, base4, up);
        Tf t4 = TriangleIntersection(ray, base4, base1, up);

        Tf t5 = SquareIntersection(ray, base1, base2, base3, base4);

        Tf t = -1;

        if(t1 > 0 && (t < 0 || t1 < t))t = t1;
        if(t2 > 0 && (t < 0 || t2 < t))t = t2;
        if(t3 > 0 && (t < 0 || t3 < t))t = t3;
        if(t4 > 0 && (t < 0 || t4 < t))t = t4;
        if(t5 > 0 && (t < 0 || t5 < t))t = t5;

        return t;
    }

    Color getColor(Point p){
        return color;
    }
    Point getNormal(Line ray, Point p){
        Point base1 = lowestPoint;
        Point base2 = lowestPoint + Point(width, 0, 0);
        Point base3 = lowestPoint + Point(width, 0, width);
        Point base4 = lowestPoint + Point(0, 0, width);
        Point up = lowestPoint + Point(width / 2, height, width / 2);
       
        if(isInsideTriangle(base1, base2, up, p))return unit(cross(base2 - base1, up - base1));
        if(isInsideTriangle(base2, base3, up, p))return unit(cross(base3 - base2, up - base2));
        if(isInsideTriangle(base3, base4, up, p))return unit(cross(base4 - base3, up - base3));
        if(isInsideTriangle(base4, base1, up, p))return unit(cross(base1 - base4, up - base4));

        if(isInsideSquare(base1, base2, base3, base4, p))return unit(p - up);        

        return {0, 0, 1};
    }

};

struct Cube : public Object{
    Point bottomleft;
    Tf side;
    Cube():Object("Cube", 3){
        bottomleft = Point(0,0,0);
        side = 0;
    }
    Cube(Point _bottomleft, Tf _side, Color _color, Tf _ambient, Tf _diffuse, Tf _specular, Tf _reflection, Tf _shininess)
        :Object("Cube", 3, _color, _ambient, _diffuse, _specular, _reflection, _shininess){
        bottomleft = _bottomleft;
        side = _side;
    }

    void draw(){
        glColor3d(color.x, color.y, color.z);

        glBegin(GL_QUADS);
        glVertex3d(bottomleft.x, bottomleft.y, bottomleft.z);
        glVertex3d(bottomleft.x + side, bottomleft.y, bottomleft.z);
        glVertex3d(bottomleft.x + side, bottomleft.y, bottomleft.z + side);
        glVertex3d(bottomleft.x, bottomleft.y, bottomleft.z + side);
        glEnd();

        glBegin(GL_QUADS);
        glVertex3d(bottomleft.x, bottomleft.y, bottomleft.z);
        glVertex3d(bottomleft.x, bottomleft.y + side, bottomleft.z);
        glVertex3d(bottomleft.x, bottomleft.y + side, bottomleft.z + side);
        glVertex3d(bottomleft.x, bottomleft.y, bottomleft.z + side);
        glEnd();

        glBegin(GL_QUADS);
        glVertex3d(bottomleft.x + side, bottomleft.y, bottomleft.z);
        glVertex3d(bottomleft.x + side, bottomleft.y + side, bottomleft.z);
        glVertex3d(bottomleft.x + side, bottomleft.y + side, bottomleft.z + side);
        glVertex3d(bottomleft.x + side, bottomleft.y, bottomleft.z + side);
        glEnd();

        glBegin(GL_QUADS);
        glVertex3d(bottomleft.x, bottomleft.y + side, bottomleft.z);
        glVertex3d(bottomleft.x + side, bottomleft.y + side, bottomleft.z);
        glVertex3d(bottomleft.x + side, bottomleft.y + side, bottomleft.z + side);
        glVertex3d(bottomleft.x, bottomleft.y + side, bottomleft.z + side);
        glEnd();

        glBegin(GL_QUADS);
        glVertex3d(bottomleft.x, bottomleft.y, bottomleft.z + side);
        glVertex3d(bottomleft.x + side, bottomleft.y, bottomleft.z + side);
        glVertex3d(bottomleft.x + side, bottomleft.y + side, bottomleft.z + side);
        glVertex3d(bottomleft.x, bottomleft.y + side, bottomleft.z + side);
        glEnd();

        glBegin(GL_QUADS);
        glVertex3d(bottomleft.x, bottomleft.y, bottomleft.z);
        glVertex3d(bottomleft.x + side, bottomleft.y, bottomleft.z);
        glVertex3d(bottomleft.x + side, bottomleft.y + side, bottomleft.z);
        glVertex3d(bottomleft.x, bottomleft.y + side, bottomleft.z);
        glEnd();

        
    }
    Tf intersect(Line ray){
        Point p1 = bottomleft;
        Point p2 = bottomleft + Point(side, 0, 0);
        Point p3 = bottomleft + Point(side, 0, side);
        Point p4 = bottomleft + Point(0, 0, side);
        Point p5 = bottomleft + Point(0, side, 0);
        Point p6 = bottomleft + Point(side, side, 0);
        Point p7 = bottomleft + Point(side, side, side);
        Point p8 = bottomleft + Point(0, side, side);

        Tf t1 = SquareIntersection(ray, p1, p2, p3, p4);
        Tf t2 = SquareIntersection(ray, p1, p2, p6, p5);
        Tf t3 = SquareIntersection(ray, p2, p3, p7, p6);
        Tf t4 = SquareIntersection(ray, p3, p4, p8, p7);
        Tf t5 = SquareIntersection(ray, p4, p1, p5, p8);
        Tf t6 = SquareIntersection(ray, p5, p6, p7, p8);

        Tf t = -1;

        if(t1 > 0 && (t < 0 || t1 < t))t = t1;
        if(t2 > 0 && (t < 0 || t2 < t))t = t2;
        if(t3 > 0 && (t < 0 || t3 < t))t = t3;
        if(t4 > 0 && (t < 0 || t4 < t))t = t4;
        if(t5 > 0 && (t < 0 || t5 < t))t = t5;
        if(t6 > 0 && (t < 0 || t6 < t))t = t6;

        return t;
    }
    Color getColor(Point p){
        return color;
    }
    Point getNormal(Line ray, Point p){
        Point p1 = bottomleft;
        Point p2 = bottomleft + Point(side, 0, 0);
        Point p3 = bottomleft + Point(side, 0, side);
        Point p4 = bottomleft + Point(0, 0, side);
        Point p5 = bottomleft + Point(0, side, 0);
        Point p6 = bottomleft + Point(side, side, 0);
        Point p7 = bottomleft + Point(side, side, side);
        Point p8 = bottomleft + Point(0, side, side);

        if(isInsideSquare(p1, p2, p3, p4, p))return unit(p - Point(0, -1, 0));
        if(isInsideSquare(p1, p2, p6, p5, p))return unit(p - Point(-1, 0, 0));
        if(isInsideSquare(p2, p3, p7, p6, p))return unit(p - Point(0, 0, -1));
        if(isInsideSquare(p3, p4, p8, p7, p))return unit(p - Point(1, 0, 0));
        if(isInsideSquare(p4, p1, p5, p8, p))return unit(p - Point(0, 0, 1));
        if(isInsideSquare(p5, p6, p7, p8, p))return unit(p - Point(0, 1, 0));

        return {0, 0, 1};
    }
};

struct CheckerBoard : public Object{
    Ti sizeOfCell;
    CheckerBoard():Object("CheckerBoard", 0){
        sizeOfCell = 1;
    }
    CheckerBoard(Ti _sizeOfCell, Tf _ambient, Tf _diffuse, Tf _reflection){
        type = "CheckerBoard";
        typeID = 0;
        ambient = _ambient;
        diffuse = _diffuse;
        reflection = _reflection;
        sizeOfCell = _sizeOfCell;
    }
    
    void draw(){
        glPushMatrix();
        int dx = Data::pos.x / sizeOfCell;
        int dz = Data::pos.z / sizeOfCell;
        // cout << Data::pos << endl;
        // cout << sizeOfCell << endl;
        const Ti n = 200;
        const Tf down = 0;
        for(int i = -n + dx; i <= n + dx; i++){
            for(int j = -n + dz; j <= n + dz; j++){
                if((i+j)%2==0)glColor3d(0,0,0);
                else glColor3d(1,1,1);
                // cout << i <<  " " << j << endl;
                glBegin(GL_QUADS);
                glVertex3d(i*sizeOfCell, down, j*sizeOfCell);
                glVertex3d((i+1)*sizeOfCell, down, j*sizeOfCell);
                glVertex3d((i+1)*sizeOfCell, down, (j+1)*sizeOfCell);
                glVertex3d(i*sizeOfCell, down, (j+1)*sizeOfCell);
                glEnd();
            }
        }
        glPopMatrix();
    }
    Tf intersect(Line ray){
        if(ray.v.y == 0)return -1;
        Tf t = -ray.a.y / ray.v.y;
        return t;
    }
    Color getColor(Point p){
        int x = p.x / sizeOfCell;
        int z = p.z / sizeOfCell;
        if((x+z)%2==0)return {0,0,0};
        return {1,1,1};
    }
    Point getNormal(Line ray, Point p){
        if(ray.v.y > 0)return {0, -1, 0};
        return {0, 1, 0};
    }
};

struct Light{
    Point position;
    Tf falloff;
    Tf base = 5;
    Light(){
        position = Point(0,0,0);
        falloff = 0;
    }
    Light(Point _position, Tf _falloff){
        position = _position;
        falloff = _falloff;
    }

    void draw(){
        glColor3d(1,1,1);
        glPushMatrix();
        glTranslated(position.x, position.y, position.z);
        glutSolidSphere(base, 100, 100);
        glPopMatrix();
    }
};

struct SpotLight : public Light{
    Point direction;
    Tf cutoff;
    Tf height = 30;
    SpotLight():Light(){
        direction = Point(0,0,0);
        cutoff = 0;
    }
    SpotLight(Point _position, Tf _falloff, Point _direction, Tf _cutoff){
        position = _position;
        falloff = _falloff;
        direction = unit(_direction);
        cutoff = _cutoff;
    }

    void draw(){
        Point cur = Point(0,0,1);
        Tf angle = -acos(dot(unit(direction), cur)) * 180 / PI;
        Point axis = cross(direction, cur);
        glColor3d(1,1,0);
        glPushMatrix();
        glTranslated(position.x, position.y, position.z);
        glRotated(angle, axis.x, axis.y, axis.z);
        glutSolidCone(base, height, 100, 100);
        glPopMatrix();
    }
};




void inputData(string filename){
    ifstream fin(filename);
    
    fin >> Data::zNear >> Data::zFar;
    fin >> Data::fov;
    fin >> Data::aspectRatio;

    fin >> Data::levelOfRecursion;
    fin >> Data::width;
    Data::height = Data::width;

    Object *cb = new CheckerBoard();

    fin >> ((CheckerBoard*)cb) -> sizeOfCell;
    fin >> ((CheckerBoard*)cb) -> ambient >> ((CheckerBoard*)cb) -> diffuse >> ((CheckerBoard*)cb) -> reflection;

    fin >> Data::numberOfObject;
    Data::numberOfObject++;

    Data::objects.push_back(cb);
    
    for(Ts i = 1; i < Data::numberOfObject; i++){
        Object *ob;
        string type;
        fin >> type;
        if(type == "sphere"){
            ob = new Sphere();
            fin >> ((Sphere *)ob) -> center;
            fin >> ((Sphere *)ob) -> radius;
        }else if(type == "pyramid"){
            ob = new Pyramid();
            fin >> ((Pyramid *)ob) -> lowestPoint;
            fin >> ((Pyramid *)ob) -> width;
            fin >> ((Pyramid *)ob) -> height;
        }else if(type == "cube"){
            ob = new Cube();
            fin >> ((Cube *)ob) -> bottomleft;
            fin >> ((Cube *)ob) -> side;
        }

        fin >> ob -> color;
        fin >> ob -> ambient >> ob -> diffuse >> ob -> specular >> ob -> reflection >> ob -> shininess;
        Data::objects.push_back(ob);
    }

    fin >> Data::numberOfLight;
    for(Ts i = 0; i < Data::numberOfLight; i++){
        Light *l = new Light();
        fin >> l -> position;
        fin >> l -> falloff;
        Data::lights.push_back(l);
    }

    fin >> Data::numberOfSpotLight;
    for(Ts i = 0; i < Data::numberOfSpotLight; i++){
        SpotLight *sl = new SpotLight();
        fin >> sl -> position;
        fin >> sl -> falloff;
        fin >> sl -> direction;
        fin >> sl -> cutoff;
        sl -> direction = unit(sl -> direction - sl -> position);
        // sl -> direction = unit(sl -> position - sl -> direction);
        Data::spotLights.push_back(sl);
    }


    Data::pos = Point(-200,50,0);
    Data::l = unit(-Data::pos);
    Data::r = Point(0,0,1);
    // Data::u = cross(Data::l, Data::r);
    Data::u = Point(0,1,0);
    Data::D = length(Data::pos);
    fin.close();

}

Tf getNearestIntersection(Line ray, Object **obj){
    Tf ret = -1;
    for(auto x : Data::objects){
        Tf t = x -> intersect(ray);
        if(t > 0 && (ret < 0 || t < ret)){
            ret = t;
            if(obj != NULL)
                *obj = x;
        }
    }
    return ret;
}

Color getColor(Line ray, Ti level){
    if(level < 0){
        return {0, 0, 0};
    }
    Color ret = {0, 0, 0};

    Object *obj = NULL;
    Tf t = getNearestIntersection(ray, &obj);
    if(obj != NULL && t > 0){
        Point p = ray.a + ray.v * t;
        if(length(p - Data::pos) > Data::zFar)return {0, 0, 0};
        Color color = obj -> getColor(p);
        Point normal = obj -> getNormal(ray, p);
        Tf ambient = obj -> ambient;
        Tf diffuse = obj -> diffuse;
        Tf specular = obj -> specular;
        Tf reflection = obj -> reflection;
        Tf shininess = obj -> shininess;
        Point reflectedRay = unit(ray.v - normal * dot(ray.v, normal) * 2);
            
        Tf d = length(p - Data::pos);
        Tf lambert = 0, phong = 0;
        for(auto l : Data::lights){
            Point dir = l -> position - p;
            Tf d = length(dir);
            dir = dir / d;
            Tf clash = getNearestIntersection({p - ray.v * EPS2, dir}, NULL);
            if(clash > 0 && clash < d){
                continue;
            }
            Tf scaling_factor = exp(-d * d * l -> falloff);
            lambert += max(dot(normal, dir), 0.0) * scaling_factor;
            phong += pow(max(dot(reflectedRay, dir), 0.0), shininess) * scaling_factor;
        }

        for(auto l : Data::spotLights){
            Point dir = l -> position - p;
            Tf d = length(dir);
            dir = dir / d;
            Tf clash = getNearestIntersection({p - ray.v * EPS2, dir}, NULL);
            if(clash > 0 && clash < d){
                continue;
            }
            Tf angle = acos(dot(unit(dir), unit(l -> direction)));
            angle = min(angle, PI - angle);
            angle = angle * 180 / PI;
            if(abs(angle) > l -> cutoff){
                continue;
            }
            Tf scaling_factor = exp(-d * d * l -> falloff);
            lambert += max(dot(normal, dir), 0.0) * scaling_factor;
            phong += pow(max(dot(reflectedRay, dir), 0.0), shininess) * scaling_factor;
        }
        
        Color reflectedColor = getColor({p + reflectedRay * EPS2, reflectedRay}, level - 1);

        ret = ret + color * (ambient + lambert * diffuse + phong * specular) + reflectedColor * reflection;
    
    }
    return ret;
}

void generateImage(){
    Tf fovy = Data::fov * PI / 180;
    Tf fovx = fovy * Data::aspectRatio;
    // cout << fovy << ' ' << fovx << endl;
    Tfl h = 2 * Data::zNear * tanf64x(fovy / 2);
    Tfl w = 2 * Data::zNear * tanf64x(fovx / 2);
    Tfl dx = w / Data::width;
    Tfl dy = h / Data::height;
    Point pointBuffer[Data::height][Data::width];
    Point p = Data::pos + Data::l * Data::zNear;
    for(Ti i = 0; i < Data::height; i++){
        for(Ti j = 0; j < Data::width; j++){
            pointBuffer[i][j] = p + Data::u * (i - (Tf)Data::height / 2.0 + 0.5) * dy + Data::r * (j - (Tf)Data::width / 2.0 + 0.5) * dx;
        }
    }

    
	bitmap_image image(Data::width, Data::height);
	image.set_all_channels(0, 0, 0);
    int seg = Data::height / 10;
    Color colorBuffer[Data::height][Data::width];
    for(Ti i = 0; i < Data::height; i++){
        for(Ti j = 0; j < Data::width; j++){
            Line ray = {pointBuffer[i][j], unit(pointBuffer[i][j] - Data::pos)};

            colorBuffer[i][j] = getColor(ray, Data::levelOfRecursion) * 255.0;
        }
        if(i % seg == seg - 1){
            cout << "Done " << (i / seg + 1) * 10 << "%" << endl;
        }
    }

    for(Ti i = 0; i < Data::height; i++){
        for(Ti j = 0; j < Data::width; j++){

            image.set_pixel(j, Data::height - 1 - i, colorBuffer[i][j].x, colorBuffer[i][j].y, colorBuffer[i][j].z);
        }
    }
	image.save_image("out.bmp");

}


void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
    glLoadIdentity();               // Reset the model-view matrix
    
    gluLookAt(Data::pos.x, Data::pos.y, Data::pos.z, 
        Data::pos.x + Data::l.x, Data::pos.y + Data::l.y, Data::pos.z + Data::l.z, 
            Data::u.x, Data::u.y, Data::u.z);

    for(auto x : Data::objects){
        x -> draw();
    }

    for(auto x : Data::lights){
        x -> draw();
    }

    for(auto x : Data::spotLights){
        x -> draw();
    }
    glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
}



void reshapeListener(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix
    gluPerspective(Data::fov, aspect, Data::zNear, Data::zFar);
}



void keyboardListener(unsigned char key, int xx,int yy){
    Tf rate = 0.1;
    using namespace Data;
	switch(key){
        case 'w':
            //move camera without changing reference
            pos = pos + l * D;
            l = l * cos(-rate) + u * sin(-rate);
            // u = u * cos(-rate) - l * sin(-rate);
            u = cross(r, l);
            pos = pos - l * D;
            // r = cross(l, u);
            break;
        case 's':
            //move camera without changing reference
            pos = pos + l * D;
            l = l * cos(rate) + u * sin(rate);
            // u = u * cos(rate) - l * sin(rate);
            u = cross(r, l);
            pos = pos - l * D;
            // r = cross(l, u);
            break;
        case 'a':
            //move camera without changing reference
            pos = pos + l * D;
            r = r * cos(-rate) + l * sin(-rate);
            // l = l * cos(-rate) - r * sin(-rate);
            l = cross(u, r);
			pos = pos - l * D;
            // u = cross(r, l);
            break;
        case 'd':
            //move camera without changing reference
            pos = pos + l * D;
            r = r * cos(rate) + l * sin(rate);
            // l = l * cos(rate) - r * sin(rate);
			l = cross(u, r);
            pos = pos - l * D;
            // u = cross(r, l);
            break;
        case '1':
            r = r * cos(rate) + l * sin(rate);
            // l = l * cos(rate) - r * sin(rate);
            // u = cross(r, l);
            l = cross(u, r);
			break;

        case '2':
            r = r * cos(-rate) + l * sin(-rate);
            l = l * cos(-rate) - r * sin(-rate);
            u = cross(r, l);
			break;

        case '3':
            l = l * cos(rate) + u * sin(rate);
            // u = u * cos(rate) - l * sin(rate);
            // r = cross(l, u);
            u = cross(r, l);
			break;

        case '4':
            l = l * cos(-rate) + u * sin(-rate);
            // u = u * cos(-rate) - l * sin(-rate);
            // r = cross(l, u);
            u = cross(r, l);
			break;

        case '5':
            u = u * cos(rate) + r * sin(rate);
            // r = r * cos(rate) - u * sin(rate);
            // l = cross(r, u);
            r = cross(l, u);
			break;

        case '6':
            u = u * cos(-rate) + r * sin(-rate);
            // r = r *  cos(-rate) - u * sin(-rate);
            // l = cross(r, u);
            r = cross(l, u);
			break;
        case '0':
            generateImage();
            break;
		default:
			break;
	}
    l = unit(l);
    r = unit(r);
    u = unit(u);

	glutPostRedisplay();
}


void specialKeyListener(int key, int x,int y)
{
	switch(key){
		case GLUT_KEY_UP:		//down arrow key
			Data::pos = Data::pos + Data::l * 10;
			break;
		case GLUT_KEY_DOWN:		// up arrow key
			Data::pos = Data::pos - Data::l * 10;
			break;

		case GLUT_KEY_RIGHT:
            Data::pos = Data::pos - Data::r * 10;
			break;
		case GLUT_KEY_LEFT :
            Data::pos = Data::pos + Data::r * 10;
			break;

		case GLUT_KEY_PAGE_UP:
            Data::pos = Data::pos + Data::u * 10;
			break;
		case GLUT_KEY_PAGE_DOWN:
            Data::pos = Data::pos - Data::u * 10;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
	glutPostRedisplay();
}








/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    glutInit(&argc, argv);                      // Initialize GLUT
    
    inputData("description.txt");

    glutInitWindowSize(Data::width, Data::height);   // Set the window's initial width & height
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Enable double buffered mode
    glutCreateWindow("Ray Tracing");  // Create window with the given title
    glutDisplayFunc(display);       // Register callback handler for window re-paint event
    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutReshapeFunc(reshapeListener);       // Register callback handler for window re-size event
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Black and opaque
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling

    glutMainLoop();                             // Enter the event-processing loop
    return 0;
}
