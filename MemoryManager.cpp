#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <list>

using std::vector;
using std::list;

// отказ на выполнение запроса
const int REJECT = -1;


// структура @MemoryBlock, описывающая блок памяти
// @realIndex - порядковый индекс блока, как он хранится в памяти
// @length - длина блока
// @isFree - индикатор занятости блока памяти
// @indexInHeap - индекс в блока в куче
// @start - начало блока в памяти
struct MemoryBlock
{
    int realIndex;
    int length;
    bool isFree;
    int indexInHeap;
    int start;

    explicit MemoryBlock (int startInMemory = 0, int length = 0, bool is_free = false,
            int indInHeap = 0, int realIndexInMemory = 0) :
        start(startInMemory),
        length(length),
        isFree(is_free),
        indexInHeap(indInHeap),
        realIndex(realIndexInMemory)
    {
    }

    MemoryBlock& operator = (const MemoryBlock& right);

    bool operator < (const MemoryBlock &right) const;

    bool operator == (const MemoryBlock &right) const;
    
    bool operator != (const MemoryBlock &right) const;
};


// класс @MaxHeap, описывающий структуру MaxHeap и её методы
// @blocks - элементы кучи
template <class T>
class MaxHeap
{
    vector<T> blocks;

    // метод @Heapify восстанавливает свойство кучи
    // index - индекс элемента, который нужно упорядочить для
    // восстановления свойств кучи
    void Heapify (int index);

public:
    // метод @isEmpty возвращает true, если в куче нет элементов
    bool isEmpty() const;

    // метод @getBlocksSize возвращает количество элементов в куче
    int getBlocksSize() const;
	
    // метод @insertNewBlock вставляет новый элемент в кучу
    // newBlock - указатель на блок в списке блоков памяти 
    void insertNewBlock(T newBlock);


    // метод @removeBlock удаляет элемент из кучи, сохраняя свойства кучи
    // element - указатель на удаляемый элемент
    void removeBlock(T &element);

    // метод @extractMin удаляет максимальный (верхний) элемент из кучи, сохраняя свойства кучи
    T extractMin();

    // метод @getMaxElement возвращает указатель на
    // максимальный (верхний) элемент из кучи
    T getMaxElement() const;
};


// структура @Query, описывающая запрос на выделение/освобождение памяти
// @value - значение запроса
// @response - ответ на запрос (либо -1, либо положительное целое число)
// @pointerToBlock - указатель на блок в памяти, если по этому запросу,
// была выделена память, иначе NULL
struct Query
{
    int value;
    int response;
    list<MemoryBlock>::iterator pointerToBlock;
    explicit Query(int val) : value(val), response(0);
};

// класс @MemoryManager, выполняющий работу по освобождению и выделению памяти
// @memoryHeap - куча, элементами которой являются указатели на блоки памяти в списке @blocksInList
// @blocksInList - двусвязный список с блоками памяти, расположенными в том порядке,
// как они располагаются в памяти

class MemoryManager
{
    MaxHeap<list<MemoryBlock>::iterator> memoryHeap;
    list<MemoryBlock> blocksInList;

public:
    // констурктор @MemoryManager создаёт блок памяти, размером @sizeOfMemory
    explicit MemoryManager(int sizeOfMemory);

    // метод @findAndAllocateMemory выполняет запрос @query на выделение 
    // памяти 
    // @query - запрос, в который записывается результат выполнения
    // процедуры веделения памяти
    void findAndAllocateMemory(Query& query);

    // метод @deallocateMemory выполняет освобождение памяти
    // по запросу @query
    // @query - запрос на освобождение памяти
    void deallocateMemory(Query& query);
    
};

// метод @fillQueryVector заполняет вектор запросов из входного потока
void fillQueryVector(vector<Query>& queries);

// метод @performQuery выполняет все запросы из @queries
void performQuery(MemoryManager& memoryManager, vector<Query>& queries);

// метод @printReport выводит результат выполнения запросов
void printReport(vector<Query>& queries);

int main()
{
    int memorySize;
    std::cin >> memorySize;

    MemoryManager memoryManager(memorySize);
    vector<Query> queries;
    // заполнение списка запросов из входного потока
    fillQueryVector(queries);
    // выполнение списка запросов в порядке поступления из входного потока
    performQuery(memoryManager, queries);
    // вывод результатов
    printReport(queries);

    return 0;
}
