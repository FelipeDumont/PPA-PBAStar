#include "AStar.h"

#include <iostream>
#include <string>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// A*
#include <unordered_set>
#include <queue>
#include <cmath>
#include <stack>
#include <semaphore.h>
#include <mutex>
#include <algorithm>
#include <vector>
#include <bits/stdc++.h>

using namespace std;



// General data
bool octoMove = false;
bool debug = false;
sem_t semaphore;
mutex* mtxVisited;
mutex* mtxSolution;

typedef struct halfdata{
	int* arr;
	int thread_num;

} halfdata;

void* halfSum(void* p){

	halfdata *ptr = (halfdata*)p;
	int n = ptr->thread_num;
	int* thread_sum = (int*) calloc(1,sizeof(int));
	
	if(n == 0){
		for(int i = 0; i < 10/2;i++)
			thread_sum[0] = thread_sum[0] + ptr->arr[i];
	}
	else{
		for(int i = 10/2; i < 10;i++)
			thread_sum[0] = thread_sum[0] + ptr->arr[i];
	}
	pthread_exit(thread_sum);
}

extern "C"{

	int EXPORT SimpleTypeReturnFun(){
		return 10;
	}

	int EXPORT SimpleTypeArgFun(int n){
		return n * 2;
	}

	void EXPORT ReferenceArgumentFun(int &n){
		n = 25;
	}

	void EXPORT SimpleCallBackFun(SimpleCallback c){
		c();
	}

	int EXPORT SimpleReturnCallbackFun(SimpleReturnCallback c){
		return c();
	}

	void EXPORT SimpleArgCallbackFun(SimpleArgCallback c){
		return c(6);
	}
	
	int EXPORT TestSum(){
	    // Declare integer array [1,2,3,4,5,6,7,8,9,10]:
	    int* int_arr = (int*) calloc(10, sizeof(int));
	    for(int i = 0; i < 10; i++)
		int_arr[i] = i + 1;
	    
	    // Declare arguments for both threads:
	    halfdata thread_data[2];
	    thread_data[0].thread_num = 0;
	    thread_data[0].arr = int_arr;
	    thread_data[1].thread_num = 1;
	    thread_data[1].arr = int_arr;
	    
	    // Declare thread IDs:
	    pthread_t tid[2];
	    
	    // Start both threads:
	    pthread_create(&tid[0], NULL, halfSum, &thread_data[0]);
	    pthread_create(&tid[1], NULL, halfSum, &thread_data[1]);
	    
	    // Declare space for sum:
	    int* sum0;
	    int* sum1;
	    
	    // Retrieve sum of threads:
	    pthread_join(tid[0], (void**)&sum0);
	    pthread_join(tid[1], (void**)&sum1);
	    
	    // printf("Sum of whole array = %i\n", *sum0 + *sum1);
	    
	    return *sum0 + *sum1;
	}
}



typedef struct Coordinate {
  int x, y;
  Coordinate(int x, int y) : x(x), y(y) {}
  bool operator==(const Coordinate& other) const {
    return x == other.x && y == other.y;
  }
} Coordinate;

struct HashCoordinate {
  size_t operator()(const Coordinate& c) const {
    return hash<int>()(c.x) ^ hash<int>()(c.y);
  }
};

typedef struct searchdata{
	bool* map;
	Coordinate* start;
	Coordinate* end;
	int threadId;
	int nSize;
	int mode;

} searchdata;

struct Node
{
//public:
	Coordinate coord;
    int g, h;
	int parentId;
	Node(Coordinate coord, int g, int h, int parentId) : coord(coord), g(g), h(h), parentId(parentId) {}
	int f() const { return g + h; }
	bool operator==(Node const& other) const{
		return other.coord.x == coord.x && other.coord.y == coord.y;
	}
};


struct CompareNode {
  bool operator()(const Node& n1, const Node& n2) const {
    return n1.f() > n2.f();
  }
};

int heuristic(Coordinate start, Coordinate end) {
  // Euclidean distance
  return sqrt(pow(start.x - end.x, 2) + pow(start.y - end.y, 2));
}

typedef struct solution{
	vector<Coordinate> path;
	solution(vector<Coordinate> path) : path(path){}
	solution(){}
} solution;

solution* solutions;
set<int>* sharedVisitedNodes;

solution search(Coordinate start, Coordinate end, bool map[], int nSize, int threadMode, int threadId) {
	unordered_set<Coordinate, HashCoordinate> visited;
  	priority_queue<Node, vector<Node>, CompareNode> open;
  	vector<Node> visitedNode;
  	open.emplace(start, 0, heuristic(start, end), -1);
  	visited.insert(start);
  	int visitedNodeId = 0;

	while (!open.empty()) {
    	Node current = open.top();
	    open.pop();
	    visitedNode.push_back(current);
	    
	    if(threadMode == 1){
	    	// cout << "Thead mode ???" << threadMode << " " <<  threadId << endl;
	    	mtxVisited[threadId].lock();
	    	int cPosition = current.coord.x + current.coord.y*nSize;
	    	
	    	bool found = (sharedVisitedNodes[threadId].count(cPosition) == 1);
	    	
	    	if(found) {
	    		// cout << "found " << current.coord.x << "," << current.coord.y << endl;
	    		mtxVisited[threadId].unlock();
			    
		        vector<Coordinate> path;
		        path.reserve(nSize*nSize);
		        while (current.parentId != -1) {
		          path.push_back(current.coord);
		          current = visitedNode[current.parentId];
		        }
		      
		        path.push_back(start);
		        solution sol(path);
		        // cout << "total visited nodes " << visitedNodeId << endl;
		        return sol;
			      
			    
	    	}
	    	else{
	    		sharedVisitedNodes[threadId].insert(cPosition);
	    	}
	    	
	    	mtxVisited[threadId].unlock();

	    }
	    
		
	    if (current.coord == end) {
	      // Found the goal
	      vector<Coordinate> path;
	      path.reserve(nSize*nSize);
	      
	      while (current.parentId != -1) {
	        path.push_back(current.coord);
	        current = visitedNode[current.parentId];
	      }
	      
	      path.push_back(start);
	      // path.shrink_to_fit();
	      solution sol(path);
	      // cout << "total visited nodes " << visitedNodeId << endl;
	      return sol;
    }// Returned the path

    // Add neighbors to the open list
    vector<Coordinate> neighbors = {
      Coordinate(current.coord.x + 1, current.coord.y),
      Coordinate(current.coord.x, current.coord.y + 1),
      Coordinate(current.coord.x - 1, current.coord.y),
      Coordinate(current.coord.x, current.coord.y - 1)
    };
    if(octoMove){
    	neighbors.push_back(Coordinate(current.coord.x + 1, current.coord.y + 1));
    	neighbors.push_back(Coordinate(current.coord.x - 1, current.coord.y + 1));
    	neighbors.push_back(Coordinate(current.coord.x + 1, current.coord.y - 1));
    	neighbors.push_back(Coordinate(current.coord.x - 1, current.coord.y - 1));
    }

    for (Coordinate next : neighbors) {
    	int lPosition = next.x + next.y*nSize;
      	if (visited.count(next) == 0 && map[lPosition] == false && next.x >= 0 && next.x < nSize && next.y >= 0 && next.y < nSize) {
      	
	      	// next, current.g + 1, heuristic(next, end)
	      	Node newNode = Node(next,current.g + 1, heuristic(next, end),visitedNodeId);

	        open.emplace(newNode);
	        visited.insert(next);

	        Node top = open.top();
	        int pid = top.parentId;
      	}
    }
    visitedNodeId++;
  }
  // cout << "total visited nodes " << visitedNodeId << endl;

  // Did not find the goal
  vector<Coordinate> empty_path;
  solution sol(empty_path);
  return sol;
}

// Normal Thread 1
void* FindPathThread(void* data){

	searchdata* sdata = (searchdata*)data;
	solution sol = search(*sdata->start, *sdata->end, sdata->map, sdata->nSize, 0,0);
	solutions[sdata->threadId] = sol;
	sem_post(&semaphore);
	pthread_exit(NULL);
}

// Halve move from thread 1 origin to end
void* FindPathThreadFrom(void* data){

	searchdata* sdata = (searchdata*)data;
	int threadId = sdata->threadId;
	// cout << "AHHHHHH "<< sdata->threadId << endl;
	solution sol = search(*sdata->start, *sdata->end, sdata->map, sdata->nSize, 1, threadId);
	// Always recieve this one, with the full or halve route
	// MUTEX

	
	mtxSolution[threadId].lock();
	
	bool thereIsSolution = solutions[threadId].path.size() > 0;
	
	if(thereIsSolution){
		solutions[threadId].path.pop_back();
		int initialPosition = sol.path.size() - 3 < 0 ? 0 : sol.path.size() - 3;
		for (int i = 0; i < sol.path.size(); ++i)
		{
			if(thereIsSolution){
				auto it = find(solutions[threadId].path.begin(), solutions[threadId].path.end(), sol.path[0]);
				int size = solutions[threadId].path.size();
				int index = it - solutions[threadId].path.begin();
				if(it != solutions[threadId].path.end()){
					for (; index < size; ++index)
					{
						solutions[threadId].path.pop_back();
					}
				}
			}
			else{
				solutions[threadId].path.push_back(sol.path[i]);
			}
		}
		solutions[threadId].path.insert(solutions[threadId].path.end(), sol.path.begin(), sol.path.end());
	}
	else{
		solutions[threadId] = sol.path;
	}
	
	mtxSolution[threadId].unlock();
	

	sem_post(&semaphore);
	pthread_exit(NULL);
}

// Halve move from thread 1 end to origin
void* FindPathThreadTo(void* data){

	searchdata* sdata = (searchdata*)data;
	int threadId = sdata->threadId;
	// cout << "AHHHHHH " << sdata->threadId << endl;
	solution sol = search(*sdata->end, *sdata->start, sdata->map, sdata->nSize, 1, threadId);
	// Always recieve this one, with the empty or halve route
	// Just add them to the solution !
	vector<Coordinate> toSolution;

	
	// MUTEX
	mtxSolution[threadId].lock();
	
	reverse(solutions[threadId].path.begin(),solutions[threadId].path.end());
	bool thereIsSolution = solutions[threadId].path.size() > 0;
	
	if(thereIsSolution){
		solutions[threadId].path.pop_back();
		int initialPosition = sol.path.size() - 3 < 0 ? 0 : sol.path.size() - 3;
		for (int i = 0; i < sol.path.size(); ++i)
		{
			if(thereIsSolution){
				auto it = find(solutions[threadId].path.begin(), solutions[threadId].path.end(), sol.path[0]);
				int size = solutions[threadId].path.size();
				int index = it - solutions[threadId].path.begin();
				if(it != solutions[threadId].path.end()){
					for (; index < size; ++index)
					{
						solutions[threadId].path.pop_back();
					}
				}
			}
			else{
				solutions[threadId].path.push_back(sol.path[i]);
			}
		}
		solutions[threadId].path.insert(solutions[threadId].path.end(), sol.path.begin(), sol.path.end());
	}
	else{
		solutions[threadId] = sol.path;
	}
	reverse(solutions[threadId].path.begin(),solutions[threadId].path.end());

	
	mtxSolution[threadId].unlock();
	
	sem_post(&semaphore);
	pthread_exit(NULL);
}



int main(int argc, char *argv[]) {
	int threadMode = atoi(argv[1]);// 0 Linear, 1 MultiThreads, 2 DualStartThreads, 3 DualStartThreads Multi
	int totalThreads = atoi(argv[2]);// from 1 to n individuals
	int nSize = atoi(argv[3]);
	int individuals = atoi(argv[4]);
	int mapType = atoi(argv[5]);// 0 default map, 1 second map
	int usedebugPath = atoi(argv[6]);// 0 false, 1 true
	debug  = usedebugPath == 1;
	solutions = (solution *)malloc(individuals*sizeof(solution));


	int nnSize = nSize*nSize;
	bool map[nnSize];

	for (int i = 0; i < nnSize; ++i)
	{
		map[i] = false;
	}

	// Map settings
	if(mapType == 1){
		for (int i = 0; i < nSize/2; ++i)
		{
			map[i];
		}
	}

	// cout << "Finished ... map" << endl;
	// N individual means n positions
	// The filling of those position will follow the next order
	// 								*
	// 				*		*		*
	// * 	* * 	* * 	* * *	* * *
	// TODO: from 0 0 to 1 1 and so on for now 
	vector<Coordinate> initialPositions;
	for (int i = 0; i < individuals; ++i)
	{
		// Set coordenates
		int x = 0 + (i%2 == 1? (i + 1)/2 : 0);
		int y = 0 + (i%2 == 0? (i + 1)/2 : 0);

  		Coordinate start(x, y);
  		initialPositions.push_back(start);
	}
	
  	// Always the same ending position !
  	Coordinate end(nSize-1, nSize-1);


  	// Linear
  	if(threadMode == 0){
  		for (int i = 0; i < individuals; ++i)
  		{
	  		if(debug) cout << "Linear" << " from ["<<initialPositions[i].x << ","<< initialPositions[i].y << "] to [" << nSize-1 << "," << nSize-1 << "]"<< endl;
	  		solution soln = search(initialPositions[i], end, map, nSize, 0,0);
	  		solutions[i] = soln;
  		}
  		if(debug) cout << "End Linear" << endl;
  	}
  	// Threads + linear
  	if(threadMode == 1){
  		// Initialice the semaphore with the is shared ? / max
  		sem_init(&semaphore,0,totalThreads);

  		// Prepare the data !
  		searchdata thread_data [individuals];
  		for (int i = 0; i < individuals; ++i)
  		{
  			thread_data[i].map = map;
	  		thread_data[i].start = &initialPositions[i];
	  		thread_data[i].end = &end;
	  		thread_data[i].nSize = nSize;
	  		thread_data[i].threadId = i;
	  		thread_data[i].mode = 0;
  		}

  		// Threads !!!
		pthread_t tid[individuals];

  		for (int i = 0; i < individuals; ++i)
  		{
  			sem_wait(&semaphore);
  			pthread_create(&tid[i], NULL, FindPathThread, &thread_data[i]);
	  		if(debug) cout << "Thread" << " from ["<<initialPositions[i].x << ","<< initialPositions[i].y << "] to [" << nSize-1 << "," << nSize-1 << "]"<< endl;
  		}

  		for (int i = 0; i < individuals; ++i)
  		{
  			pthread_join(tid[i], NULL);
  		}
	    
	    if(debug) cout << "End Thread" << endl;
	    
  	}

  	if(threadMode >= 2){// 1 or N
  		// We go in two by two ..., so we will use only the halve
  		sem_init(&semaphore,0,totalThreads);

  		for (int i = 0; i < individuals; ++i)
  		{
  			vector<Coordinate> empty;
  			solutions[i].path = empty;
  		}
  		mtxVisited = (mutex *)malloc(individuals*sizeof(mutex));
  		mtxSolution = (mutex *)malloc(individuals*sizeof(mutex));
  		sharedVisitedNodes = (set<int>*)malloc(individuals*sizeof(set<int>));

  		// Prepare the data !
  		searchdata thread_data [individuals];
  		for (int i = 0; i < individuals; ++i)
  		{
  			thread_data[i].map = map;
	  		thread_data[i].start = &initialPositions[i];
	  		thread_data[i].end = &end;
	  		thread_data[i].nSize = nSize;
	  		thread_data[i].threadId = i;
	  		thread_data[i].mode = 1;
	  		set<int> empty;
	  		sharedVisitedNodes[i] = empty;
  		}

  		// Threads we will go two by two ...
		pthread_t tid[individuals * 2];

		for (int i = 0; i < individuals; ++i)
  		{
  			
  			sem_wait(&semaphore);
  			pthread_create(&tid[(i*2)], NULL, FindPathThreadFrom, &thread_data[i]);
  			if(totalThreads > 1){
  				// cout << "hello world ?" << endl;
	  			sem_wait(&semaphore);
	  			pthread_create(&tid[(i*2) + 1], NULL, FindPathThreadTo, &thread_data[i]);	
  			}
	  		if(debug) cout << "Thread" << " from ["<<initialPositions[i].x << ","<< initialPositions[i].y << "] to [" << nSize-1 << "," << nSize-1 << "]"<< endl;
	  		
  		}

  		for (int i = 0; i < individuals; ++i)
  		{
  			pthread_join(tid[(i*2)], NULL);
  			if(totalThreads > 1){
  				pthread_join(tid[(i*2)+1], NULL);
  			}
  		}

  		
		// END
  		free(mtxVisited);
  		free(mtxSolution);
  		free(sharedVisitedNodes);
  	}

  	if(debug){
  		
  		for (int nSolutions = 0; nSolutions < individuals; ++nSolutions)
  		{
		  	cout << solutions[nSolutions].path.size()  << "   " << nSolutions << "/" << individuals-1<< endl;
		  	int size = solutions[nSolutions].path.size();
		  	// Finally print the solutions ...
		  	for (int i = 0; i < size; ++i)
		  	{
		  		Coordinate c = solutions[nSolutions].path[i];
		  		cout << "["  << c.x << "," << c.y << "] ";
		  	}
		  	cout << endl;
	  	}
	  	
  	}

  	free(solutions);
  	return 0;
}