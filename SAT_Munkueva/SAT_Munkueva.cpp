// IntervalForSATProject.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <iostream>       // std::cout
#include<conio.h>
#include "pch.h"
#include "BBV.h"
#include <string>
#include <string.h>

#include <stack>          // std::stack
using namespace std;
/*
Как было предложено в лекциях

struct Interval
{
unsigned long vec;
unsigned long dnc;
};

*/

/*
В предложенном классе булев вектор - очень полезная операция это перегруженная операция индексации,
с помощью  которой можно как установить новое значение iого разряда, так и получить значение iого разряда.
*/

class Interval
{
	BBV vec;
	BBV dnc;
public:
	explicit Interval(size_t len = 8);
	Interval(const char* vec_in, const char* dnc_in);
	explicit Interval(const char* vector);
	Interval(BBV& vec_in, BBV& dnc_in);
	void setInterval(BBV& vec, BBV& dnc);
	Interval& operator=(Interval& ibv);
	bool operator==(Interval& ibv);
	bool operator!=(Interval& ibv);

	operator string();

	int length(); // Длина или количество переменных 
	int rang();// Ранг интервала
	bool isOrthogonal(Interval& ibv);
	Interval& mergeInterval(Interval& ibv); // Построение пересечения интервалов
	bool isIntersection(Interval& ibv);
	bool isAbsorb(Interval& ibv);
	char getValue(int ix);//Получить i ое значение компоненты интервала
	BBV getVec();
	BBV getDnc();
	void setVec(BBV& v);
	void setDnc(BBV& v);

	void setValue(char value, int ix); //Установить iое значение интервала

};

/*

.i 10
.p 8
0-00111---
001-1-----
10--10000-
1000111---
0-00111---
001-1-----
10--10000-
1000111---
*/

class NodeBoolTree
{
public:
	NodeBoolTree* lt, * rt;
	Interval** dnf;//множество интервалов
	int count;// мощьность множества интервалов
	BBV mask;//
	unsigned int var;// номер переменной xi разложения
	bool fixVal_1;//флаг, указывающий на фиксацию значения 1, для рассматриваемой перемемнной xi(var) 
	bool fixVal_0;//флаг, указывающий на фиксацию значения 0, для рассматриваемой перемемнной xi(var) 
	Interval Solution;// Для сохранения решения
};

/*
Рассмотрим пример полного обхода дерева.
Прсосмотрим все переменные
Здесь на вход приходят следующие параметры

1. Interval **setIntevals -  Множество интервалов, представленной в виде массива указателей;
2. size_t size - Размер массива указателей на интервалы;
3. unsigned countVar - общее количество переменных;
*/
void TreeTraversal(Interval** setIntevals, size_t size, unsigned countVar)
{
	//Полный обход дерева

	std::stack<NodeBoolTree*> mystack;//Объявляем стек , в котором будем сохранять узлы дерева, для реализации рекурсивного обхода.
	bool continueTravesal = true;//Переменная флаг, необходимая для организации обхода. Значение true - нужно продолжать обход дерева, false - прекратить обход
	bool isSolution = false;
	bool isNotExist = false;

	NodeBoolTree* currentNode = nullptr;//указатель на текущий адрес узла дерева.

	if (setIntevals) //проверка на корректность входных данных, в частности, если  setIntevals == NULL, то обход не выполняем так как исходное множество интервалов не задано.
	{
		int posA = 0;// рабочая переменная для хранения текущей переменной
		//вспомогательные ветора и интервалы
		BBV allVars(countVar);//булев ветктор 
		BBV Nol(countVar);//нулевой булев вектор - требуется для выполнения сравнения некторого результатат на булевыми векторами с нулевым векторм
		Interval tmpI(countVar);//рабочий интервал
		BBV NotNol(countVar);//не нулевой рабочий вектор. Ветор в которм все компоненты принимают значение 1.
		NotNol = ~Nol;//инвертируем значение нулевого вектора, что бы получить 
		tmpI.setDnc(NotNol);
		//Выводим только для тестирования, в дальнейшем вывод нужно закомментировать
		cout << (string)tmpI;

		allVars = ~allVars;//Помечаем все компоненты для обхода

		currentNode = new NodeBoolTree();//Создаем начальный узел дерева обхода

		currentNode->dnf = new Interval * [size];//Инициализируем поле dnf(создаем множество интервалов для данного узла). В начальный момент времени это исходное множество.

		for (int ix = 0; ix < size; ix++) //Заполнение множества интервалов адресами интервалов исходного множества 
		{
			currentNode->dnf[ix] = setIntevals[ix];	//просто копируем адреса, из setIntevals[ix] в currentNode->dnf[ix].	
		}

		//Изначально задаем для полей  fixVal_0 и fixVal_1 значение false, что  будет говорить о том, что для данного узла еще не выполнялся анализ, 
		//не выбиралась переменная обхода и для нее не фиксировалось значение.
		currentNode->fixVal_0 = false;
		currentNode->fixVal_1 = false;
		//currentNode->Solution = tmpI;//Инициализация интервалом tmpI в котором все переменные принимают значение '-'. 

		//По мере фиксирования значений в этом поле отмечаются советующие значения
		cout << (string)currentNode->Solution;
		// currentNode - текущее множество 

		while (continueTravesal)// Пока continueTravesal принимает значение true  - продолжаем обход
		{

			if (currentNode && (!currentNode->fixVal_0 && !currentNode->fixVal_1)) //Это условие, которое указвает, на то, что для данного узла не проводился анализ 
			{
				//Здесь, необходимо анализировать текущее множество интервалов на предмет поиска переменной ветвления и фиксации значения для выбранной переменной ветвления.
				//
				/*В предлагаемой реализации, так как это просто пример, мы моделируем общее поведение, т.е. последовательно для каждой переменной осуществляем ветвление.
				В часности, всегда, для выбранной переменной фиксируем значение 0, а этот же узел, но в которм фиксируем значение 1 заносим в стек.*/
				if (currentNode->var == countVar)//Здесь, условие  - если рассматриваемы узел, в котором зафиксирована последняя переменная
					//(так как мы  последовательно перебираем переменные интервала) 
				{
					//если это условие выполняется то, текущий узел нам не нужен, так как мы уже все вершины рассмотрели. Его удаляем, а из стека достаем новый.   

					delete currentNode;
					currentNode = nullptr;

					//Достаем элемент из стека. Если стек пуст, то прекращаем обход.
					if (!mystack.empty())
					{
						currentNode = mystack.top();//записали адрес следующего элемента
						mystack.pop();//удаление из стека.
						//Вывод информации о узле, т.е. номер переменной, и какое значение было зафиксировано  
						cout << "FromSteck" << '(' << currentNode->var << ',' << "L(" << currentNode->fixVal_0 << ")," << "R(" << currentNode->fixVal_1 << ")" << endl;
					}
					else
						continueTravesal = false;
				}
				else
				{
					// Здесь фиксируем переменную обхода, так как обходим последовательно все, то выбираем следующую 
					posA++;//Переходим к следующей компоненте
					currentNode->var = posA;
					currentNode->fixVal_0 = true;//всегда, сначала фиксируем значние 0. Напомню, что сюда попадаем в случае когда для узла не проводили анализа 
					currentNode->Solution.setValue('0', posA);
					//Здесь создаем новый узел, но в которм для рассматриваем переменной инициализируем противоположное значение.
					NodeBoolTree* newNode = new NodeBoolTree;
					newNode->fixVal_1 = true;//фиксируем противоположное значение 
					newNode->var = posA;
					mystack.push(newNode);//Знасим этот узел в стек
					cout << "New Node with Var" << '(' << posA << ',' << "L(" << currentNode->fixVal_0 << ")," << "R(" << currentNode->fixVal_1 << ")" << endl;
				}
			}
			else
			{
				//Сюда мы попадаем, в случае, когда одно из значений, у рассматриваемого узла, зафиксировано, а следовательно, 
				//нужно выполнить сокращения множества интервалов относительно выбранной переменной.
				//здесь, также, можно выполнять анализ получившегося множества. Если найдено решение, то обход дерева прекращается.
				//В нашем примере мы не анализируем, а просто имитируем сокращение, но помним, что после сокращения узел обновился, а следовательно,
				//сбрасываем все ранее зафиксированные значения. Теперь узел готов для дальнейшего анализа.

				cout << "Simplification ";// относительно текущей фиксации значений
				posA = currentNode->var;
				cout << "Node" << '(' << posA << ',' << "L(" << currentNode->fixVal_0 << ")," << "R(" << currentNode->fixVal_1 << ")" << endl;
				//В целом, здесь можно примать решение о том , нужно ли для текущего продолжать анализ.
				//Если нужно, то сбрасываем зафиксированные значения, в противном случае останавливаем обход.
				currentNode->fixVal_0 = false;
				currentNode->fixVal_1 = false;

			}
		}
	}
}


class BoolEquation {

	NodeBoolTree** dnf;//множество интервалов
	Interval* root;//Корень уравнения
	int count; //количество конъюнкций
public:
	Interval FindRoot();
	void CreateTree();
	void InitEquation(Interval* set, size_t size);
	void PrintEquation();
};

void BoolEquation::InitEquation(Interval* set, size_t size)
{
	dnf = NULL;
	root = NULL;
	count = 0; //количество конъюнкций

	if (set)
	{



	}


}
Interval BoolEquation::FindRoot()
{
	Interval I("1-----");
	return I;
}
void BoolEquation::CreateTree()
{



}
void BoolEquation::PrintEquation()
{
}
int main()
{

	try
	{
		char setII[10][100] = {
			"00--1-1-100",
			"1-----1-100",
			"10011-00000",
			"--001-1----",
			"---1001-000",
			"00000-1-100",
			"--000000---",
			"1---1-0010-",
			"--0-----100",
			"1--11-1--00"
		};

		int sz = 10;
		//
		Interval** setIntevals = new Interval * [sz];

		for (int ix = 0; ix < sz; ix++)
		{
			setIntevals[ix] = new Interval(setII[ix]);
		}
		//setIntevals[ix] - хранится адрес объекта
		//*(setIntevals[ix]) - содержимое объекта
		//(string)(*setIntevals[ix]) - явное преобразование в строку
		for (int ix = 0; ix < sz; ix++)
		{
			cout << (string)(*setIntevals[ix]) << endl;
		}

		Interval** setTest = new Interval * [sz];

		for (int ix = 0; ix < sz; ix++)
		{
			setTest[ix] = nullptr;
		}
		// Объявляем интервал tmpI в котором будем фиксировать переменную xi
		Interval tmpI("-----------");
		int xi = 3;
		// Зафиксируем значение переменной xi значением 1
		tmpI.setValue('1', xi);

		cout << (string)tmpI;


		//Запишем в новое множество интервалов setTest все  интервалы которые пересекаются с интервалом  tmpI.

		for (int ix = 0; ix < sz; ix++)
		{

			if (!setIntevals[ix]->isOrthogonal(tmpI))
			{
				setTest[ix] = setIntevals[ix];// записваем не сами объекты, а адреса  объектов, хранящихся в setIntevals.  
			}
		}
		cout << "-----------------------------------------" << endl;

		//Выводим получившееся множество

		for (int ix = 0; ix < sz; ix++)
		{
			if (setTest[ix])
			{

				cout << (string)(*setTest[ix]) << endl;
			}
			else

				cout << "NULL" << endl;
		}

		//Обход дерева. Рассмотрим обход для трех переменных
		TreeTraversal(setTest, sz, 3);

		//Удаляем setIntevals полностью
		for (int ix = 0; ix < sz; ix++)
		{
			delete setIntevals[ix];
			setIntevals[ix] = nullptr;
		}

		//После удаление setIntevals, в структуре setTest все адреса не действительные, что не удивительно.
		//Нужно обнулить все ячейки
		for (int ix = 0; ix < sz; ix++)
		{
			setTest[ix] = nullptr;
		}

		//Work with interval

		BBV tmpD("111000");

		Interval I(45);
		string str = I;
		cout << str << endl;

		Interval II("10010101", "01100010");

		string ternaryVector_II = II;

		cout << "Ternary Vector  --{ " << ternaryVector_II << "}" << endl;

		Interval III("100--1101-101010---101010---11-1-1-1-1");

		string ternaryVector_III = III;

		cout << "Ternary Vector  --{ " << ternaryVector_III << "}" << endl;

		//Interval II_A("1--0-100");
		//Interval II_B("1-11-100");

		Interval II_A("1----100");
		Interval II_B("1-11-100");


		bool isOrthogonal = II_A.isOrthogonal(II_B);
		if (isOrthogonal)
		{
			cout << "Yes" << endl;
		}
		else
		{
			cout << "No" << endl;
		}
		//Здесь, изменим компоненты интервала "1--0-100";
		II_A.setValue('0', 0);
		cout << (string)II_A << endl;
		II_A.setValue('-', 6);
		cout << (string)II_A << endl;
		II_A.setValue('-', 7);
		cout << (string)II_A << endl;
		II_A.setValue('1', 1);
		cout << (string)II_A << endl;

		II_A.setValue('0', 2);
		cout << (string)II_A << endl;

		II_A.setValue('1', 3);
		cout << (string)II_A << endl;

		/////////Start tests/////////////////////////
		//Примеры использования булевого вектора/
		//Замечание: При выполнении бинарных операций ветора должны быть оденоковой длины иначе ошибка.

		BBV bv_1("0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
		//Инициализируем с помощью строки. Все что не ноль, это 1, т.е. в соответствующих разрядах будет стоять знвчение 1.

		BBV bv_2("wret00rtrtryt000yjfgyjg0000jkhjkhjkm00000jkmhjkhjkm0000000jhkhjkmhjmk000000000j,khjkhjk000jkhm,njkm,kjk000");

		BBV tmp(bv_1);
		//вывод векторов
		cout << "bv_1 = {" << bv_1 << "}" << endl;
		cout << "bv_1 = {" << bv_2 << "}" << endl;
		// С помошью перегруженных квадратных скобок можно обратится к iтому разряду, либо получить значение,  либо записать. 
		//Например, в каждый разрад установим значение 1(также что не ноль -  то 1)
	   // tmp = ~tmp;

		//for (int i = 0; i < bv_1.getSize(); i++)//Этот блок не отработал!! Исправить!!!!!!!!
		//{

			//bv_1[i] = 3;
			////вывод вектора
			//cout << bv_1 << endl;

	//	}
		//Можно скопировать поэлементно значения  другого вектора
		//for (int i = 0; i < bv_1.getSize(); i++)//Этот блок не отработал!! Исправить!!!!!!!!
		//{

			//bv_1[i] = bv_2[i];
		   // cout << bv_1 << endl;
		//}
		//Инверсия вектора
		bv_1 = ~bv_1;
		cout << "Inversion = " << bv_1 << endl;

		//Конвертирование строки в булев вектор
		char strBuff[80] = "111111111000000000101010101001010101010000000111111";
		cout << "Input str:\n";
		tmp = strBuff;
		cout << "Convert Str to BBV = " << tmp << endl;
		//Булевые операции
		cout << "operation & = " << (bv_1 & bv_2) << endl;
		cout << "operation ^ = " << (bv_1 ^ bv_2) << endl;
		cout << "operation | = " << (bv_1 | bv_1) << endl;


	}
	catch (int err)
	{
		if (err == 0)
			cout << "Memory error.\n";
		if (err == 1)
			cout << "Invalid initialization.\n";
		if (err == 2)
			cout << "Vectors are not appropriate for this operation.\n";
	}

}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.

Interval::Interval(size_t len)
{
	vec = BBV(len);
	dnc = BBV(len);
}

Interval::Interval(const char* vec_in, const char* dnc_in)
{
	//Проверяем , если строки не пустые и их длины совпадают, то инициализируем вектора, иначе ошибка и в этом случае создаем нулевые веткора мин длины(1 байт).
	if (vec_in && dnc_in && strlen(vec_in) == strlen(dnc_in))
	{
		vec = BBV(vec_in);
		dnc = BBV(dnc_in);
	}
	else
	{
		vec = BBV(8);
		dnc = BBV(8);
	}

}

Interval::Interval(const char* vector)
{
	if (vector)
	{
		size_t sz = strlen(vector);
		vec = BBV(sz);
		dnc = BBV(sz);
		int ix = 0;
		while (ix < sz)
		{
			if (vector[ix] == '-')
				dnc[ix] = 1;
			else
				if (vector[ix] == '1')
					vec[ix] = 1;
			ix++;
		}
	}
}

Interval::Interval(BBV& vec_in, BBV& dnc_in)
{
	vec = vec_in;
	dnc = dnc_in;

}

void Interval::setInterval(BBV& vec, BBV& dnc)
{
}

Interval& Interval::operator=(Interval& ibv)
{
	// TODO: вставьте здесь оператор return

	return *this;
}

bool Interval::operator==(Interval& ibv)
{
	if (vec == ibv.vec && dnc == ibv.dnc)
		return true;
	return false;
}

bool Interval::operator!=(Interval& ibv)
{
	if (vec != ibv.vec || dnc != ibv.dnc)
		return true;
	return false;
}

Interval::operator string()
{
	size_t sz = vec.getSize();
	string str(vec.getSize(), '0');

	for (int ix = 0; ix < sz; ix++)
	{
		str[ix] = getValue(ix);
	}

	return str;
}

int Interval::length()
{
	return vec.getSize();
}

int Interval::rang()
{
	//Определяем ранг интервала
	return (vec.getSize() - dnc.getWeight());
}

//Реализуем в соответсвии с примером из лекций
/*

bool IsOrthogonal(struct Interval u, struct Interval v )
{
unsigned long tmpUV, tmpU, tmpV;
tmpUV = v.dnc | u.dnc;
tmpU = u.vec | tmpUV;
tmpV = v.vec | tmpUV;
if ((tmpU ^ tmpV) != 0)
return true;
return false;
}

*/

bool Interval::isOrthogonal(Interval& ibv)
{
	cout << (string)(*this) << endl;
	cout << (string)ibv << endl;

	BBV zero(vec.getSize());
	BBV tmpUV(zero);
	BBV tmpU(zero);
	BBV tmpV(zero);
	BBV answer(zero);
	tmpUV = dnc | ibv.dnc;
	//Output vector for debug

	//cout << (string)tmpUV << endl;

	tmpU = vec | tmpUV;
	//Output vector for debug
	//cout << (string)tmpU << endl;

	tmpV = ibv.vec | tmpUV;
	//Output vector for debug

	//cout << (string)tmpV << endl;

	answer = (tmpU ^ tmpV);
	//Output vector for debug

	//cout << (string)answer << endl;

	if (answer != zero)
		return true;

	return false;

}

Interval& Interval::mergeInterval(Interval& ibv)
{

	//Если требуется, реализовать самостоятельно
	// TODO: вставьте здесь оператор return

	return *this;
}



bool Interval::isIntersection(Interval& ibv)
{
	//Если требуется, реализовать самостоятельно

	return false;
}

bool Interval::isAbsorb(Interval& ibv)
{
	//Если требуется, реализовать самостоятельно

	return false;
}

char Interval::getValue(int ix)
{
	if (ix < 0 || ix > vec.getSize())
		throw "Out of range";

	if (dnc[ix] == 1)
		return '-';

	if (vec[ix] == 1)
		return '1';
	return '0';
}

BBV Interval::getVec()
{
	return this->vec;
}

BBV Interval::getDnc()
{
	return this->dnc;
}

void Interval::setVec(BBV& v)
{
	dnc = v;
}

void Interval::setDnc(BBV& v)
{
	dnc = v;
}

void Interval::setValue(char value, int ix)
{
	if (ix < 0 || ix > vec.getSize())
		throw "Out of range";
	if (value == '-')
	{
		dnc[ix] = 1;
		vec[ix] = 0;
	}
	else
		if (value == '0') {
			vec[ix] = 0;
			dnc[ix] = 0;
		}
		else
		{
			vec[ix] = 1;
			dnc[ix] = 0;
		}
}
