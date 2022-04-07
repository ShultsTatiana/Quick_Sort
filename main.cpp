struct Reting {
	string login = "";		//Логин участника
	size_t solvedTasks = 0;	//Кол-во решенных задач
	size_t penalty = 0;		//Кол-во штрафных балов
};

bool operator<(const Reting& left, const Reting& right) {
	return
		tie(left.solvedTasks, right.penalty, right.login) <
		tie(right.solvedTasks, left.penalty, left.login);
}

istream& operator>>(istream& input, Reting& participant) {
	input >> participant.login
		>> participant.solvedTasks
		>> participant.penalty;
	return input;
}
ostream& operator<<(ostream& out, const Reting& participant) {
	out << participant.login;
	return out;
}

/*Идея:
функция возвращает итератор на первый элемент не меньший чем опорный
[head->>>  .... <<<-tail]
		++ .... --
	идем с двух сторон диапазона пока его голова и хвост не накрыли друг-друга
		голову проверяем чтоб была строго меньше опорного
			Ok: двигаем итератор головы вперед,
				проверяем не натолкнулась ли голова на хвост
			иначе: указываем, что голова хочет сделать замену
		хвост проверяем чтоб был не меньше опорного (иначе:опорный строго меньше хвоста)
			Ok: двигаем итератор хвоста назад,
				проверяем не натолкнулся ли хвост на голову
			иначе: указываем, что хвост хочет сделать замену
	когда и голова и хвост хотят поменяться местами,
		меняем содержимое местами
		сбрасываем флаги обмена
		двигаем голову и если голова не натолкнулась на хвост
			то двигаем хвост
После сталкновения головы и хвоста может быть два варианта:
	в позиции голова-хвост остался элемент меньший чем опорный
	в позиции голова стоит элемент первый элемент не меньший чем опорный
В перавом случае нужно подвинуть итератор головы
(который мы будем возвращать из функции) на одну позицию к концу диапозона
*/
template<typename Iterator>
Iterator partition(Iterator head, Iterator tail, const Reting& pivot) {
	// эти флаги введены для уменьшения числа сравнений Reting-ов в do
	bool headSwap(0), tailSwap(0);
	do {
		//поскольку голова и хвос двигаются на одном такте цикла do
		//при каждом сдвиге хвоста и головы проверяем не сталкнулись ли они
		if (!headSwap && *head < pivot) {
			//!headSwap - предотвращает сравнение *head < pivot в случаее, если
			// уже было установлено, что требуется перестановка
			//(bool проверить экономнее, чем два size_t и string)
			++head;
			if (head == tail) {
				break;
			}
		}
		else {
			headSwap = 1;
		}
		if (!tailSwap && pivot < *tail) {
			--tail;
			if (head == tail) {
				break;
			}
		}
		else {
			tailSwap = 1;
		}
		if (headSwap && tailSwap) {
			swap(*head, *tail);
			headSwap = tailSwap = 0;
			++head;
			if (head == tail) {
				break;
			}
			else {
				--tail;
			}
		}

	} while (tail != head);
	if (*head < pivot) {
		++head;
	}
	return head;
}

template<typename Iterator>
void quickSortInPlace(Iterator begin, Iterator end) {
	if (end - begin < 2) { //базовый случай: ширина диапазона 1 или 0
		return; //четко обозначаем возвращение на верхний уровень рекурсии
	}
	else {
		//https://en.cppreference.com/w/cpp/numeric/random/rand
		// std::rand() - Псевдо-рандомное целое (от ​0​ до RAND_MAX)
		// pivotIndex - число от 0 до (ширина_диапазона - 1)
		// 
		// было: 
		//size_t pivotIndex = std::rand() /((RAND_MAX + 1u) / (end - begin));
		// рекомендация: умножать быстрее чем делить
		//size_t pivotIndex = (std::rand() / (RAND_MAX + 1u)) * std::distance(begin, end); //посылка 64489871 198ms 77.41Mb
		// ответ: предложенный вариант у меня вызывает выброс исключения Stack overflow (Visual Studio)
		// и ML в тестовой системе Контеста
		// и еще у меня есть смутное подозрение, что вариант из документации чуть надежнее
		//size_t pivotIndex = (std::rand() * (end - begin)) / (RAND_MAX + 1u); //посылка 64489889 351ms 4.98Mb
		size_t pivotIndex = std::rand() / ((RAND_MAX + 1u) / std::distance(begin, end)); //посылка 64489879	355ms 4.98Mb
		
		Reting pivot = *(begin + pivotIndex); //опорный элемент

		//midle - разделитель диапазона (left < pivot and rigtht >=pivot)
		//изначально в quickSortInPlace передан [begin, end) диапазон, 
		// а для partition требуется [,] диапазон
		// передаем в partition диапазон [begin, prev(end)]
		Iterator midle = partition<Iterator>(begin, prev(end), pivot);
		//рекурсия
		quickSortInPlace<Iterator>(begin, midle);
		quickSortInPlace<Iterator>(midle, end);
	}
}

ostream& processingData(istream& input = std::cin, ostream& out = std::cout) {
	size_t participantCount; //число участников
	input >> participantCount;

	vector<Reting> participants(participantCount);
	for (Reting& participant : participants) {
		input >> participant;
	}

	std::srand(std::time(nullptr)); // use current time as seed for random generator
	//quickSortInPlace отсортирует от худшего к лучшему
	quickSortInPlace(participants.begin(), participants.end());

	//выводим участников от лучшего к худшему 
	for (auto participant(participants.rbegin()); //лудший находится в конце
		participant != participants.rend();
		++participant) {	//идем к началу
		out << *participant << endl;
	}

	return out;
}

struct Test {
	string input;
	string expect;
};

void tests() {
	vector<Test> tests{
		{"5\n"
		"alla 4 100\n" "gena 6 1000\n" "gosha 2 90\n" "rita 2 90\n" "timofey 4 80\n",
		"gena\n" "timofey\n" "alla\n" "gosha\n" "rita\n"},			//0
		{"5\n"
		"alla 0 0\n" "gena 0 0\n" "gosha 0 0\n" "rita 0 0\n" "timofey 0 0\n",
		"alla\n" "gena\n" "gosha\n" "rita\n" "timofey\n" },			//1
	};

	size_t failCounter(0), line(0);
	for (Test test : tests) {
		stringstream input(move(test.input));
		stringstream out;

		processingData(input, out);
		string strOutput = out.str();

		if (strOutput != test.expect) {
			++failCounter;
			std::cout << "Test in " << line << "\tis\n" <<
				strOutput << "!=\n" << test.expect << endl;
		}
		++line;
	}
	std::cout << (failCounter == 0 ? "All tests is Ok\n" : "FAIL\n");
}

int main()
{

	//tests();

	processingData();

	return 0;
}
