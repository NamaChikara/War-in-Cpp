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

struct Hand {
	vector<int> dp;	// player's draw piles
	vector<int> rp;	// player's rubbish piles
};

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
void draw_initializer(Hand& h1, Hand& h2) {
	vector<int> deck(52);
	for (int i = 0; i < 13; ++i) {		// 13 kinds of cards
		for (int j = 0; j < 4; j++) {	// 4 of each card
			deck[4 * i + j] = i + 1;
		}
	}
	deck = shuffle(deck);
	// split the deck between the two draw piles
	for (int i = 0; i < 26; ++i) {
		h1.dp.push_back(deck[i]);
		h2.dp.push_back(deck[i + 26]);
	}
}

// compares the leading elements of two vectors until a vector is empty
void play_war(Hand& h1, Hand& h2) {
	// figure out which deck is smallest (determines how long to compare)
	int small_d = 0;
	int size1 = h1.dp.size();
	int size2 = h2.dp.size();
	(size1 <= size2) ? small_d = size1 : small_d = size2;
	// h1.rp and h2.rp are the rubbish piles. whoever has the stronger card gets
	//  both cards put into their rubbish pile.
	for (int i = 0; i < small_d; ++i) {
		if (h1.dp[0] > h2.dp[0]) {
			h1.rp.push_back(h1.dp[0]);
			h1.rp.push_back(h2.dp[0]);
		}
		else if (h1.dp[0] < h2.dp[0]) {
			h2.rp.push_back(h1.dp[0]);
			h2.rp.push_back(h2.dp[0]);
		}
		else if (h1.dp[0] == h2.dp[0]) {
			int length1 = h1.dp.size();
			int length2 = h2.dp.size();
			// see if enough cards to burn three before comparing 4th card
			if (length1 > 4 && length2 > 4) {
				// if so, we will be removing four cards from each pile, so it will
				//  take 3 fewer comparisons to exhaust the smaller draw pile
				i += 3;
				if (h1.dp[3] > h2.dp[3]) {
					for (int i = 0; i < 4; ++i) {
						h1.rp.push_back(h1.dp[i]);
						h1.rp.push_back(h2.dp[i]);
					}
					for (int i = 0; i < 3; ++i) {
						h1.dp.erase(h1.dp.begin());
						h2.dp.erase(h2.dp.begin());
					}
				}
				else if (h1.dp[3] < h2.dp[3]) {
					for (int i = 0; i < 4; ++i) {
						h2.rp.push_back(h1.dp[i]);
						h2.rp.push_back(h2.dp[i]);
					}
					for (int i = 0; i < 3; ++i) {
						h1.dp.erase(h1.dp.begin());
						h2.dp.erase(h2.dp.begin());
					}
				}
				// if the 4th card is also a tie, just give each player back their 4 cards
				else {
					for (int i = 0; i < 4; ++i) {
						h1.rp.push_back(h1.dp[i]);
						h2.rp.push_back(h2.dp[i]);
					}
					for (int i = 0; i < 3; ++i) {
						h1.dp.erase(h1.dp.begin());
						h2.dp.erase(h2.dp.begin());
					}
				}
			}
			// if there's not enough cards to burn 3 without shuffling a deck, just give the
			//  players back the tie card
			else {
				h1.rp.push_back(h1.dp[0]);
				h1.rp.push_back(h2.dp[0]);
			}
		}
		else {
			cerr << "Error comparing values. \n";
		}
		// at the end of this, the smaller deck should be the empty vector
		h1.dp.erase(h1.dp.begin());
		h2.dp.erase(h2.dp.begin());
	}
}
// whichever deck is empty, shuffle its corresponding rubbish pile to refill,
//	then clear the rubbish pile
void reset_decks(Hand& h1, Hand& h2) {
	if (h1.dp.size() != 0 && h2.dp.size() != 0) {
		cerr << "Neither deck is empty.\n";
	}
	if (h1.dp.size() == 0) {
		h1.dp = shuffle(h1.rp);
		h1.rp = {};
	}
	if (h2.dp.size() == 0) {
		h2.dp = shuffle(h2.rp);
		h2.rp = {};
	}
	// make sure there are still 52 cards;
	int total_cards = h1.dp.size() + h1.rp.size() + h2.dp.size() + h2.rp.size();
	if (total_cards != 52) {
		cerr << "Missing cards!\n";
	}
}

int main() {
	vector<int> round_length;		// keep track of how many shuffles it takes to win
	int j = 0;
	while (j < 100) {							// play 100 games to gather statistics
		Hand h1;
		Hand h2;
		draw_initializer(h1, h2);	// 26 cards each from a shuffled 52 card deck
		string winner;
		int i = 0;
		while (winner == "") {
			play_war(h1, h2);		// compare leading card from both decks, winner 
									//  gets cards added to their 'r' pile
			// if someone has absolutely no cards left, the other player wins
			if (h1.dp.size() == 0 && h1.rp.size() == 0) {
				winner = "Player 2";
				round_length.push_back(i);
			}
			if (h2.dp.size() == 0 && h2.rp.size() == 0) {
				winner = "Player 1";
				round_length.push_back(i);
			}
			// if 400 shuffles, game is a tie
			if (i == 400) {
				winner = "no one";
				round_length.push_back(400);
			}
			// if no winner, reset the deck that is empty
			reset_decks(h1, h2);
			++i;
		}
		// reset the draw and rubbish piles for a new game
		h1.dp = {};
		h1.rp = {};
		h2.dp = {};
		h2.rp = {};
		++j;
	}
	print(round_length);
	return 0;
}
// this is similar to main() above, but it prints the deck contents
//	occasionally so the user can see the program works currectly
/*
int Notmain() {
	draw_initializer(h1, h2);
	string winner;
	int i = 0;
	while (winner == "") {
		play_war(h1.dp, h2.dp);
		if (h1.dp.size() == 0 && h1.rp.size() == 0) {
			cout << "required rounds: " << i << endl;
			winner = "Player 2";
		}
		if (h2.dp.size() == 0 && h2.rp.size() == 0) {
			cout << "required rounds: " << i << endl;
			winner = "Player 1";
		}
		if (i % 25 == 0) {
			cout << i << " hands played\n";
			cout << "deck 1:\n";
			print(h1.dp);
			print(h1.rp);
			cout << "deck 2:\n";
			print(h2.dp);
			print(h2.rp);
		}
		if (i > 401) {
			winner = "no one";
		}
		reset_decks(h1.dp, h2.dp, h1.rp, h2.rp);
		++i;
	}
	cout << winner << " wins!";
	return 0;
}
*/