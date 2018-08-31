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
#include <vector>

using namespace std;

const int burn = 3;	// how many cards to burn in case of tie
vector<int> burn_pile;
int tracker = 0;	// track how many deck resets it takes for a game to complete

struct Hand {
	vector<int> dp;	// player's draw piles
	vector<int> rp;	// player's rubbish piles
	int sum() {		
		return dp.size() + rp.size();
	}
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

//  reset an individual deck
void reset_deck(Hand &h) {
	++tracker;
	h.rp = shuffle(h.rp);
	while (h.rp.size() > 0) {
		h.dp.push_back(h.rp[0]);
		h.rp.erase(h.rp.begin());
	}
}
// declare in advance
void compare(Hand& h1, Hand& h2);

// in case of a tie
void tie(Hand &h1, Hand &h2) {
	// if there aren't enough total cards to burn and then compare,
	//  discard current tie
	if (h1.sum() < burn + 2 || h2.sum() < burn + 2) {
		h1.rp.push_back(h1.dp[0]);
		h2.rp.push_back(h2.dp[0]);
		h1.dp.erase(h1.dp.begin());
		h2.dp.erase(h2.dp.begin());
		return;
	}
	// make sure the draw pile has enough cards to burn
	if (h1.dp.size() < burn + 2)
		reset_deck(h1);
	if (h2.dp.size() < burn + 2)
		reset_deck(h2);
	for (int i = 0; i < burn+1; ++i) {
			burn_pile.push_back(h1.dp[0]);
			burn_pile.push_back(h2.dp[0]);
			h1.dp.erase(h1.dp.begin());
			h2.dp.erase(h2.dp.begin());
	}
	compare(h1, h2);
}


/////////////////////////////////
void compare(Hand &h1, Hand &h2) {
	while (h1.dp.size() != 0 && h2.dp.size() != 0) {
		if (h1.dp[0] > h2.dp[0]) {
			h1.rp.push_back(h1.dp[0]);
			h1.rp.push_back(h2.dp[0]);
			h1.dp.erase(h1.dp.begin());
			h2.dp.erase(h2.dp.begin());
			// see if there was a burn pile to be won
			if (burn_pile.size() != 0) {
				for (int i = 0; i < burn_pile.size(); ++i) {
					h1.rp.push_back(burn_pile[i]);
				}
				burn_pile = {};
			}
		}
		else if (h1.dp[0] < h2.dp[0]) {
			h2.rp.push_back(h1.dp[0]);
			h2.rp.push_back(h2.dp[0]);
			h1.dp.erase(h1.dp.begin());
			h2.dp.erase(h2.dp.begin());
			// see if there was a burn pile to be won
			if (burn_pile.size() != 0) {
				for (int i = 0; i < burn_pile.size(); ++i) {
					h2.rp.push_back(burn_pile[i]);
				}
				burn_pile = {};
			}
		}
		else {
			// burn 3 cards and then compare the next card
			tie(h1, h2);
		}
	}
	// check to see if game is over
	if (h1.sum() == 0 || h2.sum() == 0) {
		return;
	}
	if (h1.dp.size() == 0) reset_deck(h1);
	if (h2.dp.size() == 0) reset_deck(h2);
	++tracker;
	compare(h1, h2);
}

int main() {
	int j = 0;
	vector<int> round_length;
	while (j < 10) {							// play 100 games to gather statistics
		Hand h1;
		Hand h2;
		draw_initializer(h1, h2);		// 26 cards each from a shuffled 52 card deck
		compare(h1, h2);				// compare leading card from both decks, winner 
										//  gets cards added to their 'r' pile
		// if someone has absolutely no cards left, the other player wins
		if (h1.sum() == 0) {
			cout << "Player 2" << endl;
			round_length.push_back(tracker);
		}
		else if (h2.sum() == 0) {
			cout << "Player 1" << endl;
			round_length.push_back(tracker);
		}
		else {
			round_length.push_back(0);
		}
		// reset the draw and rubbish piles for a new game
		h1.dp = {};
		h1.rp = {};
		h2.dp = {};
		h2.rp = {};
		++j;
		tracker = 0;
	}
	print(round_length);
	return 0;
}
