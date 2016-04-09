#ifndef IOS_UTILS_H
#define IOS_UTILS_H

#include "touchui.h"

class TouchUI_iOS: public TouchUI {
public:
	TouchUI_iOS();
	void promptForName(const char *name);
	void showGameControls();
	void hideGameControls();
	void showButtonControls();
	void hideButtonControls();
	void onDpadLocationChanged();
};

const char* ios_get_documents_dir(bool * = NULL);
void ios_open_url(const char *);
void copy_data_to_documents_dir(const char *);

#endif
