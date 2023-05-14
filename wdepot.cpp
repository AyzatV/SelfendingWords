#include "wdepot.h"

namespace avhw14
{
	WordDepot::WordDepot()
	{
		wroot = newNode();
		loadWords();
		showInfo();
	}

	WordDepot::~WordDepot() {}
	
	TrieNode* WordDepot::newNode(void)
	{
		TrieNode* node = new TrieNode;
		for (int16_t i = 0; i < RUS_ALPHABET_SIZE; i++) node->children[i] = nullptr;
		node->isEndOfWord = false;
		return node;
	}

	TrieNode* WordDepot::keyLastNode(const std::string& key)
	{
		TrieNode* node = wroot;
		if (node!=nullptr)
		{
			for (int16_t i = 0; i < (int16_t)key.size(); i++)
			{
				int16_t k = (unsigned char)key[i] - a_RUS_LETTER;
				node = node->children[k];
				// ���������� ������, ���� ������ ���� �� ������
				if (node == nullptr) break;
			}
		}
		return node;
	}
	
	bool WordDepot::wordsLoaded(const std::string& file)
	{
		bool result = false;
		std::ifstream ifs(file);
		if (ifs.is_open())
		{
			while (ifs)
			{
				std::string word;
				std::getline(ifs, word);
				word = Norm(word);
				if (word.size() > 3) insert(word);
			}
			ifs.close();
			result = true;
		}
		return result;
	}

	bool WordDepot::isNew(const std::string& fword, std::vector<std::string>& words)
	{
		bool result = true;
		// �������� ������� ���������� ����� ���� �� ��������� ������
		for (const auto& word : words)
		{
			// ����� �������
			if (word == fword)
			{
				result = false;
				break;
			}
		}
		if (result)
		{
			std::vector<std::string> files{ RUS_WORDS_FROM_USER, RUS_WORDS_MAIN };
			// �������� ������� �������� ����� � ����� ��������
			for (const auto& file : files)
			{
				std::ifstream ifs(file);
				if (ifs.is_open())
				{
					while (ifs)
					{
						std::string word;
						std::getline(ifs, word);
						word = Norm(word);
						// ����� �������
						if (word == fword)
						{
							result = false;
							break;
						}
					}
					ifs.close();
					if (!result) break;
				}
			}
		}
		return result;
	}

	std::string WordDepot::newWordsSave(const std::string& text)
	{
		std::string info;
		std::vector<std::string> words;
		std::string word;
		bool skip = false;		// ���� true, �� ������� �������� �� ���������� ������� - ������������ ��� ����������
								// ���������� ����, � ������� ���������� �� ������ ����� �������������� ��������
		// ������������ ������� ���� �� ������
		for (const unsigned char& c : text)
		{
			// ���� ������, �� ������ ����� � ������ ��� ����� �� ������ ��������
			if (c == ' ')
			{
				if (skip) skip = false;
				else
				{
					word = Norm(word);
					// ���� � ����� ������ 3 ���� � ��� ��� � �������� � ����� ������ ����� ����, �� ������ ��� � ������
					if (word.size() > 3 && isNew(word, words)) words.push_back(word);
				}
				word.clear();
				continue;
			}
			if (skip) continue;
			// ��������, ��� ������ �������� ������ �������������� ��������
			if (191 < c && c < 256) word.append(1, c);
			else skip = true;
		}

		// ���� ������ �� ������, �� �������� ���� � ������ � �� ������ � ���������������� �������
		if (words.size() > 0)
		{
			std::ofstream ofs(RUS_WORDS_FROM_USER, std::ios::app);	// �������� ����� � ������ ���������� �������
			if (ofs.is_open())
			{
				// ������ ���� � ���� � �������� � ������ �������
				for (const auto& word : words)
				{
					ofs << word << std::endl;
					insert(word);
				}
				ofs.close();
				info = "����� ����� �� ������ ������ �������� � ���������������� �������";
			}
			else
			{
				// �������� ���� ������ � ������ �������, ���� ���� ������� ����������
				for (const auto& word : words) { insert(word); }
				info = "����� ����� �� ������ ������ �������� � �������� �������,\n�������� �� � ���� ������� �� �������";
			}
		}
		else info = "����� ����� ��� ������ � ������� � ����� ������ �� �������";
		return info;
	}

	void WordDepot::insert(const std::string& key)
	{
		if (key.size() == 0) return;
		TrieNode* node = wroot;
		for (int16_t i = 0; i < (int16_t)key.size(); i++)
		{
			int16_t k = (unsigned char)key[i] - a_RUS_LETTER;
			if (node->children[k] == nullptr)
				node->children[k] = newNode();
			node = node->children[k];
		}
		node->isEndOfWord = true;
	}

	void WordDepot::findSuffs(TrieNode* node, std::string suf, std::vector<std::string>& suffs)
	{
		if (node != nullptr)
		{
			for (int16_t i = 0; i < RUS_ALPHABET_SIZE; i++)
			{
				if (node->children[i] != nullptr)
				{
					auto suff = suf;
					// ���������� ������� � ����� �����
					suff.push_back((unsigned char)(i + a_RUS_LETTER));
					// ���� ��������� ����� �����, ������ ����� ����� � ������
					if (node->children[i]->isEndOfWord) suffs.push_back(suff);
					findSuffs(node->children[i], suff, suffs);
				}
			}
		}
	}

	void WordDepot::loadWords(void)
	{
		if (wordsLoaded(RUS_WORDS_FROM_USER)) std::cout << "���������������� ������� ��������" << std::endl;
		else std::cout << "���������������� ������� ��������� �� �������" << std::endl;
		if (wordsLoaded(RUS_WORDS_MAIN)) std::cout << "�������� ������� ��������" << std::endl;
		else std::cout << "�������� ������� ��������� �� �������" << std::endl;
	}

	void WordDepot::showInfo(void)
	{
		std::cout << std::endl << "������ ���������� ������������� �������������� ���� ��� ����� ������, �������� ������� �� ��������� ���������";
		std::cout << std::endl << "��������� ��� ����, ������������ � �������. ��� ���� ������� ����������� �������, ������� ������ ������������.";
		std::cout << std::endl << "�������������� ���� ���������� ����� ����� ������ ���� �������� ����� � ������ ��� ����� �� ���������.";
		std::cout << std::endl << "���� � ������������ �������� ��������� ����� �������� ������ ������ ��� �������, �� ����� ��� �������,";
		std::cout << std::endl << "� ����� ��������� ��������� ������� ��������� ������� \"Back\".";
		std::cout << std::endl << "���������� � �������� ���������� ��������� ����� ������ ������ ������." << std::endl;
	}
	
	void WordDepot::textProcessing(void)
	{
		bool goon = true;
		SuffSet sfs;
		std::stringstream ss;
		std::string text;
		NewWord nword;			// ����� (�������) ����� � ������, � �������� ����������� �������� ���������
		int16_t k;				// ��� ���������� �������
		while (goon)
		{
			std::cout << std::endl << "��� ����� ������ ����������� ������� ����������:";
			std::cout << std::endl << "\"F1\" - �������, \"Back\" - ����� ���������� �������, \"Tab\" - �������� ������������ ��������� ��������� �����,";
			std::cout << std::endl << "\"������\" - ����� �������� ��������, \"Esc\" - ����� �� ������������ ��������� ��������� ��� �������� �����,";
			std::cout << std::endl << "\"Enter\" - ���������� ����� ������, \"F10\" - ����� �� ����������" << std::endl;
			std::cout << std::endl << "������� ��� ����� ��������� ������� �������:" << std::endl;
			int16_t count = 0;
			nword.ready();
			sfs.reset();
			// ������� ������ �����
			ss.str(std::string{});
			ss.clear();

			// ���� ����� ������
			while (true)
			{
				if (_kbhit())
				{
					k = _getch();
					Sleep(10);
					if (k == 0) continue;

					// ������ F10 - �����
					if (k == 68)
					{
						goon = false;
						break;
					}
					// ������ F1 - ����� �������
					if (k == 59) break;
					// ����� Enter - ���� ������ ��������
					if (k == 13) break;
					if (sfs.active)
					// ����� ������ ��������� �����, ���� ������ �����������
					{
						switch (k)
						{
						case 9:
						// ������ Tab - ������������ �� ������ ������������
							sfs.showNext();
							break;
						case 27:
						// ����� Esc - ����� �� ���������� �����
							sfs.erase();
							nword.reset();
							break;
						case 32:
						// ����� ������ - �������� ���������� ��������� �����
							sfs.fix(ss);
							nword.reset();
							break;
						default:
							break;
						}
					}
					else
					// ���� � �������� �������� ������
					{
						if (k == 8)
						// �������� ���������� ������� �������� Back
						{
							eraseLastSymbol(ss);
							// ����������� ������������ ������ �����
							nword.reset();
						}
						else if (k > 31)
						// ���� ���������� ������� ������, ���� ��� �� ������� ��������� �������
						{
							auto c = (unsigned char)k;
							// ��������, ��� ������ �������� ������ �������������� ��������
							if (191 < k && k < 256)
							{
								// ���������� ������� � ������ �����, ���� ���������� ��� ������������
								if (nword.input) nword.text.append(1, c);
							}
							else
							{
								// ���������� � ������������ ������ �����, ���� ������ ������
								if (c == ' ') nword.ready();
								// ����������� ������������ ������ �����, ���� ������ �� ������ � �� ����� ��������
								else nword.reset();
							}
							// ���������� ���������� ������� � ����� � �� �������
							ss << c;
							std::cout << c;
							// ���� � ����� ����� ������� ����� 2 ��������,
							// �������� ������� ������ � ������� � ������������ ������ ��������� �����
							if (nword.text.size() > 2)
							{
								sfs.suffs = getSuffsForKey(Norm(nword.text));
								// ���� ������ �� ������, ������� � ����� ������ ��������� �����
								if (sfs.suffs.size() > 0)
								{
									sfs.showNext();
								}
							}
						}
					}
				}
			}
			text = ss.str();
			// ���� ����� ��� ������, ��� ����� ��� ��������, ����� � ���������� ����� ���� �� ������
			if (text.length() > 0)
			{
				std::cout << std::endl << std::endl << "�� ����� �����:" << std::endl << text << std::endl;
				if (text.length() > 2) std::cout << newWordsSave(text) << std::endl;
			}
			// ����� �������
			if (k == 59) showInfo();
		}
		std::cout << std::endl << "������ ���������� �� ������� ������������" << std::endl;
	}

	void WordDepot::eraseLastSymbol(std::stringstream& ss)
	{
		auto s = ss.str();
		// �������� ������� �� ������ � ������
		s.resize(s.size() - 1);
		// ������� � ������������ ������ ���������� �������
		ss.str(std::string{});
		ss.clear();
		ss << s;
		
		// �������� ������� �� ������ � ������� (����� �������� � ������� �������)
		std::cout << "\b";
		std::cout << " ";
		std::cout << "\b";
	}

	std::vector<std::string> WordDepot::getSuffsForKey(const std::string& key)
	{
		std::vector<std::string> suffs;
		TrieNode* node = keyLastNode(key);
		std::string suff;
		if (node != nullptr) findSuffs(node, suff, suffs);
		return suffs;
	}
	
	std::string WordDepot::Trim(const std::string& str)
	{
		std::string s = str;
		int last = s.length() - 1;
		if (last >= 0)
		{
			bool is_empty = true;
			for (; last >= 0; last--)
			{
				if (s[last] != ' ' && s[last] != '\b')
				{
					is_empty = false;
					break;
				}
			}
			if (is_empty) s.clear();
			else
			{
				int first = 0;
				for (; first < last; first++)
				{
					if (s[first] != ' ' && s[last] != '\b') break;
				}
				s = s.substr(first, last - first + 1);
			}
		}
		return s;
	}

	std::string WordDepot::Norm(const std::string& str)
	{
		std::stringstream ss;
		for (const unsigned char& c : Trim(str)) { ss << (unsigned char)((c > 223 || c == 32) ? c : c + 32); }
		return ss.str();
	}
	
	void SuffSet::reset(void)
	{
		active = false;
		suffs.clear();
		current = 0;
		next = 0;
		lastsize = 0;
	}
	
	void SuffSet::showNext(void)
	{
		if (next < (int16_t)suffs.size())
		{
			auto suff = suffs.at(next);
			int16_t size = (int16_t)suff.size();
			
			// �������� ���������� �������
			CONSOLE_SCREEN_BUFFER_INFO csi;
			auto hw = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(hw, &csi);

			// ���� ����� ������, �� ��������� �������� ������� ����� ������
			if (active) SetConsoleCursorPosition(hw, pos);
			else
			// ������� � ����� ������, �������� �������� ������� ����� ������,
			// �������� ��������� ����� ������ � ������� ������ ����� ������
			{
				active = true;
				pos = csi.dwCursorPosition;
				defcol = csi.wAttributes;
				SetConsoleTextAttribute(hw, FOREGROUND_RED | FOREGROUND_INTENSITY);
			}
			
			// ����������� ��������������� ���������� ��������
			// ��� ����� �������� ����������� �����
			int16_t add = lastsize > size ? lastsize - size : 0;
			// ������������ ������ ��� ������ � ������ ����������� ��������
			auto str = add > 0 ? suff.append(add, ' ').c_str() : suff.c_str();
			auto slen = (DWORD)strlen(str);
			DWORD len;
			// ����� ������ � ��������������� ���������
			WriteConsoleA(hw, str, slen, &len, NULL);
			// ��� ���������� ��������� ������� ������� �� ������� ����� ������
			if (add > 0)
			{
				auto cpos = csi.dwCursorPosition;
				cpos.X -= add;
				SetConsoleCursorPosition(hw, cpos);
			}
			
			// ���������� ������� �������� ����������
			lastsize = size;
			current = next;
			// ���� ������� ����� ����, ��������� ����� ����������� ������������ ��������� next
			if ((suffs.size() - next) > 1 || suffs.size() == 1) next++;
			else next = 0;
		}
	}
	
	void SuffSet::erase(void)
	{
		if (active)
		{
			// �������� ���������� �������
			CONSOLE_SCREEN_BUFFER_INFO csi;
			auto hw = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(hw, &csi);

			// ������� � �������� �������, ����� ��������� ������ ���������
			// � ����� ������� � �������� ������� � ������� ��������� ����� ������
			SetConsoleCursorPosition(hw, pos);
			std::string s;
			auto str = s.append(lastsize, ' ').c_str();
			auto slen = (DWORD)strlen(str);
			DWORD len;
			WriteConsoleA(hw, str, slen, &len, NULL);
			SetConsoleCursorPosition(hw, pos);
			SetConsoleTextAttribute(hw, defcol);

			// ��������� ��������� ���������
			reset();
		}
	}
	
	void SuffSet::fix(std::stringstream& ss)
	{
		if (active)
		{
			// �������� ���������� �������
			CONSOLE_SCREEN_BUFFER_INFO csi;
			auto hw = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(hw, &csi);

			// ���������� � ������ ���������� ��������
			auto s = suffs.at(current);
			ss << s;

			// ����� ���������� �������� � ������ �����, ����� � �������� �����
			// � ������� � �������� �������
			SetConsoleCursorPosition(hw, pos);
			SetConsoleTextAttribute(hw, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			auto str = s.c_str();
			auto slen = (DWORD)strlen(str);
			DWORD len;
			WriteConsoleA(hw, str, slen, &len, NULL);
			Sleep(1000);	// ����� 1 �
			SetConsoleCursorPosition(hw, pos);
			SetConsoleTextAttribute(hw, defcol);
			WriteConsoleA(hw, str, slen, &len, NULL);

			// ��������� ��������� ���������
			reset();
		}
	}
	
	void NewWord::reset(void)
	{
		text.clear();
		input = false;
	}
	
	void NewWord::ready(void)
	{
		text.clear();
		input = true;
	}
}