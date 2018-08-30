// This program simulates the card game "war."  
//	1) draw_initializer(): A 52 card deck is shuffled and divided between two players.
//	2) play_war(): the leading cards are compared and the higher card gets to add both to its rubbish pile.  
//	3) reset_decks() : Once a deck is empty, its rubbish pile is shuffled and added back to the deck.  
//	Play continues until a player has no cards in neither their rubbish deck nor draw pile.

#include <iostream>
#include <random>
#include <algorithm>
#include <iterator>
#include <string>

using namespace std;

vector<int> d1; // player's draw piles
vector<int> d2;
vector<int> r1; // player's rubbish piles
vector<int> r2;

// prints the contents of a vector to the console
void print(const vector<int>& v) {
	for (size_t i = 0; i < v.size(); ++i) {
		// new line every 8 values for clarity
		if (i % 8 == 0) {
			cout << '\n';
		}
		cout << v[i];
		if (i < v.size() - 1) {
			cout << ", ";
		}
		else {
			cout << '\n';
		}
	}
}
// shuffles elements of a vector with equal probability
//  requires <random> and <algorithm> to run
vector<int> shuffle(vector<int> r) {
	random_device rd;	// a uniformly-distributed integer random number generator
	mt19937 g(rd());	// type of the mersenne_twister_engine (a random number engine)
	shuffle(r.begin(), r.end(), g);
	return r;
}
// sets up a standard deck of 52 cards (11 == jack, etc.), shuffles it, and divides
//	it between two hands
void draw_initializer(vector<int>& d1, vector<int>& d2) {
	vector<int> deck(52);
	for (int i = 0; i < 13; ++i) {		// 13 kinds of cards
		for (int j = 0; j < 4; j++) {	// 4 of each card
			deck[4 * i + j] = i + 1;
		}
	}
	deck = shuffle(deck);
	// split the deck between the two draw piles
	for (int i = 0; i < 26; ++i) {
		d1.push_back(deck[i]);
		d2.push_back(deck[i + 26]);
	}
}
// compares the leading elements of two vectors until a vector is empty
void play_war(vector<int>& d1, vector<int>& d2) {
	// figure out which deck is smallest (determines how long to compare)
	int small_d = 0;
	int size1 = d1.size();
	int size2 = d2.size();
	(size1 <= size2) ? small_d = size1 : small_d = size2;
	// r1 and r2 are the rubbish piles. whoever has the stronger card gets
	//  both cards put into their rubbish pile.
	for (int i = 0; i < small_d; ++i) {
		if (d1[0] > d2[0]) {
			r1.push_back(d1[0]);
			r1.push_back(d2[0]);
		}
		else if (d1[0] < d2[0]) {
			r2.push_back(d1[0]);
			r2.push_back(d2[0]);
		}
		else if (d1[0] == d2[0]) {
			r1.push_back(d1[0]);
			r2.push_back(d2[0]);
		}
		/*
		else if (d1[0] == d2[0]) {
			int length1 = d1.size();
			int length2 = d2.size();
			if (length1 > 4 && length2 > 4) {
				if (d1[3] > d2[3]) {
					for (int i = 0; i < 4; ++i) {
						r1.push_back(d1[i]);
						r1.push_back(d2[i]);
					}
				}
				else {
					for (int i = 0; i < 4; ++i) {
						r2.push_back(d1[i]);
						r2.push_back(d2[i]);
					}
				}
				for (int i = 0; i < 3; ++i) {
					d1.erase(d1.begin());
					d2.erase(d2.begin());
				}
			}
			else {
				r1.push_back(d1[0]);
				r1.push_back(d2[0]);
			}
		}
		*/
		else {
			cerr << "Error comparing values. \n";
		}
		// at the end of this, the smaller deck should be the empty vector
		d1.erase(d1.begin());
		d2.erase(d2.begin());
	}
}
// whichever deck is empty, shuffle its corresponding rubbish pile to refill,
//	then clear the rubbish pile
void reset_decks(vector<int>& d1, vector<int>& d2,
	vector<int>& r1, vector<int>& r2) {
	if (d1.size() != 0 && d2.size() != 0) {
		cerr << "Neither deck is empty.\n";
	}
	if (d1.size() == 0) {
		d1 = shuffle(r1);
		r1 = {};
	}
	if (d2.size() == 0) {
		d2 = shuffle(r2);
		r2 = {};
	}
	// make sure there are still 52 cards;
	int total_cards = d1.size() + r1.size() + d2.size() + r2.size();
	if (total_cards != 52) {
		cerr << "Missing cards!\n";
	}
}

int main() {
	vector<int> round_length;		// keep track of how many shuffles it takes to win
	int j = 0;
	while (j < 100) {				// play 100 games to gather statistics
		draw_initializer(d1, d2);	// 26 cards each from a shuffled 52 card deck
		string winner;
		int i = 0;
		while (winner == "") {
			play_war(d1, d2);		// compare leading card from both decks, winner 
									//  gets cards added to their 'r' pile
			// if someone has absolutely no cards left, the other player wins
			if (d1.size() == 0 && r1.size() == 0) {
				winner = "Player 2";
				round_length.push_back(i);
			}
			if (d2.size() == 0 && r2.size() == 0) {
				winner = "Player 1";
				round_length.push_back(i);
			}
			// if 400 shuffles, game is a tie
			if (i == 400) {
				winner = "no one";
				round_length.push_back(400);
			}
			reset_decks(d1, d2, r1, r2);
			++i;
		}
		// reset the draw and rubbish piles for a new game
		d1 = {};
		r1 = {};
		d2 = {};
		r2 = {};
		++j;
	}
	print(round_length);
	return 0;
}
// this is similar to main() above, but it prints the deck contents
//	occasionally so the user can see the program works currectly
int Notmain() {
	draw_initializer(d1, d2);
	string winner;
	int i = 0;
	while (winner == "") {
		play_war(d1, d2);
		if (d1.size() == 0 && r1.size() == 0) {
			cout << "required rounds: " << i << endl;
			winner = "Player 2";
		}
		if (d2.size() == 0 && r2.size() == 0) {
			cout << "required rounds: " << i << endl;
			winner = "Player 1";
		}
		if (i % 25 == 0) {
			cout << i << " hands played\n";
			cout << "deck 1:\n";
			print(d1);
			print(r1);
			cout << "deck 2:\n";
			print(d2);
			print(r2);
		}
		if (i > 401) {
			winner = "no one";
		}
		reset_decks(d1, d2, r1, r2);
		++i;
	}
	cout << winner << " wins!";
	return 0;
}