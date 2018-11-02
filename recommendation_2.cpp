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
    string line;
    
    // Create the map of ISBN and book title
    ifstream ifs("BBooks.csv", ios::in);
        
    // Read the entries
    while (ifs.good())
    {
        getline(ifs, line);
        if (line != "")
        {
            // Read line from the file
            stringstream ss(line);
            string isbn, book_name;
            
            // Split the fields and store them into appropriate variables
            getline(ss, isbn, ';');
            getline(ss, book_name, ';');
            
            // Store them into the map
            book_details[isbn] = book_name;
        }
    }
    
    ifs.close();
    
    // Create the map of map containing Users and book and rating
    ifstream iifs("BRatings.csv", ios::in);
    
    // Read the entries
    while (iifs.good())
    {
        getline(iifs, line);
        if (line != "")
        {
            // Read line from the file
            stringstream ss(line);
            string user, isbn, rating;
            
            // Split the fields and store them into appropriate variables
            getline(ss, user, ';');
            getline(ss, isbn, ';');
            getline(ss, rating, ';');
            
            // Store them into the map
            book_data[user].insert(make_pair(isbn, rating));
        }
    }
    
    iifs.close();
}

// Helper function to verify if the data is stored correctly
void recommendation::print_data()
{
    cout << "Printing the book details : " << endl;
    
    for (map<string, string>::iterator it = book_details.begin(); it != book_details.end(); ++it)
    {
        cout << "ISBN : " << it->first << " Book Name : " << it->second << endl;
    }
    
    cout << "Printing the Users and their ratings : " << endl;
    for (map<string, map<string, string> >::iterator it = book_data.begin(); it != book_data.end(); ++it)
    {
        cout << "User : " << it->first << "\n\n";
        
        map<string, string> &internal_map = it->second;
        for (map<string, string>::iterator it2 = internal_map.begin(); it2 != internal_map.end(); ++it2)
        {
            cout << it2->first << "\t" << it2->second << endl;
        }
        cout << "---------------------" << endl;
        cout << "\n";
    }
}

// Function to compute the nearest neighbors using the 4 distance measuring techniques
void recommendation::compute_nearest_neighbors(string name)
{
    map<string, string> &user = book_data[name];
    
    if (user.empty())
    {
        cout << "Invalid user / No details found" << endl;
        exit(EXIT_FAILURE);
    }
    
    float manhattan_difference = 0.0;
    float euclidean_difference = 0.0;

    vector<pair<string, float> > manhattan_distance;
    vector<pair<string, float> > euclidean_distance;
    vector<pair<string, float> > pearson_distance;
    vector<pair<string, float> > cosine_distance;
    
    for (map<string, map<string, string> >::iterator it = book_data.begin(); it != book_data.end(); ++it)
    {
        if (it->first == name)
        {
            continue;
        }
        
        map<string, string> &internal_map = it->second;
        
        int n = 0;
        float s_x = 0.0;
        float s_y = 0.0;
        float s_xsq = 0.0;
        float s_ysq = 0.0;
        float s_xy = 0.0;
        
        for (map<string, string>::iterator it2 = user.begin(); it2 != user.end(); ++it2)
        {
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
        
        manhattan_distance.push_back(make_pair(it->first, manhattan_difference));
        euclidean_distance.push_back(make_pair(it->first, sqrt(euclidean_difference)));
        
        // Check if the result produced is a number. If not a number, its discarded
        if (!isnan(((n * s_xy) - (s_x * s_y)) / (sqrt(((n * s_xsq) - pow(s_x, 2)) * ((n * s_ysq) - pow(s_y, 2))))))
            pearson_distance.push_back(make_pair(it->first, ((n * s_xy) - (s_x * s_y)) / (sqrt(((n * s_xsq) - pow(s_x, 2)) * ((n * s_ysq) - pow(s_y, 2))))));
        
        // Check if the result produced is a number. If not a number, its discarded
        if (!isnan(s_xy / (sqrt(s_xsq) * sqrt(s_ysq))))
            cosine_distance.push_back(make_pair(it->first, (s_xy / (sqrt(s_xsq) * sqrt(s_ysq)))));
        
        manhattan_difference = 0.0;
        euclidean_difference = 0.0;
    }
    
    sort(manhattan_distance.begin(), manhattan_distance.end(), ascending);
    sort(euclidean_distance.begin(), euclidean_distance.end(), ascending);
    sort(pearson_distance.begin(), pearson_distance.end(), descending);
    sort(cosine_distance.begin(), cosine_distance.end(), descending);
    
    // Print neighbors for Manhattan and Euclidean is commented out as they produce a long list
    // print_neighbors(manhattan_distance, "Manhattan");
    // print_neighbors(euclidean_distance, "Euclidean");
    print_neighbors(pearson_distance, "Pearson");
    print_neighbors(cosine_distance, "Cosine");
}

// Function to print the nearest neighbors
void recommendation::print_neighbors(vector<pair<string, float> > neighbors, string method)
{
    int insert_counter = 0;
    cout << "Nearest neighbors (" << method << ") : ";
    for (vector<pair<string, float> >::iterator it = neighbors.begin(); it != neighbors.end(); ++it)
    {
        cout << it->first << " " << it->second << " | ";
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

// Function to recommend a book for the user 
void recommendation::recommend(string name)
{
    map<string, string> &user_data = book_data[name];
    map<string, string> &m_neighbor_data = book_data[manhattan_neighbor];
    map<string, string> &e_neighbor_data = book_data[euclidean_neighbor];
    map<string, string> &p_neighbor_data = book_data[pearson_neighbor];
    map<string, string> &c_neighbor_data = book_data[cosine_neighbor];
    
    cout << "\nRecommendations (Manhattan) : " << endl;
    print_result(user_data, m_neighbor_data);
    cout << "\nRecommendations (Euclidean) : " << endl;
    print_result(user_data, e_neighbor_data);
    cout << "\nRecommendations (Pearson) : " << endl;
    print_result(user_data, p_neighbor_data);
    cout << "\nRecommendations (Cosine) : " << endl;
    print_result(user_data, c_neighbor_data);
}

// Print out the top 20 books recommended for the user
void recommendation::print_result(map<string, string> user_data, map<string, string> neighbor_data)
{
    int counter = 0;
    for (map<string, string>::iterator it = neighbor_data.begin(); it != neighbor_data.end(); ++it)
    {
        if ((user_data.find(it->first) == user_data.end()) && (counter < 20))
        {
            cout << it->first << "\t" << book_details[it->first] << "\t" << it->second << "\n";
            counter++;
        }
    }
}
