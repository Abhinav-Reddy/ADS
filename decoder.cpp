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

	void insert(int key, string s){
		huffman_tree_node* node = root;
		if (s == "")
			return;
		for(int i=0;i<s.length();i++){
			if (s[i] == '0'){
				if (node->left == NULL){
					node->left = new huffman_tree_node();
				}
				node = node->left;
			}
			else{
				if (node->right == NULL){
					node->right = new huffman_tree_node();
				}
				node = node->right;
			}
		}
		node->val = key;
	}

};


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


void decodeText(huffman_tree* tree, char* fileName){
	ifstream f;
	ofstream o;
	int len = 0;
	f.open(fileName, std::ifstream::binary);
	f.seekg (0, f.end);
    int length = f.tellg();
    f.seekg (0, f.beg);
    //cout<<length<<endl;
    char * buffer = new char [length];
	f.read(buffer, length);
	o.open("decoded.txt");
	huffman_tree_node* node = tree->getRoot();

	for (int i=0;i<length; i++) {
		int k = (int)buffer[i];
		for(int i=0;i<8;i++){
			if (k&(1<<i))
				node = node->right;
			else
				node = node->left;
			if (node->left == NULL && node->right == NULL){
				o << node->val << endl;
				node = tree->getRoot();
			}
		}
	}
	//cout<<endl;
	f.close();
	o.close();
}


int main(int argc, char *argv[]){
	
	if (argc != 3){
		return 0;
	}
	huffman_tree* tree = new huffman_tree();
	tree->insert_root(new huffman_tree_node());
	ifstream f;
	int key;
	string val;
	f.open(argv[2]);
	while(f >> key){
		f >> val;
		code_table[key] = val;
		tree->insert(key, val);
	}
	decodeText(tree, argv[1]);
	//tree = buildTreeFromCodeTable();
}