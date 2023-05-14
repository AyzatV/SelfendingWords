#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <conio.h>
#include <iostream>
#include <Windows.h>

namespace avhw14
{
	const int16_t RUS_ALPHABET_SIZE = 32;    // размер русского алфавита
    const int16_t a_RUS_LETTER = 224;        // код символа 'а' в русском регистре

    // структура для создания узлов дерева-словаря
    struct TrieNode
    {
        TrieNode* children[RUS_ALPHABET_SIZE];
        // true, если ключ является концом слова
        bool isEndOfWord;
    };

    // структура для управления формированием нового слова, для которого подбираются окончания
    struct NewWord
    {
        std::string text;
        // true - если производится ввод
        bool input = false;
        // сброс в исходное состояние
        void reset(void);
        // подготовка к вводу нового слова
        void ready(void);
    };

    // структура для демонстрации подобранных вариантов окончаний слов
    struct SuffSet
    {
        std::vector<std::string> suffs;
        bool active = false;
        // индекс текущего варианта
        int16_t current = 0;
        // индекс следующего варианта
        int16_t next = 0;
        // размер предыдущего варианта
        int16_t lastsize = 0;
        // параметр для фиксации текущей позиции ввода
        COORD pos{};
        // параметр для фиксации исходного цвета текста
        WORD defcol{};
        // сброс в исходное состояние
        void reset(void);
        // демонстрация очередного окончания слова
        void showNext(void);
        // отмена выбора окончания слова
        void erase(void);
        // фиксация выбранного окончания слова
        void fix(std::stringstream&);
    };

    // класс для ввода текста с использованием словарей
    class WordDepot
    {
    private:
        // имя файла основного словаря
        const std::string RUS_WORDS_MAIN{ "rus_main.txt" };
        // имя файла пользовательского словаря
        const std::string RUS_WORDS_FROM_USER{ "rus_from_user.txt" };
        // корневой узел дерева
        TrieNode* wroot;
        // создание нового узла
        TrieNode* newNode(void);
        // поиск узла, соответствующего последнему символу ключа,
        // если возвращает nullptr - ключ отсутствует
        TrieNode* keyLastNode(const std::string&);
        // true, если словарь загрузился
        bool wordsLoaded(const std::string&);
        // true, если слова нет в словарях и среди введенных слов
        bool isNew(const std::string&, std::vector<std::string>&);
        // поиск новых слов во введенном тексте и запись этих слов
        // в файл пользовательского словаря с возвратом строки с результатом
        std::string newWordsSave(const std::string&);
        // вставка нового ключа в дерево
        void insert(const std::string&);
        // рекурсивная функция для формирования массива всех окончаний слов для префикса
        void findSuffs(TrieNode*, std::string, std::vector<std::string>&);
        // загрузка словарей в дерево
        void loadWords(void);
        // вывод справки по использованию приложения
        void showInfo(void);
        // удаление последнего введенного символа
        void eraseLastSymbol(std::stringstream&);
        // формирование массива окончаний слов, являющихся продолжением ключа
        std::vector<std::string> getSuffsForKey(const std::string&);
        // очистка строк от пробелов и символов забоя по краям
        std::string Trim(const std::string&);
        // нормализация слов - Trim и замена заглавных букв на строчные
        std::string Norm(const std::string&);
    public:
        WordDepot();
        ~WordDepot();
        // основная функция класса для ввода текста с использованием слов из дерева-словаря
        void textProcessing(void);
    };
}