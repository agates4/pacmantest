#include <Engine/CNCurses.h>
#include <Engine/CStateManager.h>
#include <Game/CStateMenu.h>

// the database stuff
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include "include/mysql_connection.h"
#include "include/cppconn/driver.h"
#include "include/cppconn/exception.h"
#include "include/cppconn/resultset.h"
#include "include/cppconn/statement.h"
#include "include/cppconn/prepared_statement.h"

using namespace std;

int main(int argc, char const *argv[])
{
    sql::Driver *driver;
    sql::Connection *con;
    driver = get_driver_instance();
    con = driver->connect("dbdev.cs.kent.edu", "agates10", "suLs74sD");
    con->setSchema("agates10");
    sql::PreparedStatement *pstmt = con->prepareStatement("SELECT COUNT(ID) FROM `user` WHERE `username` = aron AND `password` = gates");
    sql::ResultSet *res = pstmt->executeQuery();
    while (res->next()) {
        std::cout << "You Entered: " << res->getInt("ID") << endl;
    }
    delete pstmt;
    delete res;
    delete con;
    
    
//    char mesg[]="Do you need to register? [y/n] ";		/* message to be appeared on the screen */
//    char str[80];
//    int row,col;                                        /* to store the number of rows and */
//                                                        /* the number of colums of the screen */
//    initscr();                                          /* start the curses mode */
//    getmaxyx(stdscr,row,col);                           /* get the number of rows and columns */
//    mvprintw(row/2,(col-strlen(mesg))/2,"%s",mesg);     /* print the message at the center of the screen */
//    getstr(str);
//    mvprintw(LINES - 2, 0, "You Entered: %s", str);
//    
//    std::string whiteOut = "";
//    for (int i = 0; i < col-strlen(mesg) + col-strlen(str); ++i) {
//        whiteOut += " ";
//    }
//    mvprintw(row/2,(col-strlen(mesg))/2,"%s",whiteOut.c_str());
//    if(str[0] == 'n' && str[1] == '\0') {
//        mvprintw(row/2 - 2,(col-strlen("Login"))/2,"%s","Login");
//        mvprintw(row/2,(col-strlen("Username: "))/2,"%s","Username: ");
//        getstr(str);
//        mvprintw(LINES - 2, 0, "You Entered: %s", str);
//        std::string username = str;
//        mvprintw(row/2 + 1,(col-strlen("Password: "))/2,"%s","Password: ");
//        getstr(str);
//        std::string password = str;
//        mvprintw(LINES - 2, 0, "You Entered: %s", str);
//        
//        sql::Driver *driver;
//        sql::Connection *con;
//        driver = get_driver_instance();
//        con = driver->connect("dbdev.cs.kent.edu", "agates10", "suLs74sD");
//        con->setSchema("agates10");
//        sql::PreparedStatement *pstmt = con->prepareStatement("SELECT ID FROM `user` WHERE `username` = " + username + " AND `password` = " + password);
//        sql::ResultSet *res = pstmt->executeQuery();
//        while (res->next()) {
//            mvprintw(LINES - 2, 0, "You Entered: %s", res->getInt("ID"));
//        }
//        delete pstmt;
//        delete res;
//        delete con;
//    }
//    else {
//        mvprintw(row/2,(col-strlen("dong farts"))/2,"%s","dong farts");
//    }
//    getch();
//    
//    endwin();
//    
//    CNCurses::init();
//    CStateManager states;
//    states . run ( new CStateMenu );
//    CNCurses::exit();
//    
//    return 0;
}
