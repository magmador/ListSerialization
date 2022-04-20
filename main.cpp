#include <iostream>
#include <string>

using namespace std;

#define PATH "list.txt"

struct ListNode {
	ListNode * prev;
	ListNode * next;
	ListNode * rand;
	string data;
};

class List {
public:
	List() : head(NULL), tail(NULL) {};
	~List();
	void Add(string data);
	void Show();
	void Serialize(FILE *file);
	void Deserialize(FILE *file);
private:
	ListNode * head;
	ListNode * tail;
	int count;
};

List::~List() {
	while (head) {
		tail = head->next;
		delete head;
		head = tail;
	}
}

void List::Add(string data) {
	ListNode *tmp = new ListNode;
	tmp->next = NULL;
	tmp->data = data;

	if (head) {
		tmp->prev = tail;
		tail->next = tmp;
		tail = tmp;
	}
	else {
		tmp->prev = NULL;
		head = tail = tmp;
	}
}

void List::Show()
{
	ListNode *tmp = head;
	while (tmp)
	{
		cout << tmp->data << endl;
		tmp = tmp->next;
	}
}

void List::Serialize(FILE *file) {
	int ret = 0;
	size_t check = 0;
	ListNode *tmp = head;
	string dataToWrite;

	file = fopen(PATH, "wb");
	if (NULL == file) {
		cerr << "fopen() failed with" << strerror(errno);
		ret = errno;
		goto finally;
	}

	while (tmp)
	{
		dataToWrite += tmp->data + '\0';
		tmp = tmp->next;
	}

	/* Last item tag for deserialization */
	dataToWrite += '\0';

	check = fwrite(dataToWrite.data(), dataToWrite.size(), 1, file);
	if (1 != check) {
		cerr << "fwrite() failed";
		ret = -1;
		goto finally;
	}

	finally:

	if (file) {
		fclose(file);
	}

	if (ret) {
		exit(ret);
	}
}

void List::Deserialize(FILE *file) {
	int ret = 0;
	int check = 0;
	size_t fsize = 0;
	char *data = NULL;
	char *dataPointer = NULL;

	file = fopen(PATH, "rb");
	if (NULL == file) {
		cerr << "fopen() failed with " << strerror(errno);
		ret = errno;
		goto finally;
	}

	check = fseek(file, 0, SEEK_END);
	if (0 != check) {
		cerr << "fseek() failed with " << strerror(errno);
		ret = errno;
		goto finally;

	}

	fsize = ftell(file);
	if (fsize < 0) {
		cerr << "ftell() failed with " << strerror(errno);
		ret = errno;
		goto finally;
	}

	check = fseek(file, 0, SEEK_SET);
	if (0 != check) {
		cerr << "fseek() failed with " << strerror(errno);
		ret = errno;
		goto finally;
	}

	data = (char *)malloc(fsize + 1);
	dataPointer = data;

	check = fread(data, fsize, 1, file);
	if (1 != check) {
		cerr << "fread() failed";
		ret = -2;
		goto finally;
	}

	while (*dataPointer)
	{
		this->Add(dataPointer);

		/* Move pointer to next node data */
		dataPointer += strlen(dataPointer) + 1;
	}

	finally:

	free(data);

	if (file) {
		fclose(file);
	}

	if (ret) {
		exit(ret);
	}
}

int main() {
	List list;
	FILE * file = NULL;

	list.Add("first string");
	list.Add("second string");
	list.Add("third string");
	list.Add("fourth string");

	list.Serialize(file);

	/* Clear current list. So we could deserialize a new one */
	list.~List();

	list.Deserialize(file);

	/* Show deserialized list */
	list.Show();

	return 0;
}
