#ifndef MATRIX_H
#define MATRIX_H

#include <cstring>
#include <iostream>

using namespace std;

typedef unsigned int uint;

class Matrix
{

	private:
		uint rowsCapacity, colsCapacity, rows, cols;

		float** matrix;

	public:
		Matrix(uint rows = 0.0, uint cols = 0.0);
		Matrix(const Matrix &other);

		~Matrix();

		Matrix& operator=(const Matrix &other);
		float* operator[](uint i);
		const float* operator[](uint i) const;
		friend ostream& operator<<(ostream &out, const Matrix &matrix);

		uint height();
		uint width();

		bool erase_row(uint rowid);
		bool erase_column(uint colid);

		void insert_row();
		void insert_column();
};

#endif // MATRIX_H
