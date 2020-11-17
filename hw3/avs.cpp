#include <iostream>
#include <thread>
#include <random>

// Вариант: 26
// Выполнил: Федченко Всеволод Александрович БПИ199

//Вторая задача об Острове Сокровищ.
//Шайка пиратов подпредводительством Джона Сильвера высадилась на берег Острова Сокровищ.
//Несмотря на добытую карту старого Флинта, местоположение сокровищ по - прежнему остается загадкой,
//поэтому искать клад приходится практически на ощупь. Так как Сильвер ходит на деревянной ноге,
//то самому бродить по джунглям ему не с руки. Джон Сильвер поделил остров на участки, а пиратов на небольшие группы.
//Каждой группе поручается искать клад на нескольких участках, а сам Сильвер ждет на берегу.
//Группа пиратов, обшарив одну часть острова, переходит к другой, еще не обследованной части.
//Закончив поиски, пираты возвращаются к Сильверу и докладывают о результатах.
//Требуется создать многопоточное приложение с управляющим потоком, моделирующее действия Сильвера и пиратов.
//При решении использовать парадигму портфеля задач.

using namespace std;

//Кол-во участков.
const int numOfSectors = 10000000;
//Кол-во команд (должно нацело делить кол-во участков)
const int numOfThreads = 8;

int treasureIndex = 0;

bool islandSectors[numOfSectors];

thread threads[numOfThreads];
int threadOutput[numOfThreads];

void searchForTreasure(int i, void* reslut)
{
	(*(int*)reslut) = -1;
	int numOfSectorsForThread = numOfSectors / numOfThreads;

	for (int j = i * numOfSectorsForThread; j < i * numOfSectorsForThread + numOfSectorsForThread; j++)
	{	
		if (islandSectors[j])
		{			
			(*(int*)reslut) = j;
		}
	}
}

int main()
{
	//Создание рандомайзера.
	std::random_device rd;     
	std::mt19937 rng(rd());    
	std::uniform_int_distribution<int> uni(0, numOfSectors); 

	auto random_integer = uni(rng);

	// Рандомно задать сектор с сокровищем.
	treasureIndex = random_integer;
	islandSectors[treasureIndex] = true;
	cout << "number of sectors = " << numOfSectors << endl;
	cout << "number of teams = " << numOfThreads << endl;
	cout << "treasure index = " << treasureIndex << endl;

	//Создание потоков
	for (int i = 0; i < numOfThreads; i++)
	{
		threads[i] = thread(searchForTreasure, i, (void*)(threadOutput + i));
	}

	//Ожидания окончания выполнения всех потоков.
	for (int i = 0; i < numOfThreads; i++)
	{
		threads[i].join();
	}

	//Вывод информации.-
	for (int i = 0; i < numOfThreads; i++)
	{
		if (threadOutput[i] == -1)
			cout << "Team "<< i<< " havent found anything"<< endl;
		else
			cout << "Team " << i<< " have found treasure in sector " << threadOutput[i]<< endl;
	}
}
