/*
    Author: Shane Lynes
    STL File Generation: Cube and Cylinder
*/


#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

//the Vector class that is required for STL File creation

class Vector{
public:
    Vector(float x, float y, float z) : x(x), y(y), z(z){}


    //float is better for STL
    float x,y,z;


    friend Vector operator *(float c, Vector v);
    friend Vector operator -(Vector v);
    friend Vector operator -(Vector a, Vector b);
};

//vector addition

Vector operator -(Vector v){
    return Vector(0-v.x,0-v.y,0-v.z);
}

//vector subtraction

Vector operator -(Vector a, Vector b){
    float xn, yn, zn;
    xn = a.x - b.x;
    yn = a.y - b.y;
    zn = a.z - b.z;
    return Vector(xn, yn, zn);
}

//vector multiplication

Vector operator *(float c, Vector v){
    return Vector(c*v.x, c*v.y, c*v.z);
}


//the triangle class

class Triangle{
public:
    Vector p1,p2,p3;
    Vector normal;
    Triangle(Vector p1, Vector p2, Vector p3, Vector normal) :
            p1(p1), p2(p2), p3(p3), normal(normal){
    }
};

//the basic shape class

class Shape {
public:
    vector<Triangle> triangle;
    uint32_t triangleCount(){
        return triangle.size();
    }
};

//the cylinder extension of the shape class

class Cylinder : public Shape{
private:
    double r, h;
    int facets;
    double x,y,z;
    double pi;
    
public:
    Cylinder(double x, double y, double z, double r, double h, int facets) :
            x(x), y(y), z(z), r(r), h(h), facets(facets){
        double pi = 3.14159265358979;
        vector<Vector> p;
        Vector center_b(x,y,z);
        Vector center_t(x,y,z+h);
        for (int i = 0; i < facets; i++){
            double angle_n = 2*pi*i/facets;
            double angle_1 = angle_n - pi/facets;

            double angle_2 = angle_n + pi/facets;

            Vector normal = (Vector(cos(angle_n),sin(angle_n),0));
            p.clear();

            p.push_back(Vector(r*cos(angle_1)+x,r*sin(angle_1)+y,z));
            p.push_back(Vector(r*cos(angle_1)+x,r*sin(angle_1)+y,z+h));
            p.push_back(Vector(r*cos(angle_2)+x,r*sin(angle_2)+y,z));
            p.push_back(Vector(r*cos(angle_2)+x,r*sin(angle_2)+y,z+h));

            triangle.push_back(Triangle(p[0],p[2],p[1],normal));
            triangle.push_back(Triangle(p[1],p[2],p[3],normal));

            triangle.push_back(Triangle(p[2],p[0],center_b, Vector(0,0,-1)));
            triangle.push_back(Triangle(p[1],p[3],center_t, Vector(0,0,1)));
        }
    }
};

//the cube extension of the shape class

class Cube: public Shape {
private:
    double l;
    double x,y,z;
public:
    Cube(double x, double y, double z, double size) : x(x), y(y), z(z), l(size) {
        Vector x_n(1,0,0);
        Vector y_n(0,1,0);
        Vector z_n(0,0,1);
        vector<Vector> c;

        c.push_back(Vector(x,y,x));
        c.push_back(Vector(x+l,y,z));
        c.push_back(Vector(x+l,y+l,z));
        c.push_back(Vector(x,y+l,z));
        c.push_back(Vector(x,y,z+l));
        c.push_back(Vector(x+l,y,z+l));
        c.push_back(Vector(x+l,y+l,z+l));
        c.push_back(Vector(x,y+l,z+l));

        triangle.push_back(Triangle(c[0],c[1],c[4],-y_n));

        triangle.push_back(Triangle(c[1],c[5],c[4],-y_n));
        triangle.push_back(Triangle(c[1],c[2],c[6],x_n));
        triangle.push_back(Triangle(c[1],c[6],c[5],x_n));

        triangle.push_back(Triangle(c[5],c[6],c[4],z_n));

        triangle.push_back(Triangle(c[6],c[7],c[4],z_n));

        triangle.push_back(Triangle(c[0],c[3],c[1],-z_n));

        triangle.push_back(Triangle(c[1],c[3],c[2],-z_n));

        triangle.push_back(Triangle(c[0],c[4],c[7],-x_n));

        triangle.push_back(Triangle(c[0],c[7],c[3],-x_n));

        triangle.push_back(Triangle(c[2],c[3],c[7],y_n));
        triangle.push_back(Triangle(c[2],c[7],c[6],y_n));
    }
};


//CAD class that actually writes and generates the STL stlFile by adding createdShape

class CAD {
private:
    vector<Shape*> createdShape;
    char header[80];
public:

    void add(Shape* s){
        createdShape.push_back(s);
    }
    
    void write(string fileName){
        ofstream stlFile;

        cout << "STL File is being written to...." << '\n';

        stlFile.open(fileName, ios::out | ios::binary);
        if (stlFile.good()){
            cout << "yes";
        }

        uint32_t i = 10;

        uint16_t j = 10;

        float f = 900;


        uint32_t length = 0;

        stlFile.write(header,80);

        for (auto s : createdShape)
            length+= s->triangleCount();
        stlFile.write((char*)&length,sizeof(length));
        uint16_t attribute = 0;
        for (auto s: createdShape){
            for (auto t : s->triangle){
                stlFile.write((char*)&t.normal.x,sizeof(float));
                stlFile.write((char*)&t.normal.y,sizeof(float));
                stlFile.write((char*)&t.normal.z,sizeof(float));

                stlFile.write((char*)&t.p1.x,sizeof(float));
                stlFile.write((char*)&t.p1.y,sizeof(float));
                stlFile.write((char*)&t.p1.z,sizeof(float));

                stlFile.write((char*)&t.p2.x,sizeof(float));
                stlFile.write((char*)&t.p2.y,sizeof(float));
                stlFile.write((char*)&t.p2.z,sizeof(float));

                stlFile.write((char*)&t.p3.x,sizeof(float));
                stlFile.write((char*)&t.p3.y,sizeof(float));
                stlFile.write((char*)&t.p3.z,sizeof(float));

                
                stlFile.write((char*)&attribute,sizeof(uint16_t));
            }
        }
    }

};

//https://www.stratasysdirect.com/resources/how-to-prepare-stl-Files/
//https://www.viewstl.com/
int main() {
        CAD c;
        c.add(new Cube(0,0,0,   5));
        c.add(new Cylinder(100,0,0,    3, 10, 10));
        //writing the final stlFile to the same directory this File is located in
    c.write("test.stl");
}