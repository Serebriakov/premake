#ifndef _DICTIONARY_HPP_
#define _DICTIONARY_HPP_

#include <map>
#include <string>


/*
Template class for storing different types of objects in an associative array
*/
template<class T>
class Dictionary
{
private :
	// hide the copy constructor and operator= to prevent cloning a Dictionary
	Dictionary(const Dictionary &);
	Dictionary &operator=(const Dictionary &);


protected :
	std::map<std::string, T*>		m_Objects;									// pointers to the objects


public :
	typedef typename std::map<std::string, T*>::iterator iterator;
	typedef std::pair<typename std::map<std::string, T*>::iterator, bool> insert_t;

	Dictionary();
	virtual ~Dictionary();

	// add an element to the container
	insert_t Insert(const std::string &Key, T *Value);

	// find
	iterator Find(const std::string &Key);

	// end iterator
	iterator IteratorEnd();

	// free and remove element
	size_t Erase(const std::string &Key);

	// clear
	void Clear();
};


template<class T>
Dictionary<T>::Dictionary()
{
}

template<class T>
Dictionary<T>::~Dictionary()
{
	Clear();
}

/*
Insert an element into the dictionary
The returned pair contains an iterator pointing to the newly inserted element or the previously
stored one, and a bool indicating if a new element was inserted (true) or if an equivalent key already existed (false)
*/
template<class T>
typename Dictionary<T>::insert_t Dictionary<T>::Insert(const std::string &Key, T *Value)
{
	return m_Objects.insert(std::pair<std::string, T*>(Key, Value));
}

/*
Find an element in the map.
If it cannot be found, objects.end is returned
*/
template<class T>
typename Dictionary<T>::iterator Dictionary<T>::Find(const std::string &Key)
{
	return m_Objects.find(Key);
}

/*
Return the iterator objects.end
*/
template<class T>
typename Dictionary<T>::iterator Dictionary<T>::IteratorEnd()
{
	return m_Objects.end();
}

/*
Erase
free and remove an element
*/
template<class T>
size_t Dictionary<T>::Erase(const std::string &Key)
{
	delete m_Objects[Key];
	return m_Objects.erase(Key);
}

/*
Frees and removes all the stored elements
*/
template<class T>
void Dictionary<T>::Clear()
{
	for( iterator i = m_Objects.begin(); i != m_Objects.end(); ++i )
	{
		delete i->second;
	}
	m_Objects.clear();
}

#endif
