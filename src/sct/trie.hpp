#ifndef LIS_TRIE_HPP
#define LIS_TRIE_HPP

#include <cstring>
#include <functional>
#include <iterator>





namespace nvx
{





template<typename T, int ALPHABET_SIZE, int OFFSET>
struct _TrieNode
{
	// types
	typedef T value_t;



	// (de)constructor
	_TrieNode(): store(false), value(value_t())
	{
		memset(chs, 0, sizeof(_TrieNode*)*ALPHABET_SIZE);
	}

	~_TrieNode()
	{
		for (auto ch : chs)
		{
			if (ch)
				delete ch;
		}
	}


	// mthods
	value_t *get(char const *key)
	{
		if (*key == '\0')
			return store ? &value : nullptr;

		if (!chs[*key - OFFSET])
			return nullptr;

		return chs[*key - OFFSET]->get(key+1);
	}

	bool insert(char const *key, value_t const &val)
	{
		if (*key == '\0')
		{
			bool res = !store;
			store = true;
			value = val;
			return res;
		}

		if (!chs[*key - OFFSET])
			chs[*key - OFFSET] = new _TrieNode;

		return chs[*key - OFFSET]->insert(key+1, val);
	}

	bool erase(char const *key, bool *eraseme = nullptr)
	{
		if (*key == '\0')
		{
			if (!store)
				return false;
			store = false;

			if (eraseme)
				*eraseme = _check_eraseme();

			return true;
		}

		auto ch = chs[*key - OFFSET];
		if (ch == nullptr)
			return false;


		bool erasech = false;
		if (!ch->erase(key+1, &erasech))
			return false;

		if (erasech)
		{
			delete ch;
			chs[*key - OFFSET] = nullptr;
		}

		if (eraseme)
			*eraseme = _check_eraseme();

		return true;
	}

	template<class Ostream>
	Ostream &print(
		Ostream &os, 
		std::string const &tabme = "",
		std::string const &taboth = "",
		char key = '\0'
	) const
	{
		os << tabme << (key ? key : '@');
		if (store)
			os << " " << value;

		auto last = *(std::find_if(
			std::reverse_iterator(chs+ALPHABET_SIZE),
			std::reverse_iterator(chs),
			[](_TrieNode *t) { return (bool)t; }
		).base()-1);

		for (int i = 0; i < ALPHABET_SIZE; ++i)
		{
			if (!chs[i])
				continue;

			if (chs[i] != last)
			{
				chs[i]->print(
					os << '\n',
					taboth + uprightdown + leftright + " ",
					taboth + updown + "  ",
					i + OFFSET
				);
			}
			else
			{
				chs[i]->print(
					os << '\n',
					taboth + upright + leftright + " ",
					taboth + "   ", i + OFFSET
				);
			}
		}

		return os;
	}


	// members
	bool store;
	value_t value;
	_TrieNode *chs[ALPHABET_SIZE];


	// static
	static constexpr char const *updown      = "│";
	static constexpr char const *leftright   = "─";
	static constexpr char const *upright     = "└";
	static constexpr char const *uprightdown = "├";

private:
	bool _check_eraseme()
	{
		if (store)
			return false;

		for (auto ch : chs)
		{
			if (ch != nullptr)
				return false;
		}

		return true;
	}

};

template<class Ostream, typename T, int N, int M>
inline Ostream &operator<<( Ostream &os, _TrieNode<T, N, M> const &toprint )
{
	toprint.print(os);
	return os;
}





template<typename T, int ALPHABET_SIZE, int OFFSET>
class Trie
{
public:
	typedef T value_t;

	Trie()
	{
		root = new _TrieNode<T, ALPHABET_SIZE, OFFSET>;
	}

	~Trie()
	{
		delete root;
	} 



	/*
	 * Возвращает количество значений
	 */
	int size() const
	{
		return count;
	}



	/*
	 * Получение элемента по ключу; возвращает nullptr,
	 * если значение не найдено
	 */
	value_t *get(char const *key) const
	{
		return root->get(key);
	}

	/*
	 * Возвращает истину, если раньше такого элемента не
	 * было, и ложь, если был (но значение всё равно
	 * перезаписывается)
	 */
	bool insert(char const *key, value_t const &newval)
	{
		if (root->insert(key, newval))
		{
			++count;
			return true;
		}

		return false;
	}

	/*
	 * Удаление значение по ключу. Возвращает истину, если
	 * элементы был удалён, и ложь — если отсутствовал
	 */
	bool erase(char const *key)
	{
		return root->erase(key);
	}

	/*
	 * Вывод дерева в красивом виде на поток
	 */
	template<class Ostream>
	Ostream &print(Ostream &os, std::string const &tab = "") const
	{
		root->print(os, tab);
		return os;
	}



private:
	int count = 0;
	_TrieNode<T, ALPHABET_SIZE, OFFSET> *root;

	friend struct _TriePrivateAccess;

};

template<class Ostream, typename T, int N, int M>
inline Ostream &operator<<( Ostream &os, Trie<T, N, M> const &toprint )
{
	toprint.print(os);
	return os;
}





struct _TriePrivateAccess
{
	template<typename T, int N, int M>
	static int &count(Trie<T, N, M> &t)
	{
		return t.count;
	}

	template<typename T, int N, int M>
	static _TrieNode<T, N, M> *&root(Trie<T, N, M> &t)
	{
		return t.root;
	}
};





}





#endif
