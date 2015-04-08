/**
 * @file jMath.h
 * @date 2012/06/25
 * @author Julius Peinelt
 *
 * Copyright Julius Peinelt
 *
 * jMath is a collection of vector and matrix templates. It is written to use it
 * together with OpenGL and to compute rotations and translocations. It contains
 * templates for 2-, 3- and 4-dimensional vectors and 4x4 matrices.
 */

#ifndef JMATH_H
#define	JMATH_H

#include <string.h>
#include <cmath>
#include <GL/freeglut.h>

#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif


#define DEG2RAD(x) ((x * M_PI) / 180.0)

#ifndef EPSILON
const double epsilon = 4.37114e-05;
#define EPSILON epsilon
#endif

/*
 * Class for 2D Vector
 */
template<class T>
class Vector2 {
public:
    
    union {
        T x; //for vertex x-coordinate
        T u; //for texture u-coordinate
    };
    
    union {
        T y; //for vertex y-coordinate
        T v; //for texture v-coordinate
    };
    
    
    /**
     * Creates a zero Vector.
     */
    Vector2() : x(0), y(0) {}
    
    /**
     * Copy vector values from array.
     */
    Vector2(T ax, T ay) : x(ax), y(ay) {}
    
    /**
     * Copy constructor.
     * @param orig Data source for new instance of Vector.
     */
    Vector2(const Vector2& orig) {
        x = orig.x;
        y = orig.y;
    }
    
    /**
     * Copy casting constructor.
     * @param src Data source for new created instance of Vector2.
     */
    template<class FromT>
    Vector2(const Vector2<FromT>& orig) {
        x = static_cast<T> (orig.x);
        y = static_cast<T> (orig.y);
    }
    
    
    
    //------------------------ operators -------------------------------
    
    
    
    Vector2<T> operator=(const Vector2<T>& rhs) {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }
    
    /**
     * Casting copy operator
     */
    template<class FromT>
    Vector2<T> operator=(const Vector2<FromT>& rhs) {
        x = static_cast<T>(rhs.x);
        y = static_cast<T>(rhs.y);
        return *this;
    }

    
    bool operator==(const Vector2<T>& rhs) const {
        if ((std::fabs(x - rhs.x) >= EPSILON) && (std::fabs(y - rhs.y) >= EPSILON)) {
                return false;
        }
        return true;
    }
    
    
    bool operator!=(const Vector2<T>& rhs) const {
        return !(*this == rhs);
    }
    
    
    Vector2<T> operator+(const Vector2<T>& rhs) const {
        return Vector2<T>(x + rhs.x, y + rhs.y);
    }

    
    Vector2<T> operator-(const Vector2<T>& rhs) const {
        return Vector2<T>(x - rhs.x, y - rhs.y);
    }
    
    Vector2<T> operator*(const Vector2<T>& rhs) const {
        return Vector2<T >(x * rhs.x, y * rhs.y);
    }
    
    Vector2<T> operator/(const Vector2<T>& rhs) const {
        return Vector2<T >(x / rhs.x, y / rhs.y);
    }

    
    Vector2<T> operator+(T scalar) const {
        return Vector2<T >(x + scalar, y + scalar);
    }

    
    Vector2<T> operator-(T scalar) const {
        return Vector2<T >(x - scalar, y - scalar);
    }

    
    Vector2<T> operator*(T scalar) const {
        return Vector2<T >(x * scalar, y * scalar);
    }

    
    Vector2<T> operator/(T scalar) const {
        return Vector2<T >(x / scalar, y / scalar);
    }
    
    /**
     * Returns the dot product of this vector and the passed argument vector.
     * @param rhs a vector which serves as the other factor for the dot product.
     */
    T dotProduct(const Vector2<T>& rhs) const {
        return x * rhs.x + y * rhs.y;
    }
    
    /**
     * Returns the length of the vector in the euclidian metric.
     */
    T length() {
        return (T) std::sqrt(x*x + y*y);
    }
    
    /**
     * Normalizes the vector.
     */
    void normalize() {
	T s = length();
	x /= s;
	y /= s;
    }


    //---------------------- conversion -----------------------------

    /**
     * Conversion to pointer operator
     * @return Pointer to internally stored data.
     */
    operator T*() const {
        return (T*) this;
    }

};

/// Vector2 of floats
typedef Vector2<float> Vector2f;
/// Vector2 of doubles
typedef Vector2<double> Vector2d;
/// Vector2 of int
typedef Vector2<int> Vector2i;

///////////////////////////////////////////////////////

/*
 * Class for 3D Vector
 */
template<class T>
class Vector3 {
public:
    
    union {
        T x; //for vertex x-coordinate
        T i; //for normal i-coordinate
        T r; //for color r-component
    };
    
    union {
        T y; //for vertex y-coordinate
        T j; //for normal j-coordinate
        T g; //for color g-component
    };
    
    union {
        T z; //for vertex z-coordinate
        T k; //for normal k-coordinate
        T b; //for color b-component
    };
    
    
    /**
     * Creates zero Vector
     */
    Vector3() : x(0), y(0), z(0) {}
    
    /**
     * Copy vector values from array
     */
    Vector3(T ax, T ay, T az) : x(ax), y(ay), z(az) {}
    
    /**
     * Copy constructor.
     * @param orig Data source for new instance of Vector.
     */
    Vector3(const Vector3& orig) {
        x = orig.x;
        y = orig.y;
        z = orig.z;
    }
    
    /**
     * Copy casting constructor.
     * @param src Data source for new created instance of Vector3.
     */
    template<class FromT>
    Vector3(const Vector3<FromT>& orig) {
        x = static_cast<T> (orig.x);
        y = static_cast<T> (orig.y);
        z = static_cast<T> (orig.z);
    }
    
    
    //------------------------ operators -------------------------------
    
    Vector3<T> operator=(const Vector3<T>& rhs) {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }
    
    /**
     * Casting copy operator
     */
    template<class FromT>
    Vector3<T> operator=(const Vector3<FromT>& rhs) {
        x = static_cast<T>(rhs.x);
        y = static_cast<T>(rhs.y);
        z = static_cast<T>(rhs.z);
        return *this;
    }

    
    bool operator==(const Vector3<T>& rhs) const {
        if ((std::fabs(x - rhs.x) >= EPSILON) && (std::fabs(y - rhs.y) >= EPSILON) 
                    && (std::fabs(z - rhs.z) >= EPSILON)) {
           return false;
        }
        return true;
    }

    bool operator!=(const Vector3<T>& rhs) const {
        return !(*this == rhs);
    }
    
    
    Vector3<T> operator+(const Vector3<T>& rhs) const {
        return Vector3<T>(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    
    Vector3<T> operator-(const Vector3<T>& rhs) const {
        return Vector3<T>(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    
    Vector3<T> operator*(const Vector3<T>& rhs) const {
        return Vector3<T >(x * rhs.x, y * rhs.y, z * rhs.z);
    }
    
    Vector3<T> operator/(const Vector3<T>& rhs) const {
        return Vector3<T >(x / rhs.x, y / rhs.y, z / rhs.z);
    }

    
    Vector3<T> operator+(T scalar) const {
        return Vector3<T >(x + scalar, y + scalar, z + scalar);
    }

    
    Vector3<T> operator-(T scalar) const {
        return Vector3<T >(x - scalar, y - scalar, z - scalar);
    }

    
    Vector3<T> operator*(T scalar) const {
        return Vector3<T >(x * scalar, y * scalar, z * scalar);
    }

    
    Vector3<T> operator/(T scalar) const {
        return Vector3<T >(x / scalar, y / scalar, z / scalar);
    }

    /**
     * Computes and returns the crossproduct of this and a commited vector.
     * @param rhs a vector which serves as other factor for the crossproduct
     */
    Vector3<T> crossproduct(Vector3<T> rhs) const {
        return Vector3<T>(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
    }
    
    /**
     * Computes and returns the dotproduct of this and a commited vector.
     * @param rhs a vector which serves as other factor for the dotproduct
     */
    T dotProduct(const Vector3<T>& rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }
    
    /**
     * Returns the length of this vector.
     */
    T length() {
        return (T) std::sqrt(x*x + y*y + z*z);
    }
    
    /**
     * Normalizes the vector.
     */
    void normalize() {
	T s = length();
	x /= s;
	y /= s;
	z /= s;
    }


    //---------------------- conversion -----------------------------

    /**
     * Conversion to pointer operator
     * @return Pointer to internally stored data.
     */
    operator T*() const {
        return (T*) this;
    }

};

/// Vector3 of floats
typedef Vector3<float> Vector3f;
/// Vector3 of doubles
typedef Vector3<double> Vector3d;
/// Vector3 of int
typedef Vector3<int> Vector3i;
/// Vector3 of GLfloat
typedef Vector3<GLfloat> Vector3GLf;

///////////////////////////////////////////////////////////////



/*
 * Class for 4D Vector
 */
template<class T>
class Vector4 {
public:
    
    union {
        T x; //for vertex x-coordinate
        T r; //for color r-component
    };
    
    union {
        T y; //for vertex y-coordinate
        T g; //for color g-component
    };
    
    union {
        T z; //for vertex z-coordinate
        T b; //for color b-component
    };
    
    union {
        T w; //for vertex w-coordinate
        T a; //for color a-component
    };
    
    
    /**
     * Creates zero Vector
     */
    Vector4() : x(0), y(0), z(0), w(0){}
    
    /**
     * Copy vector values from array
     */
    Vector4(T ax, T ay, T az, T aw) : x(ax), y(ay), z(az), w(aw) {}
    
    /**
     * Copy constructor.
     * @param orig Data source for new instance of Vector.
     */
    Vector4(const Vector4& orig) {
        x = orig.x;
        y = orig.y;
        z = orig.z;
        w = orig.w;
    }
    
    /**
     * Copy casting constructor.
     * @param src Data source for new created instance of Vector2.
     */
    template<class FromT>
    Vector4(const Vector4<FromT>& orig) {
        x = static_cast<T> (orig.x);
        y = static_cast<T> (orig.y);
        z = static_cast<T> (orig.z);
        w = static_cast<T> (orig.w);
    }
    
    
    
    //------------------------ operators -------------------------------
    
    
    
    Vector4<T> operator=(const Vector4<T>& rhs) {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;
        return *this;
    }
    
    /**
     * Casting copy operator
     */
    template<class FromT>
    Vector4<T> operator=(const Vector4<FromT>& rhs) {
        x = static_cast<T>(rhs.x);
        y = static_cast<T>(rhs.y);
        z = static_cast<T>(rhs.z);
        w = static_cast<T>(rhs.w);
        return *this;
    }

    
    bool operator==(const Vector4<T>& rhs) const {
        if ((std::fabs(x - rhs.x) >= EPSILON) && (std::fabs(y - rhs.y) >= EPSILON) 
                && (std::fabs(z - rhs.z) >= EPSILON) && (std::fabs(w - rhs.w) >= EPSILON)) {
            return false;
        }
        return true;
    }
    
    
    bool operator!=(const Vector4<T>& rhs) const {
        return !(*this == rhs);
    }
    
    
    Vector4<T> operator+(const Vector4<T>& rhs) const {
        return Vector4<T>(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }

    
    Vector4<T> operator-(const Vector4<T>& rhs) const {
        return Vector4<T>(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    }
    
    Vector4<T> operator*(const Vector4<T>& rhs) const {
        return Vector4<T >(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
    }
    
    Vector4<T> operator/(const Vector4<T>& rhs) const {
        return Vector4<T >(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
    }

    
    Vector4<T> operator+(T scalar) const {
        return Vector4<T >(x + scalar, y + scalar, z + scalar, w + scalar);
    }

    
    Vector4<T> operator-(T scalar) const {
        return Vector4<T >(x - scalar, y - scalar, z - scalar, w - scalar);
    }

    
    Vector4<T> operator*(T scalar) const {
        return Vector4<T >(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    
    Vector4<T> operator/(T scalar) const {
        return Vector4<T >(x / scalar, y / scalar, z / scalar, w / scalar);
    }
    
    /**
     * Computes and returns the dotproduct of this and a commited vector.
     * @param rhs a vector which serves as other factor for the dotproduct
     */
    T dotProduct(const Vector4<T>& rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
    }
    
    /**
     * Returns the legth of this vector.
     */
    T length() {
        return (T) std::sqrt(x*x + y*y + z*z + w*w);
    }
    
    /**
     * Normalizes this vector.
     */
    void normalize() {
	T s = length();
	x /= s;
	y /= s;
        z /= s;
        w /= s;
    }


    //---------------------- conversion -----------------------------

    /**
     * Conversion to pointer operator
     * @return Pointer to internally stored data.
     */
    operator T*() const {
        return (T*) this;
    }

};

/// Vector4 of floats
typedef Vector4<float> Vector4f;
/// Vector4 of doubles
typedef Vector4<double> Vector4d;
/// Vector4 of int
typedef Vector4<int> Vector4i;

///////////////////////////////////////////////////////



/**
 * Class for matrix 4x4 
 * Data is in column major order for use with OpenGL.
 */
template<class T>
class Matrix {
public:
    /// Data stored in column major order
    T data[16];

    
    /**
     *Creates identity matrix
     */
    Matrix() {
        for (int i = 0; i < 16; i++)
            data[i] = (i % 5) ? 0 : 1;
    }

    /**
     * Copy matrix values from array (these data must be in column
     * major order!)
     */
    Matrix(const T * dt) {
        memcpy(data, dt, sizeof (T) * 16);
    }

    /**
     * Copy constructor.
     * @param orig Data source for new created instance of Matrix.
     */
    Matrix(const Matrix<T>& orig) {
        memcpy(data, orig.data, sizeof (T) * 16);
    }

    /**
     * Copy casting constructor.
     * @param src Data source for new created instance of Matrix.
     */
    template<class FromT>
    Matrix(const Matrix<FromT>& src) {
        for (int i = 0; i < 16; i++) {
            data[i] = static_cast<T> (src.data[i]);
        }
    }
    

    /**
     * Sets matrix to identity matrix.
     */
    void identity() {
        for (int i = 0; i < 16; i++)
            data[i] = (i % 5) ? 0 : 1;
    }

    /**
     * Creates rotation matrix by rotation around axis.
     * @param xDeg Angle (in degrees) of rotation around X axis.
     * @param yDeg Angle (in degrees) of rotation around Y axis.
     * @param zDeg Angle (in degrees) of rotation around Z axis.
     */
    static Matrix<T> createRotationAroundAxis(T xDeg, T yDeg, T zDeg) {
        T xRads(DEG2RAD(xDeg));
        T yRads(DEG2RAD(yDeg));
        T zRads(DEG2RAD(zDeg));

        Matrix<T> m1, m2, m3;
        float c1 = cos(xRads);
        float s1 = sin(xRads);
        float c2 = cos(yRads);
        float s2 = sin(yRads);
        float c3 = cos(zRads);
        float s3 = sin(zRads);

        m1.at(1, 1) = c1;
        m1.at(2, 1) = s1;
        m1.at(1, 2) = -s1;
        m1.at(2, 2) = c1;

        m2.at(0, 0) = c2;
        m2.at(2, 0) = -s2;
        m2.at(0, 2) = s2;
        m2.at(2, 2) = c2;

        m3.at(0, 0) = c3;
        m3.at(1, 0) = s3;
        m3.at(0, 1) = -s3;
        m3.at(1, 1) = c3;

        Matrix<T> ret = m1 * m2 * m3;
        return ret;
    }

    /**
     * Creates translation matrix.
     * @param x X-direction translation
     * @param y Y-direction translation
     * @param z Z-direction translation
     * @param w for W-coordinate translation (default to 1)
     */
    static Matrix<T> createTranslation(T x, T y, T z, T w = 1) {
        Matrix ret;
        ret.at(3, 0) = x;
        ret.at(3, 1) = y;
        ret.at(3, 2) = z;
        ret.at(3, 3) = w;

        return ret;
    }    

    /**
     * Creates new matrix 4x4 from array that represents such matrix 4x4
     * as array of tightly packed elements in row major order.
     * @param arr An array of elements for 4x4 matrix in row major order.
     * @return An instance of Matrix4<T> representing @a arr
     */
    template<class FromT>
    static Matrix<T> fromRowMajorArray(const FromT* arr) {
        const T retData[] ={static_cast<T> (arr[0]), static_cast<T> (arr[4]), static_cast<T> (arr[8]), static_cast<T> (arr[12]),
            static_cast<T> (arr[1]), static_cast<T> (arr[5]), static_cast<T> (arr[9]), static_cast<T> (arr[13]),
            static_cast<T> (arr[2]), static_cast<T> (arr[6]), static_cast<T> (arr[10]), static_cast<T> (arr[14]),
            static_cast<T> (arr[3]), static_cast<T> (arr[7]), static_cast<T> (arr[11]), static_cast<T> (arr[15])};

        return retData;
    }

    /**
     * Creates new matrix 4x4 from array that represents such matrix 4x4
     * as array of tightly packed elements in column major order.
     * @param arr An array of elements for 4x4 matrix in column major order.
     * @return An instance of Matrix4<T> representing @a arr
     */
    template<class FromT>
    static Matrix<T> fromColumnMajorArray(const FromT* arr) {
        const T retData[] ={static_cast<T> (arr[0]), static_cast<T> (arr[1]), static_cast<T> (arr[2]), static_cast<T> (arr[3]),
            static_cast<T> (arr[4]), static_cast<T> (arr[5]), static_cast<T> (arr[6]), static_cast<T> (arr[7]),
            static_cast<T> (arr[8]), static_cast<T> (arr[9]), static_cast<T> (arr[10]), static_cast<T> (arr[11]),
            static_cast<T> (arr[12]), static_cast<T> (arr[13]), static_cast<T> (arr[14]), static_cast<T> (arr[15])};

        return retData;
    }

    //------------------------ operators -------------------------------

    /**
     * Equality test operator, inaccuracy of EPSILON is allowed.
     * @param rhs Right hand side argument of binary operator.
     */
    bool operator==(const Matrix<T>& rhs) const {
        for (int i = 0; i < 16; i++) {
            if (std::fabs(data[i] - rhs.data[i]) >= EPSILON
                    )
                return false;
        }
        return true;
    }

    /**
     * Inequality test operator
     * @param rhs Right hand side argument of binary operator.
     * @return not (lhs == rhs)
     */
    bool operator!=(const Matrix<T>& rhs) const {
        return !(*this == rhs);
    }

    /**
     * Get reference of element at position (x,y).
     * @param x Number of column (0-3)
     * @param y Number of row (0-3)
     */
    T& at(int x, int y) {
        return data[x * 4 + y];
    }
    
    /**
     * Get reference of element at position (x,y).
     * @param x Number of column (0-3)
     * @param y Number of row (0-3)
     */
    const T& at(int x, int y) const {
        return data[x * 4 + y];
    }

    /**
     * Get reference of element at position (i,j), with math matrix notation
     * @param i Number of row (1..4)
     * @param j Number of column (1..4)
     */
    T& operator()(int i, int j) {
        return data[(j - 1) * 4 + i - 1];
    }

    /**
     * Sets translation part of matrix.
     * @param v Vector of translation to be set.
     */
    void setTranslation(const Vector3<T>& v) {
        at(3, 0) = v.x;
        at(3, 1) = v.y;
        at(3, 2) = v.z;
        at(3, 3) = 1;
    }

    /**
     * Returns the translation-vector of this matrix.
     */
    Vector3<T> getTranslation() {
        return Vector3<T > (at(3, 0), at(3, 1), at(3, 2));
    }

    /**
     * Sets rotation part (matrix 3x3) of matrix.
     * @param m Rotation part of matrix
     */
    void setRotation(Matrix<T>& m) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                at(i, j) = m.at(i, j);
            }
        }
    }

    /**
     * Copy operator
     * @param rhs Right hand side argument of binary operator.
     */
    Matrix<T>& operator=(const Matrix<T>& rhs) {
        memcpy(data, rhs.data, sizeof (T) * 16);
        return *this;
    }

    /**
     * Copy casting operator
     * @param rhs Right hand side argument of binary operator.
     */
    template<class FromT>
    Matrix<T>& operator=(const Matrix<FromT>& rhs) {
        for (int i = 0; i < 16; i++) {
            data[i] = static_cast<T> (rhs.data[i]);
        }
        return *this;
    }

    /**
     * Copy operator
     * @param rhs Right hand side argument of binary operator.
     */
    Matrix<T>& operator=(const T* rhs) {
        memcpy(data, rhs, sizeof (T) * 16);
        return *this;
    }


    /**
     * Addition operator
     * @param rhs Right hand side argument of binary operator.
     */
    Matrix<T> operator+(const Matrix<T>& rhs) const {
        Matrix<T> ret;
        for (int i = 0; i < 16; i++)
            ret.data[i] = data[i] + rhs.data[i];
        return ret;
    }

    /**
     * Subtraction operator
     * @param rhs Right hand side argument of binary operator.
     */
    Matrix<T> operator-(const Matrix<T>& rhs) const {
        Matrix<T> ret;
        for (int i = 0; i < 16; i++)
            ret.data[i] = data[i] - rhs.data[i];
        return ret;
    }


    /**
     * Addition operator
     * @param rhs Right hand side argument of binary operator.
     */
    Matrix<T> operator+(T rhs) const {
        Matrix<T> ret;
        for (int i = 0; i < 16; i++)
            ret.data[i] = data[i] + rhs;
        return ret;
    }

    /**
     * Subtraction operator
     * @param rhs Right hand side argument of binary operator.
     */
    Matrix<T> operator-(T rhs) const {
        Matrix<T> ret;
        for (int i = 0; i < 16; i++)
            ret.data[i] = data[i] - rhs;
        return ret;
    }

    /**
     * Multiplication operator
     * @param rhs Right hand side argument of binary operator.
     */
    Matrix<T> operator*(T rhs) const {
        Matrix<T> ret;
        for (int i = 0; i < 16; i++)
            ret.data[i] = data[i] * rhs;
        return ret;
    }

    /**
     * Division operator
     * @param rhs Right hand side argument of binary operator.
     */
    Matrix<T> operator/(T rhs) const {
        Matrix<T> ret;
        for (int i = 0; i < 16; i++)
            ret.data[i] = data[i] / rhs;
        return ret;
    }

    /**
     * Multiplication operator
     * @param rhs Right hand side argument of binary operator.
     */
    Vector3<T> operator*(const Vector3<T>& rhs) const {
        return Vector3<T> (data[0] * rhs.x + data[4] * rhs.y + data[8] * rhs.z,
                data[1] * rhs.x + data[5] * rhs.y + data[9] * rhs.z,
                data[2] * rhs.x + data[6] * rhs.y + data[10] * rhs.z);
    }

    /**
     * Multiplication operator
     * @param rhs Right hand side argument of binary operator.
     */
    Matrix<T> operator*(const Matrix<T>& rhs) const {
        Matrix<T> w;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                T n = 0;
                for (int k = 0; k < 4; k++) {
                    n += rhs.at(i, k) * at(k, j);
                }
                w.at(i, j) = n;
            }
        }
        return w;
    }

    /**
     * returns the transpose matrix.
     */
    Matrix<T> transpose() {
        Matrix<T> ret;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                ret.at(i, j) = at(j, i);
            }
        }
        return ret;
    }


    //---------------------- conversion -----------------------------

    /**
     * Conversion to pointer operator
     * @return Pointer to internally stored data for gl*[fd]v functions.
     */
    operator T*() {
        return (T*) data;
    }

    /**
     * Conversion to pointer operator
     * @return Constant Pointer to internally stored data for gl*[fd]v functions.
     */
    operator const T*() const {
        return (const T*) data;
    }
};

/// Matrix 4x4 of floats
typedef Matrix<float> Matrix4f;
/// Matrix 4x4 of doubles
typedef Matrix<double> Matrix4d;
/// Matrix 4x4 of int
typedef Matrix<int> Matrix4i;


#endif	/* JMATH_H */
