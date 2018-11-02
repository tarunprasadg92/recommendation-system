#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

class recommendation
{
private:
    // Map containing all the data read from file
    map<string, map<string, string> > book_data;
    
    // Map containing the ISBN and the title of the book
    map<string, string> book_details;
    
    // String variables storing the name of the nearest neighbor
    string manhattan_neighbor;
    string euclidean_neighbor;
    string pearson_neighbor;
    string cosine_neighbor;
    
    // Nearest neighbor computation using Manhattan technique
    void manhattan_technique(string name);
    
    // Nearest neighbor computation using Euclidean technique
    void euclidean_technique(string name);
    
    // Nearest neighbor computation using Pearson co-efficient
    void pearson_technique(string name);
    
    // Nearest neighbor computation using cosine similarity
    void cosine_similarity(string name);
    
    // Print out the neighbors
    void print_neighbors(vector<pair<string, float> > neighbors, string method);
    
    // Print the recommended output
    void print_result(map<string, string> user_data, map<string, string> neighbor_data);   

public:
    // Default constructor
    recommendation();
    
    // Print the contents in the map
    void print_data();
    
    // Compute nearest neighbors 
    void compute_nearest_neighbors(string name);
    
    // Function to recommend books for the above given user name
    void recommend(string name);
};

#endif
