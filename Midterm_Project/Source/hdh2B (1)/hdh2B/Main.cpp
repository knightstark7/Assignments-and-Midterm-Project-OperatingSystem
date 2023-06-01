#include <iostream>
#include <stdio.h>
#include"Volume.h"
#include"Entry.h"
#include<string>
#include<vector>
#include <stack>
#include "VolumeMenu.h"
#include <direct.h>
int main()
{
	FILE* fp = nullptr;
	string name = "";
	int input = -1;
	errno_t err;
	Volume* v;
	int size = 0;
	vector<char> fatTable;
	while (true)
	{
		system("cls");
		cout << "0. Tao moi volume.\n1. Mo volume.\n2. Thoat.\n> Lua chon: ";
		cin >> input;
		cin.ignore();
		switch (input)
		{
		case 0:
			cout << "Nhap ten volume muon tao: ";
			getline(cin, name);
			cout << "Nhap kich thuoc volume(>0, theo MB): ";
			cin >> size;
			initVolume(name, size * 1024 * 1024);
			cout << "Tao volume thanh cong!" << endl;
			system("pause");
			break;
		case 1:
			cout << "Nhap ten volume muon mo: ";
			getline(cin, name);
			err = fopen_s(&fp, name.c_str(), "rb+");
			if (err == 0) {
				v = readVolume(fp);
				//Kiem tra chu ky
				if (v->Signature() != 0x00444448) {
					cout << "\nChuong trinh khong ho tro loai volume nay!";
					break;
				}
				//kiem tra mat khau truoc khi mo vol
				string pw;
				cout << "Nhap mat khau cua volume muon mo: "; getline(cin, pw);
				//mat khau dung
				if (v->checkPassword(pw)) {
					fatTable = readFatTable(fp, v);
					VolumeMenu::createMenu(fp, v, fatTable);
					fclose(fp);
				}
				else {
					cout << "Mat khau sai!!!\n";
					fclose(fp);
				}
				delete v;
			}
			else {
				cout << "Ten volume vua nhap khong ton tai!" << endl;
			}
			system("pause");
			break;
		case 2:
			if (fp)
				fclose(fp);
			return 1;
			break;
		default:
			cout << "Lua chon khong hop le!" << endl;
			break;
		}
	}
	return 0;
}