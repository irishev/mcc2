#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <direct.h>
#include <vector>
#include <io.h>
#include <Windows.h>

#define N_DESC 20

using namespace std;

typedef struct dirinfo {
	string path;
	int fq;
};
int cmpstr(char* a, char* b, int c) {
	for (int i = 0; i < c; i++) {
		if (a[i] != b[i])
			return 0;
	}
	return 1;
}
void parsing() {
	char temp[4096];
	char line[4096];
	ifstream iFile("D:/project/content.rdf.u8");
	ofstream oFile("D:/project/topic");
	int pos;
	int link;
	int end;
	int deb = 1;
	while (iFile.getline(temp, 4095)) {
		pos = 0;
		while (temp[pos++] == ' ');
		if (cmpstr(temp + pos, "Topic", 5)) {
			link = 0;
			while (temp[pos++] != '=');
			end = pos;
			while (temp[++end] != '\"');
			temp[end] = 0;
			sprintf(line, "topic:%s", temp + pos + 1);
			while (true) {
				pos = 0;
				iFile.getline(temp, 4095);
				while (temp[pos++] == ' ');
				if (cmpstr(temp + pos, "/Topic>", 7)) {
					break;
				}
				if (cmpstr(temp + pos, "link", 4)) {
					link++;
				}
			}
			for (int i = 0; line[i]; i++) {
				if (!(line[i] > 0 && line[i] < 128))
					line[i] = ' ';
			}
			oFile << line << ':' << link << endl;
			deb++;
		}
	}
	iFile.close();
	oFile.close();
}
/////////////////////////////////////////////////////////////////////
char** retjson(char* a) {
	int s, e;
	char** ret = new char*[3];
	s = 0;
	int l = 0;
	int i = 0;
	for (; a[i]; i++) {
		if (a[i] == ':') {
			e = i;
			char* temp = new char[e - s + 1];
			for (int j = 0; j < e - s ; j++)
				temp[j] = a[s + j];
			temp[e - s] = 0;
			ret[l++] = temp;
			s = e + 1;
		}
	}
	char* temp = new char[i - s + 1];
	for (int j = 0; j < i - s; j++)
		temp[j] = a[s + j];
	temp[i - s] = 0;
	ret[l] = temp;

	return ret;
}
char** retpath(char* a) {
	int s, e;
	
	s = 0;
	int l = 0;
	int size = 1;
	for (int i = 0; a[i]; i++) {
		if (a[i] == '/')
			size++;
	}
	char** ret = new char*[size + 1];
	int i = 0;
	for (; a[i]; i++) {
		if (a[i] == '/') {
			e = i;
			char* temp = new char[e - s + 1];
			for (int j = 0; j < e - s ; j++)
				temp[j] = a[s + j];
			temp[e - s] = 0;
			ret[l++] = temp;
			s = e + 1;
		}
	}
	char* temp = new char[i - s + 1];
	for (int j = 0; j < i - s; j++)
		temp[j] = a[s + j];
	temp[i - s] = 0;
	ret[l++] = temp;
	ret[l] = 0;
	return ret;
}
void dirproc() {

}
//0 체크 오류 수정필요
void dellist(char** a) {
	for (int i = 0; a[i] ; i++)
		delete[] a[i];
	delete[] a;
}



dirinfo* list = new dirinfo[60000];
int load = 0;

void sumup(int i) {
	size_t k = list[i].path.find_last_of('/',list[i].path.length()-2);
	string temp = list[i].path.substr(0, k+1);
	for (int j = 0; j < load; j++) {
		if (!strcmp(list[j].path.c_str(), temp.c_str())) {
			list[j].fq += list[i].fq;
			list[i].fq = 0;
			return;
		}
	}
}

void taxonomy() {
	ifstream iFile("D:/project/topic");
	ifstream iFile2("D:/project/topic2");
	ofstream oFile("D:/project/topic2");
	char** prev = 0;
	char line[1024];
	iFile.getline(line, 1023);
	while (iFile.getline(line, 1023)) {
		char** plist = retjson(line);
		char** dir = retpath(plist[1]);
		if (!strcmp(dir[1], "World"))
			continue;
		if (!strcmp(dir[1], "Regional"))
			continue;
		int i = 0;
		for (; dir[i]; i++) {
			if (!strcmp(dir[i], "Titles")) {
				delete[] dir[i];
				for (int j = i; dir[j]; j++)
					dir[j] = dir[j + 1];
				i--;
			}
			if (strlen(dir[i]) == 1) {
				delete[] dir[i];
				for (int j = i; dir[j]; j++)
					dir[j] = dir[j + 1];
				i--;
			}
		}
		dir[i - 1] = 0;
		plist[1][0] = 0;
		for (int j = 0; dir[j]; dir++) {
			strcat(plist[1], dir[j]);
			strcat(plist[1], "/");
		}
		plist[2][0] -= 48;

		if (prev) {
			if (!strcmp(prev[1], plist[1])) {
				prev[2][0] += plist[2][0];
				continue;
			}
			int ex = 0;
			for (int i = 0; i < load; i++) {
				if (!strcmp(list[i].path.c_str(), prev[1])) {
					list[i].fq += prev[2][0];
					ex = 1;
					break;
				}
			}
			if (!ex) {
				list[load].path = prev[1];
				list[load].fq = prev[2][0];
				load++;
			}
		}
		prev = plist;
	}
	int ref = 1;
	while (ref) {
		ref = 0;
		for (int i = 1; i < load; i++)
			if (list[i].fq < N_DESC && list[i].fq > 0) {
				sumup(i);
				ref = 1;
			}
	}
	for (int i = 0; i < load; i++)
		if (list[i].fq != 0)
			oFile << list[i].path << ":" << list[i].fq << endl;
	oFile.close();
}

void createdir() {
	ifstream iFile("D:/project/topic2");
	char temp[256] = "D:/MCC/";
	while (iFile.getline(temp + 7, 248)) {
		for (int i = 7; i < 256; i++) {
			if (temp[i] == ':') {
				temp[i] = 0;
				break;
			}
		}
		_mkdir(temp);
	}
}

typedef struct wordtf {
	char word[128];
	int tf=0;
	int index;
}wordtf;

int findword(vector<wordtf> a, int k, int index, char* word) {
	for (int i = 0; i < k; i++) {
		if (a[i].index == index) {
			if (!strcmp(a[i].word, word))
				return i;
		}
	}
	return -1;
}

vector<wordtf> descproc(char* desc) {
	int pos = 0;
	int end;
	int size = 256;
	int n = 0;
	int index=0;

	vector<wordtf> temp(size);
	while (desc[pos]) {
		end = pos;
		while (desc[++end] != ' ') {
			if (desc[end] == 0)
				break;
		}
		desc[end] = 0;
		index = 0;
		char* term = desc + pos;
		for (int i = 0; i < 7 && term[i]; i++) {
			index += term[i] - 97;
		}
		int k = findword(temp, n, index, term);
		if (k == -1) {
			temp[n].tf = 1;
			temp[n].index = index;
			strcpy(temp[n++].word, term);
		}
		else {
			temp[k].tf++;
		}
		pos = end + 1;
		while (desc[pos] == ' ') {
			pos++;
		}
		if (n == size)
			size = size*1.5;
			temp.resize(size);
	}
	
	return temp;
}

void parsing2() {
	char temp[65536];
	char link[4096];
	char desc[40960];
	char path[4096];
	char filepath[256];
	ifstream iFile("D:/project/content.rdf.u8");
	int pos;
	int end;
	while (iFile.getline(temp, 65535)) {
		pos = 0;
		while (temp[pos++] == ' ');
		if (cmpstr(temp + pos, "ExternalPage", 12)) {
			while (temp[pos++] != '=');
			end = pos;
			while (temp[++end] != '\"');
			temp[end] = 0;
			sprintf(link, "%s", temp + pos + 1);
			while (true) {
				pos = 0;
				iFile.getline(temp, 65535);
				while (temp[pos++] == ' ');
				if (cmpstr(temp + pos, "/ExternalPage>", 14)) {
					break;
				}
				if (cmpstr(temp + pos, "d:Description>", 14)) {
					while (temp[pos++] != '>');
					if (temp[pos] == '<') {
						sprintf(desc, " ");
					}
					else {
						end = pos;
						while (temp[++end] != '<');
						temp[end] = 0;
						sprintf(desc, "%s", temp + pos);
					}
				}
				if (cmpstr(temp + pos, "topic>", 6)) {
					while (temp[pos++] != '>');
					end = pos;
					while (temp[++end] != '<');
					temp[end] = 0;
					sprintf(path, "%s", temp + pos);
				}
			}
			for (int i = 0; link[i]; i++) {
				if (!(link[i] > 96 && link[i] < 123))
					link[i] = '_';
			}
			for (int i = 0; desc[i]; i++) {
				if (desc[i] > 64 && desc[i] < 91)
					desc[i] += 32;
				if (!(desc[i] > 96 && desc[i] < 123))
					desc[i] = ' ';
			}
			vector<wordtf> content = descproc(desc);
			char dirpath[256];
			sprintf(dirpath, "D:/MCC/%s", path);
			int skip = 0;
			while (_access(dirpath, 0)) {
				string temp = path;
				int x = temp.find_last_of('/');
				if (x == 3)
					skip = 1;
				strcpy(path, temp.substr(0, x).c_str());
				sprintf(dirpath, "D:/MCC/%s", path);
			}
			if (skip)
				continue;
			sprintf(filepath, "%s/%s.dat", dirpath, link + 7);
			ofstream oFile(filepath);
			vector<wordtf>::iterator iter = content.begin();
			for (; iter != content.end(); iter++) {
				if ((*iter).tf == 0)
					break;
				oFile << (*iter).word << ":" << (*iter).tf << endl;
			}
			oFile.close();
		}
	}
	iFile.close();
}

typedef struct _finddata_t FILE_SEARCH;

typedef struct wordtree {
	int df = 0;
	wordtree* next[26] = { 0 };
};

wordtree root;

void addword(char* a) {
	wordtree* p = &root;
	for (int i = 0; a[i]; i++) {
		if (p->next[a[i] - 97])
			p = p->next[a[i] - 97];
		else {
			p->next[a[i] - 97] = new wordtree;
			p = p->next[a[i] - 97];
		}
	}
	p->df++;
}

void readFile(char* path) {
	ifstream iFile(path);
	char line[256];
	while (iFile.getline(line, 255)) {
		char** k = retjson(line);
		addword(k[0]);
		delete[] k[0];
		delete[] k[1];
		delete[] k;
	}
}

char curword[16] = { 0 };
int pos = 0;
void writedf(wordtree* p, ofstream& file) {
	if (pos > 15) {
		pos--;
		return;
	}
	if (p->df)
		file << curword << ":" << p->df << endl;
	for (int i = 0; i < 26; i++) {
		if (p->next[i]) {
			curword[pos++] = i + 97;
			writedf(p->next[i], file);
		}
	}
	curword[pos] = 0;
	pos--;
}

void finddf(char* path) {
	long h_file;
	char* search = new char[256];
	char* filepath = new char[256];
	FILE_SEARCH file_search;
	sprintf(search, "%s/*.*", path);
	if ((h_file = _findfirst(search, &file_search)) == -1L) {
		return;
	}
	else {
		do {
			sprintf(filepath, "%s", file_search.name);
			if (!strcmp(filepath, ".") || !strcmp(filepath, ".."))
				continue;
			if (file_search.attrib & _A_SUBDIR) {
				sprintf(filepath, "%s%s/", path, file_search.name);
				finddf(filepath);
			}
			else {
				sprintf(filepath, "%s%s", path, file_search.name);
				readFile(filepath);
			}
		} while (_findnext(h_file, &file_search) == 0);
		_findclose(h_file);
	}
	
	delete[] search;
	delete[] filepath;
}

void main() {
	//parsing();
	//taxonomy();
	//createdir();
	//parsing2();
	char path[100] = "D:/MCC/Top/";
	finddf(path);
	ofstream oFile("D:/MCC/df.dat");
	writedf(&root, oFile);
	oFile.close();
}