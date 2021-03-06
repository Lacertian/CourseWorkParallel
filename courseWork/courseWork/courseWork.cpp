// ConsoleApplication1.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>
#include <vector>
#include <string>
typedef std::chrono::high_resolution_clock Clock;

using namespace std;
int  A = 6;
int B = 4;
double a = 0.00008;
double b = 0.00008;
double tau = 0.0011;
double h = 0.033;
double getExact(double x, double t)
{
	double F = A - 6 * a*t;
	double result = (x*x) / F + 6 / sqrt((F*F*F)) - F / 8;
	return result;
}
double  getApproximate(double w_i, double w_i_plus, double w_i_minus)
{
	double sigma = tau / (h*h);
	double result = w_i + a * sigma*(0.25*pow((w_i_plus - w_i_minus), 2) + w_i * (w_i_plus - 2 * w_i + w_i_minus)) + b * tau;
	return result;
};
void countValue(double x, double t, double** Arr, int from, int to, int column, int i)
{
	for (int j = from; j < to; j++)
	{
		if (i == 0 || j == 0 || j == (column - 1)) {
			Arr[i][j] = getExact(x, t);
		}

		else {
			Arr[i][j] = getApproximate(Arr[i - 1][j], Arr[i - 1][j + 1], Arr[i - 1][j - 1]);
		}

		x += h;
	}

}

void mistake(double **array1, double **array2, int row, int column)
{
	double absolute = -1;
	int t_i;
	int t_j;
	double relative;

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			double temp_val = abs(array1[i][j] - array2[i][j]);


			if (temp_val > absolute) {
				absolute = temp_val;

				t_i = i;
				t_j = j;

			}
		}
	}

	cout << "Абсолютная погрешность: " << absolute << endl;

	relative = abs(absolute / array2[t_i][t_j]);
	cout << "Относительная погрешность: " << relative << endl;

}

void printArray(double **array)
{
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			cout << array[i][j] << " ";
		}
		cout << endl;

	}
	cout << "/////////////////////////////////////////////////////////////" << endl;
}

double writeArrayToFile(double **array, string filename, int row, int column)
{
	ofstream out;
	double serialSum = 0;
	out.open(filename, ios_base::trunc);
	out << "{";
	for (int i = 0; i < row; i++)
	{
		out << "{";
		for (int j = 0; j < column; j++)
		{
			serialSum += array[i][j];
			if (j == column - 1) {
				out << array[i][j];
			}
			else {
				out << array[i][j] << ",";
			}

		}
		out << "},";
	}
	out << "}";
	out.close();
	return serialSum;
}



int main()
{
	setlocale(LC_ALL, "Russian");
	int countThread = 4;
	const int row = 200, column = 10;
	double **Array1 = new double *[row];
	double **Array2 = new double *[row];

	for (int i = 0; i < row; i++)
	{
		Array1[i] = new double[column];
		Array2[i] = new double[column];
	}

	double x = 0;
	double t = 0;
	int from = 0;
	int to = 0;
	int temp = (column / countThread);
	
	auto start1 = std::chrono::steady_clock::now();

	for (int i = 0; i < row; i++)
	{
		x = 0;
		from = 0;
		vector < thread>  threadVector;
		for (int j = 0; j < countThread; j++)
		{
			if (j == (countThread - 1))
			{
				to = column;
			}
			else
			{
				to = temp * (j + 1);
			}
			double temp_x = (x + h) * from;
			threadVector.emplace_back(thread(countValue, temp_x, t, Array1, from, to, column, i));
			from = to;
		}
		for (int z = 0; z < threadVector.size(); z++)
		{
			threadVector[z].join();
		}
		t += tau;
	}
	auto end1 = std::chrono::steady_clock::now();
	
	printArray(Array1);

	double parallelSum = writeArrayToFile(Array1, "array1.txt", row, column);

	t = 0;
	auto start2 = std::chrono::steady_clock::now();
	for (int i = 0; i < row; i++)
	{
		x = 0;
		for (int j = 0; j < column; j++)
		{
			Array2[i][j] = getExact(x, t);
			x += h;
		}
		t += tau;
	}
	auto end2 = std::chrono::steady_clock::now();
	

	printArray(Array2);

	double serialSum = writeArrayToFile(Array2, "array2.txt", row, column);

	cout << "Сумма результатов параллельного решения: " << parallelSum << endl;
	cout << "Сумма результатов последовательного решения: " << serialSum << endl;
	cout << "Время параллельного решения " <<
		chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count() << " микросекунд" << endl;
	cout << "Время последовательного решения " <<
		chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count() << " микросекунд" << endl;

	mistake(Array2, Array1, row, column);
	for (int i = 0; i < row; i++)
		delete[]Array1[i];
	for (int i = 0; i < row; i++)
		delete[]Array2[i];

	system("pause");
	return 0;
}

