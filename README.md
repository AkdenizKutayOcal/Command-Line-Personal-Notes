# Command-Line-Personal-Notes

This project is made by Akdeniz Kutay Ocal for the CMPE_326 course.

1-Design Overview:
Project is designed to be a basic Command Line Program that is written in C that 
manages your personal notes with tags. Using the program, you can not only enter 
your notes, but also tag them for a better organization. You can search your notes
via tag expressions, which are various tags combinedwith boolean operators like 
and, or, not. These functionalities can be used as following;

ADD :   This command is used to add a new note to the system.
        Each note is composed of at least one line of text can be at most 500
        characters, but a note may have as many lines of text as the user wants.

        ADD <note_id>
        { note content }
        END

TAG :   This command is used to tag one or more notes.
        Tags are names that are composed of at most 100 alphanumeric characters
        (spaces, symbols like parenthesis, mathematical operators, or punctuation
        marks are not allowed).

        TAG <tag_name> <note_id1> <note_id2> ... -1

DISPLAY :   This command is used to display contents of a note. Output is diplayed
            starting with the line "Id: <note_id>"" and than it's content.

            DISPLAY <note_id>

FIND :  This command is used to find notes according to a tag expression.
        Tag expressions are of the form and(t1 ... tn), or(t1 ... tn), and not(t1).
        The system must display the results of a FIND command sorted by the note id in
        ascending order.

        FIND AND( todo ) ⇒ the system must find all notes that are tagged as ‘todo’.

        FIND AND( todo important ) ⇒ the system must find all notes that are tagged as
        both ‘todo’ and ‘important’.

        FIND OR( todo AND( work important ) ) ⇒ the system must find all notes that
        are tagged as either ‘todo’ or both ‘work’ and ‘important’.

        FIND AND( funny NOT( work ) ) ⇒ the system must find all notes that are tagged
        as ‘funny’ but not ‘work’.


2- Complete Specification:

    The code takes input string and processes it to do operations and call further methods. 
    For ADD operations, new note struct is created and added to the NoteLinkedList and when
    a new TAG is created and associated with it, it adds the copy of the tag object to note's
    TagList. When FIND function is called, it adds all arguments that are seperated by the 
    whitespaces to a stack and runs the and, not, or operations in a nested way. Seperate list
    of notes are created in every operation and acoording to the parent operation, it's method 
    is called which uses union in or and intercest in and. 

3- Known bugs or problems:

    No input error checks are done since it was not required in the assignment. Therefore user
    has to write correct commands or the program will not work as intended.
