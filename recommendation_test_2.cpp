#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "recommendation.h"

using namespace std;

int main()
{
    recommendation r;
    
    // r.print_data();
    
    string input_name;
    
    cout << "Enter user : ";
    cin >> input_name;
    cout << endl;
    r.compute_nearest_neighbors(input_name);
    
    r.recommend(input_name);
    
    return 0;
}
