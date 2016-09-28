#include "matrix.h"

Matrix::Matrix(uint rows, uint cols)
	   :rowsCapacity(10), colsCapacity(10)
{
	this->rows = rows;
	this->cols = cols;

	if(rows > rowsCapacity) rowsCapacity = rows + 10;
	if(cols > colsCapacity) colsCapacity = cols + 10;

	matrix = new float*[rowsCapacity];
	for(uint r = 0; r < rowsCapacity; r++)
	{
		matrix[r] = new float[colsCapacity];
		memset(matrix[r], 0, colsCapacity*sizeof(float));
	}
}

Matrix::Matrix(const Matrix &other)
	   :rowsCapacity(other.rowsCapacity), colsCapacity(other.colsCapacity), rows(other.rows), cols(other.cols)
{
	this->matrix = new float*[rowsCapacity];
	for(uint r = 0; r < this->rowsCapacity; r++)
	{
		matrix[r] = new float[colsCapacity];
		memcpy(this->matrix[r], other.matrix[r], colsCapacity*sizeof(float));
	}
}

Matrix::~Matrix()
{
	for(uint r = 0; r < rowsCapacity; r++)
		delete[] matrix[r];

	delete[] matrix;
}

Matrix& Matrix::operator=(const Matrix &other)
{
    if (this == &other)
    	return *this;

	for(uint r = 0; r < rowsCapacity; r++)
		delete[] matrix[r];
	delete[] matrix;

	this->rowsCapacity = other.rowsCapacity;
	this->colsCapacity = other.colsCapacity;
	this->rows = other.rows;
	this->cols = other.cols;

	this->matrix = new float*[rowsCapacity];
	for(uint r = 0; r < this->rowsCapacity; r++)
	{
		matrix[r] = new float[colsCapacity];
		memcpy(this->matrix[r], other.matrix[r], colsCapacity*sizeof(float));
	}

    return *this;
}

float* Matrix::operator[](uint i)
{
	return matrix[i];
}

const float* Matrix::operator[](uint i) const
{
	return matrix[i];
}

ostream& operator<<(ostream &out, const Matrix &matrix)
{
	out.precision(6);
	out.setf(ios::fixed,ios::floatfield);

	if(matrix.rows > 0 && matrix.cols >0)
	{

		for(uint r = 0; r < matrix.rows; r++)
		{
			out << matrix[r][0];

			for(uint c = 1; c < matrix.cols; c++)
				out << "\t" << matrix[r][c];

			out << endl;
		}

	}

	return out;
}

uint Matrix::height()
{
	return rows;
}

uint Matrix::width()
{
	return cols;
}

bool Matrix::erase_row(uint rowid)
{
	if(rowid < rows)
	{
		if((rows - 1) >= (rowsCapacity - 10))
		{

			float* rowarray = matrix[rowid];
			memcpy(&matrix[rowid], &matrix[rowid+1], ((rowsCapacity - rowid)-1)*sizeof(float*));
			matrix[rowsCapacity - 1] = rowarray;

		}else{

			float** rowsarray = new float*[rowsCapacity-10];

			memcpy(rowsarray, matrix, (rowid)*sizeof(float*));
			memcpy(&rowsarray[rowid], &matrix[rowid+1], ((rowsCapacity-10) - (rowid))*sizeof(float*));

			delete[] matrix[rowid];
			for(uint r = rowsCapacity - 9; r < rowsCapacity; r++)
				delete[] matrix[r];

			delete[] matrix;
			matrix = rowsarray;

			rowsCapacity -= 10;
		}

		rows--;

		return true;
	}
	return false;
}

bool Matrix::erase_column(uint colid)
{
	if(colid < cols)
	{

		if((cols - 1) >= (colsCapacity - 10))
		{
			for(uint r = 0; r < rows; r++)
				memcpy(&matrix[r][colid], &matrix[r][colid+1], ((colsCapacity - colid)-1)*sizeof(float));
		}else{

			for(uint r = 0; r < rowsCapacity; r++)
			{
				float* newrow = new float[colsCapacity - 10];
				memcpy(newrow, matrix[r], colid*sizeof(float));
				memcpy(&newrow[colid], &matrix[r][colid+1], ((colsCapacity - 10)-colid)*sizeof(float));

				delete[] matrix[r];
				matrix[r] = newrow;
			}

			colsCapacity -= 10;
		}

		cols--;
		return true;
	}

	return false;
}

void Matrix::insert_row()
{
	if(rows < rowsCapacity)
	{
		memset(matrix[rows], 0, colsCapacity*sizeof(float));
		rows++;
		return;
	}

	float** rowarray = new float*[rowsCapacity + 10];
	memcpy(rowarray, matrix, (rowsCapacity)*sizeof(float*));

	for(uint r = rowsCapacity; r < rowsCapacity + 10; r++)
	{
		rowarray[r] = new float[colsCapacity];
		memset(rowarray[r], 0, colsCapacity*sizeof(float));
	}

	delete[] matrix;
	matrix = rowarray;

	rowsCapacity += 10;
	rows++;
}

void Matrix::insert_column()
{
	if(cols < colsCapacity)
	{
		for(uint r = 0; r < rows; r++)
			matrix[r][cols] = 0;

		cols++;
		return;
	}

	for(uint r = 0; r < rowsCapacity; r++)
	{
		float* colarray = new float[colsCapacity+10];
		memcpy(colarray, matrix[r], (colsCapacity)*sizeof(float));
		memset(&colarray[colsCapacity], 0, 10*sizeof(float));

		delete[] matrix[r];
		matrix[r] = colarray;
	}

	colsCapacity += 10;
	cols++;
}
