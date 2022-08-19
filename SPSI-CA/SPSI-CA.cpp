﻿#include <iostream>
#include <vector>
#include <algorithm>
#ifndef _SPSICA_H
#define _SPSICA_Hd
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EOF
#define EOF (-1)
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef MAX_BUFFER
#define MAX_BUFFER 5000
#endif
#ifndef MAX_PATH
#ifdef _MAX_PATH
#define MAX_PATH _MAX_PATH
#else
#define MAX_PATH 260
#endif
#endif

#define kBit 128
#define N 26
#define n 11
#define r 5
#define beta 50
#define TimeToTest 50
#endif//_PSICA_H
using namespace std;
typedef unsigned long long int Element;
typedef const void* CPVOID;
vector<int> hashpi{}, archashpi{};
size_t baseNum = kBit / (sizeof(Element) << 3);
clock_t sub_start_time = clock(), sub_end_time = clock();
double timerR = 0, timerS = 0, timerC = 0;


/* 子函数 */
void init_hashpi()
{
	for (int i = 0; i < beta; ++i)
	{
		hashpi.push_back(i);
		archashpi.push_back(NULL);// initial
	}
	random_shuffle(hashpi.begin(), hashpi.end());
	for (int i = 0; i < beta; ++i)
		archashpi[hashpi[i]] = i;
	return;
}

int pi(int index)
{
	return hashpi[index % beta];
}

int arcpi(int value)
{
	return archashpi[value % beta];
}

Element getRandom()//获取随机数
{
	Element random = rand();
	random <<= 32;
	random += rand();
	return random;
}

void getInput(Element array[], int size)//获得输入
{
	char buffer[MAX_BUFFER] = { 0 }, cTmp[MAX_PATH] = { 0 };
	rewind(stdin);
	fflush(stdin);
	fgets(buffer, MAX_BUFFER, stdin);
	int cIndex = 0, eIndex = 0;
	for (int i = 0; i < MAX_BUFFER; ++i)
		if (buffer[i] >= '0' && buffer[i] <= '9')
			cTmp[cIndex++] = buffer[i];
		else if (cIndex)
		{
			char* endPtr;
			array[eIndex++] = strtoull(cTmp, &endPtr, 0);
			if (eIndex >= size)
				return;
			cIndex = 0;// Rewind cIndex
			memset(cTmp, 0, strlen(cTmp));// Rewind cTmp
		}
	return;
}

Element r_i(Element ele, int i)//哈希函数
{
	return ele << i;
}

int compare(CPVOID a, CPVOID b)//比较函数
{
	return (int)(*(Element*)a - *(Element*)b);
}

int BinarySearch(Element array_lists[], int nBegin, int nEnd, Element target, unsigned int& compareCount)
{
	if (nBegin > nEnd)
		return EOF;//未能找到目标
	int nMid = (nBegin + nEnd) >> 1;//使用位运算加速
	++compareCount;
	if (array_lists[nMid] == target)//找到目标
		return nMid;
	else if (array_lists[nMid] > target)//分而治之
		return BinarySearch(array_lists, nBegin, nMid - 1, target, compareCount);
	else
		return BinarySearch(array_lists, nMid + 1, nEnd, target, compareCount);
}

Element encode(Element a, Element b)
{
	return a + b;
}

Element decode(Element a, Element b)
{
	return a - b;
}


/* 类 */
class Receiver
{
private:
	Element X[n] = { NULL };
	Element k = NULL;
	Element X_c[beta] = { NULL };
	Element Z[beta] = { NULL };
	Element W[beta] = { NULL };
	Element U[beta] = { NULL };
	Element Z_pi[beta] = { NULL };
	vector<Element> intersection{};

public:
	void input_X()
	{
		getInput(this->X, n);
		return;
	}
	void auto_input_X()
	{
		vector<Element> v;
		while (v.size() < n)
		{
			Element tmp = getRandom();
			if (find(v.begin(), v.end(), tmp) == v.end())
				v.push_back(tmp);
		}
		for (int i = 0; i < n; ++i)
			this->X[i] = v[i];
		return;
	}
	void choose_k()
	{
		this->k = getRandom();
		return;
	}
	Element send_k()
	{
		return this->k;
	}
	void hash_X_to_X_c()
	{
		for (int i = 0; i < n; ++i)
		{
			int index = r_i(this->X[i], 1) % beta;
			if (0 != this->X_c[index])// already exist
			{
				int new_index = r_i(this->X_c[index], 2) % beta;
				if (0 != this->X_c[new_index])// still already exist
					;// abundant
				else
					this->X_c[new_index] = this->X_c[index];
			}
			else
				this->X_c[index] = X[i];
		}
		return;
	}
	void compute_Z_pi()
	{
		for (int i = 0; i < beta; ++i)
			this->Z_pi[i] = this->X_c[pi(i)] ^ this->Z[i];
		return;
	}
	Element* send_Z_pi()
	{
		return this->Z_pi;
	}
#ifdef _DEBUG
	void printArray()
	{
		cout << "X = { " << this->X[0];
		for (int i = 1; i < n; ++i)
			cout << ", " << this->X[i];
		cout << " }" << endl << endl;
		cout << "X_c: " << endl;
		for (int i = 0; i < beta; ++i)
			if (this->X_c[i])
				cout << "X_c[" << i << "] = " << this->X_c[i] << endl;
		cout << endl;
		return;
	}
#else
	void printArray()
	{
		return;
	}
#endif
	void obtain_Z()
	{
		for (int i = 0; i < beta; ++i)
			this->Z[i] = this->X_c[i];
		return;
	}
	Element* send_Z()
	{
		return this->Z;
	}
	void receive_W(Element* W)
	{
		for (int i = 0; i < beta; ++i)
			this->W[i] = *(W + i);
		return;
	}
	void generate_U()
	{
		for (int i = 0; i < beta; ++i)
			this->U[i] = getRandom();
		return;
	}
	void printIntersection()
	{
		this->intersection.clear();
		unsigned int comparation = 0;
		qsort(this->W, beta, sizeof(Element), compare);
		qsort(this->U, beta, sizeof(Element), compare);
		for (int i = 0; i < beta; ++i)
			if (this->W[i] && BinarySearch(this->U, 0, beta - 1, this->W[i], comparation))
				this->intersection.push_back(this->W[i]);
		if (this->intersection.size())
		{
#ifdef _DEBUG
			cout << "| U ∩ W | = | { " << intersection[0];
			for (size_t i = 1; i < this->intersection.size(); ++i)
				cout << ", " << this->intersection[i];
			cout << " } | = " << this->intersection.size() << endl;
#else
			cout << "| U ∩ W | = " << this->intersection.size() << endl;
#endif
		}
		else
			cout << "| U ∩ W | = 0" << endl;
		return;
	}
	size_t printSize()
	{
		cout << "Timeof(R) = " << timerR * baseNum << " / " << TimeToTest << " = " << timerR * baseNum / TimeToTest << " ms" << endl;
		cout << "sizeof(Receiver) = " << sizeof(Receiver) << " B" << endl;
		cout << "sizeof(R) = " << sizeof(this) * baseNum << " KB" << endl;
		cout << "\tsizeof(R.X) = " << sizeof(this->X) * baseNum << " B" << endl;
		cout << "\tsizeof(R.k) = " << sizeof(this->k) * baseNum << " B (*)" << endl;
		cout << "\tsizeof(R.X_c) = " << sizeof(this->X_c) * baseNum << " B" << endl;
		cout << "\tsizeof(R.Z) = " << sizeof(this->Z) * baseNum << " B" << endl;
		cout << "\tsizeof(R.W) = " << sizeof(this->W) * baseNum << " B (*)" << endl;
		cout << "\tsizeof(R.U) = " << sizeof(this->U) * baseNum << " B" << endl;
		cout << "\tsizeof(R.Z_pi) = " << sizeof(this->Z_pi) * baseNum << " B (*)" << endl;
		cout << "\tsizeof(R.intersection) = " << sizeof(this->intersection) * baseNum << " B (*)" << endl;
		cout << "\tsizeof(R.*) = " << (sizeof(this->k) + sizeof(this->W) + sizeof(this->Z_pi)) * baseNum << " B (*)" << endl;
		return (sizeof(this->k) + sizeof(this->W) + sizeof(this->Z_pi)) * baseNum;
	}
};
Receiver R;

class Sender
{
private:
	Element Y[N] = { NULL };
	Element k = NULL;
	Element V[beta] = { NULL };
	Element Z_pi[beta] = { NULL };
	Element T[N] = { NULL };

public:
	void input_Y()
	{
		getInput(this->Y, N);
	}
	void auto_input_Y()
	{
		vector<Element> v;
		while (v.size() < N)
		{
			Element tmp = getRandom();
			if (find(v.begin(), v.end(), tmp) == v.end())
				v.push_back(tmp);
		}
		for (int i = 0; i < N; ++i)
			this->Y[i] = v[i];
		return;
	}
	void receive_k(Element k)
	{
		this->k = k;
		return;
	}
	void rand_V()
	{
		for (int i = 0; i < beta; ++i)
			V[i] = this->k - rand();
		return;
	}
	void receive_Z_pi(Element* Z)
	{
		for (int i = 0; i < beta; ++i)
			this->Z_pi[i] = *(Z + i);
		return;
	}
	void compute_T()
	{
		for (int i = 0; i < N; ++i)
		{
			Element q_j = 0, I_i = 0;
			for (int j = 0; j < r; ++j)
			{
				q_j = arcpi(r_i(this->Y[i], j) % beta);
				I_i = this->Y[i] ^ this->Z_pi[q_j];
			}
			T[i] = encode(I_i, this->V[i]);
		}
		return;
	}
	Element* send_T()
	{
		return this->T;
	}
	size_t printSize()
	{
		cout << "Timeof(S) = " << timerS * baseNum << " / " << TimeToTest << " = " << timerS * baseNum / TimeToTest << " ms" << endl;
		cout << "sizeof(Sender) = " << sizeof(Sender) << " B" << endl;
		cout << "sizeof(S) = " << sizeof(this) * baseNum << " KB" << endl;
		cout << "\tsizeof(S.Y) = " << sizeof(this->Y) * baseNum << " B" << endl;
		cout << "\tsizeof(S.k) = " << sizeof(this->k) * baseNum << " B (*)" << endl;
		cout << "\tsizeof(S.V) = " << sizeof(this->V) * baseNum << " B" << endl;
		cout << "\tsizeof(S.Z_pi) = " << sizeof(this->Z_pi) * baseNum << " B (*)" << endl;
		cout << "\tsizeof(S.T) = " << sizeof(this->T) * baseNum << " B (*)" << endl;
		cout << "\tsizeof(S.*) = " << (sizeof(this->k) + sizeof(this->T) + sizeof(this->Z_pi)) * baseNum << " B (*)" << endl;
		return (sizeof(this->k) + sizeof(this->T) + sizeof(this->Z_pi)) * baseNum;
	}
};
Sender S;

class Cloud
{
private:
	Element Z[beta] = { NULL };
	Element T[N] = { NULL };
	Element W[beta] = { NULL };

public:
	void receive_Z(Element* Z)
	{
		for (int i = 0; i < beta; ++i)
			this->Z[i] = *(Z + i);
		return;
	}
	void receive_T(Element* T)
	{
		for (int i = 0; i < N; ++i)
		{
			this->T[i] = *(T + i);
			W[i] = decode(this->T[i], this->Z[i]);
		}
		return;
	}
	Element* send_W()
	{
		return this->W;
	}
	size_t printSize()
	{
		cout << "Timeof(C) = " << timerC * baseNum << " / " << TimeToTest << " = " << timerC * baseNum / TimeToTest << " ms" << endl;
		cout << "sizeof(Cloud) = " << sizeof(Cloud) << " B" << endl;
		cout << "sizeof(C) = " << sizeof(this) * baseNum << " KB" << endl;
		cout << "\tsizeof(C.Z) = " << sizeof(this->Z) * baseNum << " B" << endl;
		cout << "\tsizeof(C.T) = " << sizeof(this->T) * baseNum << " B (*)" << endl;
		cout << "\tsizeof(C.W) = " << sizeof(this->W) * baseNum << " B" << endl;
		cout << "\tsizeof(C.*) = " << sizeof(this->T) * baseNum << " B (*)" << endl;
		return sizeof(this->T) * baseNum;
	}
};
Cloud C;


/* 主函数 */
void initial(bool isAuto)
{

	init_hashpi();// setup pi
	if (isAuto)
	{
		S.auto_input_Y();
		R.auto_input_X();
	}
	else
	{
		cout << "Please input array Y with size " << N << ": " << endl;
		S.input_Y();// Sender S has input Y
		cout << endl;
		cout << "Please input array X with size " << n << ": " << endl;
		R.input_X();// Sender R has input X
		cout << endl;
	}
	return;
}

void setup()
{
	sub_start_time = clock();	R.choose_k();				sub_end_time = clock(); timerR += (double)sub_end_time - sub_start_time;// The receiver chooses a random PRG key k
	sub_start_time = clock();	S.receive_k(R.send_k());	sub_end_time = clock(); timerR += (double)sub_end_time - sub_start_time; timerS += (double)sub_end_time - sub_start_time;// k is sent to S
	sub_start_time = clock();	S.rand_V();					sub_end_time = clock(); timerS += (double)sub_end_time - sub_start_time;// rand beta
	return;
}

void distribution()
{
	sub_start_time = clock();	R.hash_X_to_X_c(); 				sub_end_time = clock(); timerR += (double)sub_end_time - sub_start_time;
#ifdef _DEBUG
	sub_start_time = clock();	R.printArray(); 				sub_end_time = clock(); timerR += (double)sub_end_time - sub_start_time;
#endif
	sub_start_time = clock();	R.obtain_Z(); 					sub_end_time = clock(); timerR += (double)sub_end_time - sub_start_time;// ss1
	sub_start_time = clock();	R.compute_Z_pi(); 				sub_end_time = clock(); timerR += (double)sub_end_time - sub_start_time;// ss2
	sub_start_time = clock();	C.receive_Z(R.send_Z());		sub_end_time = clock(); timerR += (double)sub_end_time - sub_start_time; timerC += (double)sub_end_time - sub_start_time;// Z is sent to C
	sub_start_time = clock();	S.receive_Z_pi(R.send_Z_pi()); 	sub_end_time = clock(); timerR += (double)sub_end_time - sub_start_time; timerS += (double)sub_end_time - sub_start_time;// ss2
	return;
}

void computation()
{
	sub_start_time = clock();	S.compute_T();				sub_end_time = clock(); timerS += (double)sub_end_time - sub_start_time;
	sub_start_time = clock();	C.receive_T(S.send_T());	sub_end_time = clock(); timerS += (double)sub_end_time - sub_start_time; timerC += (double)sub_end_time - sub_start_time;// T is sent to C
	sub_start_time = clock();	R.receive_W(C.send_W());	sub_end_time = clock(); timerR += (double)sub_end_time - sub_start_time; timerC += (double)sub_end_time - sub_start_time;// W is sent to R
	sub_start_time = clock();	R.generate_U();				sub_end_time = clock(); timerR += (double)sub_end_time - sub_start_time;
	sub_start_time = clock();	R.printIntersection();		sub_end_time = clock(); timerR += (double)sub_end_time - sub_start_time;
	return;
}



/* main 函数 */
void test()
{
	time_t t;
	srand((unsigned int)time(&t));
	initial(true);
	setup();
	distribution();
	computation();
	return;
}

int main()
{
	clock_t start_time = clock();
	for (int i = 0; i < TimeToTest; ++i)
	{
		cout << "/**************************************** Time: " << i + 1 << " ****************************************/" << endl;
		test();
		cout << endl << endl;
	}
	clock_t end_time = clock();
	cout << endl;
	cout << "/**************************************** SPSI-CA ****************************************/" << endl;
	cout << "kBit = " << kBit << "\t\tN = 2 ** " << N << "\t\tn = 2 ** " << n << "\t\tr = " << r << endl;
	cout << "Time: " << ((double)end_time - start_time) * baseNum << " / " << TimeToTest << " = " << ((double)end_time - start_time) * baseNum / TimeToTest << "ms" << endl;
	cout << "sizeof(*) = " << ((R.printSize() + S.printSize() + C.printSize()) >> 5) << " KB (*)" << endl << endl;
	return EXIT_SUCCESS;
}