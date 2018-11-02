CXX=g++

test: recommendation_test_1 recommendation_test_2

recommendation_test_1: recommendation_test_1.cpp recommendation_1.cpp recommendation.h
	$(CXX) recommendation_test_1.cpp recommendation_1.cpp -g -o recommendation_1

recommendation_test_2: recommendation_test_2.cpp recommendation_2.cpp recommendation.h
	$(CXX) recommendation_test_2.cpp recommendation_2.cpp -g -o recommendation_2

clean:
	rm -fr recommendation_1 recommendation_2
