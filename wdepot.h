#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <conio.h>
#include <iostream>
#include <Windows.h>

namespace avhw14
{
	const int16_t RUS_ALPHABET_SIZE = 32;    // ������ �������� ��������
    const int16_t a_RUS_LETTER = 224;        // ��� ������� '�' � ������� ��������

    // ��������� ��� �������� ����� ������-�������
    struct TrieNode
    {
        TrieNode* children[RUS_ALPHABET_SIZE];
        // true, ���� ���� �������� ������ �����
        bool isEndOfWord;
    };

    // ��������� ��� ���������� ������������� ������ �����, ��� �������� ����������� ���������
    struct NewWord
    {
        std::string text;
        // true - ���� ������������ ����
        bool input = false;
        // ����� � �������� ���������
        void reset(void);
        // ���������� � ����� ������ �����
        void ready(void);
    };

    // ��������� ��� ������������ ����������� ��������� ��������� ����
    struct SuffSet
    {
        std::vector<std::string> suffs;
        bool active = false;
        // ������ �������� ��������
        int16_t current = 0;
        // ������ ���������� ��������
        int16_t next = 0;
        // ������ ����������� ��������
        int16_t lastsize = 0;
        // �������� ��� �������� ������� ������� �����
        COORD pos{};
        // �������� ��� �������� ��������� ����� ������
        WORD defcol{};
        // ����� � �������� ���������
        void reset(void);
        // ������������ ���������� ��������� �����
        void showNext(void);
        // ������ ������ ��������� �����
        void erase(void);
        // �������� ���������� ��������� �����
        void fix(std::stringstream&);
    };

    // ����� ��� ����� ������ � �������������� ��������
    class WordDepot
    {
    private:
        // ��� ����� ��������� �������
        const std::string RUS_WORDS_MAIN{ "rus_main.txt" };
        // ��� ����� ����������������� �������
        const std::string RUS_WORDS_FROM_USER{ "rus_from_user.txt" };
        // �������� ���� ������
        TrieNode* wroot;
        // �������� ������ ����
        TrieNode* newNode(void);
        // ����� ����, ���������������� ���������� ������� �����,
        // ���� ���������� nullptr - ���� �����������
        TrieNode* keyLastNode(const std::string&);
        // true, ���� ������� ����������
        bool wordsLoaded(const std::string&);
        // true, ���� ����� ��� � �������� � ����� ��������� ����
        bool isNew(const std::string&, std::vector<std::string>&);
        // ����� ����� ���� �� ��������� ������ � ������ ���� ����
        // � ���� ����������������� ������� � ��������� ������ � �����������
        std::string newWordsSave(const std::string&);
        // ������� ������ ����� � ������
        void insert(const std::string&);
        // ����������� ������� ��� ������������ ������� ���� ��������� ���� ��� ��������
        void findSuffs(TrieNode*, std::string, std::vector<std::string>&);
        // �������� �������� � ������
        void loadWords(void);
        // ����� ������� �� ������������� ����������
        void showInfo(void);
        // �������� ���������� ���������� �������
        void eraseLastSymbol(std::stringstream&);
        // ������������ ������� ��������� ����, ���������� ������������ �����
        std::vector<std::string> getSuffsForKey(const std::string&);
        // ������� ����� �� �������� � �������� ����� �� �����
        std::string Trim(const std::string&);
        // ������������ ���� - Trim � ������ ��������� ���� �� ��������
        std::string Norm(const std::string&);
    public:
        WordDepot();
        ~WordDepot();
        // �������� ������� ������ ��� ����� ������ � �������������� ���� �� ������-�������
        void textProcessing(void);
    };
}