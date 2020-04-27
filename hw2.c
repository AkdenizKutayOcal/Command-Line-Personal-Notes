// TODO
// last line problem
// does not read the last char

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_NOTE_LENGTH 500
#define MAX_TAG_LENGTH 100

struct note
{
    /* data */
    char *content;
    int id;
    struct tag *tagList;
    struct note *next;
};

struct tag
{
    char *name;
    struct tag *next;
};

/**
 * Type for individual stack entry
 */
struct stack_entry
{
    char *data;
    struct stack_entry *next;
};

/**
 * Type for stack instance
 */
struct stack_t
{
    struct stack_entry *head;
    size_t stackSize; // not strictly necessary, but
                      // useful for logging
};

struct note *noteList; //holds all notes
int numberOfLists;

// ADD // This command is used to add a new note to the system.
// Each note is composed of at least one line of text can be at most 500
// characters, but a note may have as many lines of text as the user wants

// ADD <note_id>
// { note content }
// END

// TAG // This command is used to tag one or more notes.
// Tags are names that are composed of at most 100 alphanumeric characters
// (spaces, symbols like parenthesis, mathematical operators, or punctuation
// marks are not allowed).

// TAG <tag_name> <note_id1> <note_id2> ... -1

// DISPLAY // This command is used to display contents of a note.
//Output should start with the line "Id: <note_id>"" and than it's content

// DISPLAY <note_id>

// FIND // This command is used to find notes according to a tag expression.
// Tag expressions are of the form and(t1 ... tn), or(t1 ... tn), and not(t1).
// The system must display the results of a FIND command sorted by the note id in
// ascending order.

// FIND AND( todo ) ⇒ the system must find all notes that are tagged as ‘todo’.

// FIND AND( todo important ) ⇒ the system must find all notes that are tagged as
//both ‘todo’ and ‘important’.

// FIND OR( todo AND( work important ) ) ⇒ the system must find all notes that
//are tagged as either ‘todo’ or both ‘work’ and ‘important’.

// FIND AND( funny NOT( work ) ) ⇒ the system must find all notes that are tagged
//as ‘funny’ but not ‘work’.

// Functions that are FIND
struct note * not();
struct note * and ();
struct note * or ();
struct note *intersect();
struct note *unionOf();

void processCommand(); // Takes command line as an input and calls for needed functions
void printNoteList();
void sortedInsert();
void insertToTagList();
void printTagList();
int isContainsTag();
int isContainsNote();
void displaySingleLine();

// Stack functions
struct stack_t *newStack(void);
char *copyString();
void push();
void pop();
void clear();
void destroyStack();
char *top();

FILE *fp;

////////////////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    fp = fopen("test.in", "r");
    char input[MAX_LINE_LENGTH];
    fgets(input, sizeof(input), stdin);
    //fgets(input, sizeof(input), fp);
    input[strlen(input)-1] = 0;
    noteList = NULL;

    while (strcmp(input, "<EOF>") != 0)
    {
        if (strlen(input) == 0)
        {
            //printf("Wrong command input! Should not be empty.\n");
            break;
        }

        processCommand(input);

        fgets(input, sizeof(input), stdin);
        //fgets(input, sizeof(input), fp);
        input[strlen(input)-1] = 0;
    }

    fclose(fp);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////

void processCommand(char *input)
{
    char commandArgs[strlen(input)][MAX_TAG_LENGTH]; // array of args seperated by strings
    int numberOfArgs = 0;

    char *token = strtok(input, " ");

    while (token != NULL)
    { //seperating input to string array seperated by spaces

        strcpy(commandArgs[numberOfArgs++], token);

        token = strtok(NULL, " ");
    }

    if (strcmp(commandArgs[0], "ADD") == 0)
    {
        struct note *new_note = malloc(sizeof(struct note)); //new note mem allocation
        //(struct note *)
        new_note->id = atoi(commandArgs[1]);

        char *content = malloc(sizeof(char) * MAX_NOTE_LENGTH);

        char input[MAX_LINE_LENGTH];

        //fgets(input, sizeof(input), fp);
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = 0;

        while (strcmp(input, "END") != 0)
        {
            strcat(content, input);
            //printf("----%s----\n",content);
            strcat(content, "\n");

            //fgets(input, sizeof(input), fp);
            fgets(input, sizeof(input), stdin);
            input[strlen(input) - 1] = 0;
        }

        new_note->content = content;
        new_note->tagList = NULL;

        sortedInsert(&noteList, new_note);
    }

    else if (strcmp(commandArgs[0], "TAG") == 0)
    {

        struct tag *new_tag_note = malloc(sizeof(struct tag));
        new_tag_note->name = malloc(sizeof(char) * MAX_TAG_LENGTH);

        strcpy(new_tag_note->name, commandArgs[1]);
        new_tag_note->next = NULL;

        struct note *iterNote;

        for (int i = 2; i < numberOfArgs - 1; i++)
        {
            for (iterNote = noteList; iterNote != NULL; iterNote = iterNote->next)
            {
                if (iterNote->id == atoi(commandArgs[i]))
                {
                    insertToTagList(&(iterNote->tagList), new_tag_note); //insert new tag into tag list of the note
                    break;
                }
            }
        }
    }

    else if (strcmp(commandArgs[0], "DISPLAY") == 0)
    {
        int note_id = atoi(commandArgs[1]);
        struct note *iter;

        for (iter = noteList; iter != NULL; iter = iter->next)
        {
            if (iter->id == note_id)
            {
                printf("Id: %d\n", iter->id);
                printf("%s", iter->content);
                break;
            }
        }
    }

    else if (strcmp(commandArgs[0], "FIND") == 0)
    {

        struct stack_t *theStack = newStack();
        struct note *alterList[50];
        char *tempArr[50];
        numberOfLists = 0;
        //printf("%d==\n",numberOfArgs);
        for (int i = 1; i < numberOfArgs; i++)
        {

            if (strcmp(commandArgs[i], ")") != 0)
            {

                push(theStack, commandArgs[i]);
            }
            else
            {
                int numTags = 0;
                char *str = malloc(sizeof(char) * MAX_TAG_LENGTH);

                str = top(theStack);

                while (strcmp(str, "AND(") != 0 && strcmp(str, "NOT(") != 0 && strcmp(str, "OR(") != 0)
                {

                    tempArr[numTags] = malloc(sizeof(char) * MAX_TAG_LENGTH);
                    strcpy(tempArr[numTags++], str);
                    pop(theStack);

                    str = top(theStack);
                }

                if (strcmp(str, "AND(") == 0)
                {
                    //AND METHOD CALL
                    //printf("AND\n");
                    struct note *list = and(&noteList, alterList, tempArr, numTags);
                    alterList[numberOfLists - 1] = list;
                    //printNoteList(alterList[numberOfLists - 1]);
                }

                else if (strcmp(str, "NOT(") == 0)
                {
                    //NOT METHOD CALL
                    //printf("NOT\n");

                    struct note *list = not(&noteList, alterList, tempArr, numTags);
                    alterList[numberOfLists - 1] = list;
                    //printNoteList(alterList[numberOfLists - 1]);
                }

                else if (strcmp(str, "OR(") == 0)
                {
                    //OR METHOD CALL
                    //printf("OR\n");
                    //printf("NUMBEROFLISTS %d\n", numberOfLists);
                    struct note *list = or (&noteList, alterList, tempArr, numTags);
                    alterList[numberOfLists - 1] = list;
                    //printNoteList(alterList[numberOfLists - 1]);
                }

                pop(theStack);
                push(theStack, "nested");
            }
        }
        //displaySingleLine(alterList[0]);
        //printf("=================\n");
        /* printf("%d\n",numberOfLists);
        
        printf("\n=================\n"); */
        displaySingleLine(alterList[0]);
        //printNoteList(alterList[0]);
    }
}

struct note * and (struct note * *noteList, struct note *alterList[], char *tempArray[], int numberOfTags)
{

    struct note *andList = NULL;

    for (int i = 0; i < numberOfTags; i++)
    {
        //printf("--%s--\n",tempArray[i]);
        if (strcmp(tempArray[i], "nested") == 0)
        {
            if (andList == NULL)
            {
                andList = alterList[numberOfLists - 1];
                numberOfLists--;
            }

            else
            {
                andList = intersect(&andList, &alterList[numberOfLists - 1]);
                numberOfLists--;
            }
        }

        else
        {
            if (andList == NULL)
            {
                struct note *iter_note;

                for (iter_note = *noteList; iter_note != NULL; iter_note = iter_note->next)
                {
                    //printf("id: %d %d\n", iter_note->id, isContainsTag(iter_note, tempArray[0]));

                    if (isContainsTag(iter_note, tempArray[0]) == 1)
                    {
                        struct note *new_note = malloc(sizeof(struct note));

                        new_note->content = iter_note->content;
                        new_note->id = iter_note->id;
                        new_note->next = NULL;

                        //printf("\n%s", new_note->content);

                        sortedInsert(&andList, new_note);
                    }
                }
            }

            else
            {
                struct note *notesWithTag = NULL;

                struct note *iter_note;

                for (iter_note = *noteList; iter_note != NULL; iter_note = iter_note->next)
                {
                    //printf("id: %d %d\n", iter_note->id, isContainsTag(iter_note, tempArray[i]));

                    if (isContainsTag(iter_note, tempArray[i]) == 1)
                    {
                        struct note *new_note = malloc(sizeof(struct note));

                        new_note->content = iter_note->content;
                        new_note->id = iter_note->id;
                        new_note->next = NULL;

                        //printf("\n%s", new_note->content);

                        sortedInsert(&notesWithTag, new_note);
                    }
                }
                
                andList = intersect(&andList, &notesWithTag);
            }
        }
    }
    //printf("--------------------------------\n");
    numberOfLists++;
    return andList;
}

struct note * not(struct note * *noteList, struct note *alterList[], char *tempArray[], int numberOfTags)
{
    struct note *notList = NULL;

    if (strcmp(tempArray[0], "nested") == 0)
    {
        //do nested operation
        struct note *iter_noteList;

        for (iter_noteList = *noteList; iter_noteList != NULL; iter_noteList = iter_noteList->next)
        {
            struct note *iter_alterList = alterList[numberOfLists - 1];
            int isOkay = 1;

            for (iter_alterList; iter_alterList != NULL; iter_alterList = iter_alterList->next)
            {
                if (iter_noteList->id == iter_alterList->id)
                {
                    isOkay = 0;
                    break;
                }
            }

            if (isOkay == 1)
            {
                struct note *new_note = malloc(sizeof(struct note));

                new_note->content = iter_noteList->content;
                new_note->id = iter_noteList->id;
                new_note->next = NULL;

                //printf("\n%s", new_note->content);

                sortedInsert(&notList, new_note);
            }
        }

        return notList;
    }

    else
    {
        struct note *iter_note;

        for (iter_note = *noteList; iter_note != NULL; iter_note = iter_note->next)
        {
            //printf("id: %d %d\n", iter_note->id, isContainsTag(iter_note, tempArray[0]));

            if (isContainsTag(iter_note, tempArray[0]) == 0)
            {
                struct note *new_note = malloc(sizeof(struct note));

                new_note->content = iter_note->content;
                new_note->id = iter_note->id;
                new_note->next = NULL;

                //printf("\n%s", new_note->content);

                sortedInsert(&notList, new_note);
            }
        }

        numberOfLists++;
        return notList;
    }
}

struct note * or (struct note * *noteList, struct note *alterList[], char *tempArray[], int numberOfTags)
{
    struct note *orList = NULL;

    for (int i = 0; i < numberOfTags; i++)
    {
        if (strcmp(tempArray[i], "nested") == 0)
        {
            orList = unionOf(&orList, &alterList[numberOfLists - 1]);
            numberOfLists--;
        }

        else
        {
            struct note *notesWithTag = NULL;

            struct note *iter_note;

            for (iter_note = *noteList; iter_note != NULL; iter_note = iter_note->next)
            {
                //printf("id: %d %d\n", iter_note->id, isContainsTag(iter_note, tempArray[i]));

                if (isContainsTag(iter_note, tempArray[i]) == 1)
                {
                    struct note *new_note = malloc(sizeof(struct note));

                    new_note->content = iter_note->content;
                    new_note->id = iter_note->id;
                    new_note->next = NULL;

                    //printf("\n%s", new_note->content);

                    sortedInsert(&notesWithTag, new_note);
                }
            }

            orList = unionOf(&orList, &notesWithTag);
        }
    }
    numberOfLists++;
    return orList;
}

struct note *intersect(struct note **noteList1, struct note **noteList2)
{
    // Take two note lists and find a resulting list that contains
    // notes that are in both lists.

    struct note *intersectList = NULL;
    struct note *note1 = *noteList1;

    while (note1 != NULL)
    {
        if (isContainsNote(*noteList2, note1->id) == 1)
        {
            struct note *new_note = malloc(sizeof(struct note));

            new_note->content = note1->content;
            new_note->id = note1->id;
            new_note->next = NULL;

            sortedInsert(&intersectList, new_note);
        }

        note1 = note1->next;
    }

    return intersectList;
}

struct note *unionOf(struct note **noteList1, struct note **noteList2)
{
    // Take two note lists and find a resulting list that union of all notes

    struct note *unionList = NULL;
    struct note *note1 = *noteList1;
    struct note *note2 = *noteList2;

    while (note1 != NULL)
    {
        struct note *new_note = malloc(sizeof(struct note));

        new_note->content = note1->content;
        new_note->id = note1->id;
        new_note->next = NULL;

        sortedInsert(&unionList, new_note);
        note1 = note1->next;
    }

    while (note2 != NULL)
    {

        if (isContainsNote(unionList, note2->id) == 0)
        {
            struct note *new_note = malloc(sizeof(struct note));

            new_note->content = note2->content;
            new_note->id = note2->id;
            new_note->next = NULL;

            sortedInsert(&unionList, new_note);
        }
        note2 = note2->next;
    }

    return unionList;
}

void displaySingleLine(struct note *a_note)
{
    struct note *temp = a_note;

    printf("Results:\n");

    while (temp != NULL)
    {
        printf("Id: %d ", temp->id);
        for (int i = 0; i < strlen(temp->content); i++)
        {
            if ((temp->content)[i] == '\n')
            {
                printf("\n");
                break;
            }
            else{
                printf("%c", (temp->content)[i]);
            }
            
        }
        
        temp = temp->next;
    }
}
void printNoteList(struct note *noteList)
{
    struct note *temp = noteList;
    while (temp != NULL)
    {
        printf("ID: %d  \n", temp->id);
        printTagList(temp->tagList);
        printf("Content:");
        printf("\n%s", temp->content);
        temp = temp->next;
    }
}

void printTagList(struct tag *tagList)
{
    struct tag *temp = tagList;

    if (temp == NULL)
    {
        return;
    }

    printf("TAGS:");
    while (temp != NULL)
    {
        printf("%s---", temp->name);
        temp = temp->next;
    }
    printf("\n");
}

void sortedInsert(struct note **head_ref, struct note *new_note)
{
    //inserts given note into the noteList in a sorted way.
    // Finds correct position and adds it.

    struct note *current;

    if (*head_ref == NULL || (*head_ref)->id > new_note->id)
    {

        new_note->next = *head_ref;
        *head_ref = new_note;
    }
    else if ((*head_ref)->id == new_note->id)
    {
        printf("Given id exists. No insertion has done.");
    }
    else
    {
        /* Locate the note before the point of insertion */
        current = *head_ref;
        while (current->next != NULL && current->next->id < new_note->id)
        {
            current = current->next;
        }
        new_note->next = current->next;
        current->next = new_note;
    }
}
void deleteNote(struct note **head_ref, struct note *note)
{
    struct note *temp = *head_ref;

    while (temp->next->id != note->id)
    {
        if (temp->next == NULL)
        {
            printf("NO such note in the list");
            return;
        }
        temp = temp->next;
    }
    temp->next = note->next;
    free(note);
}

void insertToTagList(struct tag **head_ref, struct tag *new_tag)
{
    // Inserts given tag into the given tagList.
    // Adds it to the end and does not allow duplicate.

    struct tag *last = *head_ref;

    if (*head_ref == NULL)
    {
        *head_ref = new_tag;
        return;
    }

    while (last->next != NULL)
    {
        if (strcmp((last->name), new_tag->name) == 0)
        {
            printf("No duplicate allowed.\n");
            return;
        }
        last = last->next;
    }

    last->next = new_tag;
    return;
}

int isContainsTag(struct note *note, char *tag_name)
{
    //check whether given tag is in note's tag list

    int isContains = 0;

    struct tag *iter;

    for (iter = note->tagList; iter != NULL; iter = iter->next)
    {
        if (strcmp(iter->name, tag_name) == 0)
        {
            isContains = 1;
            break;
        }
    }

    return isContains;
}

int isContainsNote(struct note *note, int id)
{

    int isContains = 0;

    struct note *iter;

    for (iter = note; iter != NULL; iter = iter->next)
    {
        if (iter->id == id)
        {
            isContains = 1;
            break;
        }
    }

    return isContains;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Stack Operators
////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Create a new stack instance
 */
struct stack_t *newStack(void)
{
    struct stack_t *stack = malloc(sizeof(*stack));
    if (stack)
    {
        stack->head = NULL;
        stack->stackSize = 0;
    }
    return stack;
}

/**
 * Make a copy of the string to be stored (assumes  
 */
char *copyString(char *str)
{
    char *tmp = malloc(strlen(str) + 1);
    if (tmp)
        strcpy(tmp, str);
    return tmp;
}

/**
 * Push a value onto the stack
 */
void push(struct stack_t *theStack, char *value)
{
    struct stack_entry *entry = malloc(sizeof(*entry));
    if (entry)
    {
        entry->data = copyString(value);
        entry->next = theStack->head;
        theStack->head = entry;
        theStack->stackSize++;
    }
}

/**
 * Get the value at the top of the stack
 */
char *top(struct stack_t *theStack)
{
    if (theStack && theStack->head)
        return theStack->head->data;
    else
        return NULL;
}

/**
 * Pop the top element from the stack; this deletes both 
 * the stack entry and the string it points to
 */
void pop(struct stack_t *theStack)
{
    if (theStack->head != NULL)
    {
        struct stack_entry *tmp = theStack->head;
        theStack->head = theStack->head->next;
        free(tmp->data);
        free(tmp);
        theStack->stackSize--;
    }
}

/**
 * Clear all elements from the stack
 */
void clear(struct stack_t *theStack)
{
    while (theStack->head != NULL)
        pop(theStack);
}

/**
 * Destroy a stack instance
 */
void destroyStack(struct stack_t **theStack)
{
    clear(*theStack);
    free(*theStack);
    *theStack = NULL;
}