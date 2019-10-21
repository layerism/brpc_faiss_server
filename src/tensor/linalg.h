#ifndef _LINALG_H
#define _LINALG_H

#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <cstdint>
#include <numeric>
#include <cmath>
#include <assert.h>


#define DECLARE_PAIRWISE_OPERATOR(OP) \
    template<typename T> \
    std::vector<T> operator OP (std::vector<T> &x1, std::vector<T> &x2){ \
        assert(x1.size() == x2.size()); \
        std::vector<T> out(x1.size()); \
        for (uint32_t i = 0; i < x1.size(); ++i) { \
            out[i] = x1[i] OP x2[i]; \
        } \
        return out; \
    }



#define DECLARE_RIGHT_OPERATOR(OP) \
    template<typename T> \
    std::vector<T> operator OP (T x1, std::vector<T> &x2){ \
        std::vector<T> out(x2.size()); \
        for (uint32_t i = 0; i < x2.size(); ++i) { \
            out[i] = x1 OP x2[i]; \
        } \
        return out; \
    }



#define DECLARE_LEFT_OPERATOR(OP) \
    template<typename T> \
    std::vector<T> operator OP (std::vector<T> &x1, T x2){ \
        std::vector<T> out(x1.size()); \
        for (uint32_t i = 0; i < x1.size(); ++i) { \
            out[i] = x1[i] OP x2; \
        } \
        return out; \
    }



#define DECLARE_INPLACE_PAIRWISE_OPERATOR(OP) \
    template<typename T> \
    std::vector<T> &operator OP (std::vector<T> &x1, std::vector<T> &x2){ \
        assert(x1.size() == x2.size()); \
        for (uint32_t i = 0; i < x1.size(); ++i) { \
            x1[i] OP x2[i]; \
        } \
        return x1; \
    }



#define DECLARE_INPLACE_OPERATOR(OP) \
    template<typename T> \
    std::vector<T> &operator OP (std::vector<T> &x1, T x2){ \
        for (uint32_t i = 0; i < x1.size(); ++i) { \
            x1[i] OP x2; \
        } \
        return x1; \
    }



DECLARE_PAIRWISE_OPERATOR(+);     // return vec1 + vec2
DECLARE_PAIRWISE_OPERATOR(-);     // return vec1 - vec2
DECLARE_PAIRWISE_OPERATOR(*);     // return vec1 * vec2
DECLARE_PAIRWISE_OPERATOR(/);     // return vec1 / vec2

DECLARE_LEFT_OPERATOR(+);     // return vec + a
DECLARE_LEFT_OPERATOR(-);     // return vec - a
DECLARE_LEFT_OPERATOR(*);     // return vec * a
DECLARE_LEFT_OPERATOR(/);     // return vec / a

DECLARE_RIGHT_OPERATOR(+);     // return a + vec
DECLARE_RIGHT_OPERATOR(-);     // return a - vec
DECLARE_RIGHT_OPERATOR(*);     // return a * vec
DECLARE_RIGHT_OPERATOR(/);     // return a / vec

DECLARE_INPLACE_PAIRWISE_OPERATOR( += );     // vec1 += vec2
DECLARE_INPLACE_PAIRWISE_OPERATOR( -= );     // vec1 -= vec2
DECLARE_INPLACE_PAIRWISE_OPERATOR( *= );     // vec1 *= vec2
DECLARE_INPLACE_PAIRWISE_OPERATOR( /= );     // vec1 /= vec2

DECLARE_INPLACE_OPERATOR( += );     // vec += a
DECLARE_INPLACE_OPERATOR( -= );     // vec -= a
DECLARE_INPLACE_OPERATOR( *= );     // vec *= a
DECLARE_INPLACE_OPERATOR( /= );     // vec /= a


namespace linalg {

std::vector<float> gen_uniform_sp(uint32_t size, float scale);

void l2_normalize(std::vector<float> &x);


template<typename T>
T sum(const std::vector<T> x){
    T a = 0;
    for (uint32_t i = 0; i < x.size(); ++i) {
        a += x[i];
    }
    return a;
}



template<typename T>
float mean(const std::vector<T> x){
    float a = 0;
    for (uint32_t i = 0; i < x.size(); ++i) {
        a += float(x[i]);
    }
    return a / x.size();
}



template<typename T>
std::vector<uint32_t> argsort(const std::vector<T> &vec, bool ascent){
    std::vector<uint32_t> idx(vec.size());
    std::iota(idx.begin(), idx.end(), 0);

    if (ascent == false) {
        std::sort(
            idx.begin(),
            idx.end(),
            [&vec](uint32_t i1, uint32_t i2){
                return vec[i1] < vec[i2];
            }
        );
    } else {
        std::sort(
            idx.begin(),
            idx.end(),
            [&vec](uint32_t i1, uint32_t i2){
                return vec[i1] > vec[i2];
            }
        );
    }

    return idx;
}

}

#endif // ifndef _LINALG_H
