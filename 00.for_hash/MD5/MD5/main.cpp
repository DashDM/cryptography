#include <iostream>
#include <vector>
#include "MD5.h"

int main() {
    std::string s = "I love ZI";
    std::string hash = md5(s); // �������� hash ���������
    std::vector<long long> hash_vt = HashToVector(hash); //������� hash � ������������� vector �� 8 ��������
    long long* hash_arr = HashToArray(hash); //������� hash � ������������� ������ �� 8 ��������
}