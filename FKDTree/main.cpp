#include "FKDTree.h"
#include "KDPoint.h"
#include <chrono>
#include "KDTreeLinkerAlgoT.h"
typedef struct float4 {
    float x;
    float y;
    float z;
    float w;
} float4;

int main()
{
	int nPoints = 100000;
	FKDTree<float, 3> kdtree(nPoints);
	std::vector<KDPoint<float, 3> > points;
	std::vector<KDPoint<float, 3> > minPoints;
	std::vector<KDPoint<float, 3> > maxPoints;

	float range_x = 1;
	float range_y = 2;
	float range_z = 1;


	KDPoint<float,3> minPoint ( 0, 1, 8);
	KDPoint<float,3> maxPoint ( 1, 2, 8.3);
	for(int i = 0; i<nPoints; ++i)
	{
		float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10.1));;
		float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10.1));;
		float z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10.1));
		KDPoint<float, 3> Point(x,y,z);
		Point.setId(i);

		points.push_back(Point);
		KDPoint<float, 3> m(x-range_x,y-range_y, z-range_z );
		minPoints.push_back(m);
		KDPoint<float, 3> M(x+range_x,y+range_y, z+range_z );
		maxPoints.push_back(M);

	}
    std::chrono::steady_clock::time_point start_building = std::chrono::steady_clock::now();

	for(int i = 0; i<nPoints; ++i)
	{
		kdtree.push_back(points[i]);
	}

	kdtree.build();
    std::chrono::steady_clock::time_point end_building = std::chrono::steady_clock::now();
    std::cout << "building kdtree with " << nPoints << " points took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_building - start_building ).count() << "ms" << std::endl;


//	if(kdtree.test_correct_build(0,0))
//		std::cout << "KDTree built correctly" << std::endl;
//	else
//		std::cerr << "KDTree wrong" << std::endl;
    std::chrono::steady_clock::time_point start_searching = std::chrono::steady_clock::now();
    for(int i = 0; i< nPoints; ++i)
    	kdtree.search_in_the_box(minPoints[i], maxPoints[i]);
    std::chrono::steady_clock::time_point end_searching = std::chrono::steady_clock::now();

    std::cout << "searching points using kdtree took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_searching - start_searching ).count() << "ms" << std::endl;

//	std::cout <<points_in_the_box.size() <<  " points found using kdtree" << std::endl;
//	for(auto p : points_in_the_box)
//		p.print();
//	int pointsFoundNaive = 0;


//	start_searching = std::chrono::steady_clock::now();
//	for(auto p:points)
//	{
//		bool inTheBox = true;
//
//		for(int i =0; i< 3; ++i)
//		{
//
//				inTheBox &= (p[i] <= maxPoint[i]
//						&& p[i] >= minPoint[i] );
//
//
//
//		}
//
//		if(inTheBox)
//			pointsFoundNaive++;
//	}
//	end_searching = std::chrono::steady_clock::now();
//	std::cout  << " points found naive in "  << std::chrono::duration_cast<std::chrono::milliseconds>(end_searching - start_searching ).count() << "ms" << std::endl;

//	assert(pointsFoundNaive == points_in_the_box.size());


	float4* cmssw_points;
	float4* cmssw_range;
	KDTreeLinkerAlgo<unsigned, 3> vanilla_tree;
	std::vector<KDTreeNodeInfoT<unsigned, 3> > vanilla_nodes;
	std::vector<KDTreeNodeInfoT<unsigned, 3> > vanilla_founds;

	std::array<float, 3> minpos { { 0.0f, 0.0f, 0.0f } }, maxpos { { 0.0f, 0.0f, 0.0f } };

	cmssw_points = new float4[nPoints];
	cmssw_range = new float4[nPoints];
	for (int j = 0; j < nPoints; j++) {
		cmssw_points[j].x = points[j][0];
		cmssw_points[j].y = points[j][1];
		cmssw_points[j].z = points[j][2];
		cmssw_points[j].w = j; //Use this to save the index
		cmssw_range[j] = {1.1, 1.1, 1.1, j};

	}
	vanilla_tree.clear();
	vanilla_founds.clear();
	start_building = std::chrono::steady_clock::now();
	for (unsigned i = 0; i < nPoints; ++i) {
		float4 pos = cmssw_points[i];
		vanilla_nodes.emplace_back(i, (float) pos.x, (float) pos.y, (float) pos.z);
		if (i == 0) {
			minpos[0] = pos.x;
			minpos[1] = pos.y;
			minpos[2] = pos.z;
			maxpos[0] = pos.x;
			maxpos[1] = pos.y;
			maxpos[2] = pos.z;
		} else {
			minpos[0] = std::min((float) pos.x, minpos[0]);
			minpos[1] = std::min((float) pos.y, minpos[1]);
			minpos[2] = std::min((float) pos.z, minpos[2]);
			maxpos[0] = std::max((float) pos.x, maxpos[0]);
			maxpos[1] = std::max((float) pos.y, maxpos[1]);
			maxpos[2] = std::max((float) pos.z, maxpos[2]);
		}
	}


	KDTreeCube cluster_bounds = KDTreeCube(minpos[0], maxpos[0], minpos[1], maxpos[1], minpos[2], maxpos[2]);

	vanilla_tree.build(vanilla_nodes, cluster_bounds);
	end_building = std::chrono::steady_clock::now();
    start_searching = std::chrono::steady_clock::now();
    for(int i = 0; i< nPoints; ++i)
    {
	KDTreeCube kd_searchcube(minPoints[i][0], maxPoints[i][0],minPoints[i][1], maxPoints[i][1], minPoints[i][2], maxPoints[i][2]);
	vanilla_tree.search(kd_searchcube, vanilla_founds);
    }
	end_searching = std::chrono::steady_clock::now();

    std::cout << "building cmssw kdtree with " << nPoints << " points took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_building - start_building ).count() << "ms" << std::endl;
    std::cout << "searching points using cmssw kdtree took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_searching - start_searching ).count() << "ms" << std::endl;
	std::cout << vanilla_founds.size() << " points found cmssw in "  << std::chrono::duration_cast<std::chrono::milliseconds>(end_searching - start_searching ).count() << "ms" << std::endl;

	delete[] cmssw_points;
	delete[] cmssw_range;




	return 0;
}
