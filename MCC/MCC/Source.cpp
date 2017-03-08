#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <direct.h>
#include <vector>
#include <io.h>
#include <Windows.h>
#include <math.h>
#include <atlstr.h>
#include <atlcoll.h>
#include <stdlib.h>


#define N_DESC 20

using namespace std;

typedef struct dirinfo {
	string name;
	int fq;
	int subfq;
	vector<dirinfo*> next;
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



dirinfo* list = new dirinfo;
int load = 0;

void deleteTax(dirinfo* p) {
	vector<dirinfo*>::iterator iter = (list->next).begin();

	for (iter = p->next.begin(); iter != p->next.end();) {
		dirinfo* t = *iter;
		if (t->name == "Titles") {
			iter = p->next.erase(iter);
		}
		else if (t->name == "World") {
			iter = p->next.erase(iter);
		}
		else if (t->name == "Regional") {
			iter = p->next.erase(iter);
		}
		else if (t->name.length() == 1) {
			iter = p->next.erase(iter);
		}
		else {
			deleteTax(t);
			iter++;
		}
	}
	
	
	
}
void deleteTax3(dirinfo* p) {
	vector<dirinfo*>::iterator iter;
	for (iter = p->next.begin(); iter != p->next.end();) {
		dirinfo* t = *iter;
		if (t->subfq < 150) {
			iter = p->next.erase(iter);
		}
		else {
			deleteTax3(t);
			iter++;
		}
	}
}
void deleteTax2(dirinfo* p) {
	vector<dirinfo*>::iterator iter;

	for (iter = p->next.begin(); iter != p->next.end();) {
		dirinfo* t = *iter;
		if (t->next.size() == 0) {
			iter = p->next.erase(iter);
		}
		else {
			deleteTax2(t);
			iter++;
		}
	}
}
stringstream outstream;

void printTax(dirinfo* p, string s) {
	vector<dirinfo*>::iterator iter = (list->next).begin();
	for (iter = p->next.begin(); iter != p->next.end(); iter++) {
		string temp = s;
		dirinfo* t = *iter;
		temp += "/" + t->name;
		outstream << temp << ":" << t->fq << endl;
		printTax(t, temp);
	}
}

int mergeFq(dirinfo* p) {
	vector<dirinfo*>::iterator iter = (list->next).begin();
	p->subfq = p->fq;
	for (iter = p->next.begin(); iter != p->next.end(); iter++) {
		dirinfo* t = *iter;
		p->subfq += mergeFq(t);
	}
	return p->subfq;
}

void taxonomy() {
	ifstream iFile("D:/project/topic");
	//ifstream iFile2("D:/project/topic2");
	ofstream oFile("D:/project/topic2");
	char** prev = 0;
	char line[1024];
	iFile.getline(line, 1023);
	while (iFile.getline(line, 1023)) {
		char** plist = retjson(line);
		char** dir = retpath(plist[1]);
		dirinfo* p = list;
		for (int i = 1; dir[i]; i++) {
			vector<dirinfo*>::iterator iter = (p->next).begin();
			for (iter = (p->next).begin(); ; ++iter) {
				string temp = dir[i];
				if (iter == (p->next).end()) {
					dirinfo* a = new dirinfo();
					char* num = plist[2];
					int k = atoi(num);
					a->fq = k;
					a->name = dir[i];
					(p->next).push_back(a);
					p = a;
					break;
				}
				if (temp == (*iter)->name) {
					p = *iter;
					break;
				}
			}
		}
	}
	mergeFq(list);
	deleteTax(list);
	deleteTax2(list);
	deleteTax3(list);
	printTax(list, "Top");
	oFile << outstream.str();
	oFile.close();
}

void createdir() {
	ifstream iFile("D:/project/topic2");
	char temp[256] = "D:/MCC/";
	char read[1024];
	while (iFile.getline(read,1023)) {
		string temp2 = read;
		int s = temp2.find_first_of(':');
		//int e = temp2.find_last_of(':');
		//temp2 = temp2.substr(s + 1, e - s -1);
		temp2 = temp2.substr(0, s);
		strcpy(temp + 7, temp2.c_str());
		_mkdir(temp);
	}
}
CAtlMap<CAtlString, int> stopwords;
void loadSW() {
	ifstream iFile("D:/project/stopword");
	string temp;
	while (iFile >> temp) {
		stopwords.SetAt(temp.c_str(), 1);
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
		if (stopwords.Lookup(term)) {
			
		}
		else {
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
	loadSW();
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
			string templink = link;
			for (int i = 0; i<templink.length(); i++) {
				if (!(templink[i] > 96 && templink[i] < 123))
					templink.erase(templink.begin()+i--);
			}
			strcpy(link, templink.c_str());
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
			bool isFirst = true;
			while (_access(dirpath, 0)) {
				if (isFirst) {
					for (int i = 0; path[i]; i++) {
						if (!isalnum(path[i]) && path[i] != '/' && path[i] != '_' && path[i] != '-' && path[i] != '\'' && path[i] != ',') {
							path[i] = ' ';
							int k = i;
							while (path[k++] == ' ') {
								if (path[k] == '/')
									path[i] = 0;
							}
						}
					}
					isFirst = false;
				}
				else {
					string temp = path;
					int x = temp.find_last_of('/');
					if (x == 3)
						skip = 1;
					strcpy(path, temp.substr(0, x).c_str());
				}
				sprintf(dirpath, "D:/MCC/%s", path);
			}
			if (skip)
				continue;
			sprintf(filepath, "%s/%s.dat", dirpath, link + 4);
			ofstream oFile(filepath);
			vector<wordtf>::iterator iter = content.begin();
			for (; iter != content.end(); iter++) {
				if ((*iter).tf == 0)
					break;
				if (strlen((*iter).word) < 3)
					continue;
				oFile << (*iter).word << ":" << (*iter).tf << endl;
			}
			oFile.close();
		}
	}
	iFile.close();
}

typedef struct _finddata_t FILE_SEARCH;

typedef struct wordtree {
	char letter = 0;
	int tf = 0;
	int df = 0;
	double ccv = 0;
	char e = 0;
	int cache[26] = { -1 };
	wordtree* next[26] = { 0 };
};

wordtree root;

void addword(char* a) {
	wordtree* p = &root;
	for (int i = 0; a[i]; i++) {
		if (!isalpha(a[i]))
			continue;
		if (p->next[a[i] - 97])
			p = p->next[a[i] - 97];
		else {
			
			p->next[a[i] - 97] = new wordtree;
			p->cache[(p->e)++] = a[i] - 97;
			p = p->next[a[i] - 97];
			p->letter = a[i];
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

string curword;
int pos = 0;
void writedf(wordtree* p, ofstream& file) {
	if (p->df)
		file << curword << ":" << p->df << endl;
	
	for (int i = 0; i < p->e; i++) {
		curword.push_back(p->next[p->cache[i]]->letter);
		writedf(p->next[p->cache[i]], file);
	}

	curword.pop_back();
}

void finddf(char* path) {
	long h_file;
	char* search = new char[1024];
	char* filepath = new char[1024];
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
wordtree dftree;
void addword(char* a, int k) {
	wordtree* p = &dftree;
	for (int i = 0; a[i]; i++) {
		if (p->next[a[i] - 97])
			p = p->next[a[i] - 97];
		else {
			
			p->next[a[i] - 97] = new wordtree;
			p->cache[(p->e)++] = a[i] - 97;
			p = p->next[a[i] - 97];
			p->letter = a[i];
		}
	}
	p->df = k;
}
int documentset = 984680;
double length2 = 0;
void addwordtf(char* a, int k) {
	wordtree* p = &dftree;
	for (int i = 0; a[i]; i++) {
		if (p->next[a[i] - 97])
			p = p->next[a[i] - 97];
		else {
			p->next[a[i] - 97] = new wordtree;
			p->cache[(p->e)++] = a[i] - 97;
			p = p->next[a[i] - 97];
			p->letter = a[i];
		}
	}
	p->tf += k;
	length2 += log10((double)documentset / p->df) * p->tf * log10((double)documentset / p->df) * p->tf;
}
<<<<<<< HEAD
vector<wordtree*> dlist;
=======
/*
>>>>>>> origin/master
void normalize(wordtree* p) {
	double length = sqrt(length2);
	if (p->tf) {
		
		p->ccv += log10((double)documentset / p->df) * p->tf * log2(length) / length;
		dlist.push_back(p);
	}
	for (int i = 0; i < p->e; i++) {
		normalize(p->next[p->cache[i]]);
	}
}*/
void inittf(wordtree* p) {
	p->tf = 0;
	for (int i = 0; i < p->e; i++) {
		inittf(p->next[p->cache[i]]);
	}
}
void calcval(char* path) {
	ifstream iFile(path);
	char line[256];
	while (iFile.getline(line, 255)) {
		char** k = retjson(line);
		int l = 0;
		for (int i=0; k[1][i]; i++)
			l = l * 10 + k[1][i] - 48;
		addwordtf(k[0],l);
		delete[] k[0];
		delete[] k[1];
		delete[] k;
	}
	//normalize(&dftree);
	inittf(&dftree);
	length2 = 0;
}

void inittree(wordtree* p) {
	vector<wordtree*>::iterator iter = dlist.begin();
	for (iter = dlist.begin(); iter != dlist.end(); iter++) {
		wordtree* t = *iter;
		t->tf = 0;
		t->ccv = 0;
	}
	dlist.clear();
}

string curword2;
int pos2 = 0;
void outcc(wordtree* p, ofstream& file, int num) {
	
	//cout << curword2 << endl;
	if (p->ccv)
		file << curword2 << ":" << p->ccv << endl;
	for (int i = 0; i < p->e; i++) {
		curword2.push_back(p->next[p->cache[i]]->letter);
		outcc(p->next[p->cache[i]], file, num);
	}
	
	curword2.pop_back();
}

void checknum(char* path) {
	//documentset = 0;
	long h_file;
	char* search = new char[512];
	char* filepath = new char[512];
	FILE_SEARCH file_search;
	sprintf(search, "%s/*.*", path);
	if ((h_file = _findfirst(search, &file_search)) == -1L) {
		return;
	}
	else {
		do {
			sprintf(filepath, "%s", file_search.name);
			if (!strcmp(filepath, "cc.dat") || !strcmp(filepath, ".") || !strcmp(filepath, ".."))
				continue;
			if (file_search.attrib & _A_SUBDIR) {

			}
			else {
				sprintf(filepath, "%s%s", path, file_search.name);
				documentset++;
			}
		} while (_findnext(h_file, &file_search) == 0);


		_findclose(h_file);
		h_file = _findfirst(search, &file_search);
		do {
			sprintf(filepath, "%s", file_search.name);
			if (!strcmp(filepath, ".") || !strcmp(filepath, ".."))
				continue;
			if (file_search.attrib & _A_SUBDIR) {

				sprintf(filepath, "%s%s/", path, file_search.name);
				checknum(filepath);
			}
		} while (_findnext(h_file, &file_search) == 0);
		_findclose(h_file);
	}

	delete[] search;
	delete[] filepath;
}

void findtf(char* path) {
	long h_file;
	char* search = new char[512];
	char* filepath = new char[512];
	int num = 0;
	FILE_SEARCH file_search;
	sprintf(search, "%s/*.*", path);
	if ((h_file = _findfirst(search, &file_search)) == -1L) {
		return;
	}
	else {				
		inittree(&dftree);
		do {
			sprintf(filepath, "%s", file_search.name);
			if (!strcmp(filepath, "cc.dat") || !strcmp(filepath, ".") || !strcmp(filepath, ".."))
				continue;
			if (file_search.attrib & _A_SUBDIR) {

			}
			else {
				num++;
				sprintf(filepath, "%s%s", path, file_search.name);
				calcval(filepath);
			}
		} while (_findnext(h_file, &file_search) == 0);
		char temp[256];
		sprintf(temp, "%scc.dat", path);
		ofstream oFile(temp);
		if(num)
			outcc(&dftree, oFile, num);
		oFile.close();

		_findclose(h_file);
		h_file = _findfirst(search, &file_search);
		do {
			sprintf(filepath, "%s", file_search.name);
			if (!strcmp(filepath, ".") || !strcmp(filepath, ".."))
				continue;
			if (file_search.attrib & _A_SUBDIR) {

				sprintf(filepath, "%s%s/", path, file_search.name);
				cout << filepath << endl;
				findtf(filepath);
			}
		} while (_findnext(h_file, &file_search) == 0);
		_findclose(h_file);
	}

	delete[] search;
	delete[] filepath;
}



void evaluate(string path) {
	ifstream iFile("D:/MCC/df.dat");
	char temp[128];
	while (iFile.getline(temp, 127)) {
		int k = 0;
		int i = 0;
		for (; temp[i] != ':'; i++);
		temp[i++] = 0;
		for (; temp[i]; i++)
			k = k * 10 + temp[i] - 48;
		addword(temp, k);
	}
	char pchr[128];
	strcpy(pchr, path.c_str());
	findtf(pchr);
}
int copy(char *exist, char *anew) {
	FILE *fexist, *fanew;
	char a;
	if ((fexist = fopen(exist, "rb")) == NULL)
		return -1;
	if ((fanew = fopen(anew, "wb")) == NULL){
		fclose(fexist);
		return -1;
	}
	while (1) {
		a = fgetc(fexist);
		if (!feof(fexist))
			fputc(a, fanew);
		else
			break;
	}
	fclose(fexist);
	fclose(fanew);
	
	return 0;
}
void copyfromold(char* path) {
	long h_file;
	char* search = new char[512];
	char* filepath = new char[512];
	FILE_SEARCH file_search;
	sprintf(search, "%s/*.*", path);
	if ((h_file = _findfirst(search, &file_search)) == -1L) {
		return;
	}
	else {
		inittree(&dftree);
		do {
			sprintf(filepath, "%s", file_search.name);
			if (!strcmp(filepath, ".") || !strcmp(filepath, ".."))
				continue;
			if (file_search.attrib & _A_SUBDIR) {
				sprintf(filepath, "%s%s/", path, file_search.name);
				copyfromold(filepath);
			}
			
		} while (_findnext(h_file, &file_search) == 0);
		_findclose(h_file);
		h_file = _findfirst(search, &file_search);
		do {
			sprintf(filepath, "%s", file_search.name);
			if (!strcmp(filepath, ".") || !strcmp(filepath, ".."))
				continue;
			if (file_search.attrib & _A_SUBDIR) {

			}
			else {
				if (!strcmp(filepath, "cc.dat")) {
					char t1[512];
					char t2[512];
					sprintf(t1, "%scc.dat", path);
					int k = 0;
					sprintf(t2, "D:/MCC/%s", &path[10]);
					for (; t2[k]; k++);
					t2[k - 1] = 0;
					sprintf(t2, "%s.dat", t2);
					copy(t1, t2);
				}
			}
		} while (_findnext(h_file, &file_search) == 0);
		_findclose(h_file);
	}

	delete[] search;
	delete[] filepath;
}

void copyfromall(char* path) {
	long h_file;
	char* search = new char[512];
	char* filepath = new char[512];
	FILE_SEARCH file_search;
	sprintf(search, "%s/*.*", path);
	if ((h_file = _findfirst(search, &file_search)) == -1L) {
		return;
	}
	else {
		inittree(&dftree);
		do {
			sprintf(filepath, "%s", file_search.name);
			if (!strcmp(filepath, ".") || !strcmp(filepath, ".."))
				continue;
			if (file_search.attrib & _A_SUBDIR) {
				sprintf(filepath, "%s%s/", path, file_search.name);
				copyfromall(filepath);
			}

		} while (_findnext(h_file, &file_search) == 0);
		_findclose(h_file);
		h_file = _findfirst(search, &file_search);
		do {
			sprintf(filepath, "%s", file_search.name);
			if (!strcmp(filepath, ".") || !strcmp(filepath, ".."))
				continue;
			if (file_search.attrib & _A_SUBDIR) {

			}
			else {
				if (!strcmp(filepath, "mcc.dat")) {
					char t1[512];
					char t2[512];
					sprintf(t1, "%smcc.dat", path);
					int k = 0;
					sprintf(t2, "D:/MCC/%smcc.dat", &path[10]);
					
					copy(t1, t2);
				}
			}
		} while (_findnext(h_file, &file_search) == 0);
		_findclose(h_file);
	}

	delete[] search;
	delete[] filepath;
}

double cclength2 = 0;
void addwordccv(char* a, double k) {
	wordtree* p = &dftree;
	for (int i = 0; a[i]; i++) {
		if (p->next[a[i] - 97])
			p = p->next[a[i] - 97];
		else {
			
			p->next[a[i] - 97] = new wordtree;
			p->cache[(p->e)++] = a[i] - 97;
			p = p->next[a[i] - 97];
			p->letter = a[i];
		}
	}
<<<<<<< HEAD
	p->ccv += k * log2(cclength2) / sqrt(cclength2);
	dlist.push_back(p);
=======
	
	p->ccv += k / sqrt(cclength2);
>>>>>>> origin/master
}

void readcc(const char path[]) {
    ifstream prep(path);
    char pretemp[128];
    while(prep.getline(pretemp,127)){
<<<<<<< HEAD
		int i = 0;
		while (pretemp[i++] != ':');
		double prevalue = strtod(pretemp+i, NULL);
=======
		int i = 1;
		while (pretemp[i++] != ':');
        double prevalue = strtod(pretemp+i, NULL);
>>>>>>> origin/master
        cclength2 += prevalue * prevalue;
		
    }
    prep.close();
    if (isnan(cclength2))
			cout << "stop" << endl;
    ifstream iFile(path);
	char temp[128];
    while(iFile.getline(temp, 127)){
<<<<<<< HEAD
         int i = 0;
        while (temp[i]!=':') {
            i++;
        }
        temp[i] = 0;
		double value = strtod(temp+i+1, NULL);
		addwordccv(temp, value);
=======
        int i = 0;
        while (temp[i]!=':') {
            i++;
        }
		temp[i++] = 0;
		double value = strtod(temp+i, NULL);
        
        addwordccv(temp, value);
>>>>>>> origin/master
    }
    iFile.close();
    
    
    cclength2=0;
}

void outmcc(wordtree* p, ofstream& file) {
	if (pos2 > 14) {
		pos2--;
		return;
	}
	//cout << curword2 << endl;
	if (p->tf)
		file << curword2 << ":" << log10((double)documentset / p->df) * p->tf << endl;
	for (int i = 0; i < 26; i++) {
		if (p->next[i]) {
			curword2[pos2++] = i + 97;
			//outmcc(p->next[i], file);
		}
	}
	curword2[pos2] = 0;
	if (pos2)
		pos2--;
}


void calmcc(char* path) {
	long h_file;
	char* search = new char[512];
	char* filepath = new char[512];
	FILE_SEARCH file_search;
	sprintf(search, "%s/*.*", path);
    int num=1;
	if ((h_file = _findfirst(search, &file_search)) == -1L) {
		return;
	}
	else {
		inittree(&dftree);
		do {
			sprintf(filepath, "%s", file_search.name);
			if (!strcmp(filepath, ".") || !strcmp(filepath, ".."))
				continue;
			if (file_search.attrib & _A_SUBDIR) {
				sprintf(filepath, "%s%s/", path, file_search.name);
				calmcc(filepath);
			}

		} while (_findnext(h_file, &file_search) == 0);
		_findclose(h_file);
        
        string tempstr = path;
        tempstr.pop_back();
        int pivot = tempstr.find_last_of('/');
        string prevpath = tempstr.substr(0,pivot);
        string curcat = tempstr.substr(pivot+1);
        
        string ccpath = prevpath + "/" + curcat + ".dat";
		char temp[512];
		strcpy(temp, ccpath.c_str());
        readcc(temp);
        
        
		h_file = _findfirst(search, &file_search);
		do {
			sprintf(filepath, "%s", file_search.name);
			if (!strcmp(filepath, "mcc.dat") || !strcmp(filepath, ".") || !strcmp(filepath, ".."))
				continue;
			if (file_search.attrib & _A_SUBDIR) {

			}
			else {
				sprintf(filepath, "%s%s", path, file_search.name);
                int i=0;
                while(filepath[i])
                    i++;
                filepath[i-4]=0;
                string newpath = filepath;
                newpath += "/mcc.dat";
                num++;
				strcpy(temp, newpath.c_str());
                readcc(temp);
			}
		} while (_findnext(h_file, &file_search) == 0);
		_findclose(h_file);
        ccpath = prevpath + "/" + curcat + "/mcc.dat";
<<<<<<< HEAD
        
        ofstream oFile(ccpath.c_str());
        outcc(&dftree, oFile, num);
=======
		if (num) {
			ofstream oFile(ccpath.c_str());
			outcc(&dftree, oFile, num);
			inittree(&dftree);
		}
>>>>>>> origin/master
	}

	delete[] search;
	delete[] filepath;
}

void main(int argc, char **argv) {
	//parsing();
	//taxonomy();
	//createdir();
	//parsing2();
	
	char path[100] = "D:/MCC/Top/";
	finddf(path);
	ofstream oFile("D:/MCC/df.dat");
	writedf(&root, oFile);
	oFile.close();
	
	/*
	checknum("D:/MCC/Top/");
	cout << documentset;
	*/
	//evaluate("D:/MCC/Top/"); //for test
	/*
	if (argc>1) {
		char* temp = argv[1];
		evaluate(temp);
	}
	else {
		FILE_SEARCH filesearch;
		long hfile;
		if ((hfile = _findfirst("D:/MCC/Top/*.*", &filesearch)) == -1L) {
			return;
		}
		else {
			do {
				string temp = filesearch.name;
				if (filesearch.attrib & _A_SUBDIR && temp!="." && temp!="..") {
					STARTUPINFO StartupInfo = { 0 };
					PROCESS_INFORMATION ProcessInfo;
					StartupInfo.cb = sizeof(STARTUPINFO);
					string temp2 = "D:/MCC/Top/" + temp + "/";
					string arg = "C:/Users/JH/Documents/GitHub/mcc/MCC/Release/MCC.exe " + temp2;
					char tchars[1024];
					strcpy(tchars,arg.c_str());
					CreateProcess("C:/Users/JH/Documents/GitHub/mcc/MCC/Release/MCC.exe",
						tchars, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo);
				}
			} while (_findnext(hfile, &filesearch) == 0);
		}
	}
	*/
	//copyfromold("D:/MCCold/Top/");
<<<<<<< HEAD
	//calmcc("D:/MCC/Top/"); //for test
	/*
	if (argc>1) {
		char* temp = argv[1];
		calmcc(temp);
	}
	else {
		FILE_SEARCH filesearch;
		long hfile;
		if ((hfile = _findfirst("D:/MCC/Top/*.*", &filesearch)) == -1L) {
			return;
		}
		else {
			do {
				string temp = filesearch.name;
				if (filesearch.attrib & _A_SUBDIR && temp != "." && temp != "..") {
					STARTUPINFO StartupInfo = { 0 };
					PROCESS_INFORMATION ProcessInfo;
					StartupInfo.cb = sizeof(STARTUPINFO);
					string temp2 = "D:/MCC/Top/" + temp + "/";
					string arg = "C:/Users/JH/Documents/GitHub/mcc/MCC/Release/MCC.exe " + temp2;
					char tchars[1024];
					strcpy(tchars, arg.c_str());
					CreateProcess("C:/Users/JH/Documents/GitHub/mcc/MCC/Release/MCC.exe",
						tchars, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo);
				}
			} while (_findnext(hfile, &filesearch) == 0);
		}
	}
	*/
	//copyfromall("D:/MCCall/Top/");
=======
	calmcc("D:/MCC/Top/");
>>>>>>> origin/master
}
