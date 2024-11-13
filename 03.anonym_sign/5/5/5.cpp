#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include <vector>
#include "C:\Users\Dmitr\Desktop\учёба\Криптография\full\MD5\MD5\MD5.h"
using namespace std;

string mybinary(long long x)
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

long long modPow(long long a, long long x, long long p) {
	string x_bin = mybinary(x);
	long long y = 1, s = a;

	for (long long i = 0; i < x_bin.size(); i++) {
		if (x_bin[i] == '1')
			y = (y * s) % p;
		s = (s * s) % p;
	}

	return y;
}

long long gcd(long long a, long long b) {
	long long r = 0;
	while (b != 0) {
		r = a % b;
		a = b;
		b = r;
	}
	return a;
}

long long* Evclid(long long a, long long b) {
	long long* U = new long long[3] {a, 1, 0};
	long long V[3]{ b,0,1 }, T[3]{}, q = 0;

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

inline bool isPrime(long long p) {
	if (p <= 1)
		return false;
	long long b = (long long)pow(p, 0.5);
	for (long long i = 2; i <= b; ++i) { if ((p % i) == 0) return false; }
	return true;
}

inline long long* gen_h(long long p, long long h) {
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

inline void sign_anonym() {
	random_device R;
	default_random_engine gen(R());
	uniform_int_distribution<long long> Area16(1 << 14, (1 << 15) - 1);

	long long Q = 0, P = 0;

	do {
		Q = Area16(gen);
		P = Area16(gen);
	} while ((P == Q) || !(isPrime(P) && isPrime(Q)));

	long long N = P * Q, F = (P - 1) * (Q - 1);
	long long* cd = genCD(F + 1);
	long long c = cd[1], d = cd[0];
	delete[] cd;
	
	long long rnd = Area16(gen);
	long long v = 0;

	cout << "v = ";
	cin >> v;
	long long n = rnd + v;
	long long r = 0;

	r = Area16(gen);

	string hash = md5(to_string(n));
	cout << "|hash = " << hash;
	vector<long long> hash_to_vector = HashToVector(hash);
	long long htv_size = hash_to_vector.size();

	long long* _h = new long long[htv_size];
	long long* _s = new long long[htv_size];
	long long* s = new long long[htv_size];
	long long* correct = new long long[htv_size];

	long long* _rgen = gen_h(N, r);
	long long _r = _rgen[1];

	for (int i = 0; i < htv_size; i++) {
		_h[i] = ((hash_to_vector[i] % N) * modPow(r, d, N)) % N;

		_s[i] = modPow(_h[i], c, N);
		if (_s[i] < 0) {
			_s[i] += N;
		}

		s[i] = (_s[i] * _r) % N;

		correct[i] = modPow(s[i], d, N);
	}

	cout << "\n|!h = ";
	for (int i = 0; i < htv_size; i++)
		cout << _h[i];

	cout << "\n|!s = ";
	for (int i = 0; i < htv_size; i++)
		cout << _s[i];

	cout << "\n|s = ";
	for (int i = 0; i < htv_size; i++)
		cout << s[i];

	cout << "\n|hash = ";
	for (int i = 0; i < htv_size; i++)
		cout << hash_to_vector[i];

	cout << "\n|correct = ";
	for (int i = 0; i < htv_size; i++)
		cout << correct[i];
	cout << endl;
}

int main() {
	sign_anonym();
	return 0;
}