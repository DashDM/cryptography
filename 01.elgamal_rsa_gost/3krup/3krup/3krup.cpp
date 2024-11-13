#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include <vector>
#include "C:\Users\Dmitr\Desktop\учёба\Криптография\full\MD5\MD5\MD5.h" // подключи MD5 для хеша

using namespace std;

// из десятичной в двоичную
string binary(long long x)
{
	string bin = ""; 
	long long x_mod = 0;

	while (x) {
		x_mod = x % 2;
		bin.push_back(x_mod + 48);
		x /= 2;
	}
	return bin;
}
// быстрое возведение числа в степень по модулю (т.е. a^x mod p)
long long modPow(long long a, long long x, long long p) { 
	string x_bin = binary(x);
	long long y = 1, s = a;

	for (long long i = 0; i < x_bin.size(); i++) {
		if (x_bin[i] == '1')
			y = (y * s) % p;
		s = (s * s) % p;
	}
	return y;
}
// НОД (наиб общий делитель)
long long gcd(long long a, long long b) {
	long long r = 0;
	while (b != 0) {
		r = a % b;
		a = b;
		b = r;
	}
	return a;
}
// обобщённый алгоритм Евклида
// вернёт массив U = {НОБ(a,b), x, y};
// хелп-формула: a*x + b*y = gcd(a,b) 
long long* Evclid(long long a, long long b) {
	long long* U = new long long[3] {a, 1, 0};
	long long V[3]{ b,0,1 };
	long long T[3]{};
	long long q = 0;

	while (V[0] != 0) {
		q = U[0] / V[0];
		T[0] = U[0] % V[0];
		T[1] = U[1] - q * V[1];
		T[2] = U[2] - q * V[2];

		for (int i = 0; i < 3; i++) {
			U[i] = V[i];
			V[i] = T[i];
		}
	}
	return U;
}
// генерирует число и инверсию этого числа
// с помощью обобщённого алгоритма Евклида
long long* genCD(long long p) {
	random_device R;
	default_random_engine gen(R());
	uniform_int_distribution<long long> D(1, p - 1);
	long long* U = new long long[2] {};

	do {
		U[0] = D(gen);
	} while (!(gcd(U[0], p - 1) == 1));

	long long* result_ev = Evclid(p - 1, U[0]);

	if (result_ev[2] < 0) {
		U[1] = result_ev[2] + (p - 1);
	}
	else
		U[1] = result_ev[2];

	delete[]result_ev;
	return U;
}
// проверка числа на простоту (вернёт true, если простое)
bool isPrime(long long p) {
	if (p <= 1)
		return false;
	long long b = (long long)pow(p, 0.5);
	for (long long i = 2; i <= b; ++i) { if ((p % i) == 0) return false; }
	return true;
}

void sRSA() {
	random_device R;
	default_random_engine gen(R());
	uniform_int_distribution<long long> Area(pow(10, 4), pow(10, 5));
	
	long long Q = 0, P = 0;

	// генерируем Q и P
	do {
		Q = Area(gen);
		P = 2 * Q + 1;
	} while ((Q >= 45000) || (P <= 32500) || !(isPrime(P) && isPrime(Q)));

	// считаем числа N и F
	long long N = P * Q, F = (P - 1) * (Q - 1);

	// вычисляем d - открытый ключ, с - закрытый ключ
	long long* cd = genCD(F + 1); // создали динамический массив cd для c и d
	long long c = cd[1], d = cd[0]; // c - инверсия числа d
	delete[] cd; // освобождаем память
	// закрытый - чтобы подписать, открытый - чтобы проверить подлинность

	string message = "A"; // сообщение
	string message_to_hash = md5(message); // находим хеш с помощью хеш-функции md5()

	vector<long long> hash_to_vector = HashToVector(message_to_hash); // из 16й СС в 10ю СС в виде массива
	long long hash_sum = 0;

	for (int i = 0; i < hash_to_vector.size(); i++)
		hash_sum += hash_to_vector[i];

	long long s = modPow(hash_sum, c, N) % N;
	long long e = modPow(s, d, N) % N;
	cout << "hash = " << hash_sum << "\ne = " << e << endl;
}

void sEl_Gamal() {
	random_device R;
	default_random_engine gen(R());
	uniform_int_distribution<long long> Area(pow(10, 4), pow(10, 5));
	
	long long Q = 0, P = 0, g = 0;

	// генерируем Q и P
	do {
		Q = Area(gen);
		P = 2 * Q + 1;
	} while (!(isPrime(P) && isPrime(Q)));

	// генерируем первообразный корень g
	uniform_int_distribution<long long> for_g(1, P - 1); // диапазон для g
	do {
		g = for_g(gen); 
	} while (modPow(g, Q, P) != 1); // '!=' - знак неравенства
	// если g^Q mod P не равно 1, то заново генерируем g

	long long x = for_g(gen); // x - секретный ключ
	long long y = modPow(g, x, P); // y - открытый ключ

	string message = "A"; // сообщение
	string message_to_hash = md5(message); // находим хеш с помощью хеш-функции md5()

	vector<long long> hash_to_vector = HashToVector(message_to_hash); // из 16й СС в 10ю СС в виде массива
	long long hash_sum = 0;

	for (int i = 0; i < hash_to_vector.size(); i++)
		hash_sum += hash_to_vector[i];

	// объявляем динамический массив CD, в который записываем результат фу-ии genCD()
	// 0-й элемент - это наше k, 1-ый элемент - инверсия числа k (т.е. k^-1)
	long long* CD = genCD(P);
	long long _k = CD[1], k = CD[0], r = modPow(g, k, P);
	delete[]CD; // освобождаем память

	long long u = (hash_sum % (P - 1) - (x * r) % (P - 1)) % (P - 1); 
	// если вдруг u отрицательное, то увеличиваем его на P-1 (рекомендация от препода)
	if (u < 0)
		u += (P - 1);

	long long s = (_k * u) % (P - 1);
	long long yr = (modPow(y, r, P) * (modPow(r, s, P))) % P;
	long long gh = modPow(g, hash_sum, P);

	cout << "yr = " << yr << "\ngh = " << gh << endl;
}
// для нахождения инверсии хеша (h^-1)
long long* gen_h(long long p, long long h) {
	long long* U = new long long[2] {};

	U[0] = h;

	long long* result_ev = Evclid(p, U[0]);

	if (result_ev[2] < 0) {
		U[1] = result_ev[2] + (p);
	}
	else
		U[1] = result_ev[2];

	delete[]result_ev;
	return U;
}

void sGost() {
	random_device R;
	default_random_engine gen(R());
	// Area16 - задаёт диапазон с помощью битовый операций
	// сам диапазон (2^14 до 2^15 - 1). Нужен для генерации числа, длиной 16 бит
	uniform_int_distribution<long long> Area16(1 << 15, (1 << 16) - 1);
	
	string message = "A"; // сообщение
	string message_to_hash = md5(message); // находим хеш с помощью хеш-функции md5()
	vector <long long> hash_to_vector = HashToVector(message_to_hash); // оставляем хеш в виде массива, т.к. иначе Q < hash. А так всё гуд
	long long htv_size = hash_to_vector.size();  // в переменную htv записывае размер вектора (массива) hash_to_vector
	long long P = 0, Q = 0, b = 0, g = 0, a = 0;

	bool q_save = false, p_save = false; // для проверки
	do {
		Q = Area16(gen);
		b = Area16(gen); // генерируем, т.к. b = (p-1)/q вернём вещественное число
		P = b * Q + 1;
		// этот цикл проверяет, что hash < Q
		for (int i = 0; i < htv_size; i++) {
			// если Q < hash, то к проверочной q_save присваиваем
			// true и выходим из этого цикла for(){}
			if (Q <= hash_to_vector[i]) {
				q_save = true;
				break;
			}
			else {
				q_save = false;
			}
		}
		// проверяем, что длина числа P - 31 бит
		if (!((P >(1 << 30)) && (P < ((1 << 31) - 1)))) {
			p_save = true;
		}
		else {
			p_save = false;
		}
	} while (p_save || q_save || !(isPrime(P) && isPrime(Q))); // || - лог ИЛИ, && - лог И

	uniform_int_distribution<long long> AreaP(1, P - 1), AreaQ(1, Q);
	do {
		g = AreaP(gen);
		a = modPow(g, b, P);
	} while (a <= 1);

	long long x = AreaQ(gen), y = modPow(a, x, P); // x - секретный, y - открытый
	long long k = 0, r = 0, s = 0;

	// представляем переменные в виде массивов, т.к. хеш представлен в виде массива
	long long* S = new long long[htv_size] {};
	long long* _h = new long long[htv_size] {};
	long long* u1 = new long long[htv_size] {};
	long long* u2 = new long long[htv_size] {};
	long long* v = new long long[htv_size] {};

	do {
		bool s_save = true; // для проверки
		k = rand() % Q;
		r = modPow(a, k, P) % Q;

		for (int i = 0; i < htv_size; i++) {
			S[i] = ((k * hash_to_vector[i]) % Q + (x * r) % Q) % Q;

			if (S[i] == 0 || S[i] >= Q) {
				s_save = false;
				break;
			}
		}
		if (r != 0 && s_save)
			break;
	} while (1);

	for (int i = 0; i < htv_size; i++) {
		long long* genh = gen_h(Q, hash_to_vector[i]);
		_h[i] = genh[1];
		delete[]genh;
	}

	for (int i = 0; i < htv_size; i++) {
		u1[i] = (S[i] * _h[i]) % Q; if (u1[i] < 0) { u1[i] += Q; }
		u2[i] = (-r * _h[i]) % Q; if (u2[i] < 0) { u2[i] += Q; }
		v[i] = ((modPow(a, u1[i], P) * modPow(y, u2[i], P)) % P) % Q; if (v[i] < 0) { v[i] += Q; }
	}

	cout << "hash = ";
	for (int i = 0; i < htv_size; i++)
		cout << hash_to_vector[i] << " ";

	cout << "\n_h = ";
	for (int i = 0; i < htv_size; i++)
		cout << _h[i] << " ";

	cout << "\nv = ";
	for (int i = 0; i < htv_size; i++)
		cout << v[i] << " ";

	cout << "\nr = " << r << endl;

	delete[] S;
	delete[] _h;
	delete[] u1;
	delete[] u2;
	delete[] v;
}

int main() {
	cout << "RSA:\n";
	sRSA();
	cout << "El_Gamal:\n";
	sEl_Gamal();
	cout << "GHOST:\n";
	sGost();
	return 0;
}