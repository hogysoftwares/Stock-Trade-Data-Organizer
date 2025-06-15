#include <cstdio>
#include <cstring>
#include <map>
#include <iostream>
#include <string>

/**
Stock_Trade_Data_Organizer v1.1

// Read file in with the following format:
// [Stock],[Interval],[Volume Traded],[High],[Low]

// Calculate the total volume traded per Stock
// Calculate the total volume traded per Stock&Interval

// Write the total volume traded per Stock&Interval as a percentage of the total volume traded per Stock to stdout:
// [Stock],[Interval],[%Volume Traded]

// Write the delimiter '#' to stdout

// Write the maximum High and minimum Low for each Stock to stdout:
// [Stock],[Day High],[Day Low]

// example input:
VOD.L 1 100 184 183.7
BT.LN 1 300 449.4 448.2
VOD.L 2 25 184.1 182.4
BT.LN 2 900 449.8 449.5

// example output:
VOD.L,1,80
BT.LN,1,25
VOD.L,2,20
BT.LN,2,75
#
VOD.L,184.1,182.4
BT.LN,449.8,448.2

**/

#define DELIMITER "#" /* Could just use const std::string instead. Macros are discouraged in C++ because of potential scope issues, lack of type safety, name collision issues, etc. */

/* using namespace std; can be used for things like independent projects, but for a company program where you're not the only writer/editor of the code, the scope of the program can change. 
Also, you aren't guaranteed to be aware of all of the other files that can be used with this program. There can be name collisions. Should use std:: instead.*/
using namespace std;

typedef basic_string<char> string; /* std::string is already defined as this. This line is unnecessary and confusing.*/

class	CUpperLower
{
public:
    /* nCurLwr will never be updated when initialized to 0 if the incoming nLow values are positive, which they are expected to be. Should be using very large positive defined values like std::numeric_limits<T>::max(). 
    nCurUpr should update, but it is also better practice to make it initalized to defined limits like std::numeric_limits<float>::lowest() as well. It also reads better if you're already going to initialize nCurLwr in that style anyway. */
    CUpperLower() : nCurLwr(0), nCurUpr(0) {}; 

    void	add(int nHigh, int nLow) /* Should be float or double to not truncate values. float would be more accurate since we only need up to two decimal points to represent cents.*/
    {
        if (nHigh > nCurUpr)
            nCurUpr = nHigh;

        if (nLow < nCurLwr)
            nCurLwr = nLow;
    }

    /* This getSum() function doesn't actually serve a purpose since the program should output the lowest and highest prices for a stock separately.*/
    int& getSum() /* Return type is a reference which is wrong. This function should be pass by value returning an int (but really it should be float as mentioned in the add() function comment).*/
    {
        int sum = nCurLwr + nCurUpr;
        return sum;
    }

    /* These data members can be corrupted. They should be private with getter functions for safety.*/
    int         nCurLwr;
    int         nCurUpr;
};

/* argv[1] and argv[2] are accessed, so argc should be at least 3. Should be checking that before accessing */
int main(int argc, char* argv[])
{
    /* This version checking makes me think that the program is only looking for one argument, not 2. Either version or filename. 
    If that's the case, we need to be making that clear and checking filename should also be using argv[1]. In this case argc should be 2. */
    if (!strcmp("version", argv[1])) 
    {
        cerr << "Stock Trade Data Organizer version 1.1" << endl; /* This isn't an error so using cout would be better. */
        return 0;
    }

    /* When using fopen() with mode "r", the file must exist. If the file can't be opened fopen() will return nullptr. Must check this before calling fgets() */
    FILE*	file = fopen(argv[2], "r"); 

    cerr << "Reading file" << argv[2] << endl;

    /*Magic numbers are bad practice. Define these buffer sizes as const values with names. */
    /*These could just be strings and vectors to avoid buffer overflow. If we must use fixed-sized arrays, we should write lines to handle buffer overflow.*/
    char	line[256];
    char	Stocks[1000][10];
    int         Intervals[1000];
    int         Volumes[1000];
    float	Highs[1000];
    float	Lows[1000];

    int         i = 0;
    int         sum = 0;

    while (fgets(line, 256, file))
    {
        /* We should check the sscanf() return value to see if it actually assigned to all 5.*/
        sscanf(line, "%s %d %d %f %f",
               Stocks[i], &Intervals[i],
               &Volumes[i], &Highs[i], &Lows[i++]);
    }

    cerr << "Calculating total volumes" << endl;

    map<std::string, int>		TotalVolumes;

    /* TotalVolumes is a map so we don't need a nested for loop or strcmp(). This will cause incorrect entries. Should just be single pass with Stocks[s] as the key.*/
    for (int s = 0; s <= i; ++s) /* should be s < i */
    {
        std::string	stockname = Stocks[s];

        for (int j =0; j <= i; ++j) /* should be j < i */
        {
            if (!strcmp(Stocks[j], stockname.c_str()))
            {
                TotalVolumes[stockname] += Volumes[j];
            }
        }
    }

    cerr << "Calculating high lows" << endl;

    map<std::string, CUpperLower>	HighLows;

    for (int s = 0; s <= i; ++s) /* should be s < i */
    {
        HighLows[Stocks[s]].add(Highs[s], Lows[s]);
        cout << HighLows[Stocks[s]].getSum();
    }

    cerr << "Writing files" << endl; /* The output is to console not a file, so this is a little bit confusing. Could be something like "Writing Output" instead */

    for (int s = 0; s <= i; ++s) /* should be s < i */
    {
        /*Incorrect percentage calculation. According to the program specification the ratio should be Volumes[s] / TotalVolumes[Stocks[s]] */
        cout << Stocks[s] << "," << Intervals[s] << "," <<
                TotalVolumes[Stocks[s]] / Volumes[s] * 100 << endl; /*This is integer division, which will truncate the value. Should be (Volumes[s] * 100.0) / TotalVolumes[Stocks[s]].
                May even want to consider static casting Volumes[s] for cleaner code and to prevent integer overflow depending on the scope of this program, or making the 100.0 a const double variable. */
    }

    cout << DELIMITER << endl;

    map<std::string, CUpperLower>::iterator itr = HighLows.begin();
    while (itr != HighLows.end())
    {
        cout << (*itr).first << "," << (*itr).second.nCurLwr << "," <<
                (*itr).second.nCurUpr << endl;

        ++itr;
    }

    return 1; /* For main() I have only ever seen 0 for success and never a 1. Return value of 1 or -1 usually means error. Could be company specific, but otherwise the main function return value should be 0 for success. */
}

/*
    Since this is supposed to be a code review for an intern's work, I tried to make the in-line comments as descriptive as I can without overexplaining.
    Mainly, I wanted to make sure I am not only pointing out what the flaws are but also why something is incorrect or not the best approach. 
    This way, the code review is not just a ducttape fix but an effort for improvement in the future.
    The following are also some general comments to consider:


    Overall, C style code is being used too much when an equivalent or better C++ style exists. This is a C++ program. Tools like iostream, fstream, vectors, and string methods should be more used instead of the C counterparts like fopen(), as an example.
    This is not only because many of these C++ libraries and standards are an improvement over C, but also because this is a company program and there most likely will be other programmers using and editing this code. 
    They will be expecting C++ methodologies. A C++ program should be very C++ legible. It's not even just about individual lines of code, but the way this program flows feels very procedural to me instead of object-oriented.

    1. As a specific follow up on the above, it's better to use C++ libraries instead of C header files when you can since this is a C++ program, and because other employees may use/edit this code so it should be as legible in C++ as possible. C++ is what they're going to be expecting to see, not C.

    2. The record for each trade in an interval should be a class to make the code more object-oriented, or a struct if there are no particularly useful member functions to add.

    3. Shouldn't use magic numbers like 256 and 1000, instead define them as consts with names if you're going to use them.

    4. Many of the variable names are confusing. For example, nCurLwr can be a lot clearer with a name like "min_lowest_price". Some of the names are not at all descriptive, such as using "i" for counting the number of input lines read.

    5. Some of the indents are strange and inconsistent. Doesn't affect functionality but we should be making code as legible as we can, not only for ourselves but for future users/editors of the code.
*/
