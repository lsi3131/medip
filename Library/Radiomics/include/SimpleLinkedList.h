#pragma once
#include "./DllSupport.h"

typedef struct _Node
{
	char featureValueName[1024] = { 0 };
	double featureValue = 0.0;

	_Node *next;

}Node;

#ifdef RADIOMICS_EXPORTS
class Radiomics_API SimpleLinkedList
#else
class SimpleLinkedList
#endif
{
public:	
	SimpleLinkedList();
	~SimpleLinkedList();

	void addNode(const char *name, double value);
	Node *getHead();
	void destroyList();

private:
	Node *head;
	Node *tail;
};


