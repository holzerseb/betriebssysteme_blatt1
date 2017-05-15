#include "btree.h"
#include <assert.h>

/* Structs, and obligatory stuff */

///
/// This is the handle to work with the btree
///
struct btree
{
	btree_node* root;
	int size; //to keep track of the #Nodes
};

///
/// This represents a single node in our btree
///
struct btree_node
{
	btree_node* leftChild;
	btree_node* rightChild;
	int value;
};

char* nodeToString(const btree_node* node, int* maxStringSize, int* currentStringSize);

/* Methods */

///
/// This function creates and returns our btree
///
btree* btree_create()
{
	//plant that little baby
	btree* newTree = malloc(sizeof(btree));
	//check whether it has grown properly
	assert(newTree); //is the same as assert(newTree != NULL)
	//Initialize some stuff
	newTree->size = 0;
	newTree->root = NULL;
	//and ship it to the caller
	return newTree;
}

///
/// This function creates and returns a single node
/// value is the value you want it to store to
///
btree_node* btree_create_node(int value)
{
	assert(value); //is that even necessary for int? Like, is int in c nullable?
	
	//plant that little baby
	btree_node* newNode = malloc(sizeof(btree_node));
	//check whether it has grown properly
	assert(newNode); //is the same as assert(newTree != NULL)
	//put the given value into it
	newNode->value = value;
	newNode->leftChild = NULL;
	newNode->rightChild = NULL;
	//and ship it to the caller
	return newNode;
}

///
/// This will kill the tree :(
///
void btree_destroy(btree* t)
{
	assert(t);
	free(t); //i once had a hamster whom I called FreeTe
}

///
/// This will kill a node :(
///
void btree_destroy_node(btree_node* n)
{
	assert(n);
	//this actually is useless anyway, but it's here for the sake
	//of documentation
	n->leftChild = NULL;
	n->rightChild = NULL;
	
	//cuz on free, the childs and value gets removed anyway
	free(n);
}

///
/// Inserts the given number 'd' into tree 't'
/// if the number is already in 't', no changes are made
///
void btree_insert(btree* t, int d)
{
	assert(t);
	assert(d);
	
	btree_node* currentNode = t->root;
	
	//in case the root is null, its our lucky day
	if(!currentNode)
	{
		t->root = btree_create_node(d);
		t->size++;
		return;
	}
	
	//otherwise, we find the right place to store it
	while(true)
	{
		if(currentNode->value == d) //value already exists
			return;
		
		if(currentNode->value < d)
		{ //value of d is bigger, so it must be in its right child
			if(!(currentNode->rightChild))
			{ //if it doesn't have a child to its right, then we gonna place our int there
				currentNode->rightChild = btree_create_node(d);
				t->size++;
				return;
			}
			
			//but if it does have a child to its right, we make that child our current node
			//and try again
			currentNode = currentNode->rightChild;
			continue;
		}
		else
		{ //value is smaller, so it must be in its left child
			if(!(currentNode->leftChild))
			{ //if it doesn't have a child to its right, then we gonna place our int there
				currentNode->leftChild = btree_create_node(d);
				t->size++;
				return;
			}
			
			//but if it does have a child to its right, we make that child our current node
			//and try again
			currentNode = currentNode->leftChild;
			continue;
		}
	}
}

///
/// Removes the given number 'd' from tree 't'
///
void btree_remove(btree* t, int d)
{
	assert(t);
	assert(d);
	
	btree_node* nodeToDelete = t->root;
	//because when we remove a node, we have to make one of the childs of the removed node
	//the new "parent"-node (as replacement in the tree for the removed node)
	//now, I didn't want to implement a whole new ideom to make every node aware of his
	//parent, so let's just keep track of the previous node inside this function
	btree_node* parentNode = NULL;
	//Neither do we know, if the nodeToDelete will be the left or the right child, so
	//we have to track that as well. Im sorry. It's 1am and I guess there would have been
	//a better solution, but hell why not, maybe this is the better solution :)
	bool isLeftChild = false;
	
	//in case the root is null, we cant do stuff
	if(!nodeToDelete)
		return;
	
	//otherwise, we find the right value
	while(true)
	{
		if(nodeToDelete->value == d)//we found the correct one :)
			break;
		
		if(nodeToDelete->value < d)
		{ //value is bigger, so it must be in its right child
			if(!(nodeToDelete->rightChild))
				return; //if it doesn't have a child to its right, then the value probably doesnt exist
			
			//otherwise the search must go on
			parentNode = nodeToDelete;
			isLeftChild = false;
			nodeToDelete = nodeToDelete->rightChild;
			continue;
		}
		else
		{ //value is smaller, so it must be in its left child
			if(!(nodeToDelete->leftChild))
				return; //if it doesn't have a child to its left, then the value probably doesnt exist
			
			//otherwise the search must go on
			parentNode = nodeToDelete;
			isLeftChild = true;
			nodeToDelete = nodeToDelete->leftChild;
			continue;
		}
	}
	
	//the child, which will replace the nodeToDelete
	btree_node* replacingChild = NULL;
	
	//next, we have to ensure to keep the reference to all the childs (if it has some)
	//we have to place one of the childs at the position of nodeToDelete
	if(!nodeToDelete->leftChild && nodeToDelete->rightChild)
	{ //if the nodeToDelete does only have a child to its right, then the solution is found
		replacingChild = nodeToDelete->rightChild;
	}
	else if(nodeToDelete->leftChild && !nodeToDelete->rightChild)
	{ //if its the other way around, act the other way around, obviously
		replacingChild = nodeToDelete->leftChild;
	}
	else if(nodeToDelete->leftChild && nodeToDelete->rightChild)
	{ //now, if the nodeToDelete does have two children, we have to find the more suitable one
		//Finding the suitable can be tricky, so we specify it first, what we aim for:
		
		/* UPDATE AFTER REVIEWING THE CODE:
				I'm sorry i didnt just create a subtree and use btree_maximum, but hell I spent an hour
				just for these 19 lines of codes, so i just leave it the way it is :)
				
				And yes, while this part looks monstrous long, its really just 19 lines of pure code
				*/
		
		/* when a node has to be removed, the biggest value of ALL left child nodes must still
		 * be smaller than the smallest of ALL right childs!
		 * With this knowledge, we can safely tell, that either the biggest of the left childs
		 * OR the smallest of all right childs is a suitable new nodeToDelete
		 * Also, the biggest of all left childs cannot have a "a bigger" child (right child)
		 *
		 * So, what we need is the biggest of the left childs (or the smallest of the right childs)
		 * Note: this specific child STILL can have a child, just a smaller (right->bigger) one
		 * */
		
		//These are to store info to restructure our tree
		btree_node* biggestChild = nodeToDelete->leftChild;
		btree_node* parent_biggestChild = nodeToDelete;
		
		//search for the biggest child
		while(true)
		{
			if(!biggestChild->rightChild)
				break;
			
			parent_biggestChild = biggestChild;
			biggestChild = biggestChild->rightChild;
		}
		
		//so, if the parent of the biggest is not the nodeToDelete, we have to re-organize our references
		if(parent_biggestChild != nodeToDelete)
		{
			if(biggestChild->leftChild) //if the biggestChild does have a leftChild, make this leftCHild the new rightChild of the parent
				parent_biggestChild->rightChild = biggestChild->leftChild; //so basically the leftchild is now at the place of biggestchild
			else
				parent_biggestChild->rightChild = NULL;
				
			//then we have to make the the biggestChild adopt the left child of nodeToDelete
			biggestChild->leftChild = nodeToDelete->leftChild;
		}
		
		//and in any case, the right child must be adopted also
		biggestChild->rightChild = nodeToDelete->rightChild;
		
		//and this is just to make our code a little cleaner
		replacingChild = biggestChild;
	}
	else
	{ //otherwise it doesn't have any childs, so we just delete it and delete reference to it
		//this just exists for the sake of documentation
	}
	
	//then we assign the new child, which replaces the nodeToDelete
	if(!parentNode)
		t->root = replacingChild;
	else if(isLeftChild)
		parentNode->leftChild = replacingChild;
	else
		parentNode->rightChild = replacingChild;
	
	//next, we can delete the nodeToDelete, since there is no reference to it anymore
	btree_destroy_node(nodeToDelete); //goodbye, mate :(
	t->size--;
}

///
/// Returns the smallest number in tree 't'
///
int btree_minimum(const btree* t)
{
	assert(t);
	
	btree_node* currentNode = t->root;
	assert(currentNode); //in which case we cannot return a valid value
	
	while(true)
	{
		//i tend to place abort conditions ahead the actual code
		if(!currentNode->leftChild)
			break;
		
		currentNode = currentNode->leftChild;
	}
	
	return currentNode->value;
}

///
/// Returns the largest number in tree 't'
///
int btree_maximum(const btree* t)
{
	assert(t);
	
	btree_node* currentNode = t->root;
	assert(currentNode); //in which case we cannot return a valid value
	
	while(true)
	{
		if(!currentNode->rightChild)
			break;
		
		currentNode = currentNode->rightChild;
	}
	
	return currentNode->value;
}

///
/// Returns true if the given tree 't' contains 'd', false otherwise
///
bool btree_contains(const btree* t, int d)
{
	assert(t);
	
	btree_node* currentNode = t->root;
	
	//in case the root is null, there cannot be any value at all in here
	if(!currentNode)
		return false;
	
	while(true)
	{
		if(currentNode->value == d) //value found, wooohooooo
			return true;
		
		if(currentNode->value < d)
		{ //value is bigger, so it must be in its right child
			if(!(currentNode->rightChild)) //if it doesn't have a child to its right, then the value is not in the tree
				return false;
			
			currentNode = currentNode->rightChild;
			continue;
		}
		else
		{ //value is smaller, so it must be in its left child
			if(!(currentNode->leftChild)) //if it doesn't have a child to its left, then the value is not in the tree
				return false;
				
			currentNode = currentNode->leftChild;
			continue;
		}
	}
}

///
/// Returns the number of elements of the given btree 't'
/// NOTE: should complete in O(1) time
///
size_t btree_size(const btree* t)
{
	assert(t);
	return t->size;
}

///
/// Prints the given btree 't' to the supplied output stream 'out'
/// output format: ([LEFT], VAL, [RIGHT]) : [SIZE]
/// example empty: ( NIL ) : 0
/// example 3,4,7 in a balanced tree: ((3), 4, (7)) : 3
///
void btree_print(const btree* t, FILE* out)
{
	//gonna do that later
	assert(t);
	assert(out);
	
	int stringSize = 1;
	int maxStringSize = 6;
	char* stringToPrint = malloc(sizeof(char) * maxStringSize);
	
	if(!(t->root)) //if there is no root, print nil
		//stringToPrint = " NIL ";
		sprintf(stringToPrint, "%s", " NIL ");
	else //otherwise, we have to put the string together
	{
		//create the string
		char* newString = nodeToString(t->root, &maxStringSize, &stringSize);

		//re-alloc enough space for the string
		stringToPrint = realloc(stringToPrint, sizeof(char) * maxStringSize);
		assert(stringToPrint);
		
		//the reason I did this, was because we have to find out how long the string will actually be
		assert(sprintf(stringToPrint, "%s", newString) > 0);
		free(newString);
	}
	
	int returnValue = fprintf(out, "(%s) : %zu\n", stringToPrint, btree_size(t));
	assert(returnValue > 0); //we check if the print was successfull as well
	
	fflush(out);
	free(stringToPrint);
}

///
/// This formats a node and all its Child-Nodes
/// to a string
///
char* nodeToString(const btree_node* node, int* maxStringSize, int* currentStringSize)
{
	int requiredAdditionalSize = 1; //for each node we need minimum 1 additional char
	char* formattedString = malloc(sizeof(char) * *maxStringSize);
	
	//check, if we will need even more chars
	if(node->leftChild)
		requiredAdditionalSize += 4; //because "(x), " are 5 chars, whereas x will considered in the recursive call
	else if(node->rightChild)
		requiredAdditionalSize += 4; //because " ,(x)" are 5 chars, whereas x will considered in the recursive call
	
	//verify that we got enough space to store our chars
	if(*maxStringSize <= *currentStringSize + requiredAdditionalSize)
	{
		formattedString = realloc(formattedString, sizeof(char) * (*maxStringSize += 32));
		assert(formattedString);
	}
	
	//then we can "create" the string for the currentNode:
	if(node->leftChild && node->rightChild)
	{
		char* leftString = nodeToString(node->leftChild, maxStringSize, currentStringSize);
		char* rightString = nodeToString(node->rightChild, maxStringSize, currentStringSize);
		assert(sprintf(formattedString, "(%s), %d, (%s)", leftString, node->value, rightString) > 0);
		free(leftString);
		free(rightString);
	}
	else if(node->leftChild)
	{
		char* leftString = nodeToString(node->leftChild, maxStringSize, currentStringSize);
		assert(sprintf(formattedString, "(%s), %d", leftString, node->value) > 0);
		free(leftString);
	}
	else if(node->rightChild)
	{
		char* rightString = nodeToString(node->rightChild, maxStringSize, currentStringSize);
		assert(sprintf(formattedString, "%d, (%s)", node->value, rightString) > 0);
		free(rightString);
	}
	else
	{
		assert(sprintf(formattedString, "%d", node->value) > 0);
	}
	
	*currentStringSize += requiredAdditionalSize;
	
	return formattedString;
}
