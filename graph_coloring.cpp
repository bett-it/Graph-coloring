
// Kolorowanie grafu zachłannie -> równolegle i sekwencyjnie 
// Beata Wiśniewska

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>


#define GRAPHS 1000
#define THREADS_NUMBER 8

using namespace std;


class Graph{
public:

    int size;
    std::vector<int> body;
    std::vector<int> colors;

    Graph(){}
    Graph(int size, std::string data){
        this->size = size;

        for(int i = 0; i < this->size * this->size; ++i){
            this->body.push_back(0);
        }

        for(int i = 0; i < data.size(); ++i){
            this->body[i] = (int)data[i] - '0';
        }

        this->colors.push_back(1);
    }
    int getEdge(int i) const { return this->body[i]; }
    int getEdge(int i, int j) const { return this->body[i*this->size + j]; }  
};


std::vector<Graph> graphList;

void fillGraphs(){
    std::fstream file;
    file.open("result2.txt", std::ios::in);

    if(!file.good()){
        std::cout << "error open file" << std::endl;

        file.close();
        return;
    }

    for(int graph_couter = 0; graph_couter < GRAPHS; graph_couter++ ){

        std::string buff;
        // read 1 blank line
        getline( file, buff );

        //read info
        getline( file, buff );
        std::stringstream ss(buff);
        std::stringstream ssgraph;

        int size;
        ss >> buff >> buff >> buff >> size;

        for(int i = 0; i < size; ++i){
            getline( file, buff );
            ssgraph << buff;
        }
        graphList.push_back(Graph(size, ssgraph.str()));
    }
}


void graph_coloring(int graph_index) 
{
  int row = graphList[graph_index].size;
  bool pom = true;

  for (int i=1; i<row; i++) {
    for(int x=1; x<=row; x++) {
      pom = true;
      for(int j=0; j<i; j++){
        if(graphList[graph_index].body[i*row+j] == 1 && graphList[graph_index].colors[j] == x){
          pom = false;
          break;
        }
      }
      if(pom){
        graphList[graph_index].colors.push_back(x);
        break;
      }
    }
  }
}

void sequentially(){

  for (int j=0; j<GRAPHS; j++)
  {
    graph_coloring(j);
  }
}

void parralel(){

  #pragma omp parallel shared(graphList) num_threads(4)
  {
    #pragma omp for 
    for (int j=0; j<GRAPHS; j++)
    {
      graph_coloring(j);
    }
    #pragma omp single
    {
      cout<<"liczba wątków: " << omp_get_num_threads() << endl;
    }
  }
}

int main(int argc, char** argv)
{
  // omp_set_num_threads(THREADS_NUMBER);
  fillGraphs();

  cout<<"\nKolorowanie start!\n";

  clock_t ppstart, ppstop;
	double pswtime, pewtime;

  pswtime = omp_get_wtime();
	ppstart = clock();
  sequentially();
  ppstop = clock();
	pewtime = omp_get_wtime();

  cout<< "Czas procesorów przetwarzania sekwencyjnego  %f sekund \n" << ((double)(ppstop - ppstart)/CLOCKS_PER_SEC) << endl;
	cout << "Czas trwania obliczen sekwencyjnego - wallclock %f sekund \n" <<  pewtime-pswtime << endl;

  for (int j=0; j<GRAPHS; j++)
  {
    graphList[j].colors.clear();
    graphList[j].colors.push_back(1);
  }

  pswtime = omp_get_wtime();
	ppstart = clock();
  parralel();
  ppstop = clock();
	pewtime = omp_get_wtime();

  cout<< "Czas procesorów przetwarzania równoleglego  %f sekund \n" << ((double)(ppstop - ppstart)/CLOCKS_PER_SEC) << endl;
	cout << "Czas trwania obliczen rownoleglych - wallclock %f sekund \n" <<  pewtime-pswtime << endl;  
  
  return 0;
}