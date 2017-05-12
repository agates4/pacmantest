#include <Engine/CDialog.h>
#include <Engine/CLayout.h>
#include <Engine/CMenu.h>
#include <Engine/CTime.h>
#include <Engine/CNCurses.h>
#include <Engine/CInputManager.h>
#include <string.h>
#include <iostream>
#include <algorithm>


void CDialog::show ( const std::vector<std::string> & message,
					 const std::string & label)
	{
		int window_height, window_width, msg_width;

		msg_width = 0;

		window_height = (message) . size () + 4;

		for ( unsigned int i = 0; i < (message) . size (); ++i )
			msg_width = ((msg_width > (int) (message) [ i ] . size ()) ? msg_width : (int) (message) [ i ] . size ());


		window_width = std::max ( 33, msg_width + 4 );


		int cur_h, cur_w;
		getmaxyx ( stdscr, cur_h, cur_w );

		int window_x = cur_w / 2 - (window_width - 2)  / 2;
		int window_y = cur_h / 2 - window_height / 2;

		CWindow dialog ( window_x, window_y, window_width, window_height, true, label );

		refresh ();

		dialog . set_lower_text ( "Press any key to continue");

		dialog . clear ();

		for ( unsigned int i = 0; i < (message) . size (); ++i )
			dialog . print_str ( (message) [i], 2, i + 2);
		

		dialog . refresh ();

		refresh ();
		CTime::delay_ms ( 5000 );
		CNCurses::get_input (-1);
	}


void CDialog::show_questions ( const std::vector<std::string> & message,
                    const std::string & label , const std::vector<std::string> & input_questions)
    {
        int window_height, window_width, msg_width;
        
        msg_width = 0;
        
        window_height = (message) . size () + 4 + input_questions . size ();
        
        for ( unsigned int i = 0; i < (message) . size (); ++i )
            msg_width = ((msg_width > (int) (message) [ i ] . size ()) ? msg_width : (int) (message) [ i ] . size ());
        
        
        window_width = std::max ( 33, msg_width + 4 );
        
        
        int cur_h, cur_w;
        getmaxyx ( stdscr, cur_h, cur_w );
        
        int window_x = cur_w / 2 - (window_width - 2)  / 2;
        int window_y = cur_h / 2 - window_height / 2;
        
        CWindow dialog ( window_x, window_y, window_width, window_height, true, label );
        
        refresh ();
        
        dialog . set_lower_text ( "Press any key to continue");
        
        dialog . clear ();
        
        for ( unsigned int i = 0; i < (message) . size (); ++i )
            dialog . print_str ( (message) [i], 2, i + 2);
        
        std::string key_input;
        for ( unsigned int i = 0; i < (input_questions) . size (); ++i )
            dialog . print_str ( (input_questions) [i], 2, i + 3);
        
        dialog . refresh ();
        
        refresh ();
        CTime::delay_ms ( 5000 );
        
        while(std::cin >> key_input && key_input.length() < 4){
            for ( unsigned int i = 0; i < (message) . size (); ++i )
                dialog . print_str ( (message) [i], 2, i + 2);
            refresh ();
        }
        
//        CNCurses::get_input (-1);
    }






