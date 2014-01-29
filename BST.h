//simple binary tree node implementation
template<typename Key,typename E>
class BSTNode
{
private:
	Key k;			//the node's key
	E it;			//the node's value
	BSTNode* lc;	//pointer to left child
	BSTNode* rc;	//pointer to right child

public:
	BSTNode() { lc = rc = NULL; }
	BSTNode(Key K, E e, BSTNode* l =NULL, BSTNode* r =NULL) { k = K; it = e; lc = l; rc = r; }
	~BSTNode() {}

	//function to set and return the value and key
	E& element() { return it; }
	void setElement(const E& e) { it = e; }
	Key& key() { return k; }
	void setKey(const Key& K) { k = K; }

	//function to set and return the children
	inline BSTNode* left() const { return lc; }
	void setLeft(BSTNode<Key,E>* b) { lc = b; }
	inline BSTNode* right() const { return rc; }
	void setRight(BSTNode<Key,E>* b) { rc = b; }

	//return true if it is a leaf, false otherwise
	bool isLeaf() { return (lc == NULL) && (rc == NULL); }
};

template<typename Key,typename E,typename Comp>
class BST
{
private:
	BSTNode<Key, E>* root;	//root of the BST
	int nodeCount;			//number of nodes in the BST

	//private helper function
	void clearhelp(BSTNode<Key, E>* rt)
	{
		if (rt == NULL) return;
		clearhelp(rt->left());
		clearhelp(rt->right());
		delete rt;
	}

	BSTNode<Key, E>* inserthelp(BSTNode<Key, E>* rt, const Key& k, const E& it)
	{
		if (rt == NULL) return new BSTNode<Key, E>(k, it, NULL, NULL);	//empty tree
		if (Comp::prior(rt->key(), k)) //k < rt->key
			rt->setLeft(inserthelp(rt->left(), k, it));
		else							
			rt->setRight(inserthelp(rt->right(), k, it));
		return rt;
	}

	BSTNode<Key, E>* deletemin(BSTNode<Key, E>* rt)
	{
		if (rt->left() == NULL) return rt->right();
		else
		{
			rt->setLeft(deletemin(rt->left()));
			return rt;
		}
	}

	BSTNode<Key, E>* getmin(BSTNode<Key, E>* rt)
	{
		if (rt->left() == NULL)
			return rt;
		else return getmin(rt->left());
	}

	BSTNode<Key, E>* removehelp(BSTNode<Key, E>* rt, const Key& k)
	{
		if (rt == NULL) return NULL;	//k is not in the tree
		else if (Comp::prior(rt->key(), k)) //rt->key > k
			rt->setLeft(removehelp(rt->left(), k));
		else if (Comp::prior(k, rt->key())) //k > rt->key
			rt->setRight(removehelp(rt->right(), k));
		else	//found record
		{
			BSTNode<Key, E>* temp = rt;
			if (rt->left() == NULL)
			{
				rt = rt->right();
				delete temp;
			}
			else if (rt->right() == NULL)
			{
				rt = rt->left();
				delete temp;
			}
			else
			{
				BSTNode<Key, E>* t = getmin(rt->right());
				rt->setElement(t->element());
				rt->setKey(t->key());
				rt->setRight(deletemin(rt->right()));
				delete t;
			}
		}
		return rt;
	}

	BSTNode<Key, E>* findhelp(BSTNode<Key, E>* rt, const Key& k) const
	{
		if (rt == NULL) return NULL;   
		if (Comp::prior(rt->key(), k))			//k < rt->key
			return findhelp(rt->left(), k);		//check left child
		else if (Comp::prior(k, rt->key()))		//k > rt->key
			return findhelp(rt->right(), k);	//check right child
		else return rt;	//found record
	}

	void printhelp(BSTNode<Key, E>* rt, int level) const
	{
		if (rt == NULL) return;
		printhelp(rt->left(), level+1);
		cout<<"Key:"<<rt->key()<<endl<<"Value:"<<rt->element()<<endl<<endl;
		printhelp(rt->right(), level+1);
	}

public:
	BST() { root = NULL; nodeCount = 0; }
	~BST() { clearhelp(root); }

	//reinitialize tree
	void clear()
	{
		clearhelp(root);
		root = NULL;
		nodeCount = 0;
	}

	//insert a record into the tree
	void insert(const Key& k, const E& e)
	{
		root = inserthelp(root, k, e);
		nodeCount++;
	}

	//remove a record from the tree
	//return the record removed, or NULL if there is none
	void remove(const Key& k, const E& e)
	{
		BSTNode<Key, E> *temp = findhelp(root ,k);
		while(temp != NULL)
		{
			if (e == temp->element())
			{
				root = removehelp(root, k);
				return;
			}
			temp = findhelp(temp->right(), k);
		}
		cout<<"BST:no matching record to delete."<<endl<<endl;
	}

	//return record with key value k, NULL if none exist
	E find(const Key& k)
	{ 
		BSTNode<Key, E> *temp = findhelp(root ,k);
		return temp->element(); 
	}

	//remove all the records matching with the key, return the number of the matching records
	int removeAllNodeWithTheKey(const Key& k, E * & nodes) 
	{
		int i;
		BSTNode<Key, E> *temp = findhelp(root ,k);
		for (i = 0;temp != NULL;i++)
		{
			nodes[i] = temp->element();
			temp = findhelp(temp->right() ,k);
			root = removehelp(root, k);
		}
		return i;
	}
	
	//find all the records matching with the key, return the number of the matching records
	int findAllNodeWithTheKey(const Key& k, E * & nodes) 
	{
		int i;
		BSTNode<Key, E> *temp = findhelp(root ,k);
		for (i = 0;temp != NULL;i++)
		{
			nodes[i] = temp->element();
			temp = findhelp(temp->right() ,k);
		}
		return i;
	}

	//return the number of records in the tree
	int size() { return nodeCount; }

	void print() const
	{
		if (root == NULL) cout<<"The BST is empty."<<endl;
		else printhelp(root, 0);
	}
};
