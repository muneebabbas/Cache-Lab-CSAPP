#include <iostream>
#include <thread>
using namespace std;

void trans_helper_32(int start_col, int jump, int M, int N, int**A, int**B);
void trans_helper_64(int start_col, int jump, int M, int N, int**A, int**B);
void trans(int M, int N, int** A, int** B);


//---------------------------------------------------------------
// one thread handles odd rows and another handles even rows
//---------------------------------------------------------------

void parallel_trans_odd_even(int M, int N, int** A, int** B)
{
	std::thread oddThread([M, N, A, B]() {
		for(int i = 1; i < M; i+=2)
			for (int j = 0; j < M; j++)
			{
				B[i][j] = A[j][i];
			}
	});

	std::thread evenThread([M, N, A, B](){
		for(int i = 0; i < M; i+=2)
			for (int j = 0; j < M; j++)
			{
				B[i][j] = A[j][i];
			}
	});

	oddThread.join();
	evenThread.join();

}

//---------------------------------------------------------------
// Best optimization for two threads
// Number 8 by 8 blocks going horizontal, each thread works on 
// alternate block
//---------------------------------------------------------------

void parallel_trans_2(int M, int N, int** A, int** B)
{
	if (M == 32 && N == 32)
	{
		std::thread t1(trans_helper_32, 0, 16, M, N, A, B);
		std::thread t2(trans_helper_32, 8, 16, M, N, A, B);
		t1.join();
		t2.join();
	}
	else if(M == N)
	{
		std::thread t1(trans_helper_64, 0, 16, M, N, A, B);
		std::thread t2(trans_helper_64, 8, 16, M, N, A, B);
		t1.join();
		t2.join();
	}
	else
	{

	}


}

//---------------------------------------------------------------
// Best optimization for four threads
// Each thread will work on a block and jump four blocks 
// ahead
//---------------------------------------------------------------

void parallel_trans_4(int M, int N, int** A, int** B)
{
	if (M == 32 && N == 32)
	{
		std::thread t1(trans_helper_32, 0, 32, M, N, A, B);
		std::thread t2(trans_helper_32, 8, 32, M, N, A, B);
		std::thread t3(trans_helper_32, 16, 32, M, N, A, B);
		std::thread t4(trans_helper_32, 24, 32, M, N, A, B);

		t1.join();
		t2.join();
		t3.join();
		t4.join();

	}
	else if(M == N)
	{
		std::thread t1(trans_helper_64, 0, 32, M, N, A, B);
		std::thread t2(trans_helper_64, 8, 32, M, N, A, B);
		std::thread t3(trans_helper_64, 16, 32, M, N, A, B);
		std::thread t4(trans_helper_64, 24, 32, M, N, A, B);

		t1.join();
		t2.join();
		t3.join();
		t4.join();
	}
	else
	{

	}
}


//---------------------------------------------------------------
// Helper function for 32x32. number of columns to jumps is 
// provided as an argument along with the column to start
//---------------------------------------------------------------

void trans_helper_32(int start_col, int jump, int M, int N, int**A, int**B)
{
    int row, col, i, j, temp, diagonal;
    for (row = 0; row < N; row += 8)
        for (col = start_col; col < N; col += jump)
            for (i = row; i < row + 8; i++){
                for (j = col; j < col + 8; j++){
                    if (i == j)
                    {
                        temp = A[i][j];
                        diagonal = j;
                    }
                    else
                    {
                        B[j][i] = A[i][j];
                    }
                }
                if (row == col)
                    B[diagonal][diagonal] = temp;
            }
}
//---------------------------------------------------------------
// Helper function for matrix > 32. number of columns to jumps is 
// provided as an argument along with the column to start
//---------------------------------------------------------------

void trans_helper_64(int start_col, int jump, int M, int N, int**A, int**B)
{
    int row, col, i, j, temp;
    int var1, var2, var3, var4;	
    for (row = 0; row < N; row += 8)
        for (col = start_col; col < N; col += jump)
                {
                    // We have an 8 by 8 matrix, logically divide it into 4 four by four matrices
                    for (i = 0; i < 4; ++i)
                    {
                        var1 = A[row + i][col];
                        var2 = A[row + i][col+1];
                        var3 = A[row + i][col+2];
                        var4 = A[row + i][col+3];

                        B[col][row + i] = var1;
                        B[col+1][row + i] = var2;
                        B[col+2][row + i] = var3;
                        B[col+3][row + i] = var4;

                    }                            

                    for (i = row; i < row + 4; ++i)
                        for (j = col + 4; j < col + 8; ++j)
                        {
                            temp = A[i][j];
                            B[j-4][i+4] = temp;
                        }

                    for(i = 0; i < 4; ++i)
                    {
                        var1 = B[col+i][row + 4];
                        var2 = B[col+i][row + 5];
                        var3 = B[col+i][row + 6];
                        var4 = B[col+i][row + 7];

                        for (j = row + 4; j < row + 8; ++j)
                        {
                            B[col+i][j] = A[j][col+i];
                        }

                        B[col+4+i][row + 0] = var1;
                        B[col+4+i][row + 1] = var2;
                        B[col+4+i][row + 2] = var3;
                        B[col+4+i][row + 3] = var4;

                    }

                    for (i = 0; i < 4; ++i)
                    {
                        var1 = A[row+i+4][col+4];
                        var2 = A[row+i+4][col+5];
                        var3 = A[row+i+4][col+6];
                        var4 = A[row+i+4][col+7];

                        B[col+4][row+4+i] = var1;
                        B[col+5][row+4+i] = var2;
                        B[col+6][row+4+i] = var3;
                        B[col+7][row+4+i] = var4;
                    }

                }
}

void trans(int M, int N, int** A, int** B)
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

// This is the function from trans.c

void transpose_submit(int M, int N, int** A, int** B)
{
    int row, col, i, j;
    if (N == 32 && M == 32)
    {
        int temp, diagonal;
        for (row = 0; row < N; row += 8)
            for (col = 0; col < N; col += 8)
                for (i = row; i < row + 8; i++){
                    for (j = col; j < col + 8; j++){
                        if (i == j)
                        {
                            temp = A[i][j];
                            diagonal = j;
                        }
                        else
                        {
                            B[j][i] = A[i][j];
                        }
                    }

                    if (row == col)
                        B[diagonal][diagonal] = temp;
                }
                    

    }

    else
    {
        int var1, var2, var3, var4;
        int var5, var6, var7, var8;
        
        for (row = 0; row < N; row += 8)
            for (col = 0; col < N; col += 8)
                    {
                        // We have an 8 by 8 matrix, logically divide it into 4 four by four matrices
                        for (i = 0; i < 4; ++i)
                        {
                            var1 = A[row + i][col];
                            var2 = A[row + i][col+1];
                            var3 = A[row + i][col+2];
                            var4 = A[row + i][col+3];

                            var5 = A[row + i][col+4];
                            var6 = A[row + i][col+5];
                            var7 = A[row + i][col+6];
                            var8 = A[row + i][col+7];

                            B[col][row+4+i] = var5;
                            B[col+1][row + 4+i] = var6;
                            B[col+2][row + 4+i] = var7;
                            B[col+3][row + 4+i] = var8;   

                            B[col][row + i] = var1;
                            B[col+1][row + i] = var2;
                            B[col+2][row + i] = var3;
                            B[col+3][row + i] = var4;

                        }                            

                        for(i = 0; i < 4; ++i)
                        {
                            var1 = B[col+i][row + 4];
                            var2 = B[col+i][row + 5];
                            var3 = B[col+i][row + 6];
                            var4 = B[col+i][row + 7];

                            for (j = row + 4; j < row + 8; ++j)
                            {
                                B[col+i][j] = A[j][col+i];
                            }

                            B[col+4+i][row + 0] = var1;
                            B[col+4+i][row + 1] = var2;
                            B[col+4+i][row + 2] = var3;
                            B[col+4+i][row + 3] = var4;

                        }
                        
                        for (i = 0; i < 4; ++i)
                        {
                            var1 = A[row+i+4][col+4];
                            var2 = A[row+i+4][col+5];
                            var3 = A[row+i+4][col+6];
                            var4 = A[row+i+4][col+7];

                            B[col+4][row+4+i] = var1;
                            B[col+5][row+4+i] = var2;
                            B[col+6][row+4+i] = var3;
                            B[col+7][row+4+i] = var4;
                        }

                    }

    }
}


void alloc(int M, int N, int**& A)
{
    A = new int*[M];
    for(int i = 0; i < M; ++i)
        A[i] = new int[N];
}

void dealloc(int row, int col, int **& array)
{
    for(int i = 0; i < row; ++i)
        delete [] array[i];
    delete[] array; 
}


void init(int M, int N, int** A)
{
    int counter = 1;
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j){
            A[i][j] = counter;
            counter++;
        }
}

int is_transpose(int M, int N, int** A, int** B)
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}






















