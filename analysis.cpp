#include<iostream>
#include<fstream>
#include<vector>
using namespace std;

#define FREQ_TABLE_SIZE 1000000
int freq_table[1000000];
string code_table[1000000];


class huffman_tree_node{
public:	
	huffman_tree_node* left;
	huffman_tree_node* right;
	int val;
	huffman_tree_node(){
		left = NULL;
		right = NULL;
		val = -1;
	}

	huffman_tree_node(int v){
		left = NULL;
		right = NULL;
		val = v;
	}
};


class huffman_tree{
	huffman_tree_node* root;
	int sz;
public:
	huffman_tree(){
		root = NULL;
		sz = 0;
	}

	huffman_tree_node* getRoot(){
		return root;
	}

	void insert_root(huffman_tree_node* node){
		root = node;
		sz++;
	}

	void delete_subTree(huffman_tree_node* root){
		if (root == NULL)
			return;
		delete_subTree(root->left);
		delete_subTree(root->right);
		delete(root->left);
		delete(root->right);
	}

	void delete_tree(){
		delete_subTree(root);
		delete(root);
	}

	void combine(huffman_tree* second){
		huffman_tree_node* newRoot = new huffman_tree_node();
		newRoot->left = root;
		newRoot->right = second->root;
		sz += second->size();
		//cout<<root->val<<" "<<second->root->val<<" "<<sz<<endl;
		root = newRoot;
		delete(second);
	}

	int size(){
		return sz;
	}


};


// Heap structures


class heap_element{
public:
	int freq;
	huffman_tree* tree;
};


class binary_heap{
	vector< heap_element > arr;
public:
	void insert(heap_element newElem){
		arr.push_back(newElem);
		int idx = arr.size() - 1;
		while(idx != 0){
			if (arr[idx].freq < arr[(idx-1)/2].freq){
				swap(arr[idx], arr[(idx-1)/2]);
				idx = (idx - 1)/2;
			}
			else{
				break;
			}
		}
	}

	heap_element get_min(){
		heap_element mi = arr[0];
		arr[0] = arr.back();
		arr.pop_back();
		int idx = 0;
		int minChild = 2*idx + 1;;
		while(minChild < arr.size()){
			if (minChild+1 < arr.size() && arr[minChild].freq > arr[minChild+1].freq)
				minChild++;
			
			if (arr[idx].freq > arr[minChild].freq){
				swap(arr[idx], arr[minChild]);
				idx = minChild;
				minChild = 2*idx + 1;
			}
			else
				break;
		}
		return mi;
	}

	int size(){
		return arr.size();
	}

	void delete_heap(){
		heap_element tmp = get_min();
		tmp.tree->delete_tree();
	}

};


class four_way_heap{
	vector< heap_element > arr;
	
	public:
	four_way_heap(){
		heap_element tmp;
		for(int i=0;i<3;i++)
			arr.push_back(tmp);
	}

	void insert(heap_element newElem){
		arr.push_back(newElem);
		int idx = arr.size() - 1;
		while(idx > 3){
			if (arr[idx].freq < arr[idx/4+2].freq){
				swap(arr[idx], arr[idx/4+2]);
				idx = idx/4 + 2;
			}
			else{
				break;
			}
		}
	}

	heap_element get_min(){
		heap_element mi = arr[3];
		arr[3] = arr.back();
		arr.pop_back();
		int idx = 3;
		int minChild = 4*(idx - 2);
		while(minChild < arr.size()){
			int tmp=minChild;
			for(int i=1;i<4 && minChild+i<arr.size();i++){
				if (arr[tmp].freq > arr[minChild+i].freq)
					tmp = minChild + i;
			}
			minChild = tmp;
			
			if (arr[idx].freq > arr[minChild].freq){
				swap(arr[idx], arr[minChild]);
				idx = minChild;
				minChild = 4*(idx - 2);
			}
			else
				break;
		}
		return mi;	
	}

	int size(){
		return arr.size();
	}

	void delete_heap(){
		heap_element tmp = get_min();
		tmp.tree->delete_tree();
	}

	huffman_tree* getHuffmanTree(){
		return arr[3].tree;
	}
};


class pairing_heap_node{
public:
	vector< pairing_heap_node* > arr;
	heap_element elem;
};


class pairing_heap{
	pairing_heap_node* root;
	int sz;
public:
	pairing_heap(){
		root = NULL;
		sz = 0;
	}

	void insert(pairing_heap_node* newElem){
		sz++;
		if (root == NULL){
			root = newElem;
		}
		else if (root->elem.freq < newElem->elem.freq)
			root->arr.push_back(newElem);
		else{
			newElem->arr.push_back(root);
			root = newElem;
		}
	}

	heap_element get_min(){
		sz--;
		heap_element mi = root->elem;
		vector<pairing_heap_node* >tmp;
		for(int i=root->arr.size()-1; i>0; i=i-2){
			if (root->arr[i]->elem.freq > root->arr[i-1]->elem.freq){
				tmp.push_back(root->arr[i-1]);
				root->arr[i-1]->arr.push_back(root->arr[i]);
			}
			else{
				tmp.push_back(root->arr[i]);
				root->arr[i]->arr.push_back(root->arr[i-1]);
			}
			root->arr.pop_back();
			root->arr.pop_back();
		}

		if (root->arr.size() > 0){
			tmp.push_back(root->arr.back());
			root->arr.pop_back();
		}

		pairing_heap_node* tmpRoot = NULL;
		if (tmp.size() > 0){
			tmpRoot = tmp.back();
			for(int i=tmp.size()-2; i>=0; i--){
				if (tmp[i]->elem.freq > tmpRoot->elem.freq)
					tmpRoot->arr.push_back(tmp[i]);
				else{
					tmp[i]->arr.push_back(tmpRoot);
					tmpRoot = tmp[i];
				}
			}
		}

		delete(root);
		root = tmpRoot;
		return mi;
	}

	int size(){
		return sz;
	}

	void delete_heap(){
		root->elem.tree->delete_tree();
		delete(root);
		root = NULL;
	}


};



// End of heap structures

void build_tree_using_binary_heap(){
	binary_heap heap;
	heap_element elemOne, elemTwo;

	for (int i=0;i<FREQ_TABLE_SIZE; i++){

		if (freq_table[i] == 0)
			continue;

		elemOne.freq = freq_table[i];
		elemOne.tree = new huffman_tree();
		elemOne.tree->insert_root(new huffman_tree_node(i));
		heap.insert(elemOne);
	}

	while (heap.size() > 1){
		elemOne = heap.get_min();
		elemTwo = heap.get_min();
		//cout<<elemOne.freq<<" "<<elemTwo.freq<<endl;
		elemOne.tree->combine(elemTwo.tree);
		elemOne.freq += elemTwo.freq;
		heap.insert(elemOne);
	}

	heap.delete_heap();
}


huffman_tree* build_tree_using_4way_heap(){
	four_way_heap heap;
	heap_element elemOne, elemTwo;

	for (int i=0;i<FREQ_TABLE_SIZE; i++){

		if (freq_table[i] == 0)
			continue;

		elemOne.freq = freq_table[i];
		elemOne.tree = new huffman_tree();
		elemOne.tree->insert_root(new huffman_tree_node(i));
		heap.insert(elemOne);
	}

	while (heap.size() > 4){
		elemOne = heap.get_min();
		elemTwo = heap.get_min();
		//cout<<elemOne.freq<<" "<<elemTwo.freq<<endl;
		elemOne.tree->combine(elemTwo.tree);
		elemOne.freq += elemTwo.freq;
		heap.insert(elemOne);
	}
	heap.delete_heap();
}


void build_tree_using_pairing_heap(){
	pairing_heap heap;
	heap_element elemOne, elemTwo;
	pairing_heap_node* tmp;

	for (int i=0;i<FREQ_TABLE_SIZE; i++){

		if (freq_table[i] == 0)
			continue;

		elemOne.freq = freq_table[i];
		elemOne.tree = new huffman_tree();
		elemOne.tree->insert_root(new huffman_tree_node(i));
		tmp = new pairing_heap_node();
		tmp->elem = elemOne;
		heap.insert(tmp);
	}

	//cout<<"Initialized tree"<<endl;
	while (heap.size() > 1){
		//cout<<heap.size()<<endl;
		elemOne = heap.get_min();
		elemTwo = heap.get_min();
		//cout<<elemOne.freq<<" "<<elemTwo.freq<<endl;
		elemOne.tree->combine(elemTwo.tree);
		elemOne.freq += elemTwo.freq;
		tmp = new pairing_heap_node();
		tmp->elem = elemOne;
		heap.insert(tmp);
	}

	heap.delete_heap();
}


void display_freq_table(int size){
	cout<<endl;
	for(int i=0;i<FREQ_TABLE_SIZE && size > 0;i++){
		if (freq_table[i]){
			size--;
			cout<<i<<" "<<freq_table[i]<<endl;
		}
	}
	cout<<endl;
}


int main(){
	ifstream f;
	int var;
	for(int i=0;i<FREQ_TABLE_SIZE;i++)
		freq_table[i]=0;
	f.open("/home/abhinav/ADS/Sample2/sample_input_large.txt");
	while(f >> var){
		freq_table[var]++;
	}
	f.close();
	
	//display_freq_table(10);
	
	clock_t start_time;
	// binary heap
	start_time = clock();
	for(int i = 0; i < 10; i++){
		//run 10 times on given data set
		//cout<<endl;
		build_tree_using_binary_heap();
		//cout<<endl;
	}
	cout << "Time using binary heap (microsecond): " << (clock() - start_time)/10 << endl;
	// 4-way heap
	start_time = clock();
	for(int i = 0; i < 10; i++){
		//run 10 times on given data set
		build_tree_using_4way_heap();
	}
	cout << "Time using 4-way heap (microsecond): " << (clock() - start_time)/10 << endl;
	// pairing heap
	start_time = clock();
	for(int i = 0; i < 10; i++){
		//run 10 times on given data set
		build_tree_using_pairing_heap();
	}
	cout << "Time using pairing heap (microsecond): " << (clock() - start_time)/10 << endl;

}