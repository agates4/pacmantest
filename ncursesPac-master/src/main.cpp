#include <Engine/CNCurses.h>
#include <Engine/CStateManager.h>
#include <Game/CStateMenu.h>

// the database stuff
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{
    sqlite3 *db;
    sqlite3_stmt * stmt;
    /* Open database */
    if( sqlite3_open("database.sqlite", &db) ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    }else{
        // success
    }
    
    char mesg[]="Do you need to register? [y/n] ";		/* message to be appeared on the screen */
    char str[80];
    int row,col;                                        /* to store the number of rows and */
                                                        /* the number of colums of the screen */
    initscr();                                          /* start the curses mode */
    getmaxyx(stdscr,row,col);                           /* get the number of rows and columns */
    mvprintw(row/2,(col-strlen(mesg))/2,"%s",mesg);     /* print the message at the center of the screen */
    getstr(str);
    mvprintw(LINES - 2, 0, "You Entered: %s", str);
    
    string whiteOut = "";
    for (int i = 0; i < col-strlen(mesg) + col-strlen(str); ++i) {
        whiteOut += " ";
    }
    mvprintw(row/2,(col-strlen(mesg))/2,"%s",whiteOut.c_str());
    if(str[0] == 'n' && str[1] == '\0') {
        mvprintw(row/2 - 2,(col-strlen("Login"))/2,"%s","Login");
        mvprintw(row/2,(col-strlen("Username: "))/2,"%s","Username: ");
        getstr(str);
        mvprintw(LINES - 2, 0, "You Entered: %s", str);
        string username = str;
        mvprintw(row/2 + 1,(col-strlen("Password: "))/2,"%s","Password: ");
        getstr(str);
        string password = str;
        mvprintw(LINES - 2, 0, "You Entered: %s", str);
        
        string query = "SELECT COUNT(ID) FROM user WHERE username = '" + username + "' AND password = '" + password + "'";
        bool validUser = false;
        if ( sqlite3_prepare(db, query.c_str(), -1, &stmt, NULL ) == SQLITE_OK )
        {
            int res = 0;
            while ( 1 )
            {
                res = sqlite3_step(stmt);
                if ( res == SQLITE_ROW ) {
                    string s = (char*)sqlite3_column_text(stmt, 0);
                    if (s == "1")
                        validUser = true;
                }
                if ( res == SQLITE_DONE || res==SQLITE_ERROR)
                    break;
            }
        }
        sqlite3_finalize(stmt);
        if (validUser) {
            mvprintw(LINES - 2, 0, "Login successful: %s", "press any button to continue.");
        }
        else {
            mvprintw(LINES - 2, 0, "Login failed: %s", "this program will terminate.");
            getch();
            endwin();
            return 0;
        }

    }
    else if (str[0] == 'y' && str[1] == '\0'){
        mvprintw(row/2 - 2,(col-strlen("Register"))/2,"%s","Register");
        mvprintw(row/2,(col-strlen("Username: "))/2,"%s","Username: ");
        getstr(str);
        mvprintw(LINES - 2, 0, "You Entered: %s", str);
        string username = str;
        mvprintw(row/2 + 1,(col-strlen("Password: "))/2,"%s","Password: ");
        getstr(str);
        string password = str;
        mvprintw(LINES - 2, 0, "You Entered: %s", str);
        
        string query2 = "INSERT INTO user (username, password, role_ID) VALUES ('" + username + "', '" + password + "', 2)";
        bool validUser = false;
        if (sqlite3_prepare(db, query2.c_str(), -1, &stmt, 0) == SQLITE_OK)
        {
            int res = sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            if (res == 101) {
                validUser = true;
            }
        }
//        if ( sqlite3_prepare(db, query2.c_str(), -1, &stmt, 0 ) == SQLITE_OK )
//        {
//            int res = sqlite3_step(stmt);
//            sqlite3_finalize(stmt);
//            mvprintw(LINES - 2, 0, "The text: %s", to_string(res).c_str());
////            query2 = "INSERT INTO user (ID, username, password, role_ID) VALUES ('5', '" + username + "', '" + password + "', '2');";
////            if ( sqlite3_prepare(db, query2.c_str(), -1, &stmt, 0 ) == SQLITE_OK )
////            {
////                res = sqlite3_step(stmt);
////                sqlite3_finalize(stmt);
////                mvprintw(LINES - 2, 0, "The text: %s", to_string(res).c_str());
////            }
//        }
        if (validUser) {
            mvprintw(LINES - 2, 0, "Register successful: %s", "press any button to continue.");
        }
        else {
            mvprintw(LINES - 2, 0, "Register failed: %s", "this program will terminate.");
            getch();
            endwin();
            return 0;
        }
    }
    else {
        mvprintw(row/2,(col-strlen("Bad input. This program will terminate. "))/2,"%s","Bad input. This program will terminate. ");
        getch();
        endwin();
        return 0;
    }
    getch();
    endwin();
    
    CNCurses::init();
    CStateManager states;
    states . run ( new CStateMenu );
    CNCurses::exit();
    
    sqlite3_close(db);
    
    return 0;
}




//string query = "SELECT ID FROM user WHERE username = 'aron'";
//bool validUser = false;
//if ( sqlite3_prepare(db, query.c_str(), -1, &stmt, NULL ) == SQLITE_OK )
//{
//    validUser = true;
//    int ctotal = sqlite3_column_count(stmt);
//    int res = 0;
//    
//    while ( 1 )
//    {
//        res = sqlite3_step(stmt);
//        if ( res == SQLITE_ROW )
//            string s = (char*)sqlite3_column_text(stmt, 0);
//        cout << s << endl;
//        if ( res == SQLITE_DONE || res==SQLITE_ERROR)
//            break;
//    }
//}
