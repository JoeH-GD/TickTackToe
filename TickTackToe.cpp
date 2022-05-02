#include <iostream>
#include <random>
#include <stdLib.h>
#include <chrono>
using namespace std;

//Перечисление возможных символов для сзаполнения игрового поля
enum TCell :char {
	CROSS = 'x',
	ZERO = '0',
	EMPTY = '_'
};

//Перечисление статусов игры
enum TProgress {
	IN_PROGRESS,
	WON_HUMAN,
	WON_AI,
	DRAW

};

//Структура для координат игового поля 
struct TCoord {
	size_t y{ 0U }; //Инициализирован беззнаковым 0
	size_t x {0U};
};

//побайтовое выравнивание структуры для экономии памяти
#pragma pack (push,1)
struct TGame {
	const size_t SIZE = 3;
	TCell** ppField = nullptr;
	TProgress progress = IN_PROGRESS; //сразу выставляем статус игры "в процессе"
	size_t turn{ 0U }; //пошагово инкрементируется, четные - человек, нечетные - ПК
	TCell human;
	TCell ai;
};
#pragma pack (pop)

//====================================================================================

inline void ClearScr() //inline уменьшает время вызова функции
{
	system("cls");// очищает экран (подходит только для винды)
	//std::cout << "\x1B[2J\x1B[H";
}

//функция для вызова генератора случайных чисел в промежутке от мин до макс
int32_t __fastcall GetRandomNum(int32_t min, int32_t max) //__fastcall позволяет передавать аргументы в регистры
{
	const static auto seed = chrono::system_clock::now().time_since_epoch().count();//отсчитывает время от epoch (для винды это 1 ярваря 1601)
	static mt19937_64 generator(seed); //референс к генератору псевдослучайных чисел
	uniform_int_distribution<int32_t> dis(min, max); //распределение
	return dis(generator);//возвращаемое случайное число
}

void __fastcall InitGame(TGame& g) 
{
	g.ppField = new TCell * [g.SIZE];//выделяем столбец указателей на строки игрового поля

	for (size_t i = 0; i < g.SIZE; i++)
	{
		g.ppField[i] = new TCell[g.SIZE];//строки игрового поля
	}

	for (size_t y = 0; y < g.SIZE; y++) {
		for (size_t x = 0; x < g.SIZE; x++)
		{
			g.ppField[y][x] = EMPTY; //при инициализации все ячейки считаются пустыми
		}
	}

	//определяем очередность хода
	if (GetRandomNum(0, 1000) > 500)
	{
		g.human = CROSS;
		g.ai = ZERO;
		g.turn = 0;
	}

	else {
		g.ai = CROSS;
		g.human = ZERO;
		g.turn = 1;
	}
}

void __fastcall PrintGame(const TGame& g) 
{
	cout << "    ";
	
	for (size_t x = 0; x < g.SIZE; x++)
	{
		cout << x + 1 << "   ";
	}

		cout << endl;

	for (size_t y = 0; y < g.SIZE; y++) {

		cout << y + 1 << " | ";

		for (size_t x = 0; x < g.SIZE; x++)
		{
			cout << g.ppField[y][x] << " | ";
		}

		cout << endl;
	}

	cout <<endl<< "Human: " << g.human << endl << "Computer: " << g.ai << endl;
}

TProgress __fastcall GetWon(const TGame& g) 
{
	//проверка по строкам
	for (size_t y = 0; y < g.SIZE; y++)
	{
		if (g.ppField[y][0] == g.ppField[y][1] && g.ppField[y][0] == g.ppField[y][2])
		{

			if (g.ppField[y][0] == g.human) return WON_HUMAN;
			if (g.ppField[y][0] == g.ai) return WON_AI;
		}


	}

	//проверка по столбцам
	for (size_t x = 0; x < g.SIZE; x++)
	{
		if (g.ppField[0][x] == g.ppField[1][x] && g.ppField[0][x] == g.ppField[2][x])
		{

			if (g.ppField[0][x] == g.human) return WON_HUMAN;
			if (g.ppField[0][x] == g.ai) return WON_AI;
		}


	}


	//проверка диагоналей 
	if (g.ppField[0][0] == g.ppField[1][1] && g.ppField[0][0] == g.ppField[2][2])
	{

		if (g.ppField[0][0] == g.human) return WON_HUMAN;
		if (g.ppField[0][0] == g.ai) return WON_AI;
	}

	if (g.ppField[2][0] == g.ppField[1][1] && g.ppField[0][2] == g.ppField[1][1])
	{

		if (g.ppField[2][0] == g.human) return WON_HUMAN;
		if (g.ppField[2][0] == g.ai) return WON_AI;
	}

    //проверка на ничью
	bool bDraw = true;

	for (size_t y = 0; y < g.SIZE; y++) 
	{
		for (size_t x = 0; x < g.SIZE; x++)
		{
			if (g.ppField[x][y] == EMPTY)
			{
				bDraw = false;
				break;
			}

		}
		if (!bDraw) break;
	}

	if (bDraw) return DRAW;

	return IN_PROGRESS;

}

TCoord  __fastcall GetHumanCoord(const TGame& g)
{
	TCoord c;

	do {
		cout << "Enter x (1 ... 3): ";
		cin>> c.x;
		cout << "Enter y (1 ... 3) ";
		cin >> c.y;

		c.x--;
		c.y--;
	} while (c.x > 2 || c.y > 2 || g.ppField[c.y][c.x] != EMPTY); // повторяется, пока не будет указана правильная свободная ячейка

	return c;

}

TCoord  __fastcall GetAICoord(TGame& g)
{
	//1. Забота а выигрыше 
	for (size_t y = 0; y < g.SIZE; y++)
	{
		for (size_t x = 0; x < g.SIZE; x++)
		{
			if (g.ppField[y][x] == EMPTY)
			{
				g.ppField[y][x] = g.ai;
				if (GetWon(g) == WON_AI) {
					g.ppField[y][x] = EMPTY;
					return { y,x };
				}

			    g.ppField[y][x] = EMPTY;
			}
		}
	}
	//====================================================================================
	
	//2. мешать человеку выиграть

	for (size_t y = 0; y < g.SIZE; y++)
	{
		for (size_t x = 0; x < g.SIZE; x++)
		{
			if (g.ppField[y][x] == EMPTY)
			{
				g.ppField[y][x] = g.human;
				if (GetWon(g) == WON_HUMAN) {
					g.ppField[y][x] = EMPTY;
					return { y,x };
				}

				g.ppField[y][x] = EMPTY;
			}
		}
	}

	//=====================================================================================

	//3. Приоритеты + рандом

	//по приоритетам сначала центр
	if (g.ppField[1][1] == EMPTY) return { 1,1 };

	TCoord buff[4];
	size_t num{0};

	//теперь углы
	if (g.ppField[0][0] == EMPTY)
	{
		buff[num] = {0, 0};
		num++;
	}

	if (g.ppField[0][2] == EMPTY)
	{
		buff[num] = { 0, 2 };
		num++;
	}
	if (g.ppField[2][0] == EMPTY)
	{
		buff[num] = { 2, 0 };
		num++;
	}
	if (g.ppField[2][2] == EMPTY)
	{
		buff[num] = { 2, 2 };
		num++;
	}

	if (num > 0)
	{
		const size_t index = GetRandomNum(0, 1000) % num;
		return buff[index]; //возвращает случайный угол
	}
	//=============================================================================================

	//не углы
	if (g.ppField[0][1] == EMPTY)
	{
		buff[num] = { 0, 1 };
		num++;
	}

	if (g.ppField[1][2] == EMPTY)
	{
		buff[num] = { 1, 2 };
		num++;
	}
	if (g.ppField[2][1] == EMPTY)
	{
		buff[num] = { 2, 1 };
		num++;
	}
	if (g.ppField[1][0] == EMPTY)
	{
		buff[num] = { 1, 0 };
		num++;
	}

	if (num > 0)
	{
		const size_t index = GetRandomNum(0, 1000) % num;
		return buff[index]; //возвращает случайный не угол
	}

	return { 1,1 };//чтобы компилятор не ругался
}

void __fastcall Congrats(const TGame& g) 
{
	if (g.progress == WON_AI)
	{
		cout << "Computer wins. Go AI!"<<endl;
	}

	else if (g.progress == WON_HUMAN)
	{
		cout << "You won! Good job. But I will beat you next time!"<<endl;
	}

	else if (g.progress == DRAW)
	{
		cout << "No winner this time" << endl;
	}
}

void __fastcall DeinitGame(TGame& g)
{

	for (size_t i = 0; i < g.SIZE; i++)
	{
		delete[] g.ppField[i];
	}

	delete[] g.ppField;

	g.ppField = nullptr;//чтобы наверняка

}

//=================================================================================================

int main()
{
	TGame g;
	InitGame(g);

	ClearScr();
	PrintGame(g);

	do
	{
		//проверяем очередность хода
		if (g.turn % 2 == 0)
		{
			TCoord c = GetHumanCoord(g);
			g.ppField[c.y][c.x] = g.human; //ставим соответствующую фигурку
		}

		else {
			TCoord c = GetAICoord(g);
			g.ppField[c.y][c.x] = g.ai; //ставим соответствующую фигурку
		}

		ClearScr();
		PrintGame(g);

		g.turn++;//инкриментация очередности хода
		g.progress = GetWon(g);//проверка наличия победителя

	} while (g.progress == IN_PROGRESS);

	Congrats(g);

	DeinitGame(g);
}

