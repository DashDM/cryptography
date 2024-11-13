#include <iostream>
#include <vector>
#include "MD5.h"

int main() {
    std::string s = "I love ZI";
    std::string hash = md5(s); // Получаем hash сообщения
    std::vector<long long> hash_vt = HashToVector(hash); //Перевод hash в целочисленный vector из 8 элеметов
    long long* hash_arr = HashToArray(hash); //Перевод hash в целочисленный массив из 8 элеметов
}