#include <iostream>
#include <pthread.h>
#include <cstdlib> //int rand()
#include <stdlib.h>

using namespace std;

//Ilayda Ademoglu(23856) first cs307 homework about threads.

int mymatrix[2][50];
int turn = 0;
bool isfull = true;

void printmatrix(){ //This function is for printing matrixes
for(int i = 0; i < 2; i++){
          for(int j = 0; j< 50; j++){
          cout << mymatrix[i][j] << " ";
      }
      cout << endl;
  }

}

int randnumgenerateforx(){ //generates a number number for x
    int randomnumforx;
    
    randomnumforx = rand() % 2;
    
    return randomnumforx;
}


int randnumgeneratefory(){ //generates a random number for y
    int randomnumfory;
    randomnumfory = rand() % 50;
    return randomnumfory;
}

bool matrixfull(){ //it tells whether matrix is full or not
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 50; j++){
            if(mymatrix[i][j] == 0)
                return false;
        }
    }
    return true;
}

void *agency1(void *param)
{
       int * tid = (int *) param;
               while(true)
               {
                  if(isfull == 0){
                        break;
                    }
                   
                  
                   while(turn !=0) {};
                   int x = randnumgenerateforx();
                   int y = randnumgeneratefory();
                   if(mymatrix[x][y] == 0)
                       mymatrix[x][y] = *tid;
                   else{
                       do{
                           x = randnumgenerateforx();
                           y = randnumgeneratefory();
                       }while(mymatrix[x][y]!= 0);
                       mymatrix[x][y] = *tid;
                   }
                   int seatnumber = 0;
                   if(x == 0)
                       seatnumber = y + y + 1;
                   else if(x == 1)
                       seatnumber = 2 * y + 2;
                   cout << "Seat Number " << seatnumber << " is reserved by TravelAgencyTA1" << endl;
                   turn = 1;
                  // if(isfull == 0)
                   //     pthread_exit(NULL);
                                   //turn = 1;
                          
               }
 
    pthread_exit(NULL);

           
}

void *agency2(void *param)
{
      int * tid = (int *) param;
                     while(true)
                     {
                         if(isfull == 0){
                             break;
                         }
                        
                         while(turn !=1) {};
                         int x = randnumgenerateforx();
                         int y = randnumgeneratefory();
                         if(mymatrix[x][y] == 0)
                             mymatrix[x][y] = *tid;
                         else{
                             do{
                                 x = randnumgenerateforx();
                                 y = randnumgeneratefory();
                             }while(mymatrix[x][y]!= 0);
                             mymatrix[x][y] = *tid;
                         }
                         int seatnumber = 0;
                         if(x == 0)
                             seatnumber = y + y + 1;
                         else if(x == 1)
                             seatnumber = 2 * y + 2;
                         cout << "Seat Number " << seatnumber << " is reserved by TravelAgencyTA2" << endl;
                         turn = 2;
                        // if(isfull == 0)
                         //     pthread_exit(NULL);
                                         //turn = 1;
                                
                     }
          pthread_exit(NULL);
}

void *agency3(void *param)
{
      int * tid = (int *) param;
                    while(true)
                    {
                       if(isfull == 0)
                           break;
                        while(turn !=2) {};
                        int x = randnumgenerateforx();
                        int y = randnumgeneratefory();
                        if(mymatrix[x][y] == 0)
                            mymatrix[x][y] = *tid;
                        else{
                            do{
                                x = randnumgenerateforx();
                                y = randnumgeneratefory();
                            }while(mymatrix[x][y]!= 0);
                            mymatrix[x][y] = *tid;
                        }
                        int seatnumber = 0;
                        if(x == 0)
                            seatnumber = y + y + 1;
                        else if(x == 1)
                            seatnumber = 2 * y + 2;
                        cout << "Seat Number " << seatnumber << " is reserved by TravelAgencyTA3" << endl;
                        turn = 0;
                     /*   if(isfull == 0)
                             pthread_exit(NULL); */
                                        //turn = 1;
                               
                    }
    pthread_exit(NULL);
      
}

int main(){
  srand((int)time(NULL));

    pthread_t TravelAgency1;
    pthread_t TravelAgency2;
    pthread_t TravelAgency3;
    
    int i = 1;
    int j = 2;
    int k = 3;
    
    pthread_create(&TravelAgency1,NULL,agency1,(void*) &i);
    pthread_create(&TravelAgency2,NULL,agency2,(void*) &j);
    pthread_create(&TravelAgency3,NULL,agency3,(void*) &k);
    
    
  /*  pthread_join(TravelAgency1, NULL);
    pthread_join(TravelAgency2, NULL);
    pthread_join(TravelAgency3, NULL); */
     
    while(isfull == 1){
        if(matrixfull() == true){
            isfull = 0;
            printmatrix();
            break;
            pthread_join(TravelAgency1, NULL);
            pthread_join(TravelAgency2, NULL);
            pthread_join(TravelAgency3, NULL);
        }
   
    }
    exit(EXIT_SUCCESS);
    return 0;
  
}
