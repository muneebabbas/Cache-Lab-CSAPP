#include "parallel_trans.c"
#include <fstream>
#include <string.h>
#include <ctime>
#include <assert.h>
#define NUM_TEST_MATRICES 9
int test_val[NUM_TEST_MATRICES];
double base[NUM_TEST_MATRICES];

void calculate_speedup( void (*transpose_function)(int ,int,int**,int**), const char *fname)
{
	int **A,**B;
	ofstream fileL;
	fileL.open(fname);
	// if (fileL == NULL)
		// cout << "Error opening file\n";
	fileL<<fname<<"\n";
	fileL<<"size \t time\n";
	//for each test value
	for(int i=0;i<NUM_TEST_MATRICES;++i){

		alloc(test_val[i],test_val[i],A);
		alloc(test_val[i], test_val[i],B);
		init(test_val[i], test_val[i], A);

		clock_t start;
		double diff;
	    start = clock();

		for( int j=0;j<10;++j)
		{
			
			transpose_function(test_val[i],test_val[i],A,B);
			assert(is_transpose(test_val[i], test_val[i], A, B));
			
		}	
		diff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
		diff=diff/(double)10;
		fileL<<test_val[i]<<'\t'<<base[i]/diff<<"\n";
		dealloc(test_val[i],test_val[i],A);
		dealloc(test_val[i], test_val[i], B);
	}
	std::cout<<'\n';
}

int main()
{
	int **A, **B ;
	for(int i=0;i<NUM_TEST_MATRICES;++i)
	{
		test_val[i]=16*(1<<i);
	//	std::cout<<test_val[i];
	}


	for(int i=0;i<NUM_TEST_MATRICES;++i){

		alloc(test_val[i],test_val[i],A);
		alloc(test_val[i], test_val[i],B);
		init(test_val[i], test_val[i], A);

		clock_t start;
		double diff;
	    start = clock();

		for( int j=0;j<50;++j)
		{
			
			trans(test_val[i],test_val[i],A,B);	
		}

		diff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
		diff=diff/(double)50;
		base[i] = diff;
		std::cout<<test_val[i]<<'\t'<<diff<<"\n";
		dealloc(test_val[i],test_val[i],A);
		dealloc(test_val[i], test_val[i], B);
	}


	//MatrixOperations::init_matrices(A,B,64);

	//pass your transpose ,matrix initialization and deallocation functions here.
	//Function Prototypes
	//transpose and deallocate : void transpose (int,int,int**,int**)
	//init					: void init (int,int,int**&,int**&)
    // print_function_times(trans, "Simple Transpose");
    calculate_speedup(transpose_submit,"Blocked transpose");
    calculate_speedup(parallel_trans_odd_even,"Parallel transpose Odd Even");
    calculate_speedup(parallel_trans_2, "Parallel Transpose 2 threads");
    calculate_speedup(parallel_trans_4,"Parallel Transpose 4 threads");

}