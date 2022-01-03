# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include "autocomplete.h"

////////////////////////////////////////////////////////////////////////////////

/* HELPER FUNCTIONS FOR READ_IN_TERMS*/

// Funtion to remove tab lines from a string
char *remove_white_spaces(char *str)
{
	int i = 0, j = 0;
	while (str[i])
	{
		if (str[i] != ' ' && str[i] != '\t')
          str[j++] = str[i];
		i++;
	}
	str[j] = '\0';
	return str;
}

// Function to remove tabs and new lines from a string
char *remove_tab_new(char *str)
{
	int i = 0, j = 0;
	while (str[i])
	{
		if (str[i] != '\t' && str[i] != '\n')
          str[j++] = str[i];
		i++;
	}
	str[j] = '\0';
	return str;
}

// Function to compare elements of the array
int cmp_terms(const void* a, const void* b) 
{  
    struct term *ia = (struct term *)a;
    struct term *ib = (struct term *)b;
    return strcmp(ia -> term, ib -> term);
} 

// Function to sort the array 
void sort_struct_terms(struct term arr[], int nterms)  
{ 
    qsort(arr, nterms, sizeof(struct term), cmp_terms); 
} 

////////////////////////////////////////////////////////////////////////////////

/* READ_IN TERMS FUNCTION */

void read_in_terms(struct term **terms, int *pnterms, char *filename)
{
    // Declaring variables 
    char c[200], line[200];
    char *pointer, *ptr, *ptr_c;
    const char s[4] = "\t";
    double wght;
    int arr_count = -1;

    // Opening file
    FILE *fp = fopen(filename, "r");

    // Finding the number of terms in the file
    fscanf(fp, "%[^\n]", c);
    *pnterms = atoi(c);

    // Allocating memory for terms of file
    *terms = (struct term* )malloc(*pnterms * sizeof(struct term));

    fgets(line, sizeof(line), fp);

    // Obtaining terms and weights from file
    while(fgets(line, sizeof(line), fp) != 0 && arr_count != *pnterms)
    {
        arr_count ++;

        // Obtaining weight from line of file
        pointer = strtok(line, s);

        // Remove spaces from string
        pointer = remove_white_spaces(pointer);
    
        // Converterting string to double 
        wght = strtod(pointer, &ptr);
    
        // Obtaining the second part of the string
        pointer = strtok(NULL, s);
        pointer = remove_tab_new(pointer); 

        // Inserting weight and term into struct in array
        (*terms)[arr_count].weight = wght;

        strcpy((*terms)[arr_count].term, pointer);

    }
    
    // Sorting terms of array 
    sort_struct_terms(*terms, *pnterms);
}

////////////////////////////////////////////////////////////////////////////////

/* FUNCTION TO COMPARE TERMS OF ARRAY */

int my_strcmp_rec(char *str1, char *str2)
{
   if(*str2 == '\0'){
       return 0;
   }
   if(*str1 != *str2){
       return ((int)*str1 - (int)*str2);
   }
   return my_strcmp_rec( str1 + 1, str2 + 1 );
}

////////////////////////////////////////////////////////////////////////////////

/* FUNCTION LOWEST_MATCH */

int lowest_match(struct term *terms, int nterms, char *substr)
{ 
    int start= 0;
    int end  = nterms - 1;
    int index = -1;
    int mid;
    int temp;

    while (start <= end) 
    {              
        mid = (start + end) / 2;        
        temp = my_strcmp_rec(terms[mid].term, substr);

        if (temp == 0)
        {
            index = mid;
            end = mid - 1;
        }          
        else if (temp < 0)
        {
            start = mid + 1;
        }else if (temp > 0)
            end = mid - 1;
    }
    return index;
}

////////////////////////////////////////////////////////////////////////////////

/* FUNCTION HIGHEST_MATCH */

int highest_match(struct term *terms, int nterms, char *substr){

    int start= 0;
    int end  = nterms - 1;
    int index = -1;
    int mid, temp;

    while (start <= end) 
    {              
        mid = (start + end) / 2; 
        temp = my_strcmp_rec ((terms)[mid].term, substr); 

        if (temp == 0)
        {     
            index = mid;  
            start = mid +1;  
        }              
        else if (temp < 0)
        {
            start = mid + 1;
        }else{
            end = mid - 1;
        }
    }
    return index;
}


////////////////////////////////////////////////////////////////////////////////

/* HELPER FUNCTIONS FOR AUTOCOMPLETE */

// Function to compare weight of structs
int cmp_weight (const void *a, const void *b) 
{ 
    struct term *ia = (struct term *)a;
    struct term *ib = (struct term *)b;
    return (int)(ib -> weight - ia -> weight); // float comparison: returns negative if b > a and positive if a > b.
} 

// Function to sort structs by weight
void sort_structs_weight(struct term arr[], int nterms)  
{ 
    qsort(arr, nterms, sizeof(struct term), cmp_weight); 
} 

////////////////////////////////////////////////////////////////////////////////

/* AUTOCOMPLETE FUNCTION */

// Function to return possible answers to substring provided
void autocomplete(struct term **answer, int *n_answer, struct term *terms, int nterms, char *substr)
{
    // Defining variables
    int h_index, l_index, i, j;
    
    // Obtaining boundaries for possible string
    l_index = lowest_match(terms, nterms, substr);
    h_index = highest_match(terms, nterms, substr);

    // Obtaining number of answers
    if (l_index != 0 && h_index != 0)
    {
        *n_answer = h_index - l_index + 1;
    }else{
        *n_answer = 0; 
    }
    

    // Allocating memory for answer
    *answer = (struct term *)malloc(*n_answer * sizeof(struct term));

    j = l_index - 1;

    // Inserting answers into array
    for ( i = 0; i < *n_answer; i++)
    {
        j = j + 1;
        if (j <= h_index)
        {
            strcpy((*answer)[i].term, terms[j].term);
            (*answer)[i].weight = terms[j].weight;
        }
    }

    // Sorting array in descending order
    sort_structs_weight(*answer, *n_answer);
}
