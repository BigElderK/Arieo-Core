#pragma once
#define MATHFU_COMPILE_WITHOUT_SIMD_SUPPORT

#include <mathfu/matrix_4x4.h>
#include <mathfu/quaternion.h>
#include <mathfu/vector.h>
#include <mathfu/matrix.h>
#include <mathfu/rect.h>
#include <mathfu/utilities.h>

namespace Arieo::Base::Math
{
    using DefaultMathType = float;
    
    template<typename T, int rows, int columns = rows>
    using Matrix = mathfu::Matrix<T, rows, columns>;

    template<typename T, int columns>
    using Vector = mathfu::Vector<T, columns>;

    template<typename T>
    using Rect = mathfu::Rect<T>;

    using Vector2 = mathfu::Vector<DefaultMathType, 2>;
    using Vector3 = mathfu::Vector<DefaultMathType, 3>;
    using Vector4 = mathfu::Vector<DefaultMathType, 4>;
    using Matrix4 = mathfu::Matrix<DefaultMathType, 4, 4>;
    using Quaternion = mathfu::Quaternion<DefaultMathType>;

    constexpr float m_pi = 3.14159265358979323846;
    inline DefaultMathType covertDegreesToRadians(DefaultMathType degrees)
    {
        return degrees * (m_pi / 180.0f);
    }

    inline DefaultMathType radians(DefaultMathType degrees)
    {
        return degrees * (m_pi / 180.0f);
    }

    class Coordinate
    {
    public:
        static inline Vector3 left(){return Vector3{1.0f, 0.0f, 0.0f};}
        static inline Vector3 up(){return Vector3{0.0f, 1.0f, 0.0f};}
        static inline Vector3 forward(){return Vector3{0.0f, 0.0f, 1.0f};}

        static inline Vector3 right(){return Vector3{-1.0f, 0.0f, 0.0f};}
        static inline Vector3 down(){return Vector3{0.0f, -1.0f, 0.0f};}
        static inline Vector3 backward(){return Vector3{0.0f, 0.0f, -1.0f};}

        static inline Matrix4 getNDCMatrix()
        {
            return Base::Math::Matrix4::FromScaleVector(Base::Math::Vector3(-1.0f, -1.0f, -1.0f));
        }
    };
}