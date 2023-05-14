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
				// прерывание поиска, если данный ключ не найден
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
		// проверка наличия дубликатов новых слов во введенном тексте
		for (const auto& word : words)
		{
			// слово найдено
			if (word == fword)
			{
				result = false;
				break;
			}
		}
		if (result)
		{
			std::vector<std::string> files{ RUS_WORDS_FROM_USER, RUS_WORDS_MAIN };
			// проверка наличия искомого слова в обоих словарях
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
						// слово найдено
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
		bool skip = false;		// если true, то пропуск символов до следующего пробела - используется для исключения
								// сохранения слов, в которых содержатся не только буквы кириллического алфавита
		// формирование массива слов из текста
		for (const unsigned char& c : text)
		{
			// если пробел, то запись слова в массив или выход из режима пропуска
			if (c == ' ')
			{
				if (skip) skip = false;
				else
				{
					word = Norm(word);
					// если в слове больше 3 букв и его нет в словарях и среди других новых слов, то запись его в массив
					if (word.size() > 3 && isNew(word, words)) words.push_back(word);
				}
				word.clear();
				continue;
			}
			if (skip) continue;
			// проверка, что символ является буквой кириллического алфавита
			if (191 < c && c < 256) word.append(1, c);
			else skip = true;
		}

		// если массив не пустой, то внесение слов в дерево и их запись в пользовательский словарь
		if (words.size() > 0)
		{
			std::ofstream ofs(RUS_WORDS_FROM_USER, std::ios::app);	// открытие файла в режиме добавления записей
			if (ofs.is_open())
			{
				// запись слов в файл и внесение в дерево словаря
				for (const auto& word : words)
				{
					ofs << word << std::endl;
					insert(word);
				}
				ofs.close();
				info = "Новые слова из Вашего текста записаны в пользовательский словарь";
			}
			else
			{
				// внесение слов только в дерево словаря, если файл словаря недоступен
				for (const auto& word : words) { insert(word); }
				info = "Новые слова из Вашего текста записаны в активный словарь,\nзаписать их в файл словаря не удалось";
			}
		}
		else info = "Новые слова для записи в словарь в Вашем тексте не найдены";
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
					// добавление символа в конец слова
					suff.push_back((unsigned char)(i + a_RUS_LETTER));
					// если обнаружен конец слова, запись этого слова в массив
					if (node->children[i]->isEndOfWord) suffs.push_back(suff);
					findSuffs(node->children[i], suff, suffs);
				}
			}
		}
	}

	void WordDepot::loadWords(void)
	{
		if (wordsLoaded(RUS_WORDS_FROM_USER)) std::cout << "Пользовательский словарь загружен" << std::endl;
		else std::cout << "Пользовательский словарь загрузить не удалось" << std::endl;
		if (wordsLoaded(RUS_WORDS_MAIN)) std::cout << "Основной словарь загружен" << std::endl;
		else std::cout << "Основной словарь загрузить не удалось" << std::endl;
	}

	void WordDepot::showInfo(void)
	{
		std::cout << std::endl << "Данное приложение демонстрирует автодополнение слов при вводе текста, позволяя выбрать из имеющихся вариантов";
		std::cout << std::endl << "окончания для слов, содержащихся в словаре. При этом словарь пополняется словами, которые вводит пользователь.";
		std::cout << std::endl << "Автодополнение слов происходит после ввода первых трех символов слова и только при вводе на кириллице.";
		std::cout << std::endl << "Если в предложенном варианте окончания слова подходят только первые его символы, то можно его принять,";
		std::cout << std::endl << "а затем исправить последние символы используя клавишу \"Back\".";
		std::cout << std::endl << "Информация о клавишах управления выводится перед каждым вводом текста." << std::endl;
	}
	
	void WordDepot::textProcessing(void)
	{
		bool goon = true;
		SuffSet sfs;
		std::stringstream ss;
		std::string text;
		NewWord nword;			// новое (текущее) слово в тексте, к которому подбираются варианты окончаний
		int16_t k;				// код введенного символа
		while (goon)
		{
			std::cout << std::endl << "При вводе текста используйте клавиши управления:";
			std::cout << std::endl << "\"F1\" - справка, \"Back\" - забой последнего символа, \"Tab\" - просмотр предлагаемых вариантов окончания слова,";
			std::cout << std::endl << "\"Пробел\" - выбор текущего варианта, \"Esc\" - отказ от предлагаемых вариантов окончания для текущего слова,";
			std::cout << std::endl << "\"Enter\" - завершение ввода текста, \"F10\" - выход из приложения" << std::endl;
			std::cout << std::endl << "Введите Ваш текст используя русский регистр:" << std::endl;
			int16_t count = 0;
			nword.ready();
			sfs.reset();
			// очистка потока ввода
			ss.str(std::string{});
			ss.clear();

			// цикл ввода текста
			while (true)
			{
				if (_kbhit())
				{
					k = _getch();
					Sleep(10);
					if (k == 0) continue;

					// нажата F10 - выход
					if (k == 68)
					{
						goon = false;
						break;
					}
					// нажата F1 - вывод справки
					if (k == 59) break;
					// нажат Enter - ввод текста завершен
					if (k == 13) break;
					if (sfs.active)
					// режим выбора окончания слова, ввод текста блокируется
					{
						switch (k)
						{
						case 9:
						// нажата Tab - переключение на другую альтернативу
							sfs.showNext();
							break;
						case 27:
						// нажат Esc - отказ от дополнения слова
							sfs.erase();
							nword.reset();
							break;
						case 32:
						// нажат пробел - фиксация выбранного окончания слова
							sfs.fix(ss);
							nword.reset();
							break;
						default:
							break;
						}
					}
					else
					// ввод и удаление символов текста
					{
						if (k == 8)
						// удаление последнего символа клавишей Back
						{
							eraseLastSymbol(ss);
							// прекращение формирования нового слова
							nword.reset();
						}
						else if (k > 31)
						// ввод очередного символа текста, если это не нажатие служебной клавиши
						{
							auto c = (unsigned char)k;
							// проверка, что символ является буквой кириллического алфавита
							if (191 < k && k < 256)
							{
								// добавление символа к новому слову, если происходит его формирование
								if (nword.input) nword.text.append(1, c);
							}
							else
							{
								// подготовка к формированию нового слова, если введен пробел
								if (c == ' ') nword.ready();
								// прекращение формирования нового слова, если введен не пробел и не буква алфавита
								else nword.reset();
							}
							// добавление введенного символа в поток и на консоль
							ss << c;
							std::cout << c;
							// если в новом слове введено более 2 символов,
							// проверка наличия ключей в словаре и формирование списка окончаний слова
							if (nword.text.size() > 2)
							{
								sfs.suffs = getSuffsForKey(Norm(nword.text));
								// если список не пустой, переход в режим выбора окончания слова
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
			// если текст был введен, его вывод для контроля, поиск и сохранение новых слов из текста
			if (text.length() > 0)
			{
				std::cout << std::endl << std::endl << "Вы ввели текст:" << std::endl << text << std::endl;
				if (text.length() > 2) std::cout << newWordsSave(text) << std::endl;
			}
			// вывод справки
			if (k == 59) showInfo();
		}
		std::cout << std::endl << "Работа прекращена по запросу пользователя" << std::endl;
	}

	void WordDepot::eraseLastSymbol(std::stringstream& ss)
	{
		auto s = ss.str();
		// удаление символа из строки в потоке
		s.resize(s.size() - 1);
		// очистка и перезагрузка потока измененной строкой
		ss.str(std::string{});
		ss.clear();
		ss << s;
		
		// удаление символа из текста в консоли (забой пробелом и возврат курсора)
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
			
			// фиксация параметров консоли
			CONSOLE_SCREEN_BUFFER_INFO csi;
			auto hw = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(hw, &csi);

			// если режим выбора, то установка исходной позиции ввода текста
			if (active) SetConsoleCursorPosition(hw, pos);
			else
			// переход в режим выбора, фиксация исходной позиции ввода текста,
			// фиксация исходного цвета текста и задание нового цвета текста
			{
				active = true;
				pos = csi.dwCursorPosition;
				defcol = csi.wAttributes;
				SetConsoleTextAttribute(hw, FOREGROUND_RED | FOREGROUND_INTENSITY);
			}
			
			// определение дополнительного количества пробелов
			// для забоя символов предыдущего слова
			int16_t add = lastsize > size ? lastsize - size : 0;
			// формирование строки для вывода с учетом необходимых пробелов
			auto str = add > 0 ? suff.append(add, ' ').c_str() : suff.c_str();
			auto slen = (DWORD)strlen(str);
			DWORD len;
			// вывод строки с дополнительными пробелами
			WriteConsoleA(hw, str, slen, &len, NULL);
			// при дополнении пробелами возврат курсора на позицию после текста
			if (add > 0)
			{
				auto cpos = csi.dwCursorPosition;
				cpos.X -= add;
				SetConsoleCursorPosition(hw, cpos);
			}
			
			// обновление текущих значений параметров
			lastsize = size;
			current = next;
			// если вариант всего один, следующий вывод блокируется недопустимым значением next
			if ((suffs.size() - next) > 1 || suffs.size() == 1) next++;
			else next = 0;
		}
	}
	
	void SuffSet::erase(void)
	{
		if (active)
		{
			// фиксация параметров консоли
			CONSOLE_SCREEN_BUFFER_INFO csi;
			auto hw = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(hw, &csi);

			// возврат к исходной позиции, забой последней строки пробелами
			// и снова возврат к исходной позиции и возврат исходного цвета текста
			SetConsoleCursorPosition(hw, pos);
			std::string s;
			auto str = s.append(lastsize, ' ').c_str();
			auto slen = (DWORD)strlen(str);
			DWORD len;
			WriteConsoleA(hw, str, slen, &len, NULL);
			SetConsoleCursorPosition(hw, pos);
			SetConsoleTextAttribute(hw, defcol);

			// установка исходного состояния
			reset();
		}
	}
	
	void SuffSet::fix(std::stringstream& ss)
	{
		if (active)
		{
			// фиксация параметров консоли
			CONSOLE_SCREEN_BUFFER_INFO csi;
			auto hw = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(hw, &csi);

			// сохранение в потоке выбранного варианта
			auto s = suffs.at(current);
			ss << s;

			// вывод выбранного варианта в другом цвете, затем в исходном цвете
			// и возврат к исходной позиции
			SetConsoleCursorPosition(hw, pos);
			SetConsoleTextAttribute(hw, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			auto str = s.c_str();
			auto slen = (DWORD)strlen(str);
			DWORD len;
			WriteConsoleA(hw, str, slen, &len, NULL);
			Sleep(1000);	// пауза 1 с
			SetConsoleCursorPosition(hw, pos);
			SetConsoleTextAttribute(hw, defcol);
			WriteConsoleA(hw, str, slen, &len, NULL);

			// установка исходного состояния
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