#include<iostream>
#include<utility>
#include<vector>
#include<algorithm>
#include<cmath>
#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include"Stratego.h"
using namespace std;
vector<Piece> Piece::board;
const vector<int> Piece::DEFAULTS={0,1,2,2,2,2,2,2,2,2,3,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,8,8,9,10,11,11,11,11,11,11};
bool Piece::over=false;
vector<Piece>::const_iterator Piece::get_piece_at(pair<int,int> coor){
	return find_if(board.cbegin(),board.cend(),[coor](Piece p){return p.coordinates==coor;});
}
string Piece::end_conditions(){
	auto it=find_if(board.cbegin(),board.cend(),[](Piece p){return (p.val==0 && p.coordinates==pair<int,int>{-1,-1});});
	if(it==board.end()){return "";}
	else return (it->color?"Red wins!":"Blue wins!");
}
Piece::Piece(const bool& c,const int& v,const pair<int,int>& coor):color(c),val(v),coordinates(coor){
	switch(val){
		case -1:break;
		case 0:rank="Flag";break;
		case 1:rank="Spy";break;
		case 2:rank="Scout";break;
		case 3:rank="Miner";break;
		case 4:rank="Sergeant";break;
		case 5:rank="Lieutenant";break;
		case 6:rank="Captain";break;
		case 7:rank="Major";break;
		case 8:rank="Colonel";break;
		case 9:rank="General";break;
		case 10:rank="Marshal";break;
		case 11:rank="Bomb";break;
		default:cerr<<"Invalid value"<<endl;
	}
}
bool is_out(pair<int,int> coor){
	if(coor.first>9 || coor.first<0 || coor.second>9 || coor.second<0){return true;}
	return false;
}
bool is_water(pair<int,int> coor){
	if((coor.first==2 || coor.first==3 || coor.first==6 || coor.first==7) &&
	   (coor.second==4 || coor.second==5)){
		return true;
	}
	return false;
}
bool check(pair<int,int> coor){
	if(is_out(coor) || is_water(coor)){return false;}
	return true;
}
pair<int,int> convert_new(pair<int,int> old){return {11+51*old.first,470-51*old.second};}
pair<int,int> convert_old(pair<int,int> n_coor){return {(n_coor.first-11)/51,(520-n_coor.second)/51};}
void draw_board(bool b,pair<int,int> cursor,bool clicked,sf::RenderWindow& w){
    for(int i=0;i!=11;++i){
        sf::RectangleShape rect(sf::Vector2f(1,511));
        rect.setPosition(10+51*i,10);
        rect.setFillColor(sf::Color::Black);
        w.draw(rect);
    }
    for(int i=0;i!=11;++i){
        sf::RectangleShape rect(sf::Vector2f(511,1));
        rect.setPosition(10,10+51*i);
        rect.setFillColor(sf::Color::Black);
        w.draw(rect);
    }
    sf::RectangleShape water(sf::Vector2f(101,101));
    water.setFillColor(sf::Color::Cyan);
    water.setPosition(113,215);
    w.draw(water);
    water.setPosition(317,215);
    w.draw(water);
    for(auto p:Piece::board){
        p.draw(b,w);
    }
    auto curr_loc=Piece::get_piece_at(convert_old(cursor));
    if(check(convert_old(cursor))){
        if(curr_loc==Piece::board.cend()){
            sf::RectangleShape curs(sf::Vector2f(50,50));
            curs.setPosition(cursor.first,cursor.second);
            curs.setFillColor(sf::Color::Yellow);
            w.draw(curs);
        }else{
            if(clicked){curr_loc->draw(b,w,sf::Color::Green);}
            else if(curr_loc->color){curr_loc->draw(b,w,sf::Color(50,150,255));}
            else{curr_loc->draw(b,w,sf::Color(255,100,100));}
        }
    }
}
//void Piece::print_data()const{cout<<(color?"Blue":"Red")<<" "<<rank<<" (";print();cout<<")";}
bool is_adj(pair<int,int> p1,pair<int,int> p2){
	if((abs(p1.first-p2.first)==1 && p1.second==p2.second) ||
	   (abs(p1.second-p2.second)==1 && p1.first==p2.first)){return true;}
	return false;
}
//first indicates if it can move, second if there's a fight
//needs special cases
pair<bool,bool> Piece::can_move(pair<int,int> dest)const{
	if(is_adj(coordinates,dest) && check(dest)){
		auto it=get_piece_at(dest);
		if(it==board.cend()){return {true,false};}
		else if(it->color!=color){return {true,true};}
	}
	return {false,false};
}
//first is loser, second is if there's a tie
//needs special cases
pair<vector<Piece>::const_iterator,bool> fight(vector<Piece>::const_iterator p1,vector<Piece>::const_iterator p2){
	if(p1->val>p2->val){return {p2,false};}
	else if(p1->val<p2->val){return {p1,false};}
	else{return {p1,true};}
}
/*void turn(bool b){
	int x,y,w,z;
	pair<bool,bool> result;
	do{
		print_board(b);
		cout<<(b?"Blue":"Red")<<" player, select a piece to move: ";
		cin>>x>>y;
		while(Piece::get_piece_at({x,y})==Piece::board.cend() || (Piece::get_piece_at({x,y})->color)!=b){
			cerr<<"You don't have a piece at ("<<x<<", "<<y<<")."<<endl;
			print_board(b);
			cout<<(b?"Blue":"Red")<<" player, select a piece to move: ";
			cin>>x>>y;
		}
		cout<<"Move to: ";
		cin>>w>>z;
		auto curr=Piece::get_piece_at({x,y});
		result=curr->can_move({w,z});
		if(result.first){
			cout<<"You have moved your "<<curr->rank<<" (";curr->print();
			cout<<") from ("<<x<<", "<<y<<") to ("<<w<<", "<<z<<")."<<endl;
			if(result.second){
				auto opponent=Piece::get_piece_at({w,z});
				auto loser=fight(curr,opponent);
				curr->print_data();cout<<" vs ";opponent->print_data();cout<<": ";
				if(loser.second){
					curr->move({-1,-1});
					opponent->move({-1,-1});
					cout<<"Tie!"<<endl;
				}else{
					curr->move({w,z});
					loser.first->move({-1,-1});
					Piece::get_piece_at({w,z})->print_data();cout<<" wins!"<<endl;
				}
			}else{curr->move({w,z});}
		}
		else{cerr<<"You can't moved your "<<curr->rank<<" (";curr->print();
			 cerr<<") from ("<<x<<", "<<y<<") to ("<<w<<", "<<z<<")."<<endl;}
	}while(!result.first);
	string s=Piece::end_conditions();
	if(!s.empty()){
		cout<<s;
		Piece::over=true;
	}
}*/
/*bool Piece::move(char c,int dist){
	pair<int,int> new_coor=coordinates;
	switch(c){
		case 'w':new_coor.second+=dist;break;
		case 'a':new_coor.second-=dist;break;
		case 's':new_coor.first-=dist;break;
		case 'd':new_coor.first+=dist;break;
		case 'q':cout<<"You have chosen not to move this piece."<<endl;return false;break;
		default:cerr<<"Did not input direction correctly."<<endl;return false;break;
	}
	coordinates=new_coor;
	return true;
}*/
bool Piece::has_enemy(pair<int,int> coor){
	auto it=get_piece_at(coor);
	if(it!=board.cend() && it->color==color){return true;}
	return false;
}
/*void center_origin(sf::Text& t){
    auto rect=t.getLocalBounds();
    t.setOrigin(rect.left,rect.top);
}*/
void Piece::draw(bool b,sf::RenderWindow& w)const{
    sf::RectangleShape rect(sf::Vector2f(50,50));
    auto n_coor=convert_new(coordinates);
    rect.setPosition(n_coor.first,n_coor.second);
    rect.setFillColor(color?(sf::Color::Blue):(sf::Color::Red));
    if(color==b){
        sf::Font font;
        font.loadFromFile("arial.ttf");
        sf::Text text(text_val(),font);
        text.setFillColor(sf::Color::Black);
        if(val==10){text.setPosition(n_coor.first+7,n_coor.second+5);}
        else{text.setPosition(n_coor.first+16,n_coor.second+5);}
        w.draw(rect);
        w.draw(text);
    }else{
        w.draw(rect);
    }
}
void Piece::draw(bool b,sf::RenderWindow& w,const sf::Color& c)const{
    sf::RectangleShape rect(sf::Vector2f(50,50));
    auto n_coor=convert_new(coordinates);
    rect.setPosition(n_coor.first,n_coor.second);
    rect.setFillColor(c);
    if(color==b){
        sf::Font font;
        font.loadFromFile("arial.ttf");
        sf::Text text(text_val(),font);
        text.setFillColor(sf::Color::Black);
        if(val==10){text.setPosition(n_coor.first+7,n_coor.second+5);}
        else{text.setPosition(n_coor.first+16,n_coor.second+5);}
        w.draw(rect);
        w.draw(text);
    }else{
        w.draw(rect);
    }
}
string Piece::text_val()const{
	switch(val){
		case 0:return "F";
		case 1:return "S";
		case 11:return "B";
		default:return to_string(val);
	}
}
/*bool Piece::add(){
	if(check(coordinates)){
		board.push_back(this);
		return true;
	}else{
		cerr<<"Can't add piece to board"<<endl;
		return false;
	}
}*/
void add_defaults(){
	for(int i:Piece::DEFAULTS){
		Piece p(true,i);
		p.add();
	}
	for(int i:Piece::DEFAULTS){
		Piece p(false,i);
		p.add();
	}
}
/*void placement(bool b){
	for(auto it=Piece::board.begin();it!=Piece::board.end();++it){
		if(it->color && it->coordinates==make_pair(-1,-1)){
			int x,y;
			print_board(b);
			cout<<"Where would you like to place your "<<it->rank<<" (";it->print();cout<<")?"<<endl;
			cin>>x>>y;
			while((b && (y>3 || y<0 || x>9 || x<0)) ||
				  (!b && (y>9 || y<6 || x>9 || x<0))){
				if(is_out({x,y})){cerr<<"("<<x<<", "<<y<<") is out of bounds."<<endl;}
				else{cerr<<"You are not allowed to place at ("<<x<<", "<<y<<")."<<endl;}
				print_board(b);
				cout<<"Where would you like to place your "<<it->rank<<" (";it->print();cout<<")?"<<endl;
				cin>>x>>y;
			}
			it->coordinates={x,y};
			cout<<"You have placed your "<<it->rank<<" at ("<<x<<", "<<y<<")."<<endl;
		}
	}
}*/
vector<pair<int,int>> all_coors(bool c){
	vector<pair<int,int>> ret;
	if(c){
		for(int i=0;i!=4;++i){
			for(int j=0;j!=10;++j){
				ret.push_back({j,i});
			}
		}
	}else{
		for(int i=6;i!=10;++i){
			for(int j=0;j!=10;++j){
				ret.push_back({j,i});
			}
		}
	}

	return ret;
}
void add_all_pieces(){
    add_defaults();
	auto v1=all_coors(true),v2=all_coors(false);
	auto v1it=v1.cbegin(),v2it=v2.cbegin();
	for(auto it=Piece::board.begin();it!=Piece::board.end();++it){
		if(it->color){it->coordinates=*v1it;++v1it;}
		else{it->coordinates=*v2it;++v2it;}
	}
}
void swap(vector<Piece>::const_iterator p1,vector<Piece>::const_iterator p2){
	auto temp=p1->coordinates;
	p1->coordinates=p2->coordinates;
	p2->coordinates=temp;
}
//swap needs checking
void run_helper(bool b,pair<int,int> c,bool clicked,sf::RenderWindow& win){
    draw_board(b,c,clicked,win);

	/*int x,y,w,z;
	for(int i=1;i!=-1;--i){
		draw_board(true,win);
		cout<<"Select a piece to swap, or type -1 -1 if you're finished: ";

		cin>>x>>y;
		while(x!=-1){
			cout<<"Swap with: ";
			cin>>w>>z;
			swap(Piece::get_piece_at({x,y}),Piece::get_piece_at({w,z}));
			cout<<"You have swapped your pieces at ("<<x<<", "<<y<<") and ("<<w<<", "<<z<<")."<<endl;
			print_board(i);
			cout<<"Select a piece to swap, or type -1 -1 if you're finished: ";
			cin>>x>>y;
		}
	}*/
	/*int t=1;
	while(!Piece::get_over()){
		turn(t%2);
		++t;
	}*/
}
void run(){
    bool current_turn=true,clicked=false;
    pair<int,int> cursor={11,11};
    sf::RenderWindow window(sf::VideoMode(531,531),"STRATEGO BOIS!!!");
    add_all_pieces();
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            switch(event.type){
                case sf::Event::Closed: {window.close();break;}
                case sf::Event::MouseButtonPressed:{
                    if(event.mouseButton.button==sf::Mouse::Left){
                        cursor=convert_old({event.mouseButton.x,event.mouseButton.y});
                        clicked=true;
                    }
                    break;
                }
                case sf::Event::MouseMoved:{
                    cursor=convert_new(convert_old({event.mouseMove.x,event.mouseMove.y}));
                    break;
                }
                default:break;
            }
        }
        window.clear(sf::Color::White);
        run_helper(current_turn,cursor,clicked,window);
        window.display();
    }
}
