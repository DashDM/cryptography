#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
using namespace std;

// функция из прошлых лаб
bool Prime(long long p) {
	if (p <= 1)
		return false;

	long long b = (long long)pow(p, 0.5);

	for (long long i = 2; i <= b; ++i) { if ((p % i) == 0) return false; }
	return true;
}

// функция из прошлых лаб
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

// функция из прошлых лаб
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

// функция из прошлых лаб
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

// функция из прошлых лаб
long long gcd(long long a, long long b) {
	long long r = 0;
	while (b != 0) {
		r = a % b;
		a = b;
		b = r;
	}
	return a;
}

// функция из прошлых лаб
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

// функция из прошлых лаб
long long* genQP() {
	random_device R;
	default_random_engine gen(R());
	uniform_int_distribution<long long> Area(10000, 100000);

	long long* qp = new long long [2] {};

	do {
		qp[0] = Area(gen);
		qp[1] = 2 * qp[0] + 1;
	} while (!(Prime(qp[0]) && Prime(qp[1])));

	return qp;
}

// функция для заполнения колоды карт рандомными числами
void koloda_zap(vector<long long> &Koloda, long long cards_amount, long long P) {
	random_device R;
	default_random_engine gen(R());
	uniform_int_distribution<long long> kol_zap(1, P - 1);

	bool K_check = false; // для проверки
	do {
		for (int i = 0; i < cards_amount; i++) // заполняем колоду (всю, полностью)
			Koloda[i] = kol_zap(gen);
		for (int i = 0; i < cards_amount; i++) { // проверка, что в колоде нет одинаковых карт
			for (int j = i + 1; j < cards_amount; j++) {
				if (Koloda[i] == Koloda[j]) { // если нашлось совпадение
					K_check = true;
					break; // выходим из цикла проверки
				}
				else
					K_check = false;
			}
			if (K_check) // для того, чтобы не проверял остальные карты, если нашлось совпадение
				break;
		}
	} while (K_check);
}

struct player {
	long long c = 0; // секретный ключ
	long long d = 0; // секретный ключ
	vector<long long> hand; // рука с картами
};

void Mental_Poker() {
	long long* qp = genQP();
	long long Q = qp[0], P = qp[1];
	delete[] qp;

	long long cards_amount = 0; // кол-во карт
	long long players_amount = 0; // кол-во игроков
	long long hand_amount = 0; // кол-во карт на 1 руку
	long long table = 5; // карты на столе

	// вводим кол-во карт
	do {
		cout << "Cards (..%2 = 0): ";
		cin >> cards_amount;
	} while ((cards_amount <= 0) || (cards_amount % 2 != 0));
	
	// вводим число игроков
	do {
		cout << "Enter players (< cards): ";
		cin >> players_amount;
	} while (players_amount > cards_amount);

	// вводим число карт для 1 руки
	do {
		cout << "Enter hand (< players): ";
		cin >> hand_amount;
	} while ((hand_amount > players_amount) || ((hand_amount * players_amount + table) > cards_amount));

	vector<long long> Koloda(cards_amount);// колода карт
	vector<player> Players_in_game(players_amount);// игроки за столом
	vector<long long> Cards_on_table(table);

	koloda_zap(Koloda, cards_amount, P);// заполняем колоду картами

	cout << "\nKoloda:\n"; // выводим колоду на экран
	for (int i = 0; i < cards_amount; i++) {
		cout << i + 1 << ") " << Koloda[i] << "\n";
	}
	cout << endl;

	// генерируем c и d для каждого игрока и передаём кол-во карт в руку
	for (int i = 0; i < players_amount; i++) {
		long long* cd = genCD(P);
		Players_in_game[i].c = cd[0];
		Players_in_game[i].d = cd[1];
		Players_in_game[i].hand.resize(hand_amount); // resize() - для каждого игрока обновляем размер массива hand
		delete[]cd;
	}

	// каждый игрок шифрует и перемешивает все карты в колоде
	for (int i = 0; i < players_amount; i++) {
		for (int j = 0; j < cards_amount; j++) {
			Koloda[j] = modPow(Koloda[j], Players_in_game[i].c, P);
		}
		random_shuffle(Koloda.begin(), Koloda.end());
	}

	// раздача карт каждому игроку
	int c = 0;
	for (int i = 0; i < players_amount; i++) {
		for (int j = 0; j < hand_amount; j++) {
			Players_in_game[i].hand[j] = Koloda[c];
			c++;
		}
	}

	// раздача карт на стол
	for (int i = 0; i < table; i++) {
		Cards_on_table[i] = Koloda[c];
		c++;
	}

	// каждый игрок дешефрует свои карты
	for (int i = 0; i < players_amount; i++) {
		for (int j = 0; j < hand_amount; j++) {
			int k = i + 1;
			for (k; k < players_amount; k++) { // например: 3 -> 4 -> 5
				Players_in_game[i].hand[j] = modPow(Players_in_game[i].hand[j], Players_in_game[k].d, P);
			}
			for(int t = 0; t <= i; t++) // например: 1 -> 2 -> 3
				Players_in_game[i].hand[j] = modPow(Players_in_game[i].hand[j], Players_in_game[t].d, P);
		}
	}

	// дешефруем карты на столе
	for (int i = 0; i < table; i++) {
		for (int j = 0; j < players_amount; j++) {
			Cards_on_table[i] = modPow(Cards_on_table[i], Players_in_game[j].d, P);
		}
	}

	for (int i = 0; i < players_amount; i++) {
		cout << "Player " << i + 1 << ": ";
		for (int j = 0; j < hand_amount; j++) {
			cout << Players_in_game[i].hand[j] << " ";
		}
		cout << endl;
	}
	cout << endl;

	cout << "table: ";
	for (int i = 0; i < table; i++) {
		cout << Cards_on_table[i] << " ";
	}
	cout << endl;
}

int main() {
	cout << "!Mental_Poker:\n";
	Mental_Poker();
	return 0;
}