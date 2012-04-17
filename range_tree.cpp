#include <iostream>
#include <cstdlib>
#include <algorithm>
#define EPSILON 0.00000000001
#define LEFT_NODE 0
#define RIGHT_NODE 1
using namespace std;


struct node
{
	pair<double, double> value;
	struct node* left;
	struct node* right;
	pair<double, double> *y_root;
	int Ny;
};


void merge(pair<double, double> *A, int La, pair<double, double>* B, int Lb, pair<double, double>* merged);
bool sortX (pair<double,double> i,pair<double,double> j);
bool sortY (pair<double,double> i,pair<double,double> j);
int searchTree(struct node* root, double x1, double x2, double y1, double y2);
int double_equal(double a, double b);
int double_lt(double a, double b);
int double_gt(double a, double b);
void inorder(struct node* root);
void preorder(struct node* root);
struct node* preprocessTree(int low, int high);
struct node* searchLeftLeaf(struct node* root, double low);
struct node* searchRightLeaf(struct node* root, double high);
struct node *LCA(struct node *root, struct node *p, struct node *q);
int findLeftSubtree(struct node *root, struct node* parent, int x1, int y1, int y2, int checkAncestor, int type);
pair<double,double> *p_sx; 
int main()
{
	//Takes in N points <x,y> and R <x1,x2,y1,y2> Ranges
	//x1<x2; y1<y2
	int N, R, i; 
	cin>>N>>R;

	p_sx = (pair<double,double>*)malloc(sizeof(pair<double,double>)*N);
	
	pair<double,double> r_x[R];
	pair<double,double> r_y[R];
	double a, b, c, d;
	//Input pointset
	for(i=0;i<N;i++)
	{
		cin>>p_sx[i].first>>p_sx[i].second;

	
	}
	
	for(i=0;i<R;i++)
	{
		cin>> a >> b >> c >> d;
		
		//Ensuring x1 < x2 & y1 < y2 
		// Handle degenrate case of x1 = x2 / y1 = y2?
		
		if(double_lt(a,b))
		{
			r_x[i].first = a;
			r_x[i].second = b;
		} 
		else
		{
			r_x[i].first = b;
			r_x[i].second = a;
		}
		
		if(double_lt(c,d))
		{
			r_y[i].first = c;
			r_y[i].second = d;
		} 
		else
		{
			r_y[i].first = d;
			r_y[i].second = c;
		}
		
	}
	//points sorted by the x coordinate
	
	sort(p_sx, p_sx+N, sortX);
	
	struct node* root;
	root = preprocessTree(0, N-1);
	free(p_sx);
	
/*	cout<<"Root\n"<<root->value.first<< " "<<root->value.second;
	cout<<"Inorder \n";
	inorder(root);
	cout<<"Preorder \n";
	preorder(root);
*/
	for(i=0;i<R;i++)
	{
		cout<<"Range: "<<r_x[i].first<<" "<<r_x[i].second<<endl;
		searchTree(root, r_x[i].first, r_x[i].second, r_y[i].first, r_y[i].second);
	//	cout<<endl;
	}

}

struct node* preprocessTree(int low, int high)
{
	if(high < low)
	{
		return NULL;
	}
	
	pair<double, double>* y_subtree;
	
	int mid = (high + low)/2;
	struct node* root;
	root = new node;
	root->value = p_sx[mid];
	root->left =  preprocessTree(low, mid - 1);
	root->right = preprocessTree(mid + 1, high);
	
	if(root->left == NULL && root->right == NULL) // A leaf node
	{
		y_subtree = (pair<double, double>*) malloc(sizeof(double));
		*y_subtree = p_sx[mid];
		//cout<<"Allocated "<< p_sx[mid].second <<" to leaf "<< p_sx[mid].first<<" "<<p_sx[mid].second;
		root->Ny = 1;
		root->y_root = y_subtree;
		//cout<<"Allocated "<< *root->y_root <<" to leaf "<< p_sx[mid].first<<" "<<p_sx[mid].second;
	}
	else if (root->left ==NULL)
	{
		int i= 0;
		int y_tree_len = root->right->Ny + 1; // Current node's information
		y_subtree = (pair<double, double>*) malloc( y_tree_len * sizeof(pair<double, double>) );
		
		for(i = 0; i<y_tree_len - 1; i++)
		{
			y_subtree[i] =  root->right->y_root[i];
		}
		
		i=0;
		
		while(y_subtree[i].second < root->value.second && i < (y_tree_len -1) )i++;
		
		pair<double, double> temp = y_subtree[i]; 
		
		y_subtree[i] = root->value;
		i++;
		
		while(i < y_tree_len)
		{
			pair<double, double> k = y_subtree[i];
			y_subtree[i] = temp;
			temp = k;
			i++;
		}
		
		//y_root now points to the merged subtree
		root->y_root = y_subtree;
		root->Ny = y_tree_len;
	}
	else if (root->right ==NULL)
	{
		int i= 0;
		int y_tree_len = root->left->Ny + 1; // Current node's information
		y_subtree = (pair<double, double>*) malloc( y_tree_len * sizeof(pair<double, double>) );
		
		for(i = 0; i<y_tree_len - 1; i++)
		{
			y_subtree[i] =  root->left->y_root[i];
		}
		
		i=0;
		
		while(y_subtree[i].second < root->value.second && i < (y_tree_len -1) )i++;
		
		pair<double, double> temp = y_subtree[i]; 
		y_subtree[i] = root->value;
		i++;
		
		while(i < y_tree_len)
		{
			pair<double, double> k = y_subtree[i];
			y_subtree[i] = temp;
			temp = k;
			i++;
		}
		
		//y_root now points to the merged subtree
		root->y_root = y_subtree;
		root->Ny = y_tree_len;
	}
	else if(root->left && root->right)
	{
		int i= 0;
		int y_tree_len = root->left->Ny + root->right->Ny + 1; // Current node's information
		y_subtree = (pair<double, double>*) malloc( y_tree_len * sizeof(pair<double, double>) );
		
		//merge 
		merge(root->left->y_root, root->left->Ny, root->right->y_root, root->right->Ny, y_subtree);
		//Insert root->value in the merged array
		while(y_subtree[i].second < root->value.second && i < (y_tree_len -1) )i++;
		
		pair<double, double> temp = y_subtree[i]; 
		y_subtree[i] = root->value;
		i++;
		
		while(i < y_tree_len)
		{
			pair<double, double> k = y_subtree[i];
			y_subtree[i] = temp;
			temp = k;
			i++;
		}
		
		//y_root now points to the merged subtree
		root->y_root = y_subtree;
		root->Ny = y_tree_len;
	}
	
	return root;

}

//merges A and B into merged

void merge(pair<double, double> *A, int La, pair<double, double>* B, int Lb, pair<double, double>* merged)
{
	int i=0, j=0, k=0;
	while(i < La && j< Lb)
	{
		if(double_lt(A[i].second,B[j].second)) merged[k++] = A[i++];
		else merged[k++] = B[j++];
	}	
	while(i < La) merged[k++] = A[i++];
	while(j < Lb) merged[k++] = B[j++]; 
	
}

int searchTree(struct node* root, double x1, double x2, double y1, double y2)
{
	struct node* left_leaf = searchLeftLeaf(root, x1);
	struct node* right_leaf = searchRightLeaf(root, x2);
	struct node* lca = LCA(root, left_leaf, right_leaf);
	/*cout<<"Left Leaf "<<left_leaf->value.first<<" "<<left_leaf->value.second<<endl;
	cout<<"Right Leaf "<<right_leaf->value.first<<" "<<right_leaf->value.second<<endl;
	cout<<"LCA "<<lca->value.first<<" "<<lca->value.second<<endl;*/
	
	int left_result = findLeftSubtree(lca->left, lca, x1, y1, y2, 0, LEFT_NODE);
//	int right_result = findLeftSubtree(lca, lca->left, x1, y1, y2, 0);
}

int findLeftSubtree(struct node *root, struct node* parent, int x1, int y1, int y2, int checkAncestor, int type)
{
	int count = 0;
	if(checkAncestor == 1)
	{
		if(type == LEFT_NODE)
		{
			// Search in the parent's y subtree the range of y values
			if(parent->y_root)
			{
				
			}
		}
	}
	
	
}

struct node* searchLeftLeaf(struct node* root, double low)
{
	if(!root->left && !root->right)return root; 
	
	if(double_gt(low, root->value.first))
	{
		if(root->right)
			return searchLeftLeaf(root->right, low);
		else return root->left;    //if root->right isnt true, root->left has to be true and the leaf too! 
					  // This is because the tree is balanced
	}
	else //equal case
	{
		if(root->left)
			return searchLeftLeaf(root->left, low);
		else return root->right;  //if root->left isnt true, root->RIGHT has to be true and the leaf too! 
	}				  // This is because the tree is balanced
}

struct node* searchRightLeaf(struct node* root, double high)
{
	if(!root->left && !root->right)return root; 
	
	if(double_gt(high, root->value.first) || double_equal(high, root->value.first))
	{
		if(root->right)
			return searchLeftLeaf(root->right, high);
		else return root->left;    //if root->right isnt true, root->left has to be true and the leaf too! 
					  // This is because the tree is balanced
	}
	else 
	{
		if(root->left)
			return searchLeftLeaf(root->left, high);
		else return root->right;  //if root->left isnt true, root->RIGHT has to be true and the leaf too! 
	}	
}


struct node *LCA(struct node *root, struct node *p, struct node *q) {
  if (!root || !p || !q) return NULL;
  if (max(p->value.first, q->value.first) < root->value.first)
    return LCA(root->left, p, q);
  else if (min(p->value.first, q->value.first)  > root->value.first)
    return LCA(root->right, p, q);
  else
    return root;
}

void inorder(struct node* root)
{
	if (!root) return;
	inorder(root->left);
	
	cout<< "X "<<root->value.first<<" Y "<<root->value.second<<endl;
	
	inorder(root->right); 
}

void preorder(struct node* root)
{
	if (!root) return;
	cout<< "X "<<root->value.first<<" Y "<<root->value.second<<endl;
	cout<<"Sorted Y Array \n";
	
	int len = root->Ny;
	for(int i=0 ; i<len ; i++)
		cout<<root->y_root[i].first<< " "<<root->y_root[i].second<<" | ";
	
	cout<<endl;
	preorder(root->left);	
	preorder(root->right); 
}

int double_equal(double a, double b)
{
	if( ((a-b) < EPSILON && (a-b) >= 0) || ((b-a) < EPSILON && (b-a) >= 0) )
		return 1;
	return 0;
	     
}

int double_lt(double a, double b)
{
	if((b - a) > EPSILON) return 1;
	return 0;
}


int double_gt(double a, double b)
{
	if((a - b) > EPSILON) return 1;
	return 0;
}


bool sortX (pair<double,double> i,pair<double,double> j) 
{ 
	if(i.first < j.first)
		return true;
	else if (i.first == j.first && i.second < j.second)
		return true;
	else return false;	
}

bool sortY (pair<double,double> i,pair<double,double> j) 
{ 
	if(i.second < j.second)
		return true;
	else if (i.second == j.second && i.first < j.first)
		return true;
	else return false;	
}