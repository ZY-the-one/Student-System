#include <string>
#include <iostream>
using namespace std;
class Student
{
public:
	string Name;
	string Address;
	int AddressLength;
	unsigned ID;
	double GPA;
	string Major;
	short Bucks;

	Student()
	{
		/*AddressLength = 0;
		ID = 0;
		GPA = 0.0;
		*Major = NULL;
		Bucks = 0;*/
	}

	Student(string name, string address, int addlen, unsigned id, double gpa, string major, short bucks)
	{
		Name = name;
		Address = address;
		AddressLength = addlen;
		ID = id;
		GPA = gpa;
		Major = major;
		Bucks = bucks;
	}

	Student(Student &obj)  //copy constructor
	{
		Name = obj.Name;
		Address = obj.Address;
		AddressLength = obj.AddressLength;
		ID = obj.ID;
		GPA = obj.GPA;
		Major = obj.Major;
		Bucks = obj.Bucks;
	}

	Student & operator=(Student obj)
	{
		Name = obj.Name;
		Address = obj.Address;
		AddressLength = obj.AddressLength;
		ID = obj.ID;
		GPA = obj.GPA;
		Major = obj.Major;
		Bucks = obj.Bucks;
		return *this;
	}
	~Student(void) {}

	/*int getAddressLength()
	{
	int i = 0;
	while (Address[i] != '\0')
	i++;
	return i;
	}*/

	//return the total size of the record in bytes
	int size() const
	{
		return 30 + sizeof(int) + AddressLength + sizeof(unsigned) + sizeof(double) + 2 + sizeof(short);
	}
};


