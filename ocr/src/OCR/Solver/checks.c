#include <stdio.h>

#include "checks.h"

tTuple find_first_letter(int x, int y, int rows, int cols, char grid[rows][cols],
			const char *word)
{
	tTuple result;
	result.x = -69;
	result.y = -69;

	for (int i = y; i < rows; i++)
	{
		for (int j = x; j < cols; j++)
		{
			if (grid[i][j] == word[0])
			{
				result.x = j;
				result.y = i;
				return result;
			}
		}
	}

	return result;
}

cCoords check_horizontal(int rows, int cols, char grid[rows][cols],
			const char *word, tTuple first_coords, int word_size)
{
	cCoords result;
	result.t1 = first_coords;
	result.t2.x = -69;
	result.t2.y = -69;

	if (first_coords.x - word_size + 1 >= 0)
	{
		int found = 1;
		int word_index = 1;
		int i = first_coords.x - 1;

		while (i >= 0 && grid[first_coords.y][i] == word[word_index])
		{
			found++;
			word_index++;
			i--;
		}

		if (found == word_size)
		{
			result.t2.x = i + 1;
			result.t2.y = first_coords.y;
			return result;
		}
	}

	if (first_coords.x + word_size - 1 < cols)
	{
		int found = 1;
		int word_index = 1;
		int i = first_coords.x + 1;

		while (i < cols && grid[first_coords.y][i] == word[word_index])
		{
			found++;
			word_index++;
			i++;
		}

		if (found == word_size)
		{
			result.t2.x = i - 1;
			result.t2.y = first_coords.y;
			return result;
		}
	}

	return result;
}

cCoords check_vertical(int rows, int cols, char grid[rows][cols],
			const char *word, tTuple first_coords, int word_size)
{
	cCoords result;
	result.t1 = first_coords;
	result.t2.x = -69;
	result.t2.y = -69;

	if (first_coords.y - word_size + 1 >= 0)
	{
		int found = 1;
		int word_index = 1;
		int i = first_coords.y - 1;

		while (i >= 0 && grid[i][first_coords.x] == word[word_index])
		{
			found++;
			word_index++;
			i--;
		}

		if (found == word_size)
		{
			result.t2.x = first_coords.x;
			result.t2.y = i + 1;
			return result;
		}
	}

	if (first_coords.y + word_size - 1 < rows)
	{
		int found = 1;
		int word_index = 1;
		int i = first_coords.y + 1;

		while (i < rows && grid[i][first_coords.x] == word[word_index])
		{
			found++;
			word_index++;
			i++;
		}

		if (found == word_size)
		{
			result.t2.x = first_coords.x;
			result.t2.y = i - 1;
			return result;
		}
	}

	return result;
}

cCoords check_diags(int rows, int cols, char grid[rows][cols],
		const char *word, tTuple first_coords, int word_size)
{
	cCoords result;
	result.t1 = first_coords;
	result.t2.x = -69;
	result.t2.y = -69;

	if (first_coords.y - word_size + 1 >= 0 &&
				 first_coords.x - word_size + 1 >= 0)
	{
		int found = 1;
		int word_index = 1;
		int i = first_coords.y - 1;
		int j = first_coords.x - 1;

		while (i >= 0 && j >= 0 && grid[i][j] == word[word_index])
		{
			found++;
			word_index++;
			i--;
			j--;
		}

		if (found == word_size)
		{
			result.t2.x = j + 1;
			result.t2.y = i + 1;
			return result;
		}
	}

	if (first_coords.y - word_size + 1 >= 0 &&
	 			first_coords.x + word_size - 1 < cols)
	{
		int found = 1;
		int word_index = 1;
		int i = first_coords.y - 1;
		int j = first_coords.x + 1;

		while (i >= 0 && j < cols && grid[i][j] == word[word_index])
		{
			found++;
			word_index++;
			i--;
			j++;
		}

		if (found == word_size)
		{
			result.t2.x = j - 1;
			result.t2.y = i + 1;
			return result;
		}
	}

	if (first_coords.y + word_size - 1 < rows 
				&& first_coords.x - word_size + 1 >= 0)
	{
		int found = 1;
		int word_index = 1;
		int i = first_coords.y + 1;
		int j = first_coords.x - 1;

		while (i < rows && j >= 0 && grid[i][j] == word[word_index])
		{
			found++;
			word_index++;
			i++;
			j--;
		}

		if (found == word_size)
		{
			result.t2.x = j + 1;
			result.t2.y = i - 1;
			return result;
		}
	}

	if (first_coords.y + word_size - 1 < rows && 
				first_coords.x + word_size - 1 < cols)
	{
		int found = 1;
		int word_index = 1;
		int i = first_coords.y + 1;
		int j = first_coords.x + 1;

		while (i < rows && j < cols && grid[i][j] == word[word_index])
		{
			found++;
			word_index++;
			i++;
			j++;
		}

		if (found == word_size)
		{
			result.t2.x = j - 1;
			result.t2.y = i - 1;
			return result;
		}
	}

	return result;
}