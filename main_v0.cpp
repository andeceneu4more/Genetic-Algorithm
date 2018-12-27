#include <vector>
#include <fstream>
#include <random>
#include <string>
#include <bits/stdc++.h>
#include <cmath>
#include <algorithm>
#include <climits>
#define nmax 5005
using namespace std;

int nCrom, A, B, precizie, nGen, lungime, generatie;
double pIncrucisare, pMutatie;
double probMomentana[nmax], probPartiala[nmax];
string binar(long long x)
{
    int i = 0;
    string s = "";
    while (x || i < lungime)
    {
        s += ('0' + x % 2);
        x /= 2;
        i++;
    }
    reverse(s.begin(), s.end());
    return s;
}
long long zecimal(string s)
{
    long long x = 0, i;
    reverse(s.begin(), s.end());
    for (i = 0; i < s.size(); i++)
    {
        x = x +  (s[i] - '0') * (1 << i);
    }
    return x;
}
double Fitness(double x)
{
    return (- x * x + x + 2) * 1.0;
}
class cromozom
{
    string _cod;
    double _valoare, _fitness;
public:
    cromozom(string Cod = "", double Valoare = 0.0, double Fitness = 0.0):
    _cod(Cod), _valoare(Valoare), _fitness(Fitness){};
    inline void cod(string);
    inline string cod();
    inline double valoare();
    inline double fitness();
};
inline string cromozom :: cod()
{
    return _cod;
}
inline void cromozom :: cod(string nou)
{
    _cod = nou;
    long long x = zecimal(_cod);
    _valoare = (B - A) * 1.0 / ( (1 << lungime) - 1) * x * 1.0 + A;
    _fitness = Fitness(_valoare);
}
inline double cromozom :: valoare()
{
    return _valoare;
}
inline double cromozom :: fitness()
{
    return _fitness;
}
vector <cromozom> Crom;
vector <cromozom> populatieNoua;
vector <int> parinti;
random_device randDispozitiv;
mt19937 generator(randDispozitiv());
uniform_real_distribution <double> uniforma(0.0, 1.0);
uniform_int_distribution <int> uniformaInt(0, nmax);
ofstream fout("Evolutie.out");
void initializare()
{
    fstream fin("populatie.in");
    fin >> nCrom >> A >> B >> precizie >> nGen >> pIncrucisare >> pMutatie;
    lungime = ceil( log2((B - A) * pow(10, precizie)) );
    string individ;
    long long nrMax = (B - A) * pow(10, precizie), aleator, i;
    uniform_int_distribution <long long> uniformaInput(0, nrMax);
    cromozom C;
    Crom.push_back(C);
    for (i = 1; i <= nCrom; i++)
    {
        aleator = uniformaInput(generator);
        individ = binar(aleator);
        C.cod(individ);
        Crom.push_back(C);
    }
    fin.close();
}
void calculeazaProb()
{
    double suma = 0.0;
    int i;
    for (i = 1; i <= nCrom; i++)
        suma += Crom[i].fitness();
    for (i = 1; i <= nCrom; i++)
    {
        probMomentana[i] = Crom[i].fitness()  * 1.0 / suma;
        probPartiala[i] = probPartiala[i - 1] + probMomentana[i];
    }
}
int cautBin(int st, int dr, double x)
{
    int mid;
    while (st <= dr)
    {
        mid = (st + dr) / 2;
        if (probPartiala[mid] > x)
            dr = mid - 1;
        else
            st = mid + 1;
    }
    return st;
}
void ruleta()
{
    double aleator = uniforma(generator);
    int j = cautBin(1, nCrom, aleator);
    populatieNoua.push_back(Crom[j]);
}
void eParinte(int i)
{
    double aleator = uniforma(generator);
    vector <int> :: iterator it = find(parinti.begin(), parinti.end(), i);
    if (aleator < pIncrucisare && it == parinti.end())
    {
        parinti.push_back(i);
    }
}
void selectIndivizi()
{
    int i;
    populatieNoua.push_back(Crom[0]);
    // pentru ca numerotarea sa inceapa de la 1
    for (i = 1; i < Crom.size(); i++)
    {
        ruleta();
        eParinte(i);
    }
}
void incruciseaza()
{
    int i, aleator;
    string cod1, cod2, aux;
    for (i = 0; i < parinti.size() - 1; i += 2)
        {
            cod1 = populatieNoua[parinti[i]].cod();
            cod2 = populatieNoua[parinti[i + 1]].cod();
            aleator = uniformaInt(generator) % lungime;
            // un punct de rupere ( pe pozitia aleator )

            aux = cod1.substr(0, aleator);
            cod1.replace(0, aleator, cod2.substr(0, aleator));
            cod2.replace(0, aleator, aux);

            populatieNoua[parinti[i]].cod(cod1);
            populatieNoua[parinti[i + 1]].cod(cod2);
        }
    parinti.clear();
    // Incrucisare de tipul 1-2, 3-4, etc...
}
void mutatii()
{
    int i, pozAleator;
    double aleator;
    string aux;
    for (i = 1; i < Crom.size(); i++)
    {
        aleator = uniforma(generator);
        if (aleator < pMutatie)
        {
            aux = populatieNoua[i].cod();
            pozAleator = uniformaInt(generator) % lungime;
            if (aux[pozAleator] == '0')
                aux[pozAleator] = '1';
            else
                aux[pozAleator] = '0';
            // complementam
            populatieNoua[i].cod(aux);
        }
    }
}
void itereaza()
{
    int i;
    double valMax, fitnessMax;
    for (generatie = 1; generatie <= nGen; generatie++)
    {
        calculeazaProb();
        selectIndivizi();
        incruciseaza();
        mutatii();
        Crom.clear();
        fitnessMax = INT_MIN;
        for (i = 0; i < populatieNoua.size(); i++)
        {
            Crom.push_back(populatieNoua[i]);
            if (Crom[i].fitness() > fitnessMax)
            {
                fitnessMax = Crom[i].fitness();
                valMax = Crom[i].valoare();
            }
        }
        fout << "x = " << valMax << " f = " << fitnessMax << '\n';
        populatieNoua.clear();
    }
}
int main()
{
    initializare();
    fout << setprecision(10) << fixed;
    itereaza();
    fout.close();
    return 0;
}