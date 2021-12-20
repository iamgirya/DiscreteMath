#include <algorithm> 
#include <cmath> 
#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <Windows.h>
using namespace std;
typedef long long ll;
/////////////////////////////////////////////////////
ll fact(ll n)
{
	ll tmp = 1;
	for (ll i = 1; i <= n; i++)
		tmp *= i;
	return tmp;
}
ll C(ll k, ll n)
{
	if (k == n)
		return 1;
	if (k > n)
		return 0;
	return fact(n) / (fact(k) * fact(n - k));
}

ll kolvoSvyaz(ll p)
{
	if (p == 1 || p == 2)
		return 1;
	ll tmp = 0;
	for (ll k = 1; k <= p - 1; k++)
	{
		tmp += C(k, p) * kolvoSvyaz(k) * k * pow(2, C(2, p - k));
		int gk = 1;
	}
	return pow(2, C(2, p)) - tmp / p;
}

ll kolvoEiler(ll p)
{
	if (p == 1)
		return 1;
	if (p == 2)
		return 0;
	ll tmp = 0;
	for (ll k = 1; k <= p - 1; k++)
		tmp += C(k, p) * kolvoEiler(k) * k * pow(2, C(2, p - 1 - k));
	return pow(2, C(2, p - 1)) - tmp / p;
}

ll kolvoKraska(ll p, ll k)
{
	if (p < k)
		return 0;
	if (k == 1)
		return 1;
	ll tmp = 0;
	for (ll n = 1; n <= p - 1; n++)
		tmp += C(n, p) * kolvoKraska(n, k - 1) * pow(2, n * (p - n));
	return tmp / k;
}

void pluss(vector<ll>& f, vector<ll>& s)
{
	for (ll i = 0; i < f.size(); i++)
		f[i] += s[i];
}

vector<ll> mult(vector<ll> f, vector<ll> s)
{
	vector<ll> tmp;
	tmp.resize(1000, 0);
	for (ll i = 0; i < f.size(); i++)
		for (ll j = 0; j < s.size(); j++)
			if (i + j < tmp.size())
				tmp[i + j] += f[i] * s[j];
	return tmp;
}

void multR(vector<ll>& f, ll s)
{
	for (ll i = 0; i < f.size(); i++)
		f[i] *= s;
}

void delR(vector<ll>& f, ll s)
{
	for (ll i = 0; i < f.size(); i++)
		f[i] /= s;
}

vector<ll> Wp(ll p)
{
	vector<ll> sum;
	sum.resize(1000, 0);
	for (ll n = 0; n <= p; n++)
	{
		vector<ll> tmp;
		tmp.resize(1000, 0);
		tmp[0] = 1;
		for (ll i = 1; i <= C(2, p) - n * (p - n); i++)
			tmp = mult(tmp, *(new vector<ll>{ 1, 1 }));
		for (ll i = 1; i <= n * (p - n); i++)
			tmp = mult(tmp, *(new vector<ll>{ 1, -1 }));
		multR(tmp, C(n, p));
		pluss(sum, tmp);
	}
	delR(sum, pow(2, p));
	return sum;
}

vector<ll> kolvoPQEiler(ll p)
{

	vector<ll> sum;
	sum.resize(1000, 0);
	if (p == 1)
	{
		sum[0] = 1;
		return sum;
	}
	vector<ll> ans = Wp(p);
	for (ll k = 1; k <= p - 1; k++)
	{
		vector<ll> tmp;
		tmp.resize(1000, 0);
		tmp = mult(kolvoPQEiler(k), Wp(p - k));
		multR(tmp, C(k, p) * k);
		pluss(sum, tmp);
	}
	delR(sum, -p);
	pluss(ans, sum);
	return ans;
}
static ofstream f1("11.2.1.txt");
static ofstream f2("11.2.2.txt");
static ofstream f3("11.2.3.txt");
static ofstream f4("11.2.4.txt");

ll c1 = 0, c2 = 0, c3 = 0, c4 = 0; // количество выведенных графов по задачам
const int n = 7, reb = 6; // количество вершин в задачах 1, 2, 4 и ребёр в 4
const int color = 5; // количество цветов
const int nn = 6; // количество вершин в задаче 3
void printG(ll mask, int num, vector<int> colors = *(new vector<int>)) // вывод графа, записанного в битовой маске, в виде матрицы смежности
{
	ofstream f;
	if (num == 4)
		f.swap(f4);
	else if (num == 1)
		f.swap(f1);
	else if (num == 2)
		f.swap(f2);     //выбираем файл для каждой задачи
	else if (num == 3)
		f.swap(f3);
	int size = 0;
	if (num == 3)
		size = nn;
	else
		size = n;

	for (int i = 1; i <= size; i++)
	{
		int lvl = ((i - 2) * (i - 1)) / 2;;
		for (int j = 1; j < i; j++)
		{
			f << ((mask & (1 << lvl + j - 1)) != 0) << ' ';  // проходимся до главной диагонали
		}
		f << 0 << ' ';  // главная диагональ
		for (int j = i; j < size; j++)
		{

			lvl = ((j - 1) * (j)) / 2;
			f << (0 != (mask & (1 << lvl + i - 1))) << ' '; // проходимся после главной диагонали
		}
		f << endl;
	}
	if (colors.size() != 0)
	{
		for (int i = 0; i < colors.size(); i++)
		{
			f << colors[i];   // если это задача 3, то мы передаём в функцию ещё и надоб цветов, выведем и их
		}
		f << endl;
	}
	f << endl;
	if (num == 4)
		f.swap(f4);
	else if (num == 1)
		f.swap(f1);
	else if (num == 2)
		f.swap(f2);
	else if (num == 3)
		f.swap(f3);
}
void nextMask(ll& mask, int lvl) // получает нынешнюю маску и количество вершин. 
{
	// Добавляя единицу сдвинутую на нужное количество мест(на уровень) мы можем рассмотреть все возможные варианты соединения рёбрами
	// новой вершины с уже построенными
	// так выполняется 2^(количество старых вершин) раз. Так как каждое ребро новой вершины и старой может быть может не быть.
	lvl = ((lvl - 1) * (lvl)) / 2;
	mask += (1 << lvl);
}
/////////////////////////////////////////////////////
bool isConnected(vector<vector<bool>>& matrShej, vector<bool>& was, int nowV, int endV)
{
	if (nowV == endV)
		return true;
	was[nowV] = true;
	for (int i = 0; i < was.size(); i++)
		if (matrShej[nowV][i] && !was[i])
			if (isConnected(matrShej, was, i, endV))
				return true;
	return false;
}
void addToList(ll mask) // добавляет граф к связным и проверяет, является ли он эйлеровым или эйлеровым с reb рёбрами
{
	c1++;
	printG(mask, 1);  //  к связным
	int count = 0;
	bool f = true;
	for (int i = 1; i <= n; i++)  // считается количество единиц каждой строке.
	{
		int sum = 0;
		int lvl = ((i - 2) * (i - 1)) / 2;;
		for (int j = 1; j < i; j++)
		{
			sum += ((mask & (1 << lvl + j - 1)) != 0);
		}
		for (int j = i; j < n; j++)
		{

			lvl = ((j - 1) * (j)) / 2;
			sum += (0 != (mask & (1 << lvl + i - 1)));
		}
		count += sum;
		if (sum % 2) // если это сумма единиц в каждой строке матрицы смежности - чётное, то граф эйлеров
		{
			f = false; // иначе он не эйлеров, а значит он и не эйлеров с reb рёбрами.
			break;
		}
	}
	count /= 2;
	if (f)
	{
		c2++;
		printG(mask, 2); // если эйлеров, записываем
		if (count == reb)
		{
			c4++;
			printG(mask, 4); // если эйлеров с 6-ю рёбрами, записываем
		}
	}

}
int nextNum(int number)
{
	int i = number / n;
	int j = number - i * n;
	if (j <= i)
		number += (i - j) + 1;
	else
		number += 1;
	if (number >= n * n)
		return -1;
	return number;
}
void destroyOne(vector<vector<bool>> matrShej, int number)
{

	int i = number / n;
	int j = number - i * n;

	int nNum = nextNum(number);
	if (nNum != -1)
		destroyOne(matrShej, nNum);

	matrShej[i][j] = false;
	matrShej[j][i] = false;
	vector<bool> was; was.resize(n, false);
	if (isConnected(matrShej, was, i, j))
	{
		//addToList(matrShej);
		if (nNum != -1)
			return destroyOne(matrShej, nNum);
	}
}
////////////////////////////////////////////////////
bool isPokrashen(ll mask, vector<int>& colors, int lvl) // проверяем, не нарушает ли покраску новая вершина
{
	int sum = ((lvl - 1) * (lvl)) / 2;
	for (int i = 0; i < lvl; i++)
		if (mask & (1 << sum + i) && colors[lvl] == colors[i]) // так как в маске находятся нижнедиагональные элементы матрицы смежности
			return false;									   // то только их и будем проверять.
	return true;
}
void addNewV(ll mask, vector<int> colors, int numOfV, int numOfColors)
{
	// мы уже имеем numOfV построенныхи покрашеных вершин с нормальной покраской. В этой функции мы добавляем новую вершину
	// красим её в каждый из возможных цветов и строим все возможные сочетания рёбер, после чего вновь вызываем функцию, пока есть не добавленные вершины
	// mask - граф, colors - массив цветов, numOfV - количество построенных вершим, numOfColors - количество использованных цветов
	bool f = true; //флаг для выхода из for при увеличении количества цветов
	ll startMask = mask;
	if (nn - numOfV < color - numOfColors)
		return; // если число недобавленных цветов больше числа недобавленных вершин - можно дальше не искать
	if (numOfColors == color && numOfV == nn)// если граф уже построен и все цвета поставлены
	{
		c3++;
		return printG(mask, 3, colors); // выводим
	}
	numOfV++; // стало на 1 вершину больше
	for (int newColor = 0; newColor <= numOfColors && newColor < color; newColor++)
	{// красим её во все прошлые цвета и красим в один новый, если можем
		mask = startMask;
		colors[numOfV - 1] = newColor;
		if (newColor == numOfColors) { f = false; numOfColors++; } // последняя итерация с новым цветом. Количество цветов +1

		int iterCount = pow(2, numOfV - 1); // количество новых возможных рёбер
		for (int i = 1; i < iterCount; i++) // просматриваем все возможные комбинации новых рёбер
		{
			if (isPokrashen(mask, colors, numOfV - 1)) // если удолетворяет покраске
				addNewV(mask, colors, numOfV, numOfColors); // рекурсивно обрабатываем новый граф
			nextMask(mask, numOfV - 1);
		}
		if (isPokrashen(mask, colors, numOfV - 1)) // последний граф, где все старые смежны с новой вершиной
			addNewV(mask, colors, numOfV, numOfColors);

		if (!f) // новый цвет уже был проверен, хватит
			break;
	}
}
////////////////////////////////////////////////////

int main()
{
	SetConsoleCP(1251);  // Подключаем ввод русских букв
	SetConsoleOutputCP(1251);

	// ту ду: переделай первую задачу под битовые маски 1412412414124124124124124124124124124214141241421

	// задача 11.2.3
	vector<int> colors; colors.resize(nn, -1);   // инициализация массива цветов.
	ll rebrMask = 0;							// если у вершины цвет -1 то она непокрашена
	colors[0] = 0;								// Красим 1-ю вершину в 1-й цвет.
	cout << "Количество графов порядка 6, что можно покрасить в 5 цветов, по формуле равно: "
		<< kolvoKraska(nn, color) << endl;      // Вычислим количество формулой
	addNewV(rebrMask, colors, 1, 1);			// Для записи матрицы будем использовать битовую маску.
												// Она будет заменять нижнюю диагональ матрицы смежности, записанную подряд.
	cout << "Количество графов порядка 6, что можно покрасить в 5 цветов, выведенных в файл равно: " << c3 << endl;

	/*
	ll rebrmask2 = (1<<n+1)-1;
	addToList(rebrmask2);
	destroyOne(rebrmask2, 0);
	f1.close(); f2.close(); f3.close();
	cout << "Количество связных графов порядка 7, выведенных в файл равно: " << c1 << endl;
	cout << "Количество связных графов порядка 7, согласно формуле равно: " << kolvoSvyaz(7) << endl;
	cout << "Количество эйлеровых графов порядка 7, выведенных в файл равно: " << c2 << endl;
	cout << "Количество эйлеровых графов порядка 7, согласно формуле равно: " << kolvoEiler(7) << endl;
	cout << "Количество эйлеровых графов порядка 7 с 6-ю рёбрами, выведенных в файл равно: " << c4 << endl;
	cout << "Количество эйлеровых графов порядка 7 с 6-ю рёбрами, согласно формуле равно: " << kolvoPQEiler(7)[6] << endl;
	*/
}
