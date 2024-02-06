#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"
#include "quicksort.h"
#include "prim.h"
#include "main.h" // WARN: Needed for functions used before they are defined like permut()

int n = 0;         // Number of vertices
int iseed = 1;     // Seed used to initialize the random number generator
int** cost;        // cost[i][j] = cost[j][i] = cost of edge {i,j}
long int nbCalls = 0; // Number of calls to the recursive permut function
FILE* fd;          // File descriptor of the Python script
int bestCost = INT_MAX; // Best cost found so far

// CLI flags
bool generatePython = false;
bool verbose = false;


int nextRand(int n) {
    // Postcondition: return an integer value in [0,n-1], according to a pseudo-random sequence which is initialized with iseed
    int i = 16807 * (iseed % 127773) - 2836 * (iseed / 127773);
    if (i > 0) iseed = i;
    else iseed = 2147483647 + i;
    return iseed % n;
}

int** createCost(int n) {
    // input: the number n of vertices
    // return a symmetrical cost matrix such that, for each i,j in [0,n-1], cost[i][j] = cost of arc (i,j)
    int x[n], y[n];
    int max = 20000;

    for (int i=0; i<n; i++){
        x[i] = nextRand(max);
        y[i] = nextRand(max);
    }

    // allocate memory for cost matrix
    int** cost = (int**) malloc(n*sizeof(int*));
    for (int i=0; i<n; i++) cost[i] = (int*)malloc(n*sizeof(int));

    // init python script
    if (generatePython) {
        fprintf(fd, "import turtle\n\n");
        fprintf(fd, "from utils import mark_point\n\n");
        fprintf(fd, "def draw_path_permutations(turtle_speed):\n");
        fprintf(fd, "    # Configuration initiale\n");
        fprintf(fd, "    turtle.setworldcoordinates(0, 0, %d, %d)\n", max, max+100);
    
        // Generate n random points in python turtle space
        for (int i=0; i<n; i++) {
            x[i] = nextRand(max);
            y[i] = nextRand(max);
            fprintf(fd, "    p%d=(%d,%d)\n", i, x[i], y[i]);
            cost[i] = (int*)malloc(n*sizeof(int));
        }
        fprintf(fd, "\n\n");
        fprintf(fd, "    # Affichage des points\n");
        fprintf(fd, "    def mark_all_points():\n");
        for (int i=0; i<n; i++) fprintf(fd, "        mark_point(p%d)\n", i);
        fprintf(fd, "\n");
        fprintf(fd, "    turtle.speed(turtle_speed)\n");
        fprintf(fd, "    mark_all_points()\n\n");
        fprintf(fd, "    wait = input(\"Enter return to start\")\n\n");
    }

    // create cost matrix
    for (int i=0; i<n; i++) {
        cost[i][i] = max*max;
        for (int j=i+1; j<n; j++) {
            cost[i][j] = sqrt((x[i]-x[j])*(x[i]-x[j]) + (y[i]-y[j])*(y[i]-y[j]));
            cost[j][i] = cost[i][j];
        }
    }

    return cost;
}

/**
 * @brief Generate python code for the tour associated with the given permutation.
 * 
 * @param visited Array of visited vertices
 * @param n Number of vertices
 * 
*/
void genPythonTurtleTour(int visited[], int n) {
    // input: n = number n of vertices; sol[0..n-1] = permutation of [0,n-1]; fd = file descriptor
    // side effect: print in fd the Python script for displaying the tour associated with sol
    fprintf(fd, "    # path permutation\n");
    fprintf(fd, "    turtle.clear()\n");
    fprintf(fd, "    mark_all_points()\n");
    fprintf(fd, "    turtle.penup()\n");
    fprintf(fd, "    turtle.goto(p%d)\n", visited[0]);
    fprintf(fd, "    turtle.pendown()\n");
    for (int i=1; i<n; i++) fprintf(fd, "    turtle.goto(p%d)\n", visited[i]);
    fprintf(fd, "    turtle.goto(p%d)\n", visited[0]);
    fprintf(fd, "    turtle.update()\n");
    fprintf(fd, "    wait = input(\"Enter return to continue\")\n\n");
}

void permut(int visited[], int nbVisited, int notVisited[], int nbNotVisited){
    /*
     Input:
     - visited[0..nbVisited-1] = visited vertices
     - notVisited[0..nbNotVisited-1] = unvisited vertices
     Precondition:
     - nbVisited > 0 and visited[0] = 0 (the tour always starts with vertex 0)
     Postcondition: print all tours starting with visited[0..nbVisited-1] and ending with vertices of notVisited[0..nbNotVisited-1] (in any order), followed by 0
     */
    nbCalls++;
    // INSERT YOUR CODE HERE!
}

int main(int argc, char *argv[]) {
    n = getInputNumberOfVertices(argc, argv);
    generatePython = getGeneratePythonFlag(argc, argv);
    verbose = getVerboseFlag(argc, argv);
    bool run_tests = getRunTestsFlag(argc, argv);

    if (run_tests) {
        // Add tests here
        return 0;
    }

    if (generatePython)
        fd  = fopen("python/generated.py", "w");
    
    int** costMatrix = createCost(n);
    if (verbose) {
        printf("Cost matrix:\n");
        for (int i = 0; i < n; i++) {
            printf("[");
            for (int j = 0; j < n; j++) {
                printf("%d, ", costMatrix[i][j]);
            }
            printf("]\n");
        }
    }

    // initializations
    clock_t t = clock();
    int visited[n]; // can contain all vertices
    int notVisited[n-1]; // can contain all vertices except 0
    visited[0] = 0;
    for (int i = 0; i < (n-1); i++)
        notVisited[i] = i+1;
    
    permut(visited, 1, notVisited, n-1);

    // print results
    char * bestCostFormatted = formatNumber(bestCost);
    char * nbCallsFormatted = formatNumber(nbCalls);
    printf(
        "n = %d; bestCost = %s; nbCalls = %s; time = %.3fs\n",
        n, 
        bestCostFormatted,
        nbCallsFormatted,
        ((double) (clock() - t)) / CLOCKS_PER_SEC
    );
    free(bestCostFormatted);
    free(nbCallsFormatted);
    
    // clean up
    if (generatePython)
        fclose(fd);

    // clean up cost matrix
    for (int i = 0; i < n; i++) free(costMatrix[i]);
    free (costMatrix);

    return 0;
}
