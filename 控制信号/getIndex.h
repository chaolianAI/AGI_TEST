#pragma once
#include<vector>
#include<list>
using namespace std;

bool arrEquals(vector<int> v1, vector<int> v2)
{
	if (v1.size() != v2.size()) return false;
	if (v1.size() == 0) return true;
	for (int i = 0; i < v1.size(); i++)
		if (v1[i] != v2[i]) return false;
	return true;
}

template<typename T>
int getIndex(vector<T> arr, T obj)
{
	if (arr.size() == 0) return -1;
	for (int i = 0; i < arr.size(); i++)
		if (obj.equals(arr[i]))
			return i;
	return -1;
}

template<typename T>
int getIndex2(vector<T> arr, T obj) //针对基本类型的
{
	if (arr.size() == 0) return -1;
	for (int i = 0; i < arr.size(); i++)
		if (obj == arr[i])
			return i;
	return -1;
}

template<typename T>
int getIndex3(vector<T> arr, T obj) //针对数组类型的
{
	if (arr.size() == 0) return -1;
	for (int i = 0; i < arr.size(); i++)
		if (arrEquals(obj, arr[i]))
			return i;
	return -1;
}

template<typename T>
bool addInto(vector<T> *arr, T obj)
{
	int index = getIndex(*arr, obj);
	if (index != -1) return false;
	else
	{
		arr->push_back(obj);
		return true;
	}
}

template<typename T>
bool addInto2(vector<T> *arr, T obj)
{
	int index = getIndex2(*arr, obj);
	if (index != -1) return false;
	else
	{
		arr->push_back(obj);
		return true;
	}
}

template<typename T>
bool addInto3(vector<T> *arr, T obj)
{
	int index = getIndex3(*arr, obj);
	if (index != -1) return false;
	else
	{
		arr->push_back(obj);
		return true;
	}
}

template<typename T>
vector<T> intersect(vector<T> set1, vector<T> set2)
{
	vector<T> intersection;
	if (set1.size() == 0 || set2.size() == 0) return intersection;//空集
	for (int i = 0; i < set1.size(); i++)
		if (getIndex(set2, set1[i]) >= 0)
			intersection.push_back(set1[i]);
	return intersection;
}

/*
template<typename T>
T* list_at(list<T> *lst, int index)
{
	list<T>::iterator itor = lst->begin();
	for (int i = 0; i < index; i++)
		itor++;
	return itor;
}
*/
