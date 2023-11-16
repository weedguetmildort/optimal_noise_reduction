// Weedguet Mildort
// Optimal Noise Reduction
// Description:

#include "main.h"


// Libraries
#include <stdio.h>
#include <stdlib.h>

//
void permute(int num, int loops, int * curRow, int * used, int * target) {
    if (loops == 0) {
        // loops 0 is a lie
        // Check that the row matches the target
        int good = 1;
        for (int i = 0; i < num; i++)
            if (target[i] != 0 && curRow[i] != target[i])
                good = 0;

        // IF good then print
        if (good) {
            for (int i = 0; i < num; i++)
                printf("%d ", curRow[i]);
            printf("\n");
        }

        return;
    }

    // Try all values for the current position
    int position = num - loops;
    for (int i = 0; i < num; i++) {
        // Check if the value was already used
        if (used[i])
            continue;

        // Make the value used
        used[i] = 1;
        curRow[position] = i + 1;

        // Do the recursion
        // loops, curRow, used, target
        permute(num, loops - 1, curRow, used, target);

        // Unmake value used
        used[i] = 0;
        curRow[position] = 0;
    }
}

//
int check(int * index1, int * index2, int * cageNum, int * solution, int anExhibit, int constraintNum){

    int value = 0;

    for(int i = 0; i < constraintNum; i++) {
        if(solution[index2[i] - 1] - solution[index1[i] - 1] >= cageNum[i]){
            value++;
        }
        if(value == constraintNum){
            return 0;
        }
    }
    return 1;
}

//
void optimize(int * index1, int * index2, int * cageNum, int anExhibit, int constraintNum){
    // Variables declaration
    //int solution[constraintNum];
    int solution[] = { 2, 1, 4, 5, 3};


    while (1) {
        // Permute

        if (!check(index1, index2, cageNum, solution, anExhibit, constraintNum)) {
            printf("No Solution\n");
            break;
        }
        for (int i = 0; i < anExhibit; i++)
            printf("%d ", solution[i]);

        printf("\n\n");

        break;


    }


    // Others
    int curRow[anExhibit], used[anExhibit], target[anExhibit];
    for (int i = 0; i < anExhibit; i++) {
        curRow[i] = 0; // empty row to begin with
        used[i] = 0; // use nothing to begin with
        target[i] = 0;
    }

    // Restrictions
    //target[2] = 3;
    //target[4] = 4;

    permute(anExhibit, anExhibit, curRow, used, target);

    // Testing output
    printf("%d %d\n", anExhibit, constraintNum);
    for(int i = 0; i < constraintNum; i++) {
        printf("%d %d %d\n", index1[i], index2[i], cageNum[i]);
    }

}

// Main function
int main() {
    // Variables declaration
    int anExhibit, constraintNum;

    // User input
    scanf("%d %d", &anExhibit, &constraintNum);

    // More variables declaration for next input
    int index1[constraintNum], index2[constraintNum], cageNum[constraintNum];

    for(int i = 0; i < constraintNum; i++) {
        scanf("%d %d %d", &index1[i], &index2[i], &cageNum[i]);
    }

    // Permute and print out solution if any
    optimize(index1, index2, cageNum, anExhibit, constraintNum);

    // End the main function
    return 0;
}