#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <Windows.h>
#include"Volume.h"
using namespace std;
class Volume;
class Entry
{
private:
	uint32_t	_SizeData;//4		//Neu entry la folder thi sizedata = 0
	uint16_t	_NameLen;//2
	uint16_t	_PasswordLen;//2
	uint64_t	_OffsetData;//8		//Neu entry la folder thi offset co gia tri la 0
	uint16_t	pathLen; //2
	uint16_t	subEntrySize; //2
	uint64_t	_FatPosition;//8
	bool		_IsFolder;//1
	bool		_IsDeleted;//1
	string		_Name;
	string		path;
	string		_Password;
	vector<Entry*> subEntryList;
public:
	Entry();
	Entry(uint16_t NameLen, uint16_t PasswordLen, uint16_t pathLen,
		bool IsFolder, bool IsDeleted, string path, string Name, string Password, uint32_t SizeData = 0, uint64_t OffsetData = 0);
	uint32_t	SizeData() { return _SizeData; }
	uint16_t	NameLen() { return _NameLen; }
	uint16_t	PasswordLen() { return _PasswordLen; }
	uint64_t	OffsetData() { return _OffsetData; }
	uint64_t	FatPosition() { return _FatPosition; }
	uint16_t	PathLength() { return pathLen; }
	string		Path() { return path; }
	string		Password() { return _Password; }
	string		Name() { return _Name; }
	bool		IsFolder() { return _IsFolder; }
	void SetIsDeleted(bool value) { _IsDeleted = value; }
	void SetSubEntryList(const vector<Entry*> subEntries) { subEntryList = subEntries; }
	vector<Entry*> SubEntryList() { return subEntryList; }
	string GetFileName();
	bool add(FILE* f, Volume* v, vector<char>& fatTable);
	void AddSubEntry(Entry* e);
	void read(FILE* f);
	bool IsParent(Entry* e);
	bool FindParent(vector<Entry*>& entries, int k);
	bool checkPassword(string pw);
};