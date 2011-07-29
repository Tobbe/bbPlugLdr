#include "MessageMap.h"
#include <utility>

MessageMap::MessageMap() {
}

MessageMap::~MessageMap() {
}

void MessageMap::addMessages(const unsigned int* const messages, HWND hWnd) {
	const unsigned int* msgs = messages;
	while (*msgs) {
		msgmap.insert(std::pair<unsigned int, HWND>(*msgs, hWnd));
		msgs++;
	}
}

bool MessageMap::removeMessages(const unsigned int* const messages, HWND hWnd) {
	const unsigned int* msgs = messages;
	bool done = false;
	while (*msgs) {
		std::multimap<unsigned int, HWND>::iterator itr = msgmap.lower_bound(*msgs);
		done = false;
		while (!done && itr != msgmap.end() && itr->first == *msgs) {
			if (itr->second == hWnd) {
				msgmap.erase(itr);
				done = true;
			} else {
				itr++;
			}
		}
		msgs++;
	}

	return done;
}

void MessageMap::sendMessage(unsigned int message, WPARAM wParam, LPARAM lParam) const {
	std::multimap<unsigned int, HWND>::const_iterator itr = msgmap.lower_bound(message);
	bool done = false;
	int i = 0;
	HWND *wnds = NULL;

	if (itr->first == message) {
		wnds = new HWND[msgmap.count(message)];
		// make a local copy to be safe for messages being added or removed
		// while traversing the list...
		while (itr->first == message) {
			wnds[i] = itr->second;
			i++;
		}
	}

	// Send the message to all plugins that have registered for that message
	for (int j = 0; j < i; j++) {
		SendMessage(wnds[j], message, wParam, lParam);
	}

	if (wnds != NULL) {
		delete [] wnds;
	}
}