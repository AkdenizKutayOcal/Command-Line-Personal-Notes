// TODO
// Create Note Struct
// Learn dynamic Memory Allocation
// https://www.programiz.com/c-programming/c-dynamic-memory-allocation

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

struct note *noteList;    //holds all notes
struct tag *totalTagList; //holds all tags that are created

void add(); // This command is used to add a new note to the system.
// Each note is composed of at least one line of text can be at most 500
// characters, but a note may have as many lines of text as the user wants

// ADD <note_id>
// { note content }
// END

void tag(); // This command is used to tag one or more notes.
// Tags are names that are composed of at most 100 alphanumeric characters
// (spaces, symbols like parenthesis, mathematical operators, or punctuation
// marks are not allowed).

// TAG <tag_name> <note_id1> <note_id2> ... -1

void display(); // This command is used to display contents of a note.
//Output should start with the line "Id: <note_id>"" and than it's content

// DISPLAY <note_id>

void find(); // This command is used to find notes according to a tag expression.
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

void processCommand(); // Takes command line as an input and calls for needed functions
void printNoteList();
void sortedInsert();
void insertToTagList();

int main(int argc, char *argv[])
{

    char input[MAX_LINE_LENGTH];
    fgets(input, sizeof(input), stdin);
    input[strlen(input) - 1] = 0;
    noteList = NULL;

    while (strcmp(input, "<EOF>") != 0)
    {
        if (strlen(input) == 0)
        {
            //printf("Wrong command input! Should not be empty.\n");
            break;
        }

        processCommand(input);
        /* printf("------------------------\n");
        printNoteList(noteList);
        printf("------------------------\n"); */

        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = 0;
    }

    return 0;
}

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

        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = 0;

        while (strcmp(input, "END") != 0)
        {
            strcat(content, input);
            //printf("----%s----\n",content);
            strcat(content, "\n");

            fgets(input, sizeof(input), stdin);
            input[strlen(input) - 1] = 0;
        }

        new_note->content = content;
        new_note->tagList = NULL;

        sortedInsert(&noteList, new_note);
    }

    else if (strcmp(commandArgs[0], "TAG") == 0)
    {

        char *tag = commandArgs[1];
        struct tag *new_tag_note = malloc(sizeof(struct tag));
        struct tag *new_tag_tagList = malloc(sizeof(struct tag));

        new_tag_note->name = tag;
        new_tag_tagList->name = tag;
        new_tag_note->next = NULL;
        new_tag_tagList->next = NULL;

        insertToTagList(&totalTagList,new_tag_tagList);

        struct note *iterNote;

        for (int i = 2; i < numberOfArgs - 1; i++)
        {
            for (iterNote = noteList; iterNote != NULL; iterNote = iterNote->next)
            {
                if (iterNote->id == atoi(commandArgs[i]))
                {
                    insertToTagList(&(iterNote->tagList),tag); //insert new tag into tag list of the note
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
}

void printNoteList(struct note *noteList)
{
    struct note *temp = noteList;
    while (temp != NULL)
    {
        printf("ID: %d  \n", temp->id);
        //printf("TAG: %s  \n", temp->tag);
        printf("%s", temp->content);
        temp = temp->next;
    }
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