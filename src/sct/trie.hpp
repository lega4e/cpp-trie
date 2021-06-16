#ifndef NVX_TRIE_HPP_74040375
#define NVX_TRIE_HPP_74040375

#include <cstring>
#include <functional>
#include <iterator>





namespace nvx
{





template<typename Key>
struct _TrieNodeTraits
{
	typedef std::string str_t;

	static constexpr char const root         = '@';
	static constexpr char const *space       = " ";
	static constexpr char const *doublespace = "  ";

	static constexpr char const *updown      = "│";
	static constexpr char const *leftright   = "─";
	static constexpr char const *upright     = "└";
	static constexpr char const *uprightdown = "├";
};

template<>
struct _TrieNodeTraits<wchar_t>
{
	typedef std::wstring str_t;

	static constexpr wchar_t const root         = L'@';
	static constexpr wchar_t const *space       = L" ";
	static constexpr wchar_t const *doublespace = L"  ";

	static constexpr wchar_t const *updown      = L"│";
	static constexpr wchar_t const *leftright   = L"─";
	static constexpr wchar_t const *upright     = L"└";
	static constexpr wchar_t const *uprightdown = L"├";
};

template<
	typename T,
	int      ALPHABET_SIZE,
	int      OFFSET,
	typename Key  = char,
	Key      ESYM = '\0'
>
struct _TrieNode
{
	// types
	typedef _TrieNodeTraits<Key>    traits_t;
	typedef T                        value_t;
	typedef Key                      key_t;
	typedef typename traits_t::str_t str_t;



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


	// methods
	value_t *get(key_t const *key)
	{
		if (*key == ESYM)
			return store ? &value : nullptr;

		if (!chs[*key - OFFSET])
			return nullptr;

		return chs[*key - OFFSET]->get(key+1);
	}

	bool insert(key_t const *key, value_t const &val)
	{
		if (*key == ESYM)
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

	bool erase(key_t const *key, bool *eraseme = nullptr)
	{
		if (*key == ESYM)
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
		str_t const &tabme = str_t(),
		str_t const &taboth = str_t(),
		key_t key = ESYM
	) const
	{
		if (key)
			os << tabme << key;
		else
			os << tabme << traits_t::root;

		if (store)
			os << traits_t::space << value;

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
					taboth + traits_t::uprightdown + traits_t::leftright + traits_t::space,
					taboth + traits_t::updown + traits_t::doublespace,
					i + OFFSET
				);
			}
			else
			{
				chs[i]->print(
					os << '\n',
					taboth + traits_t::upright + traits_t::leftright + traits_t::space,
					taboth + traits_t::doublespace,
					i + OFFSET
				);
			}
		}

		return os;
	}


	// members
	bool store;
	value_t value;
	_TrieNode *chs[ALPHABET_SIZE];


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

template<class Ostream, typename T, int N, int M, typename K, K E>
inline Ostream &operator<<( Ostream &os, _TrieNode<T, N, M, K, E> const &toprint )
{
	toprint.print(os);
	return os;
}





template<
	typename T,
	int ALPHABET_SIZE,
	int OFFSET,
	typename Key = char,
	Key ESYM = '\0'
>
class Trie
{
public:
	typedef _TrieNodeTraits<Key>     traits_t;
	typedef T                        value_t;
	typedef Key                      key_t;
	typedef typename traits_t::str_t str_t;

	Trie()
	{
		root = new _TrieNode<T, ALPHABET_SIZE, OFFSET, key_t, ESYM>;
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
	value_t *get(key_t const *key) const
	{
		return root->get(key);
	}

	/*
	 * Возвращает истину, если раньше такого элемента не
	 * было, и ложь, если был (но значение всё равно
	 * перезаписывается)
	 */
	bool insert(key_t const *key, value_t const &newval)
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
	bool erase(key_t const *key)
	{
		if (root->erase(key))
		{
			--count;
			return true;
		}

		return false;
	}

	/*
	 * Вывод дерева в красивом виде на поток
	 */
	template<class Ostream>
	Ostream &print(Ostream &os, str_t const &tab = str_t()) const
	{
		root->print(os, tab);
		return os;
	}



private:
	int count = 0;
	_TrieNode<T, ALPHABET_SIZE, OFFSET, Key, ESYM> *root;

	friend struct _TriePrivateAccess;

};

template<class Ostream, typename T, int N, int M, typename K, K E>
inline Ostream &operator<<( Ostream &os, Trie<T, N, M, K, E> const &toprint )
{
	toprint.print(os);
	return os;
}





struct _TriePrivateAccess
{
	template<typename T, int N, int M, typename K, K E>
	static int &count(Trie<T, N, M, K, E> &t)
	{
		return t.count;
	}

	template<typename T, int N, int M, typename K, K E>
	static _TrieNode<T, N, M> *&root(Trie<T, N, M, K, E> &t)
	{
		return t.root;
	}
};





}





#endif
