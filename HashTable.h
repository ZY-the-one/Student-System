#include <iostream>
using namespace std;

class Record
{
private:
	unsigned key;	// the primary key value (the ID)
	int offset;		// the byte offset in the file where the record begins
	int size;		// the total size of the record in bytes
public:
	Record() {}
	Record(unsigned k, int o, int s)
	{
		key = k;
		offset = o;
		size = s;
	}
	Record(const Record& record)
	{
		key = record.getKey();
		offset = record.getOffset();
		size = record.getSize();
	}

	void operator =(const Record& record)
	{
		key = record.getKey();
		offset = record.getOffset();
		size = record.getSize();
	}

	//data member access functions
	unsigned getKey()const { return key; }
	int getOffset()const { return offset; }
	int getSize()const { return size; }

	void setKey(unsigned k) { key = k; }
};

class HashTable
{
private:
	Record *HT;			//the hash table
	int size;			//size of HT
	int current;		//the current number of elements in HT
	unsigned emptyKey;	//key value for an empty slot
	unsigned tombstone;	//a special mark in place of deleted record
	int h(unsigned key)		//the hash function(mid-square method)
	{
		return ((key * key) & 16776960) % size;//16776960 = 00000000111111111111111100000000
	}
	int h2(unsigned key, int i)		//the second hash function for probing
	{
		return i * (key % (size - 1) + 1);
	}
	int hashSearch(unsigned k)	//search the position of the record with Key k in HT
	{
		int pos,home;
		pos = home = h(k);
		for (int i =1;(emptyKey != HT[pos].getKey() && tombstone !=HT[pos].getKey()) && (k != HT[pos].getKey());i++)
			pos = (home + h2(k, i)) % size;
		return pos;
	}
public:
	HashTable(int s)
	{
		size = s;
		current = 0;
		emptyKey = 0;				//the smallest number of unsigned number
		tombstone = 4294967295;		//the biggest number of unsigned number
		HT = new Record[size];
		for (int i = 0;i < size;i++)
			HT[i].setKey(emptyKey); //initialize HT
	}

	~HashTable() { delete HT;}

	//find a slot to insert
	int Slot(const unsigned& k)
	{
		if(current >= size)
		{
			cout<<"Hash table is full"<<endl;
			return -1;
		}
		int pos,home;
		pos = home = h(k);
		for (int i = 1;emptyKey != HT[pos].getKey() && tombstone !=HT[pos].getKey();i++)
		{
			pos = (home + h2(k, i)) % size;
			cout<<"key"<<k<<"h2:"<<h2(k,i)<<endl;
			if (k == HT[pos].getKey())
			{
				cout<<"Duplicates not allowed!"<<endl;
				return -1;
			}
			if (pos == home)
			{
				cout<<"pos:"<<pos<<endl<<"home:"<<home<<endl;
				cout<<"Fail to find a slot to insert the record."<<endl;
				return -1;
			}
		}
		return pos;
	}

	//insert a record
	void insert(int pos, const Record& r)
	{
		HT[pos] = r;
		current++;
	}

	//return a record matching the key
	Record find(const unsigned& k) 
	{
		int pos = hashSearch(k);
		if (k == HT[pos].getKey()) 
			return HT[pos];
		else
			return Record(emptyKey, -1, -1);//k not in hash table
	}

	//remove and return a record
	Record remove(const unsigned& k)
	{
		int pos = hashSearch(k);
		if (k == HT[pos].getKey())
		{
			Record temp = HT[pos];
			HT[pos].setKey(tombstone);
			return temp;
			current--;
		} 
		else
		{
			return Record(emptyKey, -1, -1);
		}
	}

	//reinitialize the HT
	void clear()
	{
		for (int i = 0;i < size;i++)
			HT[i].setKey(emptyKey);
	}

	//return the size of the HT
	int getSize() { return size; }

	//print the content of HT
	void print()
	{
		cout<<"Hash Table"<<endl;
		for (int i = 0;i < size;i++)
		{
			if (HT[i].getKey() != 0)
			{
				cout<<"Hash Index:"<<i<<endl;
				cout<<"Primary Key:"<<HT[i].getKey()<<endl<<"Offset:"<<HT[i].getOffset()<<endl<<"Size:"<<HT[i].getSize()<<endl<<endl;
			}			
		}
	}
};

