/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

// Modified by Kah Keng Tay to keep only CvScalar operator definitions, and to do 
// element-wise operations instead of vector operations.

#ifndef _CVMAT_HPP_
#define _CVMAT_HPP_

/****************************************************************************************\
*                            C++ - like operations on CvScalar                           *
\****************************************************************************************/

inline CvScalar& operator += ( CvScalar& a, const CvScalar& b )
{
    double t0 = a.val[0] + b.val[0];
    double t1 = a.val[1] + b.val[1];
    a.val[0] = t0;
    a.val[1] = t1;

    t0 = a.val[2] + b.val[2];
    t1 = a.val[3] + b.val[3];
    a.val[2] = t0;
    a.val[3] = t1;

    return a;
}


inline CvScalar& operator -= ( CvScalar& a, const CvScalar& b )
{
    double t0 = a.val[0] - b.val[0];
    double t1 = a.val[1] - b.val[1];
    a.val[0] = t0;
    a.val[1] = t1;

    t0 = a.val[2] - b.val[2];
    t1 = a.val[3] - b.val[3];
    a.val[2] = t0;
    a.val[3] = t1;

    return a;
}


inline CvScalar& operator *= ( CvScalar& a, double b )
{
    double t0 = a.val[0] * b;
    double t1 = a.val[1] * b;
    a.val[0] = t0;
    a.val[1] = t1;

    t0 = a.val[2] * b;
    t1 = a.val[3] * b;
    a.val[2] = t0;
    a.val[3] = t1;

    return a;
}


inline CvScalar& operator /= ( CvScalar& a, double b )
{
    double inv_b = 1./b;
    double t0 = a.val[0] * inv_b;
    double t1 = a.val[1] * inv_b;
    a.val[0] = t0;
    a.val[1] = t1;

    t0 = a.val[2] * inv_b;
    t1 = a.val[3] * inv_b;
    a.val[2] = t0;
    a.val[3] = t1;

    return a;
}


inline CvScalar& operator *= ( CvScalar& a, const CvScalar& b )
{
    // Modified to do element-wise multiplication.
    a.val[0] *= b.val[0]; 
    a.val[1] *= b.val[1];
    a.val[2] *= b.val[2];
    a.val[3] *= b.val[3];

    return a;
}


inline CvScalar& operator /= ( CvScalar& a, const CvScalar& b )
{
    // Modified to do element-wise division.
    a.val[0] /= b.val[0]; 
    a.val[1] /= b.val[1];
    a.val[2] /= b.val[2];
    a.val[3] /= b.val[3];
    
    return a;
}


inline CvScalar& operator += ( CvScalar& a, double b )
{
    // Modified to do element-wise addition.
    a.val[0] += b;
    a.val[1] += b;
    a.val[2] += b;
    a.val[3] += b;
    return a;
}


inline CvScalar& operator -= ( CvScalar& a, double b )
{
    // Modified to do element-wise subtraction.
    a.val[0] -= b;
    a.val[1] -= b;
    a.val[2] -= b;
    a.val[3] -= b;
    return a;
}


inline CvScalar operator + ( const CvScalar& a, const CvScalar& b )
{
    return cvScalar( a.val[0] + b.val[0], a.val[1] + b.val[1],
                     a.val[2] + b.val[2], a.val[3] + b.val[3] );
}


inline CvScalar operator - ( const CvScalar& a, const CvScalar& b )
{
    return cvScalar( a.val[0] - b.val[0], a.val[1] - b.val[1],
                     a.val[2] - b.val[2], a.val[3] - b.val[3] );
}


inline CvScalar operator + ( const CvScalar& a, double b )
{
    // Modified to do element-wise addition.
    return cvScalar( a.val[0] + b, a.val[1] + b, a.val[2] + b, a.val[3] + b );
}


inline CvScalar operator - ( const CvScalar& a, double b )
{
    // Modified to do element-wise subtraction.
    return cvScalar( a.val[0] - b, a.val[1] - b, a.val[2] - b, a.val[3] - b );
}


inline CvScalar operator + ( double a, const CvScalar& b )
{
    // Modified to do element-wise addition.
    return cvScalar( a + b.val[0], a + b.val[1], a + b.val[2], a + b.val[3] );
}


inline CvScalar operator - ( double a, const CvScalar& b )
{
    // Modified to do element-wise subtraction.
    return cvScalar( a - b.val[0], a - b.val[1], a - b.val[2], a - b.val[3] );
}


inline CvScalar operator - ( const CvScalar& b )
{
    return cvScalar( -b.val[0], -b.val[1], -b.val[2], -b.val[3] );
}


inline CvScalar operator * ( const CvScalar& a, const CvScalar& b )
{
    CvScalar c = a;

    return (c *= b);
}


inline CvScalar operator * ( const CvScalar& a, double b )
{
    return cvScalar( a.val[0]*b, a.val[1]*b, a.val[2]*b, a.val[3]*b );
}


inline CvScalar operator * ( double a, const CvScalar& b )
{
    return cvScalar( b.val[0]*a, b.val[1]*a, b.val[2]*a, b.val[3]*a );
}


inline CvScalar operator / ( const CvScalar& a, const CvScalar& b )
{
    CvScalar c = a;
    return (c /= b);
}


inline CvScalar operator / ( const CvScalar& a, double b )
{
    double inv_b = 1./b;
    return cvScalar( a.val[0]*inv_b, a.val[1]*inv_b,
                     a.val[2]*inv_b, a.val[3]*inv_b );
}


inline CvScalar operator / ( double a, const CvScalar& b )
{
    double inv_d = -a/(b.val[0]*b.val[0] + b.val[1]*b.val[1] +
                       b.val[2]*b.val[2] + b.val[3]*b.val[3]);
    return cvScalar( b.val[0] * -inv_d, b.val[1] * inv_d,
                     b.val[2] * inv_d, b.val[3] * inv_d );
}


inline CvScalar& operator &= ( CvScalar& a, const CvScalar& b )
{
    int t0 = cvRound(a.val[0]) & cvRound(b.val[0]);
    int t1 = cvRound(a.val[1]) & cvRound(b.val[1]);
    a.val[0] = t0;
    a.val[1] = t1;

    t0 = cvRound(a.val[2]) & cvRound(b.val[2]);
    t1 = cvRound(a.val[3]) & cvRound(b.val[3]);
    a.val[2] = t0;
    a.val[3] = t1;

    return a;
}


inline CvScalar& operator |= ( CvScalar& a, const CvScalar& b )
{
    int t0 = cvRound(a.val[0]) | cvRound(b.val[0]);
    int t1 = cvRound(a.val[1]) | cvRound(b.val[1]);
    a.val[0] = t0;
    a.val[1] = t1;

    t0 = cvRound(a.val[2]) | cvRound(b.val[2]);
    t1 = cvRound(a.val[3]) | cvRound(b.val[3]);
    a.val[2] = t0;
    a.val[3] = t1;

    return a;
}


inline CvScalar& operator ^= ( CvScalar& a, const CvScalar& b )
{
    int t0 = cvRound(a.val[0]) ^ cvRound(b.val[0]);
    int t1 = cvRound(a.val[1]) ^ cvRound(b.val[1]);
    a.val[0] = t0;
    a.val[1] = t1;

    t0 = cvRound(a.val[2]) ^ cvRound(b.val[2]);
    t1 = cvRound(a.val[3]) ^ cvRound(b.val[3]);
    a.val[2] = t0;
    a.val[3] = t1;

    return a;
}


inline CvScalar operator & ( const CvScalar& a, const CvScalar& b )
{
    CvScalar c = a;
    return (c &= b);
}


inline CvScalar operator | ( const CvScalar& a, const CvScalar& b )
{
    CvScalar c = a;
    return (c |= b);
}


inline CvScalar operator ^ ( const CvScalar& a, const CvScalar& b )
{
    CvScalar c = a;
    return (c ^= b);
}


inline CvScalar operator ~ ( const CvScalar& a )
{
    return cvScalar( ~cvRound(a.val[0]), ~cvRound(a.val[1]),
                     ~cvRound(a.val[2]), ~cvRound(a.val[3])); 
}

#endif /*_CVMAT_HPP_*/

