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

	void traverse(huffman_tree_node* node, char* ch, int len){
		if (node == NULL)
			return;
		if (node->left == NULL && node->right == NULL){
			code_table[node->val].assign(ch, len);
			//cout<<node->val<<" "<<code_table[node->val]<<endl; 
		}
		else{
			ch[len] = '0';
			traverse(node->left, ch, len+1);
			ch[len] = '1';
			traverse(node->right, ch, len+1);
		}
	}

};


// Heap structures


class heap_element{
public:
	int freq;
	huffman_tree* tree;
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
			if (arr[idx].freq < arr[idx/4+2].freq || 
				(arr[idx].freq == arr[idx/4+2].freq && arr[idx/4+2].tree->size() > arr[idx].tree->size())){
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
				if (arr[tmp].freq > arr[minChild+i].freq || 
					(arr[tmp].freq == arr[minChild+i].freq && arr[tmp].tree->size() > arr[minChild+i].tree->size()) )
					tmp = minChild + i;
			}
			minChild = tmp;
			
			if (arr[idx].freq > arr[minChild].freq || 
					(arr[idx].freq == arr[minChild].freq && arr[idx].tree->size() > arr[minChild].tree->size())){
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
		if (arr.size() > 3)
			return arr[3].tree;
		else
			return NULL;
	}
};



// End of heap structures



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
	return heap.getHuffmanTree();
}


void displayCodeTable(int size){
	cout<<endl;
	for(int i=0;i<FREQ_TABLE_SIZE && size > 0;i++){
		if (code_table[i] != ""){
			size--;
			cout<<i<<" "<<code_table[i]<<endl;
		}
	}
	cout<<endl;
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


void encodeText(string str){
	ifstream f;
	ofstream o;
	int var;
	int len=0, tmp=0;
	f.open(str.c_str());
	o.open("encoded.bin", std::ios::binary);
	while(f >> var){
		for(int i=0; i<code_table[var].size(); i++){
			if (code_table[var][i] == '1')
				tmp += (1 << len);
			len++;
			if (len == 8){
				o << (char)tmp;
				len = 0;
				tmp = 0;
			}
		}
	}
	f.close();
	o.close();

	o.open("code_table.txt");
	for(int i=0;i<FREQ_TABLE_SIZE ;i++){
		if (code_table[i] != ""){
			o<<i<<" "<<code_table[i]<<endl;
		}
	}
	o.close();

}


int main(int argc, char *argv[]){
	if (argc != 2){
		return 0;
	}
	ifstream f;
	int var;
	for(int i=0;i<FREQ_TABLE_SIZE;i++)
		freq_table[i]=0;
	f.open(argv[1]);
//	f.open("unit_test.txt");
	while(f >> var){
		freq_table[var]++;
	}
	f.close();
	
	//display_freq_table(10);
	huffman_tree* tree = build_tree_using_4way_heap();
	if (tree == NULL)
		return 0;
	else if (tree->size() == 1){
		code_table[tree->getRoot()->val] = "0";
	}
	else{
		char ch[1000001];
		tree->traverse(tree->getRoot(), ch, 0);
	}
	//displayCodeTable(10);
	encodeText(argv[1]);
	return 0;
}