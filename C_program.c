const int SIZE = 7;
int cnt;
int tenth;
char c[SIZE];
int data;
int flag;
int opdv;
/*
Convert char j to an integer if possible.
If the flag is not set use the sign indicator s
and the tenths indicator to compute the actual
value of j. Add the value to the accumulator opdv.
*/
void opd(char s, int k, char j) {
    int t = 0; // Local var
    if (j == '0') // Convert
        t = 0;
    else if (j == '1')
        t = 1;
    else if (j == '2')
        t = 2;
    else if (j == '3')
        t = 3;
    else if (j == '4')
        t = 4;
    else if (j == '5')
        t = 5;
    else if (j == '6')
        t = 6;
    else if (j == '7')
        t = 7;
    else if (j == '8')
        t = 8;
    else if (j == '9')
        t = 9;
    else {
        printf("%c is not a number\n", j);
        flag = 1;
    }
    if (!flag) {
        if (s == '+')
            t *= k;
        else
            t *= -k;
            opdv += t;
    }
}
/*
Discard keyboard input until a newline ‘\n’ is
Encountered.
*/
void flush() {
    data = 0;
    c[0] = getchar();
    while (c[0] != '\n')
        c[0] = getchar();
}
/*
Read one char at a time from the keyboard after a
newline ‘\n’ has been entered. If there is room
place the char in the array c
otherwise indicate the number is too big and
flush the keyboard input.
*/
void getdata() {
    if (cnt < SIZE) { // Get data if there is room
        c[cnt] = getchar(); // Your TRP 4 should work like getchar()
        cnt++;
    } else {
        printf("Number too Big\n");
        flush();
    }
}
/*
Reset c to all 0’s
Assign values to data, opdv, cnt and flag.
*/
void reset(int w, int x, int y, int z) {
    int k; // Local var
    for (k= 0; k < SIZE; k++)
        c[k] = 0;
        data = w;
        opdv = x;
        cnt = y;
        flag = z;
}
/*
Get input from the keyboard until the symbol ‘@’ is encountered.
Convert the char data input from the keyboard to an integer be
Sure to account for the sign of the number. If no sign is used
always assume the number is positive.
*/
void main() {
    reset(1, 0, 0, 0); // Reset globals
    getdata(); // Get data
    while (c[0] != '@') { // Check for stop symbol '@'
        if (c[0] == '+' || c[0] == '-') { // Determine sign
            getdata(); // Get most significant byte
        } else { // Default sign is '+'
            c[1] = c[0]; // Make room for the sign
            c[0] = '+';
            cnt++;
        }
        while(data) { // Loop while there is data to process
            if (c[cnt-1] == '\n') { // Process data now
                data = 0;
                tenth = 1;
                cnt = cnt - 2;
                while (!flag && cnt != 0) { // Compute a number
                    opd(c[0], tenth, c[cnt]);
                    cnt--;
                    tenth *= 10;
                }
                if (!flag) // Good number entered
                    printf("Operand is %d\n", opdv);
            }
            else
                getdata(); // Get next byte of data
        }
        reset(1, 0, 0, 0); // Reset globals
        getdata(); // Get data
    }
}