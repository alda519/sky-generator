#include <iostream>
#include <math.h>

using namespace std;

#define PI 3.1415926535897932384626

extern "C"
{
#include "colorbuffer.h"
}

typedef struct         /**< point, also a vector */
  {
    double x;
    double y;
    double z;
  } point_3D;

typedef struct
  {
    point_3D a;
    point_3D b;
    point_3D c;
  } triangle_3D;

class line_3D
  {
    protected:
      /* parametric line equation:
         x(t) = c0 + q0 * t;
         y(t) = c1 + q1 * t;
         z(t) = c2 + q2 * t; */

      double c0;
      double q0;
      double c1;
      double q1;
      double c2;
      double q2;

    public:
      line_3D(point_3D point1, point_3D point2);

        /**<
          Class constructor, makes a line by two given points.

          @param point1 first point, the value t = 0 in parametric
                 equation will give this point
          @param point2 second point, the value t = 1 in parametric
                 equation will give this point
          */

      void get_point(double t, point_3D &point);

        /**<
          Gets a point of this line by given parameter value.

          @param t parameter value
          @param point in this variable the line point will be returned
         */

      bool intersects_triangle(triangle_3D triangle, double &a, double &b, double &c);

        /**<
          Checks whether the line intersects given triangle plus
          computes the barycentric coordination od the intersection in
          the triangle.

          @param a in this variable the first coordination of the
                 barycentric coordinations of the intersection will
                 be returned
          @param b in this variable the second coordination of the
                 barycentric coordinations of the intersection will
                 be returned
          @param c in this variable the third coordination of the
                 barycentric coordinations of the intersection will
                 be returned
          @return true if the triangle is intersected by the line
         */
  };

void print_point(point_3D point)
  {
    cout << "(" << point.x << ", " << point.y << ", " << point.z << ")" << endl;
  }

void cross_product(point_3D vector1, point_3D vector2, point_3D &final_vector)
  {
    final_vector.x = vector1.y * vector2.z - vector1.z * vector2.y;
    final_vector.y = vector1.z * vector2.x - vector1.x * vector2.z;
    final_vector.z = vector1.x * vector2.y - vector1.y * vector2.x;
  }

void substract_vectors(point_3D vector1, point_3D vector2, point_3D &final_vector)
  {
    final_vector.x = vector2.x - vector1.x;
    final_vector.y = vector2.y - vector1.y;
    final_vector.z = vector2.z - vector1.z;
  }

double point_distance(point_3D a, point_3D b)
  {
    point_3D difference;

    substract_vectors(a,b,difference);

    return sqrt(difference.x * difference.x + difference.y * difference.y + difference.z * difference.z);
  }

double vector_length(point_3D vector)
  {
    return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
  }

double dot_product(point_3D vector1, point_3D vector2)
  {
    return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
  }

void normalize(point_3D &vector)
  {
    double length = vector_length(vector);

    vector.x /= length;
    vector.y /= length;
    vector.z /= length;
  }

double vectors_angle(point_3D vector1, point_3D vector2)
  {
    normalize(vector1);
    normalize(vector2);
    return acos(dot_product(vector1,vector2));
  }

line_3D::line_3D(point_3D point1, point_3D point2)
  {
    this->c0 = point1.x;
    this->q0 = point2.x - point1.x;
    this->c1 = point1.y;
    this->q1 = point2.y - point1.y;
    this->c2 = point1.z;
    this->q2 = point2.z - point1.z;
  }

void revert_vector(point_3D &vector)
  {
    vector.x *= -1;
    vector.y *= -1;
    vector.z *= -1;
  }

void line_3D::get_point(double t, point_3D &point)
  {
    point.x = this->c0 + this->q0 * t;
    point.y = this->c1 + this->q1 * t;
    point.z = this->c2 + this->q2 * t;
  }

bool line_3D::intersects_triangle(triangle_3D triangle, double &a, double &b, double &c)
  {
    point_3D vector1,vector2,vector3,normal;

    a = 0.0;
    b = 0.0;
    c = 0.0;

    substract_vectors(triangle.a,triangle.b,vector1);
    substract_vectors(triangle.a,triangle.c,vector2);

    cross_product(vector1,vector2,normal);

    /*
     Compute general plane equation in form
     qa * x + qb * y + qc * z + d = 0:
     */

    double qa = normal.x;
    double qb = normal.y;
    double qc = normal.z;
    double d = -1 * (qa * triangle.a.x + qb * triangle.a.y + qc * triangle.a.z);

    /* Solve for t: */

    double denominator = (qa * this->q0 + qb * this->q1 + qc * this->q2);

    if (denominator == 0)
      return false;

    double t = (-qa * this->c0 - qb * this->c1 - qc * this->c2 - d) / denominator;

    /* t now contains parameter value for the intersection */

    if (t < 0.0)
      return false;

    point_3D intersection;

    this->get_point(t,intersection);  // intersection in 3D space

    // vectors from the intersection to each triangle vertex:

    substract_vectors(intersection,triangle.a,vector1);
    substract_vectors(intersection,triangle.b,vector2);
    substract_vectors(intersection,triangle.c,vector3);

    point_3D normal1, normal2, normal3;

    // now multiply the vectors to get their normals:

    cross_product(vector1,vector2,normal1);
    cross_product(vector2,vector3,normal2);
    cross_product(vector3,vector1,normal3);

    // if one of the vectors points in other direction than the others, the point is not inside the triangle:

    if (dot_product(normal1,normal2) < 0 || dot_product(normal2,normal3) < 0)
      return false;

    // now compute the barycentric coordination:

    double dab, dac, dbc, da, db, dc;
    double alpha, beta, gamma;
    double alpha2, beta2, gamma2;
    double helper;
    point_3D helper_vector1, helper_vector2;

    dab = point_distance(triangle.a,triangle.b); // side lengths
    dac = point_distance(triangle.a,triangle.c);
    dbc = point_distance(triangle.b,triangle.c);

    da = point_distance(triangle.a,intersection); // distance from sides to intersection
    db = point_distance(triangle.b,intersection);
    dc = point_distance(triangle.c,intersection);

    revert_vector(vector1); // A to intersection
    revert_vector(vector2); // B to intersection
    revert_vector(vector3); // C to intersection

    substract_vectors(triangle.c,triangle.a,helper_vector1);
    substract_vectors(triangle.b,triangle.a,helper_vector2);
    alpha = vectors_angle(helper_vector1,helper_vector2);
    alpha2 = vectors_angle(vector1,helper_vector1);

    helper = (alpha - alpha2) / (alpha);
    a = 1.0 - da / (helper * dac + (1 - helper) * dac);

    substract_vectors(triangle.a,triangle.b,helper_vector1);
    substract_vectors(triangle.c,triangle.b,helper_vector2);
    beta = vectors_angle(helper_vector1,helper_vector2);
    beta2 = vectors_angle(vector2,helper_vector1);

    helper = (beta - beta2) / (beta);
    b = 1.0 - db / (helper * dab + (1 - helper) * dab);

    substract_vectors(triangle.a,triangle.c,helper_vector1);
    substract_vectors(triangle.b,triangle.c,helper_vector2);
    gamma = vectors_angle(helper_vector1,helper_vector2);
    gamma2 = vectors_angle(vector3,helper_vector1);

    helper = (gamma - gamma2) / (gamma);
    c = 1.0 - dc / (helper * dac + (1 - helper) * dac);

    // now adjust the coords to sum up to 1.0:

    double sum = a + b + c;

    a /= sum;
    b /= sum;
    c /= sum;

    return true;
  }

int main(void)
  {
    point_3D p1,p2,p3,pt1,pt2,pt3;

    triangle_3D triangle;

    pt1.x = -1.0;
    pt1.y = 0.517;
    pt1.z = 0.349;

    pt2.x = 0.585;
    pt2.y = 3.095;
    pt2.z = -1.098;

    pt3.x = 0.062;
    pt3.y = 5.095;
    pt3.z = 1.0;

    triangle.a = pt1;
    triangle.b = pt2;
    triangle.c = pt3;

    unsigned int width, height, i, j;
    t_color_buffer buffer;
    t_color_buffer texture;

    width = 640;
    height = 480;

    color_buffer_init(&buffer,width,height);
    color_buffer_load_from_png(&texture,"water.png");

    p1.x = 0.0;
    p1.y = 0.0;
    p1.z = 0.0;

    double a,b,c;

    for (j = 0; j < height; j++)
      {
        for (i = 0; i < width; i++)
          {
            p2.x = ((i / ((double) width)) - 0.5);
            p2.y = 0.5;
            p2.z = ((j / ((double) height)) - 0.5) * 0.8;

            line_3D line(p1,p2);

            if (line.intersects_triangle(triangle,a,b,c))
              {
                //color_buffer_set_pixel(&buffer,i,j,255,255,0);

                int coord_x = b * texture.width;
                int coord_y = c * texture.width;

                unsigned char r,g,b;

                color_buffer_get_pixel(&texture,coord_x,coord_y,&r,&g,&b);

                color_buffer_set_pixel(&buffer,i,j,r,g,b);
                //color_buffer_set_pixel(&buffer,i,j,a < 0.5 ? 0 : 255,b < 0.5 ? 0 : 255,c < 0.5 ? 0 : 255);

            /*    if (a > 0.9 || b > 0.9 || c > 0.9)
                  color_buffer_set_pixel(&buffer,i,j,a * 255,b * 255,c * 255);
                else
                 color_buffer_set_pixel(&buffer,i,j,0,0,0);
*/
              }
            else
              color_buffer_set_pixel(&buffer,i,j,0,0,0);
          }

        cout << "line " << j << endl;
      }

    color_buffer_save_to_png(&buffer,"picture.png");
    color_buffer_destroy(&buffer);


    point_3D f,g;

    f.x = 0.0;
    f.y = 0.0;
    f.z = 0.0;


    g.x = 1.0;
    g.y = 1.0;
    g.z = 1.0;

    return 0;
  }
