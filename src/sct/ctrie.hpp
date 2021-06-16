#ifndef NVX_CTRIE_HPP_8491303
#define NVX_CTRIE_HPP_8491303

#include <cstring>
#include <functional>
#include <iterator>





namespace nvx
{





/*
 * Действует аналогично библиотечной strcmp,
 * но если первая строка является подстрокой
 * второй, возвращает -2, а если наоборот — 2,
 * при этом в c (если задано) помещается номер
 * символа, на котором было найдено различие
 * (если же строки равны, то там будет их размер;
 * если одна из строк является подстрокой другой,
 * то там будет размер подстроки)
 */
template<typename Char>
int cmp(Char const *lhs, Char const *rhs, int *c = nullptr)
{
	if(c) *c = 0;
	while(*lhs != '\0' && *rhs != '\0')
	{
		if(*lhs < *rhs)
			return -1;
		if(*lhs > *rhs)
			return 1;
		++lhs, ++rhs;
		if(c) ++*c;
	}

	if(*lhs == '\0' && *rhs == '\0')
		return 0;
	return *lhs == '\0' ? -2 : 2;
}

template<typename Char>
void addto(Char **dest, Char const *val, Char esym)
{
	int dlen    = strlen(*dest);
	int newlen  = dlen + strlen(val);
	Char *tmp   = new Char[newlen+1];
	tmp[newlen] = esym;
	memcpy(tmp, *dest, dlen * sizeof(Char));
	memcpy(tmp+dlen, val, (newlen-dlen) * sizeof(Char));

	delete *dest;
	*dest = tmp;
	return;
}



template<typename Key>
struct _CTrieNodeTraits
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
struct _CTrieNodeTraits<wchar_t>
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
	typename Key  = char,
	Key      ESYM = '\0'
>
struct _CTrieNode
{
	// types
	typedef _CTrieNode               node_t;
	typedef _CTrieNodeTraits<Key>    traits_t;
	typedef T                        value_t;
	typedef Key                      key_t;
	typedef typename traits_t::str_t str_t;



	// (de)constructor
	_CTrieNode():
		val(value_t()), key(nullptr),
		next(nullptr),  chs(nullptr), store(false) {}

	~_CTrieNode()
	{
		if (key)  delete[] key;
	}

	void free()
	{
		if (chs)
		{
			chs->free();
			delete chs;
		}

		if (next)
		{
			next->free();
			delete next;
		}
	}


	// methods
	value_t *get(key_t const *akey)
	{
		if(*akey == ESYM)
			return store ? &val : nullptr;

		int len;
		int cmpres;
		for(node_t *ch = chs; ch; ch = ch->next)
		{
			cmpres = cmp(ch->key, akey, &len);
			if(cmpres == 0 || cmpres == -2)
				return ch->get(akey+len);
			if(cmpres > 0)
				return nullptr;
		}

		return nullptr;
	}

	bool insert(key_t const *akey, value_t const &newval)
	{
		if(*akey == ESYM)
		{
			val = newval;

			if(store)
				return false;

			store = true;
			return true;
		}

		/*
		 * Если вставлять нужно не непосредственно в этот узел,
		 * пройдёмся по каждому ребёнку и найдён того, кому можно
		 * поручь вставить. Если такого не найдётся, нужно создать
		 * нового ребёнка
		 */
		int cmpres;
		int len;
		for(node_t *ch = chs, *prev = nullptr; true; prev = ch, ch = ch->next)
		{
			/*
			 * Если детей нет, требутся создать нового ребёнка,
			 * ключ которого будет полностью совпадать с тем,
			 * который необходимо вставить
			 */
			if(!ch)
			{
			insert_new_child_label:
				node_t *newch = new node_t;
				newch->val    = newval;
				newch->store  = true;
				newch->key    = new key_t[strlen(akey)+1];
				strcpy(newch->key, akey);

				// Приведём в соответствие список детей
				prev ? prev->next = newch : chs = newch;
				newch->next = ch;
				return true;
			}

			cmpres = cmp(ch->key, akey, &len);

			/*
			 * Если ключ ребёнка больше вставляемого по первому символу,
			 * а такого ключа ребёнка, что был бы подстрокой ключа
			 * вставляемого элемента нет, то вставляем новый элемент
			 */
			if(cmpres == 1 && !len)
				goto insert_new_child_label;

			/*
			 * Если ключи равны или ключ ребёнка является подстрокой
			 * ключа вставляемого элемента, то делегируем вставку
			 * элемента ребёнку
			 */
			if(!cmpres || cmpres == -2)
			{
				return ch->insert(akey+len, newval);
			}

			/*
			 * Если строки не равны и вместе с тем ни одна не является
			 * подстрокой другой, а различие их не в первом символе, то
			 * разбиваем ребёнка на двое и к промежуточному присоединяем
			 * новый:
			 *
			 * (Необходимо вставить ac)
			 *
			 *    Было       Стало
			 *           |
			 *           |      /--> b
			 *   -> ab   |  -> a
			 *    + ac   |      \--> c
			 *           |
			 */
			if( (cmpres == 1 || cmpres == -1) && len )
			{
				// Создадим узел, на котором будет разветвление
				node_t *brch          = new node_t;
				brch->key             = new key_t[len+1];
				brch->key[len]        = ESYM;
				memcpy(brch->key, ch->key, sizeof(key_t) * len);

				// Вставляем ветвлящийся узел вместо текущего
				prev ? prev->next     = brch : chs = brch;
				brch->next            = ch->next;
				ch->next              = nullptr;

				// Удаляем префикс у осиротевшего ребёнка
				int tmpkeylen         = strlen(ch->key) - len;
				key_t *tmpkey         = new key_t[tmpkeylen+1];
				tmpkey[tmpkeylen]     = ESYM;
				memcpy(tmpkey, ch->key + len, sizeof(key_t) * tmpkeylen);
				delete[] ch->key;
				ch->key               = tmpkey;

				// Создаём новый узел, который будет хранить вставляемый элемент
				node_t *newch         = new node_t;
				newch->val            = newval;
				newch->store          = true;
				int newkeylen         = strlen(akey) - len;
				newch->key            = new key_t[newkeylen+1];
				newch->key[newkeylen] = ESYM;
				memcpy(newch->key, akey + len, sizeof(key_t) * newkeylen);

				// Вставляем осиротевшего и нового ребёнка в ветвлящийся
				if(cmpres < 0)
				{
					brch->chs = ch;
					ch->next  = newch;
				}
				else
				{
					brch->chs   = newch;
					newch->next = ch;
				}

				return true;
			}

			/*
			 * Если ключ вставляемого элемента является подстрокой
			 * ключа ребёнка, то разбиваем ребёнка на две части и
			 * в середину вставляем элемент
			 */
			if(cmpres == 2)
			{
				// Создаём новый узел
				node_t *newch     = new node_t;
				newch->val        = newval;
				newch->store      = true;
				int newkeylen     = strlen(akey);
				newch->key        = new char[newkeylen+1];
				strcpy(newch->key, akey);

				// Обрезаем ключ ребёнка
				int tmpkeylen     = strlen(ch->key) - newkeylen;
				char *tmpkey      = new char[tmpkeylen+1];
				memcpy(tmpkey, ch->key + newkeylen, sizeof(char) * tmpkeylen);
				tmpkey[tmpkeylen] = '\0';
				delete[] ch->key;
				ch->key           = tmpkey;

				// Вставляем новый узел
				prev ? prev->next = newch : chs = newch;
				newch->next       = ch->next;
				ch->next          = nullptr;
				newch->chs        = ch;

				return true;
			}
		}
	}

	bool erase(key_t const *akey)
	{
		int len;
		int cmpres;

		for(node_t *ch = chs, *prev = nullptr; ch; prev = ch, ch = ch->next)
		{
			cmpres = cmp(akey, ch->key, &len);

			if (cmpres == -2 || cmpres == -1 || (cmpres == 1 && len))
				return false;

			if (cmpres == 2)
				return ch->erase(akey + len);

			if (!cmpres)
			{
				if (!ch->chs)
				{
					prev ? prev->next = ch->next : chs = ch->next;
					delete ch;
					if (*key != ESYM && chs && !chs->next && !store)
					{
						addto(&key, chs->key, ESYM);
						if (chs->store)
							val = std::move(chs->val), store = true;
						node_t *tmp = chs;
						chs = tmp->chs;
						delete tmp;
					}

					return true;
				}

				if (!ch->chs->next)
				{
					int chkeylen      = strlen(ch->key);
					int tmpkeylen     = chkeylen + strlen(ch->chs->key);
					key_t *tmpkey     = new key_t[tmpkeylen+1];
					tmpkey[tmpkeylen] = ESYM;
					memcpy(tmpkey, ch->key, chkeylen * sizeof(key_t));
					memcpy(tmpkey+chkeylen, ch->chs->key, (tmpkeylen-chkeylen) * sizeof(key_t));

					prev ? prev->next = ch->chs : chs = ch->chs;
					ch->chs->next     = ch->next;
					delete ch->chs->key;
					ch->chs->key      = tmpkey;
					delete ch;

					return true;
				}

				if (!ch->store)
					return false;

				ch->store = false;
				return true;
			}
		}

		return false;

	}

	template<class Ostream>
	Ostream &print(
		Ostream &os,
		str_t const &tabme  = str_t(),
		str_t const &taboth = str_t()
	) const
	{
		if (*key == ESYM)
			os << tabme << traits_t::root;
		else
			os << tabme << key;

		if (store)
			os << traits_t::space << val;

		for (node_t *ch = chs; ch; ch = ch->next)
		{
			if (ch->next)
			{
				ch->print(
					os << '\n',
					taboth + traits_t::uprightdown + traits_t::leftright + traits_t::space,
					taboth + traits_t::updown + traits_t::doublespace
				);
			}
			else
			{
				ch->print(
					os << '\n',
					taboth + traits_t::upright + traits_t::leftright + traits_t::space,
					taboth + traits_t::doublespace
				);
			}
		}

		return os;
	}


	// members
	value_t val;
	key_t   *key;
	node_t  *next, *chs;
	bool    store;


private:
	bool _check_eraseme()
	{
		return false;
	}

};

template<class Ostream, typename T, typename K, K E>
inline Ostream &operator<<( Ostream &os, _CTrieNode<T, K, E> const &toprint )
{
	toprint.print(os);
	return os;
}





template<
	typename T,
	typename Key = char,
	Key ESYM = '\0'
>
class CTrie
{
public:
	typedef _CTrieNodeTraits<Key>     traits_t;
	typedef T                        value_t;
	typedef Key                      key_t;
	typedef typename traits_t::str_t str_t;

	CTrie()
	{
		root = new _CTrieNode<T, key_t, ESYM>;
		root->key = new char[1];
		root->key[0] = ESYM;
	}

	~CTrie()
	{
		root->free();
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
	_CTrieNode<T, Key, ESYM> *root;

	friend struct _CTriePrivateAccess;

};

template<class Ostream, typename T, typename K, K E>
inline Ostream &operator<<( Ostream &os, CTrie<T, K, E> const &toprint )
{
	toprint.print(os);
	return os;
}





struct _CTriePrivateAccess
{
	template<typename T, typename K, K E>
	static int &count(CTrie<T, K, E> &t)
	{
		return t.count;
	}

	template<typename T, typename K, K E>
	static _CTrieNode<T, K, E> *&root(CTrie<T, K, E> &t)
	{
		return t.root;
	}
};





}





#endif
