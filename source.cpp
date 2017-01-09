typedef struct idftree{
	char a=0;
	int value=0;
	idftree* next[26]=0;
}

idftree tree;

void evaluate(){
    ifstream iFile("D:/MCC/idf");
    char temp[128];
    int k=0;
    iFile.getline(temp,127);
    int i=0;
    idftree* ptree = &tree;
    
    for(;temp[i]!=':';i++){
        if(ptree)
    }
    temp[i++]=0;
    for(;temp[i];i++){
        k=k*10 + temp[i]-48;
    }
    
}
