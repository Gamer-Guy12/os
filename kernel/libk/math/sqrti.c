#include <stdbool.h>
#include <libk/math.h>
/// written at 11:00. don't expect this to be good
/// doesn't take other roots cuz my sleepy ass couldn't fucking bother
float math_sqrti64(int_64 number, int ) //fuck this naming convenction
{
    int accuracy = 12; //completely arbitrary. change it
    int i = 0; //loop number
    float guess = 0;
    float squareroot = 0;
    foundnumberbool = false;
    

    //initial number finding
    
    while not foundnumberbool {
        guess++;
        if (math_powf64(guess,2) > number) { //tf do i even use (update: use powf)
            guess = guess - 1;
            foundnumberbool = true;
        }
    } 
    
    //found initial guess.
    squareroot = guess; //i forgot what i was going to write
    //resetting variables
    guess = 0;
    foundnumberbool = false;
    while (i < accuracy) {
        while not foundnumberbool {
            //*inception horn noise*
            guess++;
            //if (math_powi) //wait
            //you didn't write the goddamn float version
            //also whats an uint
            //finished writing the float version
            //you're welcome
            if (math_powf64(squareroot + (guess/((i+1)*10)),2) > number) {
                squareroot = squareroot + ((guess-1)/((i+1)*10))
            }
        }
        i++;
    }
    return squareroot;
}
//no idea if this works
