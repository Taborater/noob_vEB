#ifndef vEB_class_hpp
#define vEB_class_hpp

#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

class vEB_Tree {

private:
	int u;		//Universum size
	int min;	//Min element of this vEB three
	int max;	//Max element of this vEB three
	vEB_Tree* summary;
	vector<vEB_Tree*> cluster;

public:


	vEB_Tree(int un_size = 16) {
		//...Add check for u if u is a power of 2
		u = un_size;

		//-1 mean 'non set'
		min = -1;		
		max = -1;
		summary = NULL;

		//After loop clusters contain sqrt(u) NULL pointers
		//
		//By using pow ,log and ceil get Lower square root
		//So when squre root NOT integer
		//we compute this root and get highter number
		//that is power of 2
		int numberOfClusters = pow(2, (int)(ceil((log2(u)/2))));
		
		for(int i = 0; i < numberOfClusters; ++i) {
			vEB_Tree* null_vEB_Tree = NULL;
			cluster.push_back(null_vEB_Tree);
		}
	}

//Subfunctions

	//Get the cluster num for x
	//
	//By using pow and log get Lower square root
	//So when squre root NOT integer
	//we compute this root and get lower number
	//that is power of 2
	//(same rool in low() and index())
	int hight(int x) {

		return (int)(x / (pow(2, (int)(log2(u)/2))));
	}

	//Get the position in cluster for x
	//Every cluster start indexing from 0
	int low(int x) {

		return (int)(x % (pow(2, (int)(log2(u)/2))));
	}

	//Take x(cluster number) and y(position in cluster)
	//Return the original number
	int index(int x, int y) {

		return (x*(pow(2, (int)(log2(u)/2))) + y);
	}

	//If vEB tree is empty, inserted element will be stored in min, not in clusters
	//And if vEB three has onlu one element, it will be max and min in the same time
	void vEB_Empty_Tree_Insert(int x) {
		min = x;
		max = x;
	}

//Mainfunctions

	int vEB_Tree_Min() {
		return min;
	}

	int vEB_Tree_Max() {
		return max;
	}

	//Need comment
	void vEB_Tree_Insert(int x) {
		//Need tmp for swaping x and min
		int tmp;

		//Basic case - tree is empty
		if(min == -1) {
			vEB_Empty_Tree_Insert(x);
			return;

		//Min already exists, min > x
		//After swap x become min and need new place for min
		} else if(x < min) {
			tmp = min;
			min = x;
			x = tmp;
		}


		//Not basic case - need cluster check
		if(u > 2) {

			//If cluster[hight(x)] not exists - allocate it
 			if(cluster[hight(x)] == NULL) {

 				int numberOfClusters = pow(2, (int)(log2(u)/2));
				vEB_Tree* buffTree = new vEB_Tree(numberOfClusters);
				cluster[hight(x)] = buffTree;
			}

			//Check emptyness of cluster
			if(cluster[hight(x)]->vEB_Tree_Min() == -1) {

				//If summory for this three not exists = allocate it
				if(summary == NULL) {
					int numberOfClusters = pow(2, (int)(ceil((log2(u)/2))));
					vEB_Tree* buffSummaryTree = new vEB_Tree(numberOfClusters);
					summary = buffSummaryTree;
				}

				//Need to refresh local summary info about x cluster (using hight(x))
				summary->vEB_Tree_Insert(hight(x));

				//And perform vEB_Empty_Tree_Insert for cluster
				cluster[hight(x)]->vEB_Empty_Tree_Insert(low(x));

			} else {
				//Cluster already has some data - adding by low(x) position
				cluster[hight(x)]->vEB_Tree_Insert(low(x));
			}
		}

		if(x > max) {
			max = x;
		}
	}

	//Understandable
	void vEB_Tree_Delete(int x) {

		//Basic case 0 - only one unit in tree
		if(min == x && max == x) {
			min = -1;
			max = -1;
			return;

		//Basic case 1 - only 0 and 1 in tree
		} else if(u == 2) {
			if(x == min && x == 0)
				min = 1;
			else if(x == min && x == 1)
				min = 0;
			max = min;
			return;

		//Not basic case
		} else {

			if(x == min) {
				//Get the cluster with the first minimum after min
				int firstMinCluster = summary->vEB_Tree_Min();

				//Get the min in founded cluster
				x = index(firstMinCluster, cluster[firstMinCluster]->vEB_Tree_Min());
				min = x;
			}


			//Delete x from cluster
			if(cluster[hight(x)] != NULL) 
				cluster[hight(x)]->vEB_Tree_Delete(low(x));


			//Cluster become empty?
			if(cluster[hight(x)]->vEB_Tree_Min() == -1) {

				//Clear memory
				delete cluster[hight(x)];
				cluster[hight(x)] = NULL;	
				//Refresh summary
				summary->vEB_Tree_Delete(hight(x));

				//Deleted was max
				if(x == max) {
					//Get new max
					int summaryMax = summary->vEB_Tree_Max();

					//No new max - only 1 unit in tree exists
					if(summaryMax == -1)
						max = min;
					else
					//Need comment
						max = index(summaryMax, cluster[summaryMax]->vEB_Tree_Max());
				}

			//Cluster not empty after del
			//Need to refresh max?
			} else if(x == max)
				max = index(hight(x), cluster[hight(x)]->vEB_Tree_Max());
		}
	}

	int vEB_Tree_Successor(int x) {

		//Basic case 0 - only 2 elements
		if(u == 2) {
			if(x == 0 && max == 1)
				return 1;
			else
				return -1;

		//We came to the cluster and it's min > x
		//So this min is succesor
		} else if(min != -1 && x < min) {
			return min;

		//Not basic case and x >= min
		} else if(min == max) {
			//We nee to controll this cause u maybe > 2, but ther only 2 elements
			return x;

		} else {

			//Get max in cluster where x stored

			int max_low = -1;

			//If x = min - it is not in the any of clusters
			if(cluster[hight(x)] != NULL)
				max_low = cluster[hight(x)]->vEB_Tree_Max();


			//Not basic case #0 
			//If got max and position of x in this cluster is lower than that max
			if(max_low != -1 && low(x) < max_low) {

				//So lets find succersor in this cluster
				int offset = cluster[hight(x)]->vEB_Tree_Successor(low(x));
				return index(hight(x), offset);		//Done


			//Not basic case #1 
			} else {

				//If x == max, lets look in summary for next existing cluster
				int succ_cluster = summary->vEB_Tree_Successor(hight(x));
		
				//Case: x == global max
				if(succ_cluster == -1) {
					return -1;
				} else {

					//Same in the Not Basic case #0
					//We found succ cluster and perform search there
					if(cluster[succ_cluster] != NULL) {
						int offset = cluster[succ_cluster]->vEB_Tree_Min();
						return index(succ_cluster, offset);
					} else {
						return -1;
					}
				}
			}

		}	
	}

	int vEB_Tree_Predecessor(int x) {

		//Basic case 0 - only 2 elements
		if(u == 2) {
			if(x == 1 && min == 0)
				return 0;
			else
				return -1;

		//We came to the cluster and it's x > max
		//So this max is predecesor
		} else if(max != -1 && x > max) {
			return max;

		//Not basic case and x < max
		} else if(min == max) {
			//We nee to controll this cause u maybe > 2, but ther only 2 elements
			return x;

		} else {

			//Get min in cluster where x stored

			int min_low = -1;

			if(cluster[hight(x)] != NULL)
				min_low = cluster[hight(x)]->vEB_Tree_Min();


			//Not basic case #0 
			//If got min and position of x in this cluster is bigger than that min
			if(min_low != -1 && low(x) > min_low) {

				//So lets find predecersor in this cluster
				int offset = cluster[hight(x)]->vEB_Tree_Predecessor(low(x));
				return index(hight(x), offset);		//Done


			//Not basic case #1 
			} else {

				//If x == min, lets look in summary for previous existing cluster
				int pred_cluster = summary->vEB_Tree_Predecessor(hight(x));
		
				//Case: x == global min
				if(pred_cluster == -1) {
					if(min != -1 && x > min)
						return min;
					else
						return -1;
				} else {

					//Same in the Not Basic case #0
					//We found pred cluster and perform search there
					if(cluster[pred_cluster] != NULL) {
						int offset = cluster[pred_cluster]->vEB_Tree_Max();
						return index(pred_cluster, offset);
					} else {
						return -1;
					}
				}
			}

		}	
	}

	//Member of three or not
	bool vEB_Tree_Member(int x) {

		//Basic cases (=min or =max) or (!=min !=max and u = 2)
		if(x == min || x == max)
			return true;
		else if(u == 2)
			return false;
		else
			//Not basic case (u > 2) and (!=min and max)
			//So get the cluster index of x using hight(x)
			//and recursively search in this cluster using the position
			//by low(x)
			if(cluster[hight(x)] != NULL)
				return cluster[hight(x)]->vEB_Tree_Member(low(x));
			else
				return false;
	}

};



#endif