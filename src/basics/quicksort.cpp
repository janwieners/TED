#include "quicksort.h"

void swap(int &a, int &b)
{
    int temp;
    temp=a;
    a=b;
    b=temp;
}

/* This function does the quicksort
   Arguments :
             array - the array to be sorted
             startIndex - index of the first element of the section
             endIndex - index of the last element of the section
   */
void QuickSort(int* array, int startIndex, int endIndex)
{
    int pivot = array[startIndex];                    //pivot element is the leftmost element
    int splitPoint;
    
    if(endIndex > startIndex)                         //if they are equal, it means there is
                                                      //only one element and quicksort's job
                                                      //here is finished
    {
        splitPoint = SplitArray(array, pivot, startIndex, endIndex);
                                                      //SplitArray() returns the position where
                                                      //pivot belongs to
        array[splitPoint] = pivot;
        QuickSort(array, startIndex, splitPoint-1);   //Quick sort first half
        QuickSort(array, splitPoint+1, endIndex);     //Quick sort second half
    }
}

/* This function splits the array around the pivot
   Arguments :
             array - the array to be split
             pivot - pivot element whose position will be returned
             startIndex - index of the first element of the section
             endIndex - index of the last element of the section
   Returns :
           the position of the pivot
   */
int SplitArray(int* array, int pivot, int startIndex, int endIndex)
{
    int leftBoundary = startIndex;
    int rightBoundary = endIndex;
    
    while(leftBoundary < rightBoundary)               //shuttle pivot until the boundaries meet
    {
         while( pivot < array[rightBoundary]          //keep moving until a lesser element is found
                && rightBoundary > leftBoundary)      //or until the leftBoundary is reached
         {
              rightBoundary--;                        //move left
         }
         swap(array[leftBoundary], array[rightBoundary]);
        
         while( pivot >= array[leftBoundary]          //keep moving until a greater or equal element is found
                && leftBoundary < rightBoundary)      //or until the rightBoundary is reached
         {
              leftBoundary++;                         //move right
         }
         swap(array[rightBoundary], array[leftBoundary]);
    }
    return leftBoundary;                              //leftBoundary is the split point because
                                                      //the above while loop exits only when
                                                      //leftBoundary and rightBoundary are equal
}


