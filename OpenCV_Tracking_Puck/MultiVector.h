#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <thread>
#include <utility>

template<class T>
struct object
{
	T data;
	size_t readers;
	bool flag;
};

template<class T>
class MultiVector
{
	std::vector<object<T>> vect;
	std::vector<object<T>>::iterator it;

	std::unordered_map<std::thread::id, std::vector<std::pair<T, size_t>>::iterator> it_listeners;

	size_t listeners = 0;

public:

	MultiVector(size_t i) :
	{
		vect.reserve(i);
	}

	void addReader()
	{
		listeners++;
		it_listeners.insert({ std::this_thread::get_id(), vect.begin() });
	}

	T& get()
	{
		std::thread::id i = std::this_thread::get_id();
		std::vector<std::pair<T, size_t>>::iterator it = it_listeners[i];
		T frame = it.first;
		it.second--;
		if (it.second == 0) vect(it);
		return frame;
	}

	bool set(T& t)
	{
		if (vect.size() < vect.capacity())
		{
			vect.push_back(std::pair<T, size_t>(t, listeners));
			return true;
		}
		else return false;
	}
};

