// util.cpp

#include "util.h"
#include <iostream>

namespace util {

    void printVec3(vec3 vec) {
        cout << "---------" << endl;
        for (int i = 0 ; i < 3 ; i++) {
            cout << vec[i] << " ";
        }
        cout << endl;
    }

    void printVec4(vec4 vec) {
        cout << "---------" << endl;
        for (int i = 0 ; i < 4 ; i++) {
            cout << vec[i] << " ";
        }
        cout << endl;
    }

    void printMat3(mat3 mat) {
        cout << "---------" << endl;
        for (int i = 0 ; i < 3 ; i++) {
            for (int j = 0 ; j < 3 ; j++) {
                cout << mat[i][j] << " ";
            }
            cout << endl;
        }
    }

    void printMat4(mat4 mat) {
        cout << "---------" << endl;
        for (int i = 0 ; i < 4 ; i++) {
            for (int j = 0 ; j < 4 ; j++) {
                cout << mat[i][j] << " ";
            }
            cout << endl;
        }
    }
}
