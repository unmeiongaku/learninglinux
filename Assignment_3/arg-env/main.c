#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]){
    int i;
    if(argc!=2){
        printf("\n[%s:%s:%d] ==>> ", __FILE__, __func__, __LINE__);
        printf("./app /path/\n");
    }
    //In ra so luong commmand-line truyền vào
    printf("Number Commandlines: %d\n",argc);
    //In ra nội dung commandline
    for(i=0;i<argc;i++){
      printf("argv[%d]: %s \n",i+1,argv[i]);
    }
    return 0;
}

