#ifndef CDIALOG_H_DEFINED
#define CDIALOG_H_DEFINED

	
#include <string>
#include <vector>
/// Function to show quick GUI-like dialog window on screen.
namespace CDialog 
	{
		/// Shows message on screen.
		/// Function spawns multi-line dialog window,
		/// window is centered and wraps around input strings.
		///
		/// @note One line is one element in std::vector
		///
		/// @param message Which strings to show on screen.
		/// @param label Label on right-left side of dialog.

		void show ( const std::vector<std::string> & message,
                   const std::string & label);
        
        void show_questions ( const std::vector<std::string> & message,
                   const std::string & label, const std::vector<std::string> & input_questions);
	}

#endif //CDIALOG_H_DEFINED
