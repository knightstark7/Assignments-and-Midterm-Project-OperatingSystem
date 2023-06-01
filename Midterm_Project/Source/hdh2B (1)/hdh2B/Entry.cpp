#include "Entry.h"
#include<fstream>
#include<string>

Entry::Entry()
{
	this->_SizeData = 0;
	this->_NameLen = 4;
	this->_PasswordLen = 3;
	this->_OffsetData = 0;
	this->_FatPosition = 0;
	this->_IsDeleted = false;
	this->pathLen = 0;
	this->path = "";
	this->subEntrySize = 0;
	this->_IsFolder = false;
	this->_Password = "";
	this->_Name = "";
	this->subEntryList = vector<Entry*>(0);
}

Entry::Entry(uint16_t NameLen, uint16_t PasswordLen, uint16_t pathLen,
	bool IsFolder, bool IsDeleted, string path, string Name, string Password, uint32_t SizeData, uint64_t OffsetData)
{
	this->_SizeData = SizeData;
	this->_NameLen = NameLen;
	this->_PasswordLen = PasswordLen;
	this->_OffsetData = OffsetData;
	this->pathLen = pathLen;
	this->subEntrySize = 0;
	this->_IsFolder = IsFolder;
	this->_IsDeleted = IsDeleted;
	this->path = path;
	this->_Password = Password;
	this->_Name = Name;
	this->subEntryList = vector<Entry*>(0);
}

void Entry::AddSubEntry(Entry* e) {
	subEntryList.push_back(e);
	subEntrySize += 1;
}
string Entry::GetFileName() {
	string filename;
	int pos = path.find_last_of("\\");
	if (pos == string::npos)
		filename = path;
	else
		filename = path.substr(path.find_last_of("\\") + 1);
	return filename;
}
bool Entry::add(FILE* f, Volume* v, vector<char>& fatTable) {
	int position = -1;
	//uu tien tim cluster con trong
	for (int i = 0; i < fatTable.size(); i++) {
		if (fatTable[i] == '0') {
			position = i;
			fatTable[position] = '1';
			break;
		}
	}
	//neu khong con cho trong thi ghi vao cluster cua file/folder da xoa
	if (position == -1)
		for (int i = 0; i < fatTable.size(); i++) {
			if (fatTable[i] == '2') {
				position = i;
				fatTable[position] = '1';
				break;
			}
		}
	//neu tat ca cluster da chua file/folder
	if (position == -1)
		return false;
	this->_FatPosition = position;
	fseek(f, v->volumeSize() + fatTable.size() + 2048 * position, SEEK_SET);
	fwrite(&(this->_SizeData), sizeof(this->_SizeData), 1, f);
	fwrite(&(this->_NameLen), sizeof(this->_NameLen), 1, f);
	fwrite(&(this->_PasswordLen), sizeof(this->_PasswordLen), 1, f);
	fwrite(&(this->_OffsetData), sizeof(this->_OffsetData), 1, f);
	fwrite(&(this->pathLen), 1, sizeof(this->pathLen), f);
	fwrite(&(this->subEntrySize), 1, sizeof(this->subEntrySize), f);
	fwrite(&(this->_FatPosition), sizeof(this->_FatPosition), 1, f);
	fwrite(&(this->_IsFolder), sizeof(this->_IsFolder), 1, f);
	fwrite(this->_Name.c_str(), this->_NameLen, 1, f);
	fwrite(this->_Password.c_str(), this->_PasswordLen, 1, f);
	fwrite(this->path.c_str(), 1, this->pathLen, f);

	v->SetSizeEntryTable(v->SizeEntryTable() + sizeof(*this));
	v->SetNumberOfEntry(v->NumberOfEntry() + 1);
	//Update SizeEntryTable and NumberOfEntry
	v->UpdateVol(f);
	return true;
}

void Entry::read(FILE* f)
{
	fread(&(this->_SizeData), sizeof(this->_SizeData), 1, f);
	fread(&(this->_NameLen), sizeof(this->_NameLen), 1, f);
	fread(&(this->_PasswordLen), sizeof(this->_PasswordLen), 1, f);
	fread(&(this->_OffsetData), sizeof(this->_OffsetData), 1, f);
	fread(&(this->pathLen), 1, sizeof(this->pathLen), f);
	fread(&(this->subEntrySize), 1, sizeof(this->subEntrySize), f);
	fread(&(this->_FatPosition), sizeof(this->_FatPosition), 1, f);
	fread(&(this->_IsFolder), sizeof(this->_IsFolder), 1, f);

	string name(this->_NameLen, '\0');
	fread(&name[0], sizeof(char), this->_NameLen, f);
	this->_Name = name;
	string password(this->_PasswordLen, '\0');
	fread(&password[0], sizeof(char), this->_PasswordLen, f);
	this->_Password = password;
	string path(this->pathLen, '\0');
	fread(&path[0], 1, this->pathLen, f);
	this->path = path;

}

bool Entry::IsParent(Entry* e) {
	if (this->pathLen < e->PathLength())
		return false;
	int pos = this->path.find_last_of('\\');
	//This is a file or a folder that doesnt have any parent
	if (pos == string::npos)
		return false;
	string parent = this->path.substr(0, pos);
	if (parent.compare(e->Path()) != 0)
		return false;
	return true;
}

bool Entry::FindParent(vector<Entry*>& entries, int k) {
	for (int i = k; i >= 0; --i) {
		if (this->IsParent(entries[i])) {
			entries[i]->AddSubEntry(this);
			return true;
		}
	}
	return false;
}

bool Entry::checkPassword(string pw) {
	// Kiem tra string da hash
	return (this->_Password.compare(pw) == 0);
}