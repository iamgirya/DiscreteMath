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

static ofstream fileOut;


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



vector<char> baseMoh = { 'a','b','c','d','e','f', 'g','h','j','k'};
// множество состоит из пар элемента и булевого значения, включён ли символ. Она используется для запрещения выбора элементов
pair<char, vector<pair<char, int>>> madeNextEltMoh(vector<char>& alphabet)
// коротко: пара из односвязного списка и его первого элемента
// создаёт пару из первого элемента алфавита и массива, в котором для каждого символа данного алфавита
// элемент под индексом этого символа равен паре следующего символа в алфавите и номеру нынешнего символа(начиная с 1)
// то есть для базового алфавита в созданном массиве 'следующих' под индексом 'b' хранится символ 'c' и число 2.
// данный массив необходим для быстрого определения следующей буквы при построении слов, а также для быстрой проверки возможности постоить слово.
{
    vector<pair<char, int>> nextElt(255);
    int number = 1;
    for (int i = 0; i < alphabet.size() - 1; i++)
    {
        nextElt[alphabet[i]] = make_pair(alphabet[i + 1], number);
        number++;
    }
    nextElt[alphabet[alphabet.size() - 1]] = make_pair(alphabet[0], number); // в конце зацикливаем созданный массив
                                                                              // после последнего элемента идёт первый
    return make_pair(alphabet[0], nextElt);
}
bool takeNextPost(pair<char, vector<pair<char, int>>>& nextElt, vector<char>& word)
// получает на вход слово, первую букву и множество следующих элементов. Изменяет слово на следующее в лексикографическом порядке.
{
    int i = word.size() - 1; // проходимся c конца
    char firstElt = nextElt.first; //получаем первую букву
    while (i != -1)
    {
        // меняем нынешнюю букву на следующую
        word[i] = nextElt.second[word[i]].first;
        if (word[i] != firstElt) // если был сделан цикл, то необходимо поменять и следующую букву
            return true;
        i--;
    }
    return false;
}
bool takeNextMoh(pair<char, vector<pair<char, int>>>& nextElt, vector<char>& moh, int coutOfOn)
// получает на вход первый элемент, множество следующих элементов. Изменяет подмножество на следующее в лексикографическом порядке того же размера.
{
    int top = 0;
    int i = moh.size() - 1;
    char firstElt = nextElt.first;
    moh[i] = nextElt.second[moh[i]].first; // меняем последнюю букву
    if (moh[i] != firstElt) // если не было цикла, всё отлично
        return true;
    while (true)
    {
        top++; // иначе начинаем идти по слову в начало
        if (i - top == -1)
            return false;

        if (coutOfOn - nextElt.second[moh[i - top]].second > top)
            break; // необходимо выбрать такую букву, что при изменении её на следующую, буквы справа от неё можно будет расположить в лексикографическом порядке
    }
    moh[i - top] = nextElt.second[moh[i - top]].first; // нашли такую букву
    while (top > 0)
    {
        top--; // меняем весь хвост слова
        moh[i - top] = nextElt.second[moh[i - top - 1]].first;
    }
}
bool takeNextPerest(pair<char, vector<pair<char, int>>>& nextElt, vector<char>& moh)
// получает на вход перестановку и выписывает следующую в лексикографическом порядке
{
    int top = 0;
    int i = moh.size() - 1;
    char firstElt = nextElt.first;
    bool noOk = true;
    while (noOk)
    {
        noOk = false;
        moh[i] = nextElt.second[moh[i]].first; // меняем последнюю букву
        if (moh[i] != firstElt)
            for (int j = i - 1; j > -1; j--)
                if (moh[i] == moh[j])
                    noOk = true;
    }
    if (moh[i] != firstElt) // если не было цикла, всё отлично
        return true;
    while (true)
    {
        top++; // иначе начинаем идти по слову в начало
        if (i - top == -1)
            return false;
        noOk = true;
        while (noOk)
        {
            noOk = false;
            moh[i - top] = nextElt.second[moh[i - top]].first;
            if (moh[i - top] != firstElt)
                for (int j = i - top - 1; j > -1; j--)
                    if (moh[i - top] == moh[j])
                        noOk = true;
        }
        if (moh[i - top] != firstElt) // если не было цикла, всё отлично
            break;
    }
    while (top > 0)
    {
        top--; // меняем весь хвост слова
        for (int j = i - top - 1; j > -1; j--)
            if (moh[i - top] == moh[j])
                noOk = true;
        while (noOk)
        {
            noOk = false;
            moh[i - top] = nextElt.second[moh[i - top]].first;
            if (moh[i - top] != firstElt)
                for (int j = i - top - 1; j > -1; j--)
                    if (moh[i - top] == moh[j])
                        noOk = true;
        }
    }
    return true;
}
pair<char, vector<pair<char, int>>> deactivateElt(pair<char, vector<pair<char, int>>>& nextElt, vector<char>& word)
// функция создаёт множество следующих, в котором элементы, находящиеся в word исключены.
// причём создаётся новое множество следующих, чтобы не уничтожать старое
{
    vector<pair<char, int>> newNextElt(255);
    int iter = 0, number = 1;
    char firstChar = nextElt.first;
    char tmp = firstChar, last = tmp;
    char newfirstChar = firstChar;
    do
    {
        if (iter < word.size() && tmp == word[iter] && tmp == newfirstChar)
        {
            newfirstChar = nextElt.second[tmp].first;
            iter++;
            last = nextElt.second[tmp].first;
        }
        else if (iter == word.size() || nextElt.second[tmp].first != word[iter])
        {
            newNextElt[last] = make_pair(nextElt.second[tmp].first, number);
            last = nextElt.second[tmp].first;
            number++;
        }
        else if (iter < word.size() && nextElt.second[tmp].first == word[iter])
        {
            iter++;
        }
        tmp = nextElt.second[tmp].first;
    } while (nextElt.second[tmp].first != firstChar);
    newNextElt[last] = make_pair(newfirstChar, number);

    return make_pair(newfirstChar, newNextElt);
}
char takeNnextElt(pair<char, vector<pair<char, int>>>& nextElt, int n)
{
    char c = nextElt.first;
    for (int i = 1; i < n; i++)
    {
        c = nextElt.second[c].first;
    }
    return c;
}
bool takeNextMohPov(pair<char, vector<pair<char, int>>>& nextElt, vector<char>& moh)
// получает на вход первый элемент, множество следующих элементов. Изменяет подмножество с повторениями на следующее в лексикографическом порядке того же размера.
{
    int top = 0;
    int i = moh.size() - 1;
    char firstElt = nextElt.first;
    moh[i] = nextElt.second[moh[i]].first; // меняем последнюю букву
    if (moh[i] != firstElt) // если не было цикла, всё отлично
        return true;
    while (true)
    {
        top++; // иначе начинаем идти по слову в начало
        if (i - top == -1)
            return false;

        if (nextElt.second[moh[i - top]].first != firstElt)
            break; // необходимо выбрать такую букву, что при изменении её на следующую, буквы справа от неё можно будет расположить в лексикографическом порядке
    }
    moh[i - top] = nextElt.second[moh[i - top]].first; // нашли такую букву
    while (top > 0)
    {
        top--; // меняем весь хвост слова
        moh[i - top] = moh[i - top - 1];
    }
}


int main()
{  SetConsoleCP(1251); SetConsoleOutputCP(1251);  // Подключаем вывод русских букв





    // Необходимо построить все слова длины n, в которых две буквы повторяются не более k раз.
    // алфавит состоит из: a, b, c, d, e, f, g, h, j, k





    int n, k;
    while (true)
    {
        cout << "Введите длину слова: ";
        cin >> n;
        cout << "Введите максимальное количество повторений букв: ";
        cin >> k;

        if (2 * k <= n && n <= 2 * k + 8 && k >= 2)
            break;
        cout << "Непривильно введены изначальные данные, попробуйте снова";
    }


    ll sum = 0;
    for (int k1 = 2; k1 <= k; k1++)
    {
        for (int k2 = k1; k2 <= k; k2++)
        {
            if (k1 != k2)
            {
                sum += 10 * 9 * C(k1, n) * C(k2, n - k1) * C(n - k1 - k2, 8) * fact(n - k1 - k2);
            }
            else
            {
                sum += 5 * 9 * C(k1, n) * C(k2, n - k1) * C(n - k1 - k2, 8) * fact(n - k1 - k2);
            }
        }
    }
    cout << "Количество слов по формуле: " << sum << endl;




    vector<char> mohOfNum;
    vector<char> c1, c2, c3, c4, c5, c6, a1, a2, a3, a4, a5, a6;
    vector<char> startMoh = { 'a','a','a' };
    pair<char, vector<pair<char, int>>> nextMoh, numNextMoh, nextMoh1, numNextMoh1, nextMoh2, numNextMoh2, nextMoh3, nextMoh4, numNextMoh3, numNextMoh4;
    ll count = 0;
    fileOut.open(("IZ_10.2.txt"));
    nextMoh = madeNextEltMoh(baseMoh);
    for (int i = 1; i <= n; i++)
        mohOfNum.push_back(i + 48);
    numNextMoh = madeNextEltMoh(mohOfNum);
    for (int k1 = 2; k1 <= k; k1++)
    {
        for (int k2 = k1; k2 <= k; k2++)
        {


            if (k1 != k2)
            {
                a1 = { 'a' };
                do// выбираем 1 букву из 10
                {
                    nextMoh1 = deactivateElt(nextMoh, a1); // деактивируем буквы, пересобираем множество следующих
                    
                    c1 = {};
                    for (int i = 1; i <= k1; i++)
                        c1.push_back(takeNnextElt(numNextMoh, i));

                    do // выбираем k1 мест под 1-ю повторяющуся букву из n
                    {
                        numNextMoh1 = deactivateElt(numNextMoh, c1);

                        a2 = { takeNnextElt(nextMoh1,1) };
                        do // выбираем 1 символ из 9
                        {
                            nextMoh2 = deactivateElt(nextMoh1, a2);
                            c2 = {};
                            for (int i = 1; i <= k2; i++)
                                c2.push_back(takeNnextElt(numNextMoh1, i));
                            do // выбираем k2 места под 2-ю повторяющуся букву из n-k1
                            {
                                numNextMoh2 = deactivateElt(numNextMoh1, c2);
                                a3 = {};
                                for (int i = 1; i <= n-k1-k2; i++)
                                    a3.push_back(takeNnextElt(nextMoh2, i));
                                do // выбираем оставшиеся n-k1-k2 символа из 8
                                {
                                    c3 = {};
                                    for (int i = 1; i <= n - k1 - k2; i++)
                                        c3.push_back(takeNnextElt(numNextMoh2,i));
                                    do // выбираем перестановку для последних n-k1-k2 символов
                                    {
                                        for (int i = 1; i <= n; i++)
                                        {
                                            for (int j = 0; j < n; j++)
                                            {
                                                if (j < c1.size() && c1[j] - 48 == i)
                                                {
                                                    fileOut << a1[0];
                                                    break;
                                                }
                                                else if (j < c2.size() && c2[j] - 48 == i)
                                                {
                                                    fileOut << a2[0];
                                                    break;
                                                }
                                                else if (j < c3.size() && c3[j] - 48 == i)
                                                {
                                                    fileOut << a3[j];
                                                    break;
                                                }
                                            }
                                        }
                                        fileOut << endl;
                                        count++;
                                    } while (takeNextPerest(numNextMoh2, c3));
                                } while (takeNextMoh(nextMoh2, a3, 8));
                            } while (takeNextMoh(numNextMoh1, c2, n-k1));
                        } while (takeNextMoh(nextMoh1, a2, 9));
                    } while (takeNextMoh(numNextMoh, c1, n));
                } while (takeNextMoh(nextMoh, a1, 10));
            }
            else
            {
                a1 = { 'a', 'b' };
                do// выбираем 2 букву из 10
                {
                    nextMoh1 = deactivateElt(nextMoh, a1); // деактивируем буквы, пересобираем множество следующих

                    c1 = {};
                    for (int i = 1; i <= k1; i++)
                        c1.push_back(takeNnextElt(numNextMoh, i));

                    do // выбираем k1 мест под 1-ю повторяющуся букву из n
                    {
                        numNextMoh1 = deactivateElt(numNextMoh, c1);
                        c2 = {};
                        for (int i = 1; i <= k2; i++)
                            c2.push_back(takeNnextElt(numNextMoh1, i));
                        do // выбираем k1 места под 2-ю повторяющуся букву из n-k1
                        {
                            numNextMoh2 = deactivateElt(numNextMoh1, c2);
                            
                            if (2 * k1 != n)
                            {
                                a2 = {};
                                for (int i = 1; i <= n - k1 - k2; i++)
                                    a2.push_back(takeNnextElt(nextMoh1, i));
                                do // выбираем оставшиеся n-2*k1 символа из 8
                                {
                                    c3 = {};
                                    for (int i = 1; i <= n - k1 - k2; i++)
                                        c3.push_back(takeNnextElt(numNextMoh2, i));
                                    do // выбираем перестановку для последних n-k1-k2 символов
                                    {
                                        for (int i = 1; i <= n; i++)
                                        {
                                            for (int j = 0; j < n; j++)
                                            {
                                                if (j < c1.size() && c1[j] - 48 == i)
                                                {
                                                    fileOut << a1[0];
                                                    break;
                                                }
                                                else if (j < c2.size() && c2[j] - 48 == i)
                                                {
                                                    fileOut << a1[1];
                                                    break;
                                                }
                                                else if (j < c3.size() && c3[j] - 48 == i)
                                                {
                                                    fileOut << a2[j];
                                                    break;
                                                }
                                            }
                                        }
                                        fileOut << endl;
                                        count++;
                                    } while (takeNextPerest(numNextMoh2, c3));
                                } while (takeNextMoh(nextMoh1, a2, 8));
                            }
                            else
                            {
                                for (int i = 1; i <= n; i++)
                                {
                                    for (int j = 0; j < n; j++)
                                    {
                                        if (j < c1.size() && c1[j] - 48 == i)
                                        {
                                            fileOut << a1[0];
                                            break;
                                        }
                                        else if (j < c2.size() && c2[j] - 48 == i)
                                        {
                                            fileOut << a1[1];
                                            break;
                                        }
                                    }
                                }
                                fileOut << endl;
                                count++;
                            }
                        } while (takeNextMoh(numNextMoh1, c2, n - k1));
                    } while (takeNextMoh(numNextMoh, c1, n));
                } while (takeNextMoh(nextMoh, a1, 10));
            }
        }
    }
    
    cout << "Количество слов в файле: " << count << endl;

    
}