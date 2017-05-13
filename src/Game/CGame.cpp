#include <Game/CGame.h>
  
#include <Game/CGameLayout.h>
#include <Engine/CInputManager.h>
#include <Game/CBoardUtils.h>
#include <Engine/CDialog.h>

#include <stdlib.h>
#include <sstream>
#include <fstream>

// the database stuff
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <string>
using namespace std;

template<typename T> void safe_delete(T*& a) 
	{
	  delete a;
	  a = NULL;
	}
string startTime = "";
string startDate = "";

CGame::CGame ():
	game_over ( true ),
	paused (false),
	show_pause_menu (false),
	player (NULL),
	board (NULL),
	layout (NULL), 
	ghost ( NULL ),
	pause_menu ( NULL ),
	bool_exit ( false ),
	bool_menu ( false)
	{
		load_high_score ();
	}
CGame::CGame ( std::string map, int difficulty ):
	game_over ( true ),
	paused (false),
	show_pause_menu (false),
	player (NULL),
	board (NULL),
	layout (NULL), 
	ghost ( NULL ),
	pause_menu ( NULL ),
	bool_exit ( false ),
	bool_menu ( false),
	map ( map),
	difficulty ( difficulty ),
	stage ( 1 ),
	high_score ( 0 ),
	points ( 0 )
	{
		load_high_score ();
	}

CGame::~CGame ()
	{
		safe_delete ( board );
		safe_delete ( player );
		safe_delete ( layout );
		safe_delete ( ghost );
		safe_delete ( pause_menu );

	}
void CGame::start (  )
	{
		safe_delete ( board );
		safe_delete ( player );
		safe_delete ( layout );
		safe_delete ( ghost );
		safe_delete ( pause_menu );
        
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << put_time(&tm, "%d-%m-%Y");
        auto str = oss.str();
        startDate = str;
        std::ostringstream osss;
        osss << put_time(&tm, "%H-%M-%S");
        auto str2 = osss.str();
        startTime = str2;
        
        
		if ( high_score >= points)
				save_high_score ();

		if ( game_over )
			{
				points = 0;
				lives  = 3;
				stage  = 1;
			}
		load_high_score ();

		game_over 			= false;
		show_pause_menu  	= false;
		paused 			 	= false;
		

		if ( ! map . empty ())
			board = CBoardUtils::load_file ( map );
		else
			board = CBoardUtils::load_file ( "default" );

		player = new CPlayer ( board -> get_start_pos () );

		ghost = new CGhostManager ( board -> get_ghost_x (),
									board -> get_ghost_y (),
									difficulty, points );

		int cur_h, cur_w;
		getmaxyx ( stdscr, cur_h, cur_w );

		if ( cur_h - 2 < board -> get_height () || 
			 cur_w - 2 < board -> get_width () )
			{
				std::vector<std::string> v = {  "Map is bigger than your actual",
				"window. Please resize it ", "and run game again" };
				CDialog::show ( v, "ERROR" );
				bool_exit = true;
			}

		layout = new CGameLayout (this, cur_w, cur_h);

		pause_menu = new CMenu ( 1, 1, layout -> pause_menu -> get_width () - 2,
								 layout -> pause_menu -> get_height () - 2 );

		CMenuItem * item;

		item = new CMenuItem ( "Resume Game", RESUME);
		pause_menu -> add ( item );

		item = new CMenuItem ( "Restart Game", RESTART);
		pause_menu -> add ( item );

		pause_menu -> add ( NULL );

		item = new CMenuItem ( "Main Menu", MAIN_MENU);
		pause_menu -> add ( item );

		item = new CMenuItem ( "Exit Game", EXIT_GAME);
		pause_menu -> add ( item );

		timer_player . start();
		timer_ghost  . start();
		timer . start ();

	}
void CGame::restart ()
	{
		safe_delete ( player );
		safe_delete ( layout );
		safe_delete ( ghost );

		player = new CPlayer ( board -> get_start_pos () );
		ghost = new CGhostManager ( 32, 9, difficulty, points );
		int cur_h, cur_w;
		getmaxyx ( stdscr, cur_h, cur_w );

		layout = new CGameLayout (this, cur_w, cur_h);

		timer_player . start();
		timer_ghost  . start();
		timer . start ();

	}

bool CGame::is_paused () const
	{
		return paused;
	}
bool CGame::is_over () const
	{
		return game_over;
	}
bool CGame::will_exit () const
	{
		return bool_exit;
	}
bool CGame::will_go_to_menu () const
	{
		return bool_menu;
	}
void CGame::handle_input ()
	{
		if ( ! CInputManager::any_key_pressed () )
			return;

		if ( CInputManager::is_pressed ( 'p' ))
			{
				paused = ( is_paused () ) ? false : true;
				return;
			}
		if ( is_paused () )
			{
				pause_menu -> HandleInput ();
				return;
			}
		if ( CInputManager::is_pressed (	KEY_RIGHT  ))
			this -> player -> move ( CPlayer::RIGHT );
		if ( CInputManager::is_pressed (	KEY_LEFT  )) 
			this -> player -> move ( CPlayer::LEFT );
		if ( CInputManager::is_pressed (	KEY_DOWN  ))
			this -> player -> move ( CPlayer::DOWN );
		if ( CInputManager::is_pressed (	KEY_UP  ))
			this -> player -> move ( CPlayer::UP );



	}
bool CGame::is_food_eaten ( )
	{
		if ( board -> is_dot ( player -> getX (), player -> getY () ))
			{
				board -> eat_dot ( player -> getX (), player -> getY () );
				points += 10;
				return true;
			}
		return false;
	}
bool CGame::is_star_eaten ( )
	{
		if ( board -> is_star ( player -> getX (), player -> getY () ))
			{
				board -> eat_dot ( player -> getX (), player -> getY () );
				points += 50;
				ghost -> set_frightened_mode ( true );
				return true;
			}
		return false;
	}
void CGame::update ()
	{
		timer_player . pause ();
		timer_ghost  . pause ();

		int delta = this -> get_delay ( stage );
		int delta_ghost;

		if ( points > high_score )
			high_score = points;

		if ( board -> all_dots_eaten () )
			{
				std::vector<std::string> v = {  "Your score is " +  points };
				CDialog::show ( v, "LEVEL PASSED" );
				stage++;
				start ();
			}

		if ( is_paused () )
			{
				if ( pause_menu -> WillExit () )
					{
						int option = pause_menu -> CurrentID ();

						switch ( option )
							{
								case RESUME:
									paused = false;
									break;
								case RESTART:
									game_over = true;
									start ();
									return;
								case MAIN_MENU:
									bool_menu = true;
									return;
								case EXIT_GAME:
									bool_exit = true;
									return;
							}
						pause_menu -> reset ();
					}

				return;
			}

		if ( timer_player . d_ms () >= delta )
			{
				if ( ! player -> is_alive ())
					{
                        auto t = std::time(nullptr);
                        auto tm = *std::localtime(&t);
                        std::ostringstream oss;
                        oss << put_time(&tm, "%d-%m-%Y");
                        auto str = oss.str();
                        string endDate = str;
                        std::ostringstream osss;
                        osss << put_time(&tm, "%H-%M-%S");
                        auto str2 = osss.str();
                        string endTime = str2;
                        
						if ( lives > 0 )
							{
                                sqlite3 *db;
                                sqlite3_stmt * stmt;
                                /* Open database */
                                if( sqlite3_open("database.db", &db) ){
                                    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                                    return;
                                }else{
                                    string query2 = "INSERT INTO life (s_level, s_difficulty, s_time, s_date, levels_passed, e_time, e_date) VALUES ('1', '" + to_string(difficulty) + "', '" + startTime + "', '" + startDate + "', '" + to_string(stage) + "', '" + endTime + "', '" + endDate + "')";
                                    if (sqlite3_prepare(db, query2.c_str(), -1, &stmt, 0) == SQLITE_OK)
                                    {
                                        int res = sqlite3_step(stmt);
                                        sqlite3_finalize(stmt);
                                    }
                                }
								lives--;
								std::vector<std::string> v = {  "You have " + 
																std::to_string(lives) +
																" lives left." };
								CDialog::show ( v, "You Died" );
								restart ();
								
							}	
						else 
							{
                                sqlite3 *db;
                                sqlite3_stmt * stmt;
                                /* Open database */
                                if( sqlite3_open("database.db", &db) ){
                                    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                                    return;
                                }else{
                                    string query2 = "INSERT INTO life (s_level, s_difficulty, s_time, s_date, levels_passed, e_time, e_date) VALUES ('1', '" + to_string(difficulty) + "', '" + startTime + "', '" + startDate + "', '" + to_string(stage) + "', '" + endTime + "', '" + endDate + "')";
                                    if (sqlite3_prepare(db, query2.c_str(), -1, &stmt, 0) == SQLITE_OK)
                                    {
                                        int res = sqlite3_step(stmt);
                                        sqlite3_finalize(stmt);
                                    }
                                    string username;
                                    ifstream myfile ("user.txt");
                                    if (myfile.is_open())
                                    {
                                        while ( getline (myfile,username) )
                                        {
                                            cout << username;
                                        }
                                        myfile.close();
                                    }
                                    string query3 = "SELECT ID FROM user WHERE username = '" + username + "'";
                                    string userID = "";
                                    if ( sqlite3_prepare(db, query3.c_str(), -1, &stmt, NULL ) == SQLITE_OK )
                                    {
                                        int res = 0;
                                        while ( 1 )
                                        {
                                            res = sqlite3_step(stmt);
                                            if ( res == SQLITE_ROW ) {
                                                string s = (char*)sqlite3_column_text(stmt, 0);
                                                userID = s;
                                            }
                                            if ( res == SQLITE_DONE || res==SQLITE_ERROR)
                                                break;
                                        }
                                    }
                                    sqlite3_finalize(stmt);
                                    string query4 = "INSERT INTO score (user_ID, score) VALUES ('" + userID + "', '" + to_string(points) + "')";
                                    if (sqlite3_prepare(db, query4.c_str(), -1, &stmt, 0) == SQLITE_OK)
                                    {
                                        int res = sqlite3_step(stmt);
                                        sqlite3_finalize(stmt);
                                    }
                                }
								game_over = true;
								std::vector<std::string> v = {  "Your score is: " +
																 std::to_string(points) + "." };
								if ( high_score <= points )
									v . push_back ( "NEW HIGHSCORE!!!");
								CDialog::show ( v , "GAME OVER");
								paused = true;
								pause_menu -> RemoveByID ( RESUME );
							}
					}
				else
					{
						player -> update ( board );
						ghost -> check_collisions ( player );
						is_food_eaten ();
						is_star_eaten ();

						
					}

				timer_player . start ();
			}
		else
			{
				timer_player . unpause ();
			}
		if ( ghost -> are_frightened () )
			delta_ghost = 1.6 * delta;
		else delta_ghost = delta;

		if ( timer_ghost . d_ms () >= delta_ghost )
			{
				ghost ->  update ( board, player );
				ghost -> check_collisions ( player );
				timer_ghost . start ();
			}
		else timer_ghost . unpause ();
	}
void CGame::draw ()
	{
		layout -> draw ( pause_menu );	
	}
int  CGame::get_delay ( int speed ) const
	{
		switch ( speed )
			{
				case 1:
					return 120;
				case 2:
					return 100;
				case 3:
					return 90;
				case 4:
					return 80;
				case 5:
					return 60;
				default:
					return 50;
			}
	}
void CGame::load_high_score ()
	{
		std::ifstream score("./examples/scores/" + map + ".pacscore");
     if ( score . good () )
     	score >> high_score;
     else high_score = 0;
	}
void CGame::save_high_score ()
	{
		std::ofstream score;
		score . open("./examples/scores/" + map + ".pacscore", std::ofstream::out | std::ofstream::trunc);
		score << high_score;
		score . close();
	}
