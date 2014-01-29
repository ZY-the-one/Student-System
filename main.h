// On my honor: 
// 
// - I have not used C++ language code obtained from another student, 
// or any other unauthorized source, either modified or unmodified. 
// 
// - All C++ language code and documentation used in my program 
// is either my original work, or was derived, by me, from the source 
// code published in the textbook for this course. 
// 
// - I have not discussed coding details about this project with anyone 
// other than my instructor, ACM/UPE tutors or the GTAs assigned to this 
// course. I understand that I may discuss the concepts of this program 
// with other students, and that another student may help me debug my 
// program so long as neither of us writes anything during the discussion 
// or modifies any computer file during the discussion. I have violated 
// neither the spirit nor letter of this restriction. 

#include <fstream>
#include <string>
#include <iostream>
using namespace std;

#include "BST.h"
#include "HashTable.h"
#include "Student.h"

//comparator function
class stringComp;
class unsignedComp;
class doubleComp;
class shortComp;

const Student endMark("noName","noAddress",9,0,0,"no",0);
bool isEndMark(Student);			//determine whether the record is end mark
void initial(string);				//initial the file 
Student inputRecord();				//read the input stream of the record
void outputRecord(const Student);	//print the record
int writeDate(string, Student);		//write the information of students into the file, return  the byte location of the record
Student readDate(string, int);		//read the information of students from the file
void selectCommand(string, string, HashTable&, BST<string, unsigned, stringComp>&, BST<double, unsigned, doubleComp>&,
	BST<string, unsigned, stringComp>&, BST<short, unsigned, shortComp>&);	//select the commands, and make actions accordingly

const unsigned emptyKey = 0;				//the empty key(ID) in HT
const unsigned tombstone = 4294967295;	//the tombstone key(ID) int HT

int main()
{
	cout<<"studentdb ";

	string fileName;	//the name of the data record file
	int tableSize;		//the size of the hashtable
	string command;

	cin>>fileName>>tableSize;

	HashTable table(tableSize);
	BST<string, unsigned, stringComp> nameBST;
	BST<double, unsigned, doubleComp> GPABST;
	BST<string, unsigned, stringComp> majorBST;
	BST<short, unsigned, shortComp> bucksBST;

	
	initial(fileName);
	while(1)
	{
		cin>>command;		
		selectCommand(fileName, command, table, nameBST, GPABST, majorBST, bucksBST);
	}
}

void initial(string fileName)
{
	fstream filedat;
	filedat.open(fileName, ios::out | ios::binary);
	filedat.seekp(0, ios::beg);
	//write the end mark at the beginning of the file
	filedat.write(endMark.Name.c_str() ,30);
	filedat.write((char*)& endMark.AddressLength, sizeof(int));
	filedat.write(endMark.Address.c_str(), endMark.AddressLength);
	filedat.write((char*)&endMark.ID, sizeof(unsigned));
	filedat.write((char*)&endMark.GPA, sizeof(double));
	filedat.write(endMark.Major.c_str(),2);
	filedat.write((char*)&endMark.Bucks, sizeof(short));
	filedat.close();
}

Student inputRecord()
{
	string Name;
	string Address;
	int AddressLength;
	unsigned ID = 0;
	char *num = new char [11];
	double GPA;
	string Major;
	short Bucks;

	cin.get();
	getline(cin, Name, '\n');	
	getline(cin, Address, '\n');
	AddressLength = Address.length();
	bool idComplete = true;
	do 
	{
		cin>>num;
		for (int i = 0;i < 11;i++)
		{
			if (num[i] >= '0' && num[i] <= '9' && i != 3 && i != 6)
			{
				ID = ID * 10 + (num[i] - '0');
			}
			else if((i == 3 || i ==6) && num[i] == '-') {}
			else { cout<<"Wrong ID format"<<endl; idComplete = false; break; }				
		}	
	} while (!idComplete);	
	cin>>GPA;
	cin.get();
	getline(cin, Major, '\n');
	cin>>Bucks;
	return Student(Name, Address, AddressLength, ID, GPA, Major, Bucks);
}

void outputRecord(const Student obj)
{
	cout<<obj.Name<<endl;
	for (int i = 0;i < obj.AddressLength;i++)
	{
		cout<<obj.Address[i];
	}
	cout<<endl;
	unsigned temp = obj.ID;
	for (int i = 0, ratio = 100000000;i < 11;i++)
	{
		if (i != 3 && i != 6)
		{
			cout<<temp/ratio;
			temp = temp % ratio;
			ratio = ratio / 10;
		}
		else { cout<<"-"; }			
	}	
	cout<<endl<<obj.GPA<<endl<<obj.Major[0]<<obj.Major[1]<<endl<<obj.Bucks<<endl;
}

int writeDate(string fileName, Student stu)
{
	Student temp;
	int offset = 0;
	fstream filedat;
	filedat.open(fileName, ios::in | ios::out | ios::binary);
	filedat.seekg(0, ios::beg);
	do 
	{
		temp = readDate(fileName, offset);
		offset = offset + temp.size();
	} while (!isEndMark(temp));
	offset = offset - endMark.size();

	filedat.seekp(offset, ios::beg);
	
	filedat.write(stu.Name.c_str() ,30);
	filedat.write((char*)& stu.AddressLength, sizeof(int));
	filedat.write(stu.Address.c_str(), stu.AddressLength);
	filedat.write((char*)&stu.ID, sizeof(unsigned));
	filedat.write((char*)&stu.GPA, sizeof(double));
	filedat.write(stu.Major.c_str(),2);
	filedat.write((char*)&stu.Bucks, sizeof(short));

	//write the end mark after the last record
	filedat.write(endMark.Name.c_str() ,30);
	filedat.write((char*)& endMark.AddressLength, sizeof(int));
	filedat.write(endMark.Address.c_str(), endMark.AddressLength);
	filedat.write((char*)&endMark.ID, sizeof(unsigned));
	filedat.write((char*)&endMark.GPA, sizeof(double));
	filedat.write(endMark.Major.c_str(),2);
	filedat.write((char*)&endMark.Bucks, sizeof(short));
	filedat.close();

	return offset;
}

Student readDate(string fileName, int offset)
{
	char *s = new char[30];
	char *add;
	int l;
	unsigned i;
	double g;
	char m[2];
	short b;

	fstream filedat;
	filedat.open(fileName,ios::in | ios::binary);
	filedat.seekg(offset, ios::beg);
	filedat.read(s, 30);
	filedat.read((char*)&l, sizeof(int));
	add = new char[l];
	filedat.read(add, l);
	filedat.read((char*)&i, sizeof(unsigned));
	filedat.read((char*)&g, sizeof(double));
	filedat.read(m,2);
	filedat.read((char*)&b, sizeof(short));
	Student stu(s, add, l, i, g, m, b);
	filedat.close();
	return stu;
}

void selectCommand(string fileName, string command, HashTable &table, BST<string, unsigned, stringComp>& nameTree, BST<double, unsigned, doubleComp>& GPATree,
	BST<string, unsigned, stringComp>& majorTree, BST<short, unsigned, shortComp>& bucksTree)
{
	if (command == "enter")
	{
		Student stu = inputRecord();
		int pos = table.Slot(stu.ID); //get the slot's position in HT
		if(pos > -1)
		{
			int offset = writeDate(fileName, stu);
			//insert the record in the HT
			Record r(stu.ID, offset, stu.size());
			table.insert(pos, r);
			//insert the record in the BSTs
			nameTree.insert(stu.Name, stu.ID);
			GPATree.insert(stu.GPA, stu.ID);
			majorTree.insert(stu.Major, stu.ID);
			bucksTree.insert(stu.Bucks, stu.ID);

			cout<<endl<<"enter successfully"<<endl;
			outputRecord(readDate(fileName, offset));
			cout<<"Byte Location:"<<offset<<"    Data Length:"<<stu.size()<<"    Hash Index:"<<pos<<endl<<endl;
		}
		return ;
	}
	if (command == "delete")
	{
		string field;
		cin>>field;		
		if (field == "ID")
		{
			unsigned ID = 0;
			char *num = new char [11];
			bool idComplete = true;
			do //normalize the ID
			{
				cin>>num;
				for (int i = 0;i < 11;i++)
				{
					if (num[i] >= '0' && num[i] <= '9' && i != 3 && i != 6)
					{
						ID = ID * 10 + (num[i] - '0');
					}
					else if((i == 3 || i ==6) && num[i] == '-') {}
					else { cout<<"Wrong ID format"<<endl; idComplete = false; break; }				
				}	
			} while (!idComplete);
			Record r = table.remove(ID);//get the information of the record stored in HT
			if (r.getKey() == emptyKey)	//no matching record with the value
			{
				cout<<"No matching record, no records are deleted."<<endl<<endl;
				return;
			}
			else	//delete the record
			{
				int offset = r.getOffset();
				Student temp = readDate(fileName, offset);

				//remove the record from the BSTs precisely
				nameTree.remove(temp.Name, temp.ID);
				GPATree.remove(temp.GPA, temp.ID);
				majorTree.remove(temp.Major, temp.ID);
				bucksTree.remove(temp.Bucks, temp.ID);

				cout<<"Delete successfully"<<endl;
				outputRecord(temp);
				cout<<"Byte Location:"<<offset<<"    Data Length:"<<temp.size()<<endl;
				cout<<endl;
				return;
			}
		}
		if (field == "Name")
		{
			int num = table.getSize();
			unsigned *ID = new unsigned[num];//an array stored all the ID of records matching the key
			string value;
			cin.get();
			getline(cin, value, '\n');
			num = nameTree.removeAllNodeWithTheKey(value, ID);
			for (int i = 0;i < num;i++)
			{
				Record r = table.remove(ID[i]);//get the information of the record stored in HT
				if (r.getKey() == emptyKey)	//no matching record with the value
				{
					cout<<"No matching record, no records are deleted."<<endl<<endl;
				}
				else	//delete the record
				{
					int offset = r.getOffset();
					Student temp = readDate(fileName, offset);
					cout<<"Delete successfully"<<endl;
					outputRecord(temp);
					cout<<"Byte Location:"<<offset<<"    Data Length:"<<temp.size()<<endl;
					cout<<endl;
				}
			}
			return;
		}
		if (field == "GPA")
		{
			int num = table.getSize();
			unsigned *ID = new unsigned[num];	//an array stored all the ID of records matching the key
			double value;
			cin>>value;
			num = GPATree.removeAllNodeWithTheKey(value, ID);
			for (int i = 0;i < num;i++)
			{
				Record r = table.remove(ID[i]);//get the information of the record stored in HT
				if (r.getKey() == emptyKey)	//no matching record with the value
				{
					cout<<"No matching record, no records are deleted."<<endl<<endl;
				}
				else	//delete the record
				{
					int offset = r.getOffset();
					Student temp = readDate(fileName, offset);
					cout<<"Delete successfully"<<endl;
					outputRecord(temp);
					cout<<"Byte Location:"<<offset<<"    Data Length:"<<temp.size()<<endl;
					cout<<endl;
				}
			}
			return;
		}
		if (field == "Major")
		{
			int num = table.getSize();
			unsigned *ID = new unsigned[num];	//an array stored all the ID of records matching the key
			string value;
			cin.get();
			getline(cin, value, '\n');
			num = majorTree.removeAllNodeWithTheKey(value, ID);
			for (int i = 0;i < num;i++)
			{
				Record r = table.remove(ID[i]);//get the information of the record stored in HT
				if (r.getKey() == emptyKey)	//no matching record with the value
				{
					cout<<"No matching record, no records are deleted."<<endl<<endl;
				}
				else	//delete the record
				{
					int offset = r.getOffset();
					Student temp = readDate(fileName, offset);
					cout<<"Delete successfully"<<endl;
					outputRecord(temp);
					cout<<"Byte Location:"<<offset<<"    Data Length:"<<temp.size()<<endl;
					cout<<endl;
				}
			}
			return;
		}
		if (field == "Bucks")
		{
			int num = table.getSize();
			unsigned *ID = new unsigned[num];	//an array stored all the ID of records matching the key
			short value;
			cin>>value;
			num = bucksTree.removeAllNodeWithTheKey(value, ID);
			for(int i = 0;i < num;i++)
			{
				Record r = table.remove(ID[i]);//get the information of the record stored in HT
				if (r.getKey() == emptyKey)	//no matching record with the value
				{
					cout<<"No matching record, no records are deleted."<<endl<<endl;
				}
				else	//delete the record
				{
					int offset = r.getOffset();
					Student temp = readDate(fileName, offset);
					cout<<"Delete successfully"<<endl;
					outputRecord(temp);
					cout<<"Byte Location:"<<offset<<"    Data Length:"<<temp.size()<<endl;
					cout<<endl;
				}
			}
			return;
		}
		return ;
	}
	if (command == "search")
	{
		string field;
		cin>>field;
		if (field == "ID")
		{
			unsigned ID = 0;
			char *num = new char [11];
			bool idComplete = true;
			do //normalize the ID
			{
				cin>>num;
				for (int i = 0;i < 11;i++)
				{
					if (num[i] >= '0' && num[i] <= '9' && i != 3 && i != 6)
					{
						ID = ID * 10 + (num[i] - '0');
					}
					else if((i == 3 || i ==6) && num[i] == '-') {}
					else { cout<<"Wrong ID format"<<endl; idComplete = false; break; }				
				}	
			} while (!idComplete);

			Record r = table.find(ID);	//search for the record in HT
			if (r.getKey() == emptyKey)	//no matching record with the value
			{
				cout<<"No matching record found."<<endl<<endl;
				return;
			}
			else	//print the record
			{
				int offset = r.getOffset();
				Student temp = readDate(fileName, offset);
				cout<<"Find the record successfully"<<endl;
				outputRecord(temp);
				cout<<"Byte Location:"<<offset<<"    Data Length:"<<temp.size()<<endl;
				cout<<endl;
				return;
			}
		}
		if (field == "Name")
		{		
			int num = table.getSize();
			unsigned *ID = new unsigned[num];	//an array stored all the ID of records matching the key
			string value;
			cin.get();
			getline(cin, value, '\n');
			num = nameTree.findAllNodeWithTheKey(value, ID);	//get the ID from the BST
			for (int i = 0;i < num;i++)
			{
				Record r = table.find(ID[i]);//get the information of the record stored in HT
				if (r.getKey() == emptyKey)	//no matching record with the value
				{
					cout<<"No matching record found."<<endl<<endl;
				}
				else	//print the record
				{
					int offset = r.getOffset();
					Student temp = readDate(fileName, offset);
					cout<<"Delete successfully"<<endl;
					outputRecord(temp);
					cout<<"Byte Location:"<<offset<<"    Data Length:"<<temp.size()<<endl;
					cout<<endl;
				}
			}
			return;
		}
		if (field == "GPA")
		{
			int num = table.getSize();
			unsigned *ID = new unsigned[num];	//an array stored all the ID of records matching the key
			double value;
			cin>>value;
			num = GPATree.findAllNodeWithTheKey(value, ID);
			for(int i = 0;i < num;i++)
			{
				Record r = table.find(ID[i]);//get the information of the record stored in HT
				if (r.getKey() == emptyKey)	//no matching record with the value
				{
					cout<<"No matching record found."<<endl<<endl;
				}
				else	//print the record
				{
					int offset = r.getOffset();
					Student temp = readDate(fileName, offset);
					cout<<"Delete successfully"<<endl;
					outputRecord(temp);
					cout<<"Byte Location:"<<offset<<"    Data Length:"<<temp.size()<<endl;
					cout<<endl;
				}
			}
			return;
		}
		if (field == "Major")
		{
			int num = table.getSize();
			unsigned *ID = new unsigned[num];	//an array stored all the ID of records matching the key
			string value;
			cin.get();
			getline(cin, value, '\n');
			num = majorTree.findAllNodeWithTheKey(value, ID);	//get the ID from the BST
			for (int i = 0;i < num;i++)
			{
				Record r = table.find(ID[i]);//get the information of the record stored in HT
				if (r.getKey() == emptyKey)	//no matching record with the value
				{
					cout<<"No matching record found."<<endl<<endl;
				}
				else	//print the record
				{
					int offset = r.getOffset();
					Student temp = readDate(fileName, offset);
					cout<<"Delete successfully"<<endl;
					outputRecord(temp);
					cout<<"Byte Location:"<<offset<<"    Data Length:"<<temp.size()<<endl;
					cout<<endl;
				}
			}
			return;
		}
		if (field == "Bucks")
		{
			int num = table.getSize();
			unsigned *ID = new unsigned[num];	//an array stored all the ID of records matching the key
			short value;
			cin>>value;
			num = bucksTree.findAllNodeWithTheKey(value, ID);
			for(int i = 0;i < num;i++)
			{
				Record r = table.find(ID[i]);//get the information of the record stored in HT
				if (r.getKey() == emptyKey)	//no matching record with the value
				{
					cout<<"No matching record found."<<endl<<endl;
				}
				else	//print the record
				{
					int offset = r.getOffset();
					Student temp = readDate(fileName, offset);
					cout<<"Delete successfully"<<endl;
					outputRecord(temp);
					cout<<"Byte Location:"<<offset<<"    Data Length:"<<temp.size()<<endl;
					cout<<endl;
				}
			}
			return;
		}
		return ;
	}
	if (command == "searchr")
	{
		string field;
		cin>>field;
		if (field == "ID")
		{
			cout<<"Error: \"Searchr\" should not operate on the ID field."<<endl<<endl;
			return;
		}
		if (field == "Name")
		{
			return;
		}
		if (field == "GPA")
		{
			int num = table.getSize();
			unsigned *ID = new unsigned[num];	//an array stored all the ID of records matching the key
			double min, max;
			cin>>min>>max;
			for (double d = min;d <= max; d = d + 0.1)
			{
				num = GPATree.findAllNodeWithTheKey(d, ID);	//get the ID from the BST
				for (int i = 0;i < num;i++)
				{
					Record r = table.find(ID[i]);//get the information of the record stored in HT
					if (r.getKey() == emptyKey)	//no matching record with the value
					{
						cout<<"No matching record found."<<endl<<endl;
					}
					else	//print the record
					{
						int offset = r.getOffset();
						Student temp = readDate(fileName, offset);
						cout<<"Find the record successfully"<<endl;
						outputRecord(temp);
						cout<<"Byte Location:"<<offset<<"    Data Length:"<<temp.size()<<endl;
						cout<<endl;
					}
				}
			}
			return;
		}
		if (field == "Major")
		{
			return;
		}
		if (field == "Bucks")
		{
			int num = table.getSize();
			unsigned *ID = new unsigned[num];	//an array stored all the ID of records matching the key
			short min, max;
			cin>>min>>max;
			for (short d = min;d <= max; d = d + 1)
			{
				num = bucksTree.findAllNodeWithTheKey(d, ID);	//get the ID from the BST
				for (int i = 0;i < num;i++)
				{
					Record r = table.find(ID[i]);//get the information of the record stored in HT
					if (r.getKey() == emptyKey)	//no matching record with the value
					{
						cout<<"No matching record found."<<endl<<endl;
					}
					else	//print the record
					{
						int offset = r.getOffset();
						Student temp = readDate(fileName, offset);
						cout<<"Delete successfully"<<endl;
						outputRecord(temp);
						cout<<"Byte Location:"<<offset<<"    Data Length:"<<temp.size()<<endl;
						cout<<endl;
					}
				}
			}
			return;
		}
		return ;
	}
	if (command == "dump")
	{
		table.print();
		cout<<"Name BST"<<endl;
		nameTree.print();
		cout<<"GPA BST"<<endl;
		GPATree.print();
		cout<<"Major BST"<<endl;
		majorTree.print();
		cout<<"Bucks BST"<<endl;
		bucksTree.print();
		return ;
	}
	if (command == "makenull")
	{
		initial(fileName);
		table.clear();
		nameTree.clear();
		GPATree.clear();
		majorTree.clear();
		bucksTree.clear();
		return ;
	}
}

bool isEndMark(Student stu)
{
	if (stu.ID == 0 && stu.Name == "noName")
		return true;
	else
		return false;
}

class stringComp
{
public:
	static bool prior(string s1, string s2)
	{
		if (strcmp(s1.c_str(), s2.c_str()) > 0)
			return true;
		else
			return false;
	}
};
class unsignedComp
{
public:
	static bool prior(unsigned u1, unsigned u2)
	{
		return u1 > u2;
	}
};
class doubleComp
{
public:
	static bool prior(double u1, double u2)
	{
		return u1 > u2;
	}
};
class shortComp
{
public:
	static bool prior(short u1, short u2)
	{
		return u1 > u2;
	}
};
