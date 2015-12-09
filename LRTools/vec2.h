#ifndef engine_vec2_h
#define engine_vec2_h

namespace engine {
    
    template <typename T>
    struct vec2 {
        T x, y;
        
        vec2() : x(0), y(0) {}
        vec2(T x, T y) : x(x), y(y) {}
        vec2(const vec2<T>& other) : x(other.x), y(other.y) {}
        
        vec2<T> operator +(T rhs) const { return vec2(x + rhs, y + rhs); }
        vec2<T> operator -(T rhs) const { return vec2(x - rhs, y - rhs); }
        vec2<T> operator *(T rhs) const { return vec2(x * rhs, y * rhs); }
        vec2<T> operator /(T rhs) const { return vec2(x / rhs, y / rhs); }
        
        vec2<T>& operator +=(T rhs) { x += rhs; y += rhs; return *this; }
        vec2<T>& operator -=(T rhs) { x -= rhs; y -= rhs; return *this; }
        vec2<T>& operator *=(T rhs) { x *= rhs; y *= rhs; return *this; }
        vec2<T>& operator /=(T rhs) { x /= rhs; y /= rhs; return *this; }
        
        vec2<T> operator +(const vec2<T>& rhs) const { return vec2(x + rhs.x, y + rhs.y); }
        vec2<T> operator -(const vec2<T>& rhs) const { return vec2(x - rhs.x, y - rhs.y); }
        vec2<T> operator *(const vec2<T>& rhs) const { return vec2(x * rhs.x, y * rhs.y); }
        vec2<T> operator /(const vec2<T>& rhs) const { return vec2(x / rhs.x, y / rhs.y); }
        
        vec2<T>& operator +=(const vec2<T>& rhs) { x += rhs.x; y += rhs.y; return *this; }
        vec2<T>& operator -=(const vec2<T>& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
        vec2<T>& operator *=(const vec2<T>& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
        vec2<T>& operator /=(const vec2<T>& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
        
        bool operator ==(const vec2<T>& rhs) { return x == rhs.x && y == rhs.y; }
        bool operator !=(const vec2<T>& rhs) { return x != rhs.x || y != rhs.y; }
        
        T magnitude()
        {
            return sqrt((x * x) + (y * y));
        }
        
        T dot(const vec2<T>& rhs)
        {
            return (x * rhs.x) + (y * rhs.y);
        }
    };
    
    typedef vec2<double> vec2d;
}

#endif
