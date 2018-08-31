// This program simulates the card game "war."  
//	1) draw_initializer(): A 52 card deck is shuffled and divided between two players.
//	2) compare(): the leading cards are compared and the higher card gets to add both to its rubbish pile.  
//	3) reset_deck() : Once a deck is empty, its rubbish pile is shuffled and added back to the deck.  
//	Play continues until a player has no cards in neither their rubbish deck nor draw pile.

#include <iostream>
#include <random>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

using namespace std;

const int burn = 3;	// how many cards to burn in case of tie
vector<int> burn_pile;	// storage for burned cards
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

// moves the top draw pile cards into the winner's rubbish pile
void win_lose(Hand &won, Hand &lost) {
	won.rp.push_back(lost.dp[0]);
	won.rp.push_back(won.dp[0]);
	won.dp.erase(won.dp.begin());
	lost.dp.erase(lost.dp.begin());
}

// declare in advance so accessible to tie()
void compare(Hand& h1, Hand& h2);

// in case of a tie
void tie(Hand &h1, Hand &h2) {
	// make sure the draw pile has enough cards to burn
	if (h1.dp.size() < burn + 2)
		reset_deck(h1);
	if (h2.dp.size() < burn + 2)
		reset_deck(h2);
	// if there aren't enough total cards to burn and then compare,
	//    discard current tie
	if (h1.sum() < burn + 2 || h2.sum() < burn + 2) {
		h1.rp.push_back(h1.dp[0]);
		h2.rp.push_back(h2.dp[0]);
		h1.dp.erase(h1.dp.begin());
		h2.dp.erase(h2.dp.begin());
		return;
	}
	// move the tied top cards and the specified burn number into the burn pile
	for (int i = 0; i < burn+1; ++i) {
			burn_pile.push_back(h1.dp[0]);
			burn_pile.push_back(h2.dp[0]);
			h1.dp.erase(h1.dp.begin());
			h2.dp.erase(h2.dp.begin());
	}
	compare(h1, h2);
}

// if the burn pile has any cards, move them to hand h, then clear the burn pile
void burn_check(Hand& h) {
	if (burn_pile.size() != 0) {
		for (int i = 0; i < burn_pile.size(); ++i) {
			h.rp.push_back(burn_pile[i]);
		}
		burn_pile = {};
	}
}

// compare leading cards, reset decck(s) when no draw pile cards remain
void compare(Hand &h1, Hand &h2) {
	while (h1.dp.size() != 0 && h2.dp.size() != 0) {
		if (h1.dp[0] > h2.dp[0]) {
			win_lose(h1, h2);	
			burn_check(h1);
		}
		else if (h1.dp[0] < h2.dp[0]) {
			win_lose(h2, h1);
			burn_check(h2);
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
	compare(h1, h2);
}

int main() {
	int j = 0;					// keep track of number of rounds played
	vector<int> round_length;	// store number of deck resets per round
	while (j < 1000) {							// play 100 games to gather statistics
		Hand h1;
		Hand h2;
		draw_initializer(h1, h2);		// 26 cards each from a shuffled 52 card deck
		compare(h1, h2);				// compare leading card from both decks, winner 
										//  gets cards added to their 'r' pile
		// if someone has absolutely no cards left, the other player wins
		if (h1.sum() == 0 || h2.sum() == 0) 
			round_length.push_back(tracker);
		else 
			round_length.push_back(0);
		// reset the draw and rubbish piles for a new game
		h1.dp = {};
		h1.rp = {};
		h2.dp = {};
		h2.rp = {};
		++j;
		tracker = 0;
	}
	print(round_length);
	// calculate mean
	int total = 0;
	for (int i = 0; i < round_length.size(); ++i) {
		total += round_length[i];
	}
	cout << endl << "Mean round length = " 
		<< total / round_length.size() << endl;
	return 0;
}