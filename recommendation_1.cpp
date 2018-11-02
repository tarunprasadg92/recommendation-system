#include "recommendation.h"

using namespace std;

// Helper function for sorting the neighbors in ascending order
bool ascending(const pair<string, float>& a1, const pair<string, float>& a2)
{
    return a1.second < a2.second;
}

// Helper function for sorting the neighbors in descending order
bool descending(const pair<string, float>& a1, const pair<string, float>& a2)
{
    return a1.second > a2.second;
}

// Default constructor - reading the data into map
recommendation::recommendation()
{
    // Open data file for reading contents
    ifstream ifs("Band Ratings.csv", ios::in);
    string line;
    
    // Read the entries
    while (ifs.good())
    {
        getline(ifs, line);
        if (line != "")
        {
            // Read one line at a time
            stringstream ss(line);
            string book_name, reviewer_name, rating;
            
            // Split into fields and store them into corresponding variables
            getline(ss, book_name, ',');
            getline(ss, reviewer_name, ',');
            getline(ss, rating, ',');
            
            // Insert the data into map
            book_data[reviewer_name].insert(make_pair(book_name, rating));
        }
    }
    
    ifs.close();
}

// Helper function to verify if the contents have been read correctly
void recommendation::print_data()
{
    // Iterating over every user present
    for (map<string, map<string, string> >::iterator it = book_data.begin(); it != book_data.end(); ++it)
    {
        cout << it->first << "\n\n";
        
        map<string, string> &internal_map = it->second;
        
        // Iterate over the map of the books and rating given by that user
        for (map<string, string>::iterator it2 = internal_map.begin(); it2 != internal_map.end(); ++it2)
        {
            cout << it2->first << "\t" << it2->second << endl;
        }
        cout << "---------------------" << endl;
        cout << "\n";
    }
}

// Function to compute the nearest neighbors using all the 4 techniques
void recommendation::compute_nearest_neighbors(string name)
{
    map<string, string> &user = book_data[name];
    
    // Check if the user has entered a valid/existing name
    if (user.empty())
    {
        cout << "Invalid user / No details found" << endl;
        exit(EXIT_FAILURE);
    }
    
    // Variables for computing Manhattan and Euclidean distance
    float manhattan_difference = 0.0;
    float euclidean_difference = 0.0;

    // Vector to hold the neighbors
    vector<pair<string, float> > manhattan_distance;
    vector<pair<string, float> > euclidean_distance;
    vector<pair<string, float> > pearson_distance;
    vector<pair<string, float> > cosine_distance;
    
    // Iterate over all the users
    for (map<string, map<string, string> >::iterator it = book_data.begin(); it != book_data.end(); ++it)
    {
        // Skip if the entry is the given user
        if (it->first == name)
        {
            continue;
        }
        
        map<string, string> &internal_map = it->second;
        
        // Variables for computing distance using Pearson and Cosine
        int n = 0;
        float s_x = 0.0;
        float s_y = 0.0;
        float s_xsq = 0.0;
        float s_ysq = 0.0;
        float s_xy = 0.0;
        
        // Iterate over every book the user has rated
        for (map<string, string>::iterator it2 = user.begin(); it2 != user.end(); ++it2)
        {
            // If the other person has also rated the same book, we can compute the distance
            if (internal_map.find(it2->first) != internal_map.end())
            {
                manhattan_difference += abs(atof(internal_map[it2->first].c_str()) - atof((it2->second).c_str()));
                euclidean_difference += pow(abs(atof(internal_map[it2->first].c_str()) - atof((it2->second).c_str())), 2);
                
                float x = atof((it2->second).c_str());
                float y = atof(internal_map[it2->first].c_str());
                s_x += x;
                s_y += y;
                s_xy += x * y;
                s_xsq += pow(x, 2);
                s_ysq += pow(y, 2);
                n++;
            }
        }
        
        // Store the name of the user and the distance in the corresponding vector based on the method
        manhattan_distance.push_back(make_pair(it->first, manhattan_difference));
        euclidean_distance.push_back(make_pair(it->first, sqrt(euclidean_difference)));
        pearson_distance.push_back(make_pair(it->first, ((n * s_xy) - (s_x * s_y)) / (sqrt(((n * s_xsq) - pow(s_x, 2)) * ((n * s_ysq) - pow(s_y, 2))))));
        cosine_distance.push_back(make_pair(it->first, (s_xy / (sqrt(s_xsq) * sqrt(s_ysq)))));
        
        manhattan_difference = 0.0;
        euclidean_difference = 0.0;
    }
    
    // Sort the names in the vector based on the distance
    sort(manhattan_distance.begin(), manhattan_distance.end(), ascending);
    sort(euclidean_distance.begin(), euclidean_distance.end(), ascending);
    sort(pearson_distance.begin(), pearson_distance.end(), descending);
    sort(cosine_distance.begin(), cosine_distance.end(), descending);
    
    // Print out the neighbors
    print_neighbors(manhattan_distance, "Manhattan");
    print_neighbors(euclidean_distance, "Euclidean");
    print_neighbors(pearson_distance, "Pearson");
    print_neighbors(cosine_distance, "Cosine");
}

// Function to print out the neighbors
void recommendation::print_neighbors(vector<pair<string, float> > neighbors, string method)
{
    // Counter variable to extract the closest neighbor
    int insert_counter = 0;
    
    cout << "Nearest neighbors (" << method << ") : ";
    
    // Iterate over the vector of neighbors
    for (vector<pair<string, float> >::iterator it = neighbors.begin(); it != neighbors.end(); ++it)
    {
        cout << it->first << " " << it->second << " | ";
        
        // The closest neighbor is stored in the corresponding variable
        if (insert_counter == 0)
        {
            if (method == "Manhattan")
                manhattan_neighbor = it->first;
            else if (method == "Euclidean")
                euclidean_neighbor = it->first;
            else if (method == "Pearson")
                pearson_neighbor = it->first;
            else if (method == "Cosine")
                cosine_neighbor = it->first;

            insert_counter++;
        }
    }
    cout << endl;
}

// Function to recommend a book
void recommendation::recommend(string name)
{
    // Getting the user's book-rating details and the neighbor's book-rating details
    map<string, string> &user_data = book_data[name];
    map<string, string> &m_neighbor_data = book_data[manhattan_neighbor];
    map<string, string> &e_neighbor_data = book_data[euclidean_neighbor];
    map<string, string> &p_neighbor_data = book_data[pearson_neighbor];
    map<string, string> &c_neighbor_data = book_data[cosine_neighbor];
    
    // Print out the results based on the neighbor
    cout << "\nRecommendations (Manhattan) : " << endl;
    print_result(user_data, m_neighbor_data);
    cout << "\nRecommendations (Euclidean) : " << endl;
    print_result(user_data, e_neighbor_data);
    cout << "\nRecommendations (Pearson) : " << endl;
    print_result(user_data, p_neighbor_data);
    cout << "\nRecommendations (Cosine) : " << endl;
    print_result(user_data, c_neighbor_data);
}

// Function to print the recommendations
void recommendation::print_result(map<string, string> user_data, map<string, string> neighbor_data)
{
    // Iterate over the book-rating details of the neighbor
    for (map<string, string>::iterator it = neighbor_data.begin(); it != neighbor_data.end(); ++it)
    {
        // For every book the neighbor has rated and the user has not, print them
        if (user_data.find(it->first) == user_data.end())
        {
            cout << it->first << "\t" << it->second << "\n";
        }
    }
}
