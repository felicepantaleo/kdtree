#include "FKDTree.h"
#include "KDPoint.h"
#include <chrono>

int main()
{
	int nPoints = 100000000;
	FKDTree<float, 3> kdtree(nPoints);
	std::vector<KDPoint<float, 3> > points;

	KDPoint<float,3> minPoint ( 0, 1, 8);
	KDPoint<float,3> maxPoint ( 0.1, 1.2, 8.3);
	for(int i = 0; i<nPoints; ++i)
	{
		float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10.1));;
		float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10.1));;
		float z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10.1));
		KDPoint<float, 3> Point(x,y,z);
		Point.setId(i);
		kdtree.push_back(Point);

		points.push_back(Point);

	}
    std::chrono::steady_clock::time_point start_building = std::chrono::steady_clock::now();

	kdtree.build();
    std::chrono::steady_clock::time_point end_building = std::chrono::steady_clock::now();
    std::cout << "building kdtree with " << nPoints << " points took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_building - start_building ).count() << "ms" << std::endl;


//	if(kdtree.test_correct_build(0,0))
//		std::cout << "KDTree built correctly" << std::endl;
//	else
//		std::cerr << "KDTree wrong" << std::endl;
    std::chrono::steady_clock::time_point start_searching = std::chrono::steady_clock::now();

	std::vector<KDPoint<float, 3> > points_in_the_box = kdtree.search_in_the_box(minPoint, maxPoint);
    std::chrono::steady_clock::time_point end_searching = std::chrono::steady_clock::now();

    std::cout << "searching points using kdtree took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_searching - start_searching ).count() << "ms" << std::endl;

	std::cout <<points_in_the_box.size() <<  " points found using kdtree" << std::endl;
//	for(auto p : points_in_the_box)
//		p.print();
	int pointsFoundNaive = 0;
	start_searching = std::chrono::steady_clock::now();
	for(auto p:points)
	{
		bool inTheBox = true;

		for(int i =0; i< 3; ++i)
		{

				inTheBox &= (p[i] <= maxPoint[i]
						&& p[i] >= minPoint[i] );



		}

		if(inTheBox)
			pointsFoundNaive++;
	}
	end_searching = std::chrono::steady_clock::now();
	std::cout << pointsFoundNaive << " points found naive in "  << std::chrono::duration_cast<std::chrono::milliseconds>(end_searching - start_searching ).count() << "ms" << std::endl;

	assert(pointsFoundNaive == points_in_the_box.size());





	return 0;
}
