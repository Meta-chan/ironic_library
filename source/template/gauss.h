/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

template<class T> bool ir::gauss(size_t size, T **matrix, T *vector, T *result) noexcept
{
	//Eleminating bottom left corner
	for (size_t i = 0; i < size - 1; i++)
	{
		//Searching for maximal value
		T maxabs = matrix[i][i];
		if (maxabs < 0) maxabs = -maxabs;
		size_t maxline = i;
		for (size_t maxsearch = i + 1; maxsearch < size; maxsearch++)
		{
			T newmaxabs = matrix[maxsearch][i];
			if (newmaxabs < 0) newmaxabs = -newmaxabs;
			if (newmaxabs > maxabs)
			{
				maxabs = newmaxabs;
				maxline = maxsearch;
			}
		}
		
		if (maxabs == 0) return false;

		//Swapping lines
		if (maxline != i)
		{
			T *prbuf = matrix[i];
			matrix[i] = matrix[maxline];
			matrix[maxline] = prbuf;

			T rbuf = vector[i];
			vector[i] = vector[maxline];
			vector[maxline] = rbuf;
		}

		//Multiplying rows
		for (size_t j = i + 1; j < size; j++)
		{
			T koef = matrix[j][i] / matrix[i][i];
			//matrix[j][i] = 0;	//actually not needed
			for (size_t k = i + 1; k < size; k++)
			{
				matrix[j][k] = matrix[j][k] - koef * matrix[i][k];
			}
			vector[j] = vector[j] - koef * vector[i];
		}
	}

	//Eleminating top right corner
	for (size_t i = size - 1; i > 0; i--)
	{
		for (size_t j = 0; j < i; j++)
		{
			T koef = matrix[j][i] / matrix[i][i];
			//matrix[j][i] = 0; //actually not needed
			vector[j] = vector[j] - koef * vector[i];
		}
	}

	//Writing to vector/result
	if (result != nullptr)
	{
		for (size_t i = 0; i < size; i++) result[i] = vector[i] / matrix[i][i];
	}
	else
	{
		for (size_t i = 0; i < size; i++) vector[i] /= matrix[i][i];
	}

	return true;
}