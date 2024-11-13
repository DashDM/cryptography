#include <iostream>
#include <string>
#include <random>
#include <ctime>

using namespace std;

//FirstLab

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

bool miillerTest(long long n, long long d)
{
	long long a = 2 + rand() % (n - 4);
	long long x = modPow(a, d, n);

	if (x == 1 || x == n - 1)
		return true;

	while (d != n - 1)
	{
		x = (x * x) % n;
		d *= 2;

		if (x == 1)
			return false;
		if (x == n - 1)
			return true;
	}

	return false;
}

bool isPrime(long long n, long long k)
{
	if (n <= 1 || n == 4)
		return false;
	if (n <= 3)
		return true;

	long long d = n - 1;

	while (d % 2 == 0)
		d /= 2;

	for (long long i = 0; i < k; i++)
		if (!miillerTest(n, d))
			return false;

	return true;
}

long long ggen(long long P, long long Q) {
	random_device R;
	default_random_engine gen(R());
	uniform_int_distribution<long long> X(1, P - 1);
	long long g = 0;
	do {
		g = X(gen);
	} while (modPow(g, Q, P) != 1);
	return g;
}

long long* genQPG(int a) {
	if (a == 1) {
		random_device R;
		default_random_engine gen(R());
		uniform_int_distribution<long long> Big_Area(pow(10, 4), pow(10, 5));
		long long* qpg = new long long[3] {};

		do {
			qpg[0] = Big_Area(gen);
			qpg[1] = 2 * qpg[0] + 1;
		} while ((qpg[0] >= 40000) || (qpg[1] <= 32500) || !(isPrime(qpg[0], log2(qpg[0])) && isPrime(qpg[1], log2(qpg[1]))));
		qpg[2] = ggen(qpg[1], qpg[0]);

		cout << "|Q = " << qpg[0] << "\n|P = " << qpg[1] << "\n|g = " << qpg[2] << endl;
		return qpg;
	}
	if (a == 2) {
	{
		srand(time(NULL));
		long long* qpg = new long long[3] {};

		do {
			qpg[0] = 10000 + rand()%100000;
			qpg[1] = 2 * qpg[0] + 1;
		} while (!(isPrime(qpg[0], log2(qpg[0])) && isPrime(qpg[1], log2(qpg[1]))));
		qpg[2] = ggen(qpg[1], qpg[0]);

		cout << "|Q = " << qpg[0] << "\n|P = " << qpg[1] << "\n|g = " << qpg[2] << endl;
		return qpg;
	}
	}
}

long long DH() {

	long long* qpg = genQPG(1);
	long long Q = qpg[0], P = qpg[1], g = qpg[2];
	delete[]qpg;

	random_device R;
	default_random_engine gen(R());
	uniform_int_distribution<long long> X(1, P - 1);

	long long Xa = X(gen);
	long long Xb = X(gen);

	long long Ya = modPow(g, Xa, P);
	long long Yb = modPow(g, Xb, P);

	long long Zab = modPow(Yb, Xa, P);
	long long Zba = modPow(Ya, Xb, P);

	cout << "|Xa = " << Xa << "\n|Xb = " << Xb << "\n|Ya = " << Ya << "\n|Yb = " << Yb << "\n|Zab = " << Zab << "\n|Zba = " << Zba << endl << endl;

	return Zab;
}

//SecondLab

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

void Shamir() {
	random_device R;
	default_random_engine gen(R());

	long long* qpg = genQPG(1);
	long long p = qpg[1];
	delete[]qpg;

	uniform_int_distribution<unsigned long long> D(1, p);
	long long m = D(gen);

	long long* CDa = genCD(p), * CDb = genCD(p);
	long long Ca = CDa[0], Da = CDa[1], Cb = CDb[0], Db = CDb[1];

	long long x1 = modPow(m, Ca, p);
	long long x2 = modPow(x1, Cb, p);
	long long x3 = modPow(x2, Da, p);
	long long x4 = modPow(x3, Db, p);

	delete[]CDa;
	delete[]CDb;

	cout << "|Ca = " << Ca << "\n|Da = " << Da << "\n|Cb = " << Cb << "\n|Db = " << Db << endl;
	cout << "|x1 = " << x1 << "\n|x2 = " << x2 << "\n|x3 = " << x3 << "\n|x4 = " << x4 << "\n| m = " << m << endl;
}

void El_Gamal() {
	random_device R;
	default_random_engine gen(R());

	long long* qpg = genQPG(1);
	long long p = qpg[1], g = qpg[2];
	delete[]qpg;

	uniform_int_distribution<unsigned long long> M(1, p), Dm1(1, p - 1);
	long long m = M(gen), x = M(gen), y = modPow(g, x, p), k = Dm1(gen);

	long long a = modPow(g, k, p), b = m * modPow(y, k, p) % p;
	long long mi = b * modPow(a, p - 1 - x, p) % p;

	cout << "|x = " << x << "\n|y = " << y << "\n|k = " << k << endl;
	cout << "|a = " << a << "\n|b = " << b << "\n| m = " << m << "\n|mi = " << mi << endl;
}

void RSA() {
	random_device R;
	default_random_engine gen(R());

	long long* qpg = genQPG(2);
	long long p = qpg[1], q = qpg[0], N = p * q, f = (p - 1) * (q - 1);
	long long* CD = genCD(f + 1);
	long long c = CD[1], d = CD[0];
	delete[]CD;
	delete[]qpg;

	uniform_int_distribution<long long> D(1, f), M(1, N);
	long long m = M(gen);

	long long e = modPow(m, d, N), mi = modPow(e, c, N);


	cout << "|N = " << N << "\n|f = " << f << "\n|c = " << c << "\n|d = " << d << endl;
	cout << "|e = " << e << "\n| m = " << m << "\n|mi = " << mi << endl;
}

void Vernam(char str[]) {
	random_device R;
	default_random_engine gen(R());

	int len = strlen(str);

	char* key = new char[len];
	char* shStr = new char[len];
	char* deshStr = new char[len];

	for (int i = 0; i <= len; i++) {
		key[i] = (char)rand() % 255;
	}

	for (int i = 0; i <= len; i++) {
		shStr[i] = str[i] ^ key[i];
	}

	for (int i = 0; i <= len; i++) {
		deshStr[i] = shStr[i] ^ key[i];
	}

	cout << "|message: " << str << endl;
	cout << "-----------------------------------\n";
	cout << "|key: " << key << endl;
	cout << "-----------------------------------\n";
	cout << "|shifr: " << shStr << endl;
	cout << "-----------------------------------\n";
	cout << "|deshifr: " << deshStr << endl;
}