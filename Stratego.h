#ifndef STRATEGO_H
#define STRATEGO_H
#include<iostream>
#include<utility>
#include<vector>
#include<algorithm>
using namespace std;
class Piece{
public:
	friend void placement(bool);
	friend void add_all_pieces();
	friend void swap(vector<Piece>::const_iterator,vector<Piece>::const_iterator);
	friend void turn(bool);
	friend void draw_board(bool,pair<int,int>,bool,sf::RenderWindow&);
	static string end_conditions();
	static const vector<int> DEFAULTS;
	static vector<Piece>::const_iterator get_piece_at(pair<int,int>);
	static bool get_over(){return over;}
	//true is blue, false is red
	const bool color;
	const int val;
	Piece(const bool& c,const int& v,const pair<int,int>& coor={-1,-1});
	void move(pair<int,int> dest)const{coordinates=dest;}
	//bool move(char c,int dist=1);
	pair<int,int> get_coordinates()const{return coordinates;}
	void set_coordinates(const pair<int,int>& coor){coordinates=coor;}
	void add(){board.push_back(*this);}
	void draw(bool,sf::RenderWindow&)const;
	void draw(bool,sf::RenderWindow&,const sf::Color&)const;
	string text_val()const;
	void print_data()const;
	//testing
	//static vector<Piece*> board;
	//string rank;
private:
    friend void add_defaults();
	string rank;
	mutable pair<int,int> coordinates;
	pair<bool,bool> can_move(pair<int,int>)const;
	bool has_enemy(pair<int,int>);
	static bool over;
	static vector<Piece> board;
};
void run_helper();
void run();
#endif
