typedef struct idftree{
	char a=0;
	int value=0;
	idftree* next=0;
}


void evaluate(){
  ifstream iFile("D:/MCC/idf");
  char temp[128];
  int k;
  iFile.getline(temp,127);
  int i=0;
  while(temp[i]!=':'){
    i++;
  }
  temp[i++]=0;
  for(;temp[i];i++){
    k=temp[i]-
  }
}
