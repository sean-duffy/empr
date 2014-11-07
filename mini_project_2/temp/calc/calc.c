#include <stdio.h>
#include <string.h>

char * get_result(char* input_string);

int main(void){
    char input_string[20];
    char* result;

    while (1){
    printf("Input:\t");
    scanf("%s",input_string);
    result = get_result(input_string);
    printf("=%s\n",result);
    }
}

char * get_result(char* input_string){
    char* p_to_op = strpbrk(input_string, "+*/-");
    char operation;
    static char result[20];

    if (p_to_op == NULL){
        operation = '0';
        strcpy(result, input_string);
    } else {
        operation = *p_to_op;
        char op[] = {operation};
    
        char* plus = strtok(input_string,op);
        
        //Case no first variable
        if (input_string[0] == operation){
           return strcpy(result, plus);
        }

        
        int first_n = atoi(plus);
        plus = strtok(NULL,op);
       
        //Case no second variable
        if (plus == NULL){
            sprintf(result,"%d",first_n);
            return result;
        }

        int second_n = atoi(plus);
    
        int r;
        double dr;

        switch (operation) {
            case '+': // ADD 
                r = first_n + second_n;
                sprintf(result,"%d",r);
                break;
            
            case '*': // MULTIPLY
                r = first_n * second_n;
                sprintf(result,"%d",r);
                break;

            case '-': // MINUS
                r = first_n - second_n;
                sprintf(result,"%d",r);
                break;

            case '/': // DIVIDE
                dr = (double)first_n / (double)second_n;
                sprintf(result,"%f",dr);
                break;
    
            default:
                strcpy(result,"Err: Unkown Opp");
                break;
        }
    }

    return result;
}


